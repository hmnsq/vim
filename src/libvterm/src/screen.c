#ضمن "vterm_internal.h"

// vim: set sw=2 :
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "rect.h"
#ضمن "utf8.h"

#وصف_ڪريو UNICODE_SPACE 0x20
#وصف_ڪريو UNICODE_LINEFEED 0x0a

#وصف_رد DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
تعريفنوع بنية
{
  /* After the bitfield */
  VTermColor   fg, bg;

  بنانشان صحيح bold      : 1;
  بنانشان صحيح underline : 2;
  بنانشان صحيح italic    : 1;
  بنانشان صحيح blink     : 1;
  بنانشان صحيح ابتو   : 1;
  بنانشان صحيح conceal   : 1;
  بنانشان صحيح strike    : 1;
  بنانشان صحيح font      : 4; /* 0 to 9 */
  بنانشان صحيح small     : 1;
  بنانشان صحيح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  بنانشان صحيح protected_cell : 1;
  بنانشان صحيح dwl            : 1; /* on a DECDWL or DECDHL line */
  بنانشان صحيح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
تعريفنوع بنية
{
  uint32_t chars[VTERM_MAX_CHARS_في_CELL];
  ScreenPen pen;
} ScreenCell;

بنية VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_pushline4;

  VTerm_Damage_سائيز damage_ملايو;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحيح pending_scroll_downward, pending_scroll_rightward;

  صحيح صفون_2;
  صحيح کانن_ڳڻپ;

  بنانشان صحيح global_ابتو : 1;
  بنانشان صحيح reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *بفر;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_بفر;

  ScreenPen pen;
};

ساكن فراغ clearcell(ثابت VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ساكن ScreenCell *getcell(ثابت VTermScreen *screen, صحيح صف, صحيح کانو)
{
  إذا(صف < 0 || صف >= screen->صفون_2)
    أرجع عدم;
  إذا(کانو < 0 || کانو >= screen->کانن_ڳڻپ)
    أرجع عدم;
  إذا (screen->بفر == عدم)
    أرجع عدم;
  أرجع screen->بفر + (screen->کانن_ڳڻپ * صف) + کانو;
}

ساكن ScreenCell *alloc_بفر(VTermScreen *screen, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * صفون_2 * کانن_ڳڻپ);

  لكل(صحيح صف = 0; صف < صفون_2; صف++) {
    لكل(صحيح کانو = 0; کانو < کانن_ڳڻپ; کانو++) {
      clearcell(screen, &new_بفر[صف * کانن_ڳڻپ + کانو]);
    }
  }

  أرجع new_بفر;
}

ساكن فراغ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  بدل(screen->damage_ملايو) {
  حالة VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    اكسر;

  حالة VTERM_DAMAGE_صف:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    إذا(rect.پڇاڙي_صف > rect.شروعات_صف + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وإلا إذا(screen->damaged.شروعات_صف == -1) {
      // None stored yet
      screen->damaged = rect;
      أرجع;
    }
    وإلا إذا(rect.شروعات_صف == screen->damaged.شروعات_صف) {
      // Merge with the stored line
      إذا(screen->damaged.شروعات_کانو > rect.شروعات_کانو)
        screen->damaged.شروعات_کانو = rect.شروعات_کانو;
      إذا(screen->damaged.پڇاڙي_کانو < rect.پڇاڙي_کانو)
        screen->damaged.پڇاڙي_کانو = rect.پڇاڙي_کانو;
      أرجع;
    }
    وإلا {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    اكسر;

  حالة VTERM_DAMAGE_SCREEN:
  حالة VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    إذا(screen->damaged.شروعات_صف == -1)
      screen->damaged = rect;
    وإلا {
      rect_expand(&screen->damaged, &rect);
    }
    أرجع;

  افتراضي:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_ملايو);
    أرجع;
  }

  إذا(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ساكن فراغ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.پڇاڙي_صف = screen->صفون_2;
  rect.پڇاڙي_کانو = screen->کانن_ڳڻپ;

  damagerect(screen, rect);
}

ساكن صحيح putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.صف, pos.کانو);

  إذا(!cell)
    أرجع 0;

  صحيح i;
  لكل(i = 0; i < VTERM_MAX_CHARS_في_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  إذا(i < VTERM_MAX_CHARS_في_CELL)
    cell->chars[i] = 0;

  لكل(صحيح کانو = 1; کانو < info->ويڪر; کانو++)
  {
    ScreenCell *onecell = getcell(screen, pos.صف, pos.کانو + کانو);
    إذا (onecell == عدم)
      اكسر;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.شروعات_صف = pos.صف;
  rect.پڇاڙي_صف   = pos.صف+1;
  rect.شروعات_کانو = pos.کانو;
  rect.پڇاڙي_کانو   = pos.کانو+info->ويڪر;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن فراغ sb_pushline_from_صف(VTermScreen *screen, صحيح صف, صحيح continuation)
{
  VTermPos pos;
  pos.صف = صف;
  لكل(pos.کانو = 0; pos.کانو < screen->کانن_ڳڻپ; pos.کانو++)
    vterm_screen_get_cell(screen, pos, screen->sb_بفر + pos.کانو);

  إذا(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->کانن_ڳڻپ, screen->sb_بفر, continuation, screen->cbdata);
  وإلا
    (screen->callbacks->sb_pushline)(screen->کانن_ڳڻپ, screen->sb_بفر, screen->cbdata);
}

ساكن صحيح premove(VTermRect rect, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.شروعات_صف == 0 && rect.شروعات_کانو == 0 &&        // starts top-left corner
     rect.پڇاڙي_کانو == screen->کانن_ڳڻپ &&                      // full width
     screen->بفر == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    لكل(صحيح صف = 0; صف < rect.پڇاڙي_صف; صف++) {
      ثابت VTerm_سٽ_Info *lineinfo = vterm_state_get_lineinfo(screen->state, صف);
      sb_pushline_from_صف(screen, صف, lineinfo->continuation);
    }
  }

  أرجع 1;
}

ساكن صحيح moverect_اندروني(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح کانن_ڳڻپ = src.پڇاڙي_کانو - src.شروعات_کانو;
  صحيح downward = src.شروعات_صف - dest.شروعات_صف;

  صحيح init_صف, test_صف, inc_صف;
  إذا(downward < 0) {
    init_صف = dest.پڇاڙي_صف - 1;
    test_صف = dest.شروعات_صف - 1;
    inc_صف  = -1;
  }
  وإلا {
    init_صف = dest.شروعات_صف;
    test_صف = dest.پڇاڙي_صف;
    inc_صف  = +1;
  }

  لكل(صحيح صف = init_صف; صف != test_صف; صف += inc_صف)
    memmove(getcell(screen, صف, dest.شروعات_کانو),
            getcell(screen, صف + downward, src.شروعات_کانو),
            کانن_ڳڻپ * حجم(ScreenCell));

  أرجع 1;
}

ساكن صحيح moverect_user(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->moverect) {
    إذا(screen->damage_ملايو != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    إذا((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      أرجع 1;
  }

  damagerect(screen, dest);

  أرجع 1;
}

ساكن صحيح erase_اندروني(VTermRect rect, صحيح selective, فراغ *user)
{
  VTermScreen *screen = user;

  لكل(صحيح صف = rect.شروعات_صف; صف < screen->state->صفون_2 && صف < rect.پڇاڙي_صف; صف++) {
    ثابت VTerm_سٽ_Info *info = vterm_state_get_lineinfo(screen->state, صف);

    لكل(صحيح کانو = rect.شروعات_کانو; کانو < rect.پڇاڙي_کانو; کانو++) {
      ScreenCell *cell = getcell(screen, صف, کانو);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     صف, کانو);
	أرجع 1;
      }
      إذا(selective && cell->pen.protected_cell)
        استمر;

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

  أرجع 1;
}

ساكن صحيح erase_user(VTermRect rect, صحيح selective اڻ_استعمال_ٿيل, فراغ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن صحيح erase(VTermRect rect, صحيح selective, فراغ *user)
{
  erase_اندروني(rect, selective, user);
  أرجع erase_user(rect, 0, user);
}

ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->damage_ملايو != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_اندروني, erase_اندروني, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    أرجع 1;
  }

  إذا(screen->damaged.شروعات_صف != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  إذا(screen->pending_scrollrect.شروعات_صف == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  وإلا إذا(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  وإلا {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_اندروني, erase_اندروني, screen);

  إذا(screen->damaged.شروعات_صف == -1)
    أرجع 1;

  إذا(rect_شامل_آهي(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وإلا إذا(rect.شروعات_کانو <= screen->damaged.شروعات_کانو &&
          rect.پڇاڙي_کانو   >= screen->damaged.پڇاڙي_کانو &&
          rightward == 0) {
    إذا(screen->damaged.شروعات_صف >= rect.شروعات_صف &&
       screen->damaged.شروعات_صف  < rect.پڇاڙي_صف) {
      screen->damaged.شروعات_صف -= downward;
      إذا(screen->damaged.شروعات_صف < rect.شروعات_صف)
        screen->damaged.شروعات_صف = rect.شروعات_صف;
      إذا(screen->damaged.شروعات_صف > rect.پڇاڙي_صف)
        screen->damaged.شروعات_صف = rect.پڇاڙي_صف;
    }
    إذا(screen->damaged.پڇاڙي_صف >= rect.شروعات_صف &&
       screen->damaged.پڇاڙي_صف  < rect.پڇاڙي_صف) {
      screen->damaged.پڇاڙي_صف -= downward;
      إذا(screen->damaged.پڇاڙي_صف < rect.شروعات_صف)
        screen->damaged.پڇاڙي_صف = rect.شروعات_صف;
      إذا(screen->damaged.پڇاڙي_صف > rect.پڇاڙي_صف)
        screen->damaged.پڇاڙي_صف = rect.پڇاڙي_صف;
    }
  }
  وإلا {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  أرجع 1;
}

ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos, صحيح visible, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->movecursor)
    أرجع (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  أرجع 0;
}

ساكن صحيح setpenattr(VTermAttr attr, VTerm_قدر *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->عدد;
    أرجع 1;
  حالة VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_ابتو:
    screen->pen.ابتو = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_FONT:
    screen->pen.font = val->عدد;
    أرجع 1;
  حالة VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    أرجع 1;
  حالة VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    أرجع 1;
  حالة VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->عدد;
    أرجع 1;

  حالة VTERM_N_ATTRS:
    أرجع 0;
  }

  أرجع 0;
}

ساكن صحيح settermprop(VTermProp prop, VTerm_قدر *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(prop) {
  حالة VTERM_PROP_ALTSCREEN:
    إذا(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      أرجع 0;

    screen->بفر = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    إذا(!val->boolean)
      damagescreen(screen);
    اكسر;
  حالة VTERM_PROP_ابتو:
    screen->global_ابتو = val->boolean;
    damagescreen(screen);
    اكسر;
  افتراضي:
    ; /* ignore */
  }

  إذا(screen->callbacks && screen->callbacks->settermprop)
    أرجع (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  أرجع 1;
}

ساكن صحيح bell(فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->bell)
    أرجع (*screen->callbacks->bell)(screen->cbdata);

  أرجع 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ساكن صحيح سٽ_popcount(ScreenCell *بفر, صحيح صف, صحيح صفون_2 اڻ_استعمال_ٿيل, صحيح کانن_ڳڻپ)
{
  صحيح کانو = کانن_ڳڻپ - 1;
  طالما(کانو >= 0 && بفر[صف * کانن_ڳڻپ + کانو].chars[0] == 0)
    کانو--;
  أرجع کانو + 1;
}

#وصف_ڪريو REFLOW (screen->reflow)

ساكن فراغ resize_بفر(VTermScreen *screen, صحيح bufidx, صحيح new_صفون, صحيح new_کانن_ڳڻپ, صحيح سرگرم, VTermStateFields *statefields)
{
  صحيح old_صفون = screen->صفون_2;
  صحيح old_کانن_ڳڻپ = screen->کانن_ڳڻپ;

  ScreenCell *old_بفر = screen->buffers[bufidx];
  VTerm_سٽ_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_صفون * new_کانن_ڳڻپ);
  VTerm_سٽ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_صفون);

  // Find the final row of old buffer content
  صحيح old_صف = old_صفون - 1;
  صحيح new_صف = new_صفون - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#جيڪڏهن_وصف_ٿيل DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_کانن_ڳڻپ, old_صفون, new_کانن_ڳڻپ, new_صفون, old_cursor.کانو, old_cursor.صف);
#جيڪڏهن_پورو

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحيح final_blank_صف = new_صفون;

  طالما(old_صف >= 0) {
    صحيح old_صف_پڇاڙي = old_صف;
    /* TODO: Stop if dwl or dhl */
    طالما(REFLOW && old_lineinfo && old_صف >= 0 && old_lineinfo[old_صف].continuation)
      old_صف--;
    صحيح old_صف_شروعات = old_صف;

    صحيح ويڪر = 0;
    لكل(صحيح صف = old_صف_شروعات; صف <= old_صف_پڇاڙي; صف++) {
      إذا(REFLOW && صف < (old_صفون - 1) && old_lineinfo[صف + 1].continuation)
        ويڪر += old_کانن_ڳڻپ;
      وإلا
        ويڪر += سٽ_popcount(old_بفر, صف, old_صفون, old_کانن_ڳڻپ);
    }

    إذا(final_blank_صف == (new_صف + 1) && ويڪر == 0)
      final_blank_صف = new_صف;

    صحيح new_اوچائي = REFLOW
      ? ويڪر ? (ويڪر + new_کانن_ڳڻپ - 1) / new_کانن_ڳڻپ : 1
      : 1;

    صحيح new_صف_پڇاڙي = new_صف;
    صحيح new_صف_شروعات = new_صف - new_اوچائي + 1;

    old_صف = old_صف_شروعات;
    صحيح old_کانو = 0;

    صحيح اضافي_صفون = new_صفون - final_blank_صف;

    إذا(new_صف_شروعات < 0 && /* we'd fall off the top */
        اضافي_صفون >= 0 && /* we actually have spare rows */
        (!سرگرم || new_cursor.صف == -1 || (new_cursor.صف - new_صف_شروعات) < new_صفون))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحيح downwards = -new_صف_شروعات;
      إذا(downwards > اضافي_صفون)
        downwards = اضافي_صفون;
      صحيح rowcount = new_صفون - downwards;

#جيڪڏهن_وصف_ٿيل DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#جيڪڏهن_پورو

      memmove(&new_بفر[downwards * new_کانن_ڳڻپ], &new_بفر[0],   rowcount * new_کانن_ڳڻپ * حجم(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * حجم(new_lineinfo[0]));

      new_صف += downwards;
      new_صف_شروعات += downwards;
      new_صف_پڇاڙي += downwards;

      إذا(new_cursor.صف >= 0)
        new_cursor.صف += downwards;

      final_blank_صف += downwards;
    }

#جيڪڏهن_وصف_ٿيل DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_صف_شروعات, new_صف_پڇاڙي, old_صف_شروعات, old_صف_پڇاڙي, ويڪر);
#جيڪڏهن_پورو

    إذا(new_صف_شروعات < 0) {
      إذا(old_صف_شروعات <= old_cursor.صف && old_cursor.صف < old_صف_پڇاڙي) {
        new_cursor.صف = 0;
        new_cursor.کانو = old_cursor.کانو;
        إذا(new_cursor.کانو >= new_کانن_ڳڻپ)
          new_cursor.کانو = new_کانن_ڳڻپ-1;
      }
      اكسر;
    }

    لكل(new_صف = new_صف_شروعات, old_صف = old_صف_شروعات; new_صف <= new_صف_پڇاڙي; new_صف++) {
      صحيح ڳڻپ = ويڪر >= new_کانن_ڳڻپ ? new_کانن_ڳڻپ : ويڪر;
      ويڪر -= ڳڻپ;

      صحيح new_کانو = 0;

      طالما(ڳڻپ) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_بفر[new_صف * new_کانن_ڳڻپ + new_کانو] = old_بفر[old_صف * old_کانن_ڳڻپ + old_کانو];

        إذا(old_cursor.صف == old_صف && old_cursor.کانو == old_کانو)
          new_cursor.صف = new_صف, new_cursor.کانو = new_کانو;

        old_کانو++;
        إذا(old_کانو == old_کانن_ڳڻپ) {
          old_صف++;

          إذا(!REFLOW) {
            new_کانو++;
            اكسر;
          }
          old_کانو = 0;
        }

        new_کانو++;
        ڳڻپ--;
      }

      إذا(old_cursor.صف == old_صف && old_cursor.کانو >= old_کانو) {
        new_cursor.صف = new_صف, new_cursor.کانو = (old_cursor.کانو - old_کانو + new_کانو);
        إذا(new_cursor.کانو >= new_کانن_ڳڻپ)
          new_cursor.کانو = new_کانن_ڳڻپ-1;
      }

      طالما(new_کانو < new_کانن_ڳڻپ) {
        clearcell(screen, &new_بفر[new_صف * new_کانن_ڳڻپ + new_کانو]);
        new_کانو++;
      }

      new_lineinfo[new_صف].continuation = (new_صف > new_صف_شروعات);
    }

    old_صف = old_صف_شروعات - 1;
    new_صف = new_صف_شروعات - 1;
  }

  إذا(old_cursor.صف <= old_صف) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.صف = 0, new_cursor.کانو = old_cursor.کانو;
    إذا(new_cursor.کانو >= new_کانن_ڳڻپ)
      new_cursor.کانو = new_کانن_ڳڻپ-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  إذا(سرگرم && (new_cursor.صف == -1 || new_cursor.کانو == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    إذا (new_cursor.صف < 0)
      new_cursor.صف = 0;
    إذا (new_cursor.کانو < 0)
      new_cursor.کانو = 0;
  }

  إذا(old_صف >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    إذا((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      لكل(صحيح صف = 0; صف <= old_صف; صف++) {
        ثابت VTerm_سٽ_Info *lineinfo = old_lineinfo + صف;
        sb_pushline_from_صف(screen, صف, lineinfo->continuation);
      }
    إذا(سرگرم)
      statefields->pos.صف -= (old_صف + 1);
  }
  إذا(new_صف >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    طالما(new_صف >= 0) {
      VTermPos pos;
      إذا(!(screen->callbacks->sb_popline(old_کانن_ڳڻپ, screen->sb_بفر, screen->cbdata)))
        اكسر;

      pos.صف = new_صف;
      لكل(pos.کانو = 0; pos.کانو < old_کانن_ڳڻپ && pos.کانو < new_کانن_ڳڻپ; pos.کانو += screen->sb_بفر[pos.کانو].ويڪر) {
        VTermScreenCell *src = &screen->sb_بفر[pos.کانو];
        ScreenCell *dst = &new_بفر[pos.صف * new_کانن_ڳڻپ + pos.کانو];

        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_في_CELL; i++) {
          dst->chars[i] = src->chars[i];
          إذا(!src->chars[i])
            اكسر;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.ابتو   = src->attrs.ابتو ^ screen->global_ابتو;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        إذا(src->ويڪر == 2 && pos.کانو < (new_کانن_ڳڻپ-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      لكل( ; pos.کانو < new_کانن_ڳڻپ; pos.کانو++)
        clearcell(screen, &new_بفر[pos.صف * new_کانن_ڳڻپ + pos.کانو]);
      new_صف--;

      إذا(سرگرم)
        statefields->pos.صف++;
    }
  }
  إذا(new_صف >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحيح moverows = new_صفون - new_صف - 1;
    memmove(&new_بفر[0], &new_بفر[(new_صف + 1) * new_کانن_ڳڻپ], moverows * new_کانن_ڳڻپ * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_صف + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.صف -= (new_صف + 1);

    لكل(new_صف = moverows; new_صف < new_صفون; new_صف++) {
      لكل(صحيح کانو = 0; کانو < new_کانن_ڳڻپ; کانو++)
        clearcell(screen, &new_بفر[new_صف * new_کانن_ڳڻپ + کانو]);
      new_lineinfo[new_صف] = (VTerm_سٽ_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_بفر);
  screen->buffers[bufidx] = new_بفر;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  إذا(سرگرم)
    statefields->pos = new_cursor;

  أرجع;
}

ساكن صحيح resize(صحيح new_صفون, صحيح new_کانن_ڳڻپ, VTermStateFields *fields, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح altscreen_سرگرم = (screen->buffers[BUFIDX_ALTSCREEN] && screen->بفر == screen->buffers[BUFIDX_ALTSCREEN]);

  صحيح old_صفون = screen->صفون_2;
  صحيح old_کانن_ڳڻپ = screen->کانن_ڳڻپ;

  إذا(new_کانن_ڳڻپ > old_کانن_ڳڻپ) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    إذا(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    إذا (new_کانن_ڳڻپ > VTERM_MAX_کانن_ڳڻپ)
      new_کانن_ڳڻپ = VTERM_MAX_کانن_ڳڻپ;

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_کانن_ڳڻپ);
  }

  إذا (new_صفون > VTERM_MAX_صفون)
    new_صفون = VTERM_MAX_صفون;

  resize_بفر(screen, 0, new_صفون, new_کانن_ڳڻپ, !altscreen_سرگرم, fields);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    resize_بفر(screen, 1, new_صفون, new_کانن_ڳڻپ, altscreen_سرگرم, fields);
  وإلا إذا(new_صفون != old_صفون) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_سٽ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_صفون);
    لكل(صحيح صف = 0; صف < new_صفون; صف++)
      new_lineinfo[صف] = (VTerm_سٽ_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->بفر = altscreen_سرگرم ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->صفون_2 = new_صفون;
  screen->کانن_ڳڻپ = new_کانن_ڳڻپ;

  إذا(new_کانن_ڳڻپ <= old_کانن_ڳڻپ) {
    إذا(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_کانن_ڳڻپ);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  إذا(screen->callbacks && screen->callbacks->resize)
    أرجع (*screen->callbacks->resize)(new_صفون, new_کانن_ڳڻپ, screen->cbdata);

  أرجع 1;
}

ساكن صحيح setlineinfo(صحيح صف, ثابت VTerm_سٽ_Info *newinfo, ثابت VTerm_سٽ_Info *oldinfo, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    لكل(صحيح کانو = 0; کانو < screen->کانن_ڳڻپ; کانو++) {
      ScreenCell *cell = getcell(screen, صف, کانو);
      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     صف, کانو);
	أرجع 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.شروعات_صف = صف;
    rect.پڇاڙي_صف   = صف + 1;
    rect.شروعات_کانو = 0;
    rect.پڇاڙي_کانو   = newinfo->doublewidth ? screen->کانن_ڳڻپ / 2 : screen->کانن_ڳڻپ;
    damagerect(screen, rect);

    إذا(newinfo->doublewidth) {
      rect.شروعات_کانو = screen->کانن_ڳڻپ / 2;
      rect.پڇاڙي_کانو   = screen->کانن_ڳڻپ;

      erase_اندروني(rect, 0, user);
    }
  }

  أرجع 1;
}

ساكن صحيح sb_clear(فراغ *user) {
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->sb_clear)
    إذا((*screen->callbacks->sb_clear)(screen->cbdata))
      أرجع 1;

  أرجع 0;
}

ساكن VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  عدم, // moverect
  &erase, // erase
  عدم, // initpen
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
ساكن VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  إذا(!state)
    أرجع عدم;

  VTermScreen *screen = vterm_allocator_malloc(vt, حجم(VTermScreen));
  إذا (screen == عدم)
    أرجع عدم;
  صحيح صفون_2, کانن_ڳڻپ;

  vterm_get_سائيز(vt, &صفون_2, &کانن_ڳڻپ);

  screen->vt = vt;
  screen->state = state;

  screen->damage_ملايو = VTERM_DAMAGE_CELL;
  screen->damaged.شروعات_صف = -1;
  screen->pending_scrollrect.شروعات_صف = -1;

  screen->صفون_2 = صفون_2;
  screen->کانن_ڳڻپ = کانن_ڳڻپ;

  screen->global_ابتو = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = عدم;
  screen->cbdata    = عدم;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_بفر(screen, صفون_2, کانن_ڳڻپ);

  screen->بفر = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * کانن_ڳڻپ);
  إذا (screen->بفر == عدم || screen->sb_بفر == عدم)
  {
    vterm_screen_free(screen);
    أرجع عدم;
  }

  vterm_state_سيٽ_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  أرجع screen;
}

اندروني فراغ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_بفر);

  vterm_allocator_free(screen->vt, screen);
}

فراغ vterm_screen_reset(VTermScreen *screen, صحيح hard)
{
  screen->damaged.شروعات_صف = -1;
  screen->pending_scrollrect.شروعات_صف = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ساكن size_t _get_chars(ثابت VTermScreen *screen, ثابت صحيح utf8, فراغ *بفر, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحيح padding = 0;

#وصف_ڪريو PUT(c)                                             \
  إذا(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    إذا(بفر && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (حرف *)بفر + outpos);   \
    وإلا                                                   \
      outpos += thislen;                                   \
  }                                                        \
  وإلا {                                                   \
    إذا(بفر && outpos + 1 <= len)                        \
      ((uint32_t*)بفر)[outpos++] = (c);                 \
    وإلا                                                   \
      outpos++;                                            \
  }

  لكل(صحيح صف = rect.شروعات_صف; صف < rect.پڇاڙي_صف; صف++) {
    لكل(صحيح کانو = rect.شروعات_کانو; کانو < rect.پڇاڙي_کانو; کانو++) {
      ScreenCell *cell = getcell(screen, صف, کانو);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     صف, کانو);
	أرجع 1;
      }
      إذا(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      وإلا إذا(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      وإلا {
        طالما(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_في_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    إذا(صف < rect.پڇاڙي_صف - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  أرجع outpos;
}

size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect)
{
  أرجع _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_متن(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect)
{
  أرجع _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
صحيح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.صف, pos.کانو);

  إذا(!intcell)
    أرجع 0;

  لكل(صحيح i = 0; i < VTERM_MAX_CHARS_في_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    إذا(!intcell->chars[i])
      اكسر;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.ابتو   = intcell->pen.ابتو ^ screen->global_ابتو;
  cell->attrs.conceal   = intcell->pen.conceal;
  cell->attrs.strike    = intcell->pen.strike;
  cell->attrs.font      = intcell->pen.font;
  cell->attrs.small     = intcell->pen.small;
  cell->attrs.baseline  = intcell->pen.baseline;

  cell->attrs.dwl = intcell->pen.dwl;
  cell->attrs.dhl = intcell->pen.dhl;

  cell->fg = intcell->pen.fg;
  cell->bg = intcell->pen.bg;

  إذا(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    إذا(pos.کانو < (screen->کانن_ڳڻپ - 1) &&
       getcell(screen, pos.صف, pos.کانو + 1)->chars[0] == (uint32_t)-1) {
      إذا(getcell(screen, pos.صف, pos.کانو)->chars[0] == 0x20) {
        getcell(screen, pos.صف, pos.کانو)->chars[0] = 0;
        cell->ويڪر = 2;
      } وإلا إذا(getcell(screen, pos.صف, pos.کانو)->chars[0] == 0) {
        getcell(screen, pos.صف, pos.کانو + 1)->chars[0] = 0;
        cell->ويڪر = 1;
      } وإلا {
        cell->ويڪر = 2;
      }
    } وإلا
      cell->ويڪر = 1;
  } وإلا {
    إذا(pos.کانو < (screen->کانن_ڳڻپ - 1) &&
       getcell(screen, pos.صف, pos.کانو + 1)->chars[0] == (uint32_t)-1)
      cell->ويڪر = 2;
    وإلا
      cell->ويڪر = 1;
  }

  أرجع 1;
}

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  لكل(; pos.کانو < screen->کانن_ڳڻپ; pos.کانو++) {
    ScreenCell *cell = getcell(screen, pos.صف, pos.کانو);
    إذا(cell->chars[0] != 0)
      أرجع 0;
  }

  أرجع 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  إذا(vt->screen)
    أرجع vt->screen;

  vt->screen = screen_new(vt);
  أرجع vt->screen;
}

فراغ vterm_screen_enable_reflow(VTermScreen *screen, صحيح reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

فراغ vterm_screen_enable_altscreen(VTermScreen *screen, صحيح altscreen)
{
  إذا(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    صحيح صفون_2, کانن_ڳڻپ;
    vterm_get_سائيز(screen->vt, &صفون_2, &کانن_ڳڻپ);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_بفر(screen, صفون_2, کانن_ڳڻپ);
  }
}

فراغ vterm_screen_سيٽ_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, فراغ *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

فراغ *vterm_screen_get_cbdata(VTermScreen *screen)
{
  أرجع screen->cbdata;
}

فراغ vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

فراغ vterm_screen_سيٽ_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  vterm_state_سيٽ_unrecognised_fallbacks(screen->state, fallbacks, user);
}

فراغ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  أرجع vterm_state_get_unrecognised_fbdata(screen->state);
}

فراغ vterm_screen_flush_damage(VTermScreen *screen)
{
  إذا(screen->pending_scrollrect.شروعات_صف != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.شروعات_صف = -1;
  }

  إذا(screen->damaged.شروعات_صف != -1) {
    إذا(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.شروعات_صف = -1;
  }
}

فراغ vterm_screen_سيٽ_damage_ملايو(VTermScreen *screen, VTerm_Damage_سائيز سائيز)
{
  vterm_screen_flush_damage(screen);
  screen->damage_ملايو = سائيز;
}

ساكن صحيح attrs_فرق(VTerm_Attr_نقاب attrs, ScreenCell *a, ScreenCell *b)
{
  إذا((attrs & VTERM_ATTR_BOLD_نقاب)       && (a->pen.bold != b->pen.bold))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_UNDERLINE_نقاب)  && (a->pen.underline != b->pen.underline))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_ITALIC_نقاب)     && (a->pen.italic != b->pen.italic))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BLINK_نقاب)      && (a->pen.blink != b->pen.blink))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_ابتو_نقاب)    && (a->pen.ابتو != b->pen.ابتو))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_CONCEAL_نقاب)    && (a->pen.conceal != b->pen.conceal))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_STRIKE_نقاب)     && (a->pen.strike != b->pen.strike))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_FONT_نقاب)       && (a->pen.font != b->pen.font))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_FOREGROUND_نقاب) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BACKGROUND_نقاب) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_SMALL_نقاب)    && (a->pen.small != b->pen.small))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BASELINE_نقاب)    && (a->pen.baseline != b->pen.baseline))
    أرجع 1;

  أرجع 0;
}

صحيح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_نقاب attrs)
{
  ScreenCell *هدف = getcell(screen, pos.صف, pos.کانو);

  // TODO: bounds check
  extent->شروعات_صف = pos.صف;
  extent->پڇاڙي_صف   = pos.صف + 1;

  إذا(extent->شروعات_کانو < 0)
    extent->شروعات_کانو = 0;
  إذا(extent->پڇاڙي_کانو < 0)
    extent->پڇاڙي_کانو = screen->کانن_ڳڻپ;

  صحيح کانو;

  لكل(کانو = pos.کانو - 1; کانو >= extent->شروعات_کانو; کانو--)
    إذا(attrs_فرق(attrs, هدف, getcell(screen, pos.صف, کانو)))
      اكسر;
  extent->شروعات_کانو = کانو + 1;

  لكل(کانو = pos.کانو + 1; کانو < extent->پڇاڙي_کانو; کانو++)
    إذا(attrs_فرق(attrs, هدف, getcell(screen, pos.صف, کانو)))
      اكسر;
  extent->پڇاڙي_کانو = کانو - 1;

  أرجع 1;
}

فراغ vterm_screen_convert_color_منزل_rgb(ثابت VTermScreen *screen, VTermColor *کانو)
{
  vterm_state_convert_color_منزل_rgb(screen->state, کانو);
}

ساكن فراغ reset_default_colours(VTermScreen *screen, ScreenCell *بفر)
{
  لكل(صحيح صف = 0; صف <= screen->صفون_2 - 1; صف++)
    لكل(صحيح کانو = 0; کانو <= screen->کانن_ڳڻپ - 1; کانو++) {
      ScreenCell *cell = &بفر[صف * screen->کانن_ڳڻپ + کانو];
      إذا(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      إذا(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

فراغ vterm_screen_سيٽ_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_سيٽ_default_colors(screen->state, default_fg, default_bg);

  إذا(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  إذا(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  إذا(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
