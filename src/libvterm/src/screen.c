#ضمن "vterm_internal.h"

// vim: set sw=2 :
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "rect.h"
#ضمن "utf8.h"

#تعریف_کن UNICODE_SPACE 0x20
#تعریف_کن UNICODE_LINEFEED 0x0a

#لغو_تعریف DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
تعريفنوع بنية
{
  /* After the bitfield */
  VTermColor   fg, bg;

  بیعلامت صحيح bold      : 1;
  بیعلامت صحيح underline : 2;
  بیعلامت صحيح italic    : 1;
  بیعلامت صحيح blink     : 1;
  بیعلامت صحيح معکوس   : 1;
  بیعلامت صحيح conceal   : 1;
  بیعلامت صحيح strike    : 1;
  بیعلامت صحيح font      : 4; /* 0 to 9 */
  بیعلامت صحيح small     : 1;
  بیعلامت صحيح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  بیعلامت صحيح protected_cell : 1;
  بیعلامت صحيح dwl            : 1; /* on a DECDWL or DECDHL line */
  بیعلامت صحيح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
تعريفنوع بنية
{
  uint32_t chars[VTERM_MAX_CHARS_در_هر_CELL];
  ScreenPen pen;
} ScreenCell;

بنية VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_pushline4;

  VTerm_Damage_اندازه damage_ادغام;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحيح pending_scroll_downward, pending_scroll_rightward;

  صحيح ردیفها_2;
  صحيح تعداد_ستون;

  بیعلامت صحيح global_معکوس : 1;
  بیعلامت صحيح reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *میانگیر;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_میانگیر;

  ScreenPen pen;
};

ساكن فراغ clearcell(ثابت VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ساكن ScreenCell *getcell(ثابت VTermScreen *screen, صحيح ردیف, صحيح ستون)
{
  إذا(ردیف < 0 || ردیف >= screen->ردیفها_2)
    أرجع عدم;
  إذا(ستون < 0 || ستون >= screen->تعداد_ستون)
    أرجع عدم;
  إذا (screen->میانگیر == عدم)
    أرجع عدم;
  أرجع screen->میانگیر + (screen->تعداد_ستون * ردیف) + ستون;
}

ساكن ScreenCell *alloc_میانگیر(VTermScreen *screen, صحيح ردیفها_2, صحيح تعداد_ستون)
{
  ScreenCell *new_میانگیر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * ردیفها_2 * تعداد_ستون);

  لكل(صحيح ردیف = 0; ردیف < ردیفها_2; ردیف++) {
    لكل(صحيح ستون = 0; ستون < تعداد_ستون; ستون++) {
      clearcell(screen, &new_میانگیر[ردیف * تعداد_ستون + ستون]);
    }
  }

  أرجع new_میانگیر;
}

ساكن فراغ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  بدل(screen->damage_ادغام) {
  حالة VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    اكسر;

  حالة VTERM_DAMAGE_ردیف:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    إذا(rect.انتها_ردیف > rect.شروع_ردیف + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وإلا إذا(screen->damaged.شروع_ردیف == -1) {
      // None stored yet
      screen->damaged = rect;
      أرجع;
    }
    وإلا إذا(rect.شروع_ردیف == screen->damaged.شروع_ردیف) {
      // Merge with the stored line
      إذا(screen->damaged.شروع_ستون > rect.شروع_ستون)
        screen->damaged.شروع_ستون = rect.شروع_ستون;
      إذا(screen->damaged.انتها_ستون < rect.انتها_ستون)
        screen->damaged.انتها_ستون = rect.انتها_ستون;
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
    إذا(screen->damaged.شروع_ردیف == -1)
      screen->damaged = rect;
    وإلا {
      rect_expand(&screen->damaged, &rect);
    }
    أرجع;

  افتراضي:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_ادغام);
    أرجع;
  }

  إذا(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ساكن فراغ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.انتها_ردیف = screen->ردیفها_2;
  rect.انتها_ستون = screen->تعداد_ستون;

  damagerect(screen, rect);
}

ساكن صحيح putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.ردیف, pos.ستون);

  إذا(!cell)
    أرجع 0;

  صحيح i;
  لكل(i = 0; i < VTERM_MAX_CHARS_در_هر_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  إذا(i < VTERM_MAX_CHARS_در_هر_CELL)
    cell->chars[i] = 0;

  لكل(صحيح ستون = 1; ستون < info->عرض; ستون++)
  {
    ScreenCell *onecell = getcell(screen, pos.ردیف, pos.ستون + ستون);
    إذا (onecell == عدم)
      اكسر;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.شروع_ردیف = pos.ردیف;
  rect.انتها_ردیف   = pos.ردیف+1;
  rect.شروع_ستون = pos.ستون;
  rect.انتها_ستون   = pos.ستون+info->عرض;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن فراغ sb_pushline_from_ردیف(VTermScreen *screen, صحيح ردیف, صحيح continuation)
{
  VTermPos pos;
  pos.ردیف = ردیف;
  لكل(pos.ستون = 0; pos.ستون < screen->تعداد_ستون; pos.ستون++)
    vterm_screen_get_cell(screen, pos, screen->sb_میانگیر + pos.ستون);

  إذا(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->تعداد_ستون, screen->sb_میانگیر, continuation, screen->cbdata);
  وإلا
    (screen->callbacks->sb_pushline)(screen->تعداد_ستون, screen->sb_میانگیر, screen->cbdata);
}

ساكن صحيح premove(VTermRect rect, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.شروع_ردیف == 0 && rect.شروع_ستون == 0 &&        // starts top-left corner
     rect.انتها_ستون == screen->تعداد_ستون &&                      // full width
     screen->میانگیر == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    لكل(صحيح ردیف = 0; ردیف < rect.انتها_ردیف; ردیف++) {
      ثابت VTerm_سطر_Info *lineinfo = vterm_state_get_lineinfo(screen->state, ردیف);
      sb_pushline_from_ردیف(screen, ردیف, lineinfo->continuation);
    }
  }

  أرجع 1;
}

ساكن صحيح moverect_داخلی(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح تعداد_ستون = src.انتها_ستون - src.شروع_ستون;
  صحيح downward = src.شروع_ردیف - dest.شروع_ردیف;

  صحيح init_ردیف, test_ردیف, inc_ردیف;
  إذا(downward < 0) {
    init_ردیف = dest.انتها_ردیف - 1;
    test_ردیف = dest.شروع_ردیف - 1;
    inc_ردیف  = -1;
  }
  وإلا {
    init_ردیف = dest.شروع_ردیف;
    test_ردیف = dest.انتها_ردیف;
    inc_ردیف  = +1;
  }

  لكل(صحيح ردیف = init_ردیف; ردیف != test_ردیف; ردیف += inc_ردیف)
    memmove(getcell(screen, ردیف, dest.شروع_ستون),
            getcell(screen, ردیف + downward, src.شروع_ستون),
            تعداد_ستون * حجم(ScreenCell));

  أرجع 1;
}

ساكن صحيح moverect_user(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->moverect) {
    إذا(screen->damage_ادغام != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    إذا((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      أرجع 1;
  }

  damagerect(screen, dest);

  أرجع 1;
}

ساكن صحيح erase_داخلی(VTermRect rect, صحيح selective, فراغ *user)
{
  VTermScreen *screen = user;

  لكل(صحيح ردیف = rect.شروع_ردیف; ردیف < screen->state->ردیفها_2 && ردیف < rect.انتها_ردیف; ردیف++) {
    ثابت VTerm_سطر_Info *info = vterm_state_get_lineinfo(screen->state, ردیف);

    لكل(صحيح ستون = rect.شروع_ستون; ستون < rect.انتها_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, ردیف, ستون);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     ردیف, ستون);
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

ساكن صحيح erase_user(VTermRect rect, صحيح selective استفادهنشده, فراغ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن صحيح erase(VTermRect rect, صحيح selective, فراغ *user)
{
  erase_داخلی(rect, selective, user);
  أرجع erase_user(rect, 0, user);
}

ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->damage_ادغام != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_داخلی, erase_داخلی, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    أرجع 1;
  }

  إذا(screen->damaged.شروع_ردیف != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  إذا(screen->pending_scrollrect.شروع_ردیف == -1) {
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
      moverect_داخلی, erase_داخلی, screen);

  إذا(screen->damaged.شروع_ردیف == -1)
    أرجع 1;

  إذا(rect_شامل_است(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وإلا إذا(rect.شروع_ستون <= screen->damaged.شروع_ستون &&
          rect.انتها_ستون   >= screen->damaged.انتها_ستون &&
          rightward == 0) {
    إذا(screen->damaged.شروع_ردیف >= rect.شروع_ردیف &&
       screen->damaged.شروع_ردیف  < rect.انتها_ردیف) {
      screen->damaged.شروع_ردیف -= downward;
      إذا(screen->damaged.شروع_ردیف < rect.شروع_ردیف)
        screen->damaged.شروع_ردیف = rect.شروع_ردیف;
      إذا(screen->damaged.شروع_ردیف > rect.انتها_ردیف)
        screen->damaged.شروع_ردیف = rect.انتها_ردیف;
    }
    إذا(screen->damaged.انتها_ردیف >= rect.شروع_ردیف &&
       screen->damaged.انتها_ردیف  < rect.انتها_ردیف) {
      screen->damaged.انتها_ردیف -= downward;
      إذا(screen->damaged.انتها_ردیف < rect.شروع_ردیف)
        screen->damaged.انتها_ردیف = rect.شروع_ردیف;
      إذا(screen->damaged.انتها_ردیف > rect.انتها_ردیف)
        screen->damaged.انتها_ردیف = rect.انتها_ردیف;
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

ساكن صحيح setpenattr(VTermAttr attr, VTerm_مقدار *val, فراغ *user)
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
  حالة VTERM_ATTR_معکوس:
    screen->pen.معکوس = val->boolean;
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

ساكن صحيح settermprop(VTermProp prop, VTerm_مقدار *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(prop) {
  حالة VTERM_PROP_ALTSCREEN:
    إذا(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      أرجع 0;

    screen->میانگیر = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    إذا(!val->boolean)
      damagescreen(screen);
    اكسر;
  حالة VTERM_PROP_معکوس:
    screen->global_معکوس = val->boolean;
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
ساكن صحيح سطر_popcount(ScreenCell *میانگیر, صحيح ردیف, صحيح ردیفها_2 استفادهنشده, صحيح تعداد_ستون)
{
  صحيح ستون = تعداد_ستون - 1;
  طالما(ستون >= 0 && میانگیر[ردیف * تعداد_ستون + ستون].chars[0] == 0)
    ستون--;
  أرجع ستون + 1;
}

#تعریف_کن REFLOW (screen->reflow)

ساكن فراغ resize_میانگیر(VTermScreen *screen, صحيح bufidx, صحيح new_ردیفها, صحيح new_تعداد_ستون, صحيح فعال, VTermStateFields *statefields)
{
  صحيح old_ردیفها = screen->ردیفها_2;
  صحيح old_تعداد_ستون = screen->تعداد_ستون;

  ScreenCell *old_میانگیر = screen->buffers[bufidx];
  VTerm_سطر_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_میانگیر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_ردیفها * new_تعداد_ستون);
  VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_ردیفها);

  // Find the final row of old buffer content
  صحيح old_ردیف = old_ردیفها - 1;
  صحيح new_ردیف = new_ردیفها - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#اگر_تعریف_شده DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_تعداد_ستون, old_ردیفها, new_تعداد_ستون, new_ردیفها, old_cursor.ستون, old_cursor.ردیف);
#پایان_اگر

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحيح final_blank_ردیف = new_ردیفها;

  طالما(old_ردیف >= 0) {
    صحيح old_ردیف_انتها = old_ردیف;
    /* TODO: Stop if dwl or dhl */
    طالما(REFLOW && old_lineinfo && old_ردیف >= 0 && old_lineinfo[old_ردیف].continuation)
      old_ردیف--;
    صحيح old_ردیف_شروع = old_ردیف;

    صحيح عرض = 0;
    لكل(صحيح ردیف = old_ردیف_شروع; ردیف <= old_ردیف_انتها; ردیف++) {
      إذا(REFLOW && ردیف < (old_ردیفها - 1) && old_lineinfo[ردیف + 1].continuation)
        عرض += old_تعداد_ستون;
      وإلا
        عرض += سطر_popcount(old_میانگیر, ردیف, old_ردیفها, old_تعداد_ستون);
    }

    إذا(final_blank_ردیف == (new_ردیف + 1) && عرض == 0)
      final_blank_ردیف = new_ردیف;

    صحيح new_ارتفاع = REFLOW
      ? عرض ? (عرض + new_تعداد_ستون - 1) / new_تعداد_ستون : 1
      : 1;

    صحيح new_ردیف_انتها = new_ردیف;
    صحيح new_ردیف_شروع = new_ردیف - new_ارتفاع + 1;

    old_ردیف = old_ردیف_شروع;
    صحيح old_ستون = 0;

    صحيح یدکی_ردیفها = new_ردیفها - final_blank_ردیف;

    إذا(new_ردیف_شروع < 0 && /* we'd fall off the top */
        یدکی_ردیفها >= 0 && /* we actually have spare rows */
        (!فعال || new_cursor.ردیف == -1 || (new_cursor.ردیف - new_ردیف_شروع) < new_ردیفها))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحيح downwards = -new_ردیف_شروع;
      إذا(downwards > یدکی_ردیفها)
        downwards = یدکی_ردیفها;
      صحيح rowcount = new_ردیفها - downwards;

#اگر_تعریف_شده DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#پایان_اگر

      memmove(&new_میانگیر[downwards * new_تعداد_ستون], &new_میانگیر[0],   rowcount * new_تعداد_ستون * حجم(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * حجم(new_lineinfo[0]));

      new_ردیف += downwards;
      new_ردیف_شروع += downwards;
      new_ردیف_انتها += downwards;

      إذا(new_cursor.ردیف >= 0)
        new_cursor.ردیف += downwards;

      final_blank_ردیف += downwards;
    }

#اگر_تعریف_شده DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_ردیف_شروع, new_ردیف_انتها, old_ردیف_شروع, old_ردیف_انتها, عرض);
#پایان_اگر

    إذا(new_ردیف_شروع < 0) {
      إذا(old_ردیف_شروع <= old_cursor.ردیف && old_cursor.ردیف < old_ردیف_انتها) {
        new_cursor.ردیف = 0;
        new_cursor.ستون = old_cursor.ستون;
        إذا(new_cursor.ستون >= new_تعداد_ستون)
          new_cursor.ستون = new_تعداد_ستون-1;
      }
      اكسر;
    }

    لكل(new_ردیف = new_ردیف_شروع, old_ردیف = old_ردیف_شروع; new_ردیف <= new_ردیف_انتها; new_ردیف++) {
      صحيح شمار = عرض >= new_تعداد_ستون ? new_تعداد_ستون : عرض;
      عرض -= شمار;

      صحيح new_ستون = 0;

      طالما(شمار) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_میانگیر[new_ردیف * new_تعداد_ستون + new_ستون] = old_میانگیر[old_ردیف * old_تعداد_ستون + old_ستون];

        إذا(old_cursor.ردیف == old_ردیف && old_cursor.ستون == old_ستون)
          new_cursor.ردیف = new_ردیف, new_cursor.ستون = new_ستون;

        old_ستون++;
        إذا(old_ستون == old_تعداد_ستون) {
          old_ردیف++;

          إذا(!REFLOW) {
            new_ستون++;
            اكسر;
          }
          old_ستون = 0;
        }

        new_ستون++;
        شمار--;
      }

      إذا(old_cursor.ردیف == old_ردیف && old_cursor.ستون >= old_ستون) {
        new_cursor.ردیف = new_ردیف, new_cursor.ستون = (old_cursor.ستون - old_ستون + new_ستون);
        إذا(new_cursor.ستون >= new_تعداد_ستون)
          new_cursor.ستون = new_تعداد_ستون-1;
      }

      طالما(new_ستون < new_تعداد_ستون) {
        clearcell(screen, &new_میانگیر[new_ردیف * new_تعداد_ستون + new_ستون]);
        new_ستون++;
      }

      new_lineinfo[new_ردیف].continuation = (new_ردیف > new_ردیف_شروع);
    }

    old_ردیف = old_ردیف_شروع - 1;
    new_ردیف = new_ردیف_شروع - 1;
  }

  إذا(old_cursor.ردیف <= old_ردیف) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.ردیف = 0, new_cursor.ستون = old_cursor.ستون;
    إذا(new_cursor.ستون >= new_تعداد_ستون)
      new_cursor.ستون = new_تعداد_ستون-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  إذا(فعال && (new_cursor.ردیف == -1 || new_cursor.ستون == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    إذا (new_cursor.ردیف < 0)
      new_cursor.ردیف = 0;
    إذا (new_cursor.ستون < 0)
      new_cursor.ستون = 0;
  }

  إذا(old_ردیف >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    إذا((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      لكل(صحيح ردیف = 0; ردیف <= old_ردیف; ردیف++) {
        ثابت VTerm_سطر_Info *lineinfo = old_lineinfo + ردیف;
        sb_pushline_from_ردیف(screen, ردیف, lineinfo->continuation);
      }
    إذا(فعال)
      statefields->pos.ردیف -= (old_ردیف + 1);
  }
  إذا(new_ردیف >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    طالما(new_ردیف >= 0) {
      VTermPos pos;
      إذا(!(screen->callbacks->sb_popline(old_تعداد_ستون, screen->sb_میانگیر, screen->cbdata)))
        اكسر;

      pos.ردیف = new_ردیف;
      لكل(pos.ستون = 0; pos.ستون < old_تعداد_ستون && pos.ستون < new_تعداد_ستون; pos.ستون += screen->sb_میانگیر[pos.ستون].عرض) {
        VTermScreenCell *src = &screen->sb_میانگیر[pos.ستون];
        ScreenCell *dst = &new_میانگیر[pos.ردیف * new_تعداد_ستون + pos.ستون];

        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_در_هر_CELL; i++) {
          dst->chars[i] = src->chars[i];
          إذا(!src->chars[i])
            اكسر;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.معکوس   = src->attrs.معکوس ^ screen->global_معکوس;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        إذا(src->عرض == 2 && pos.ستون < (new_تعداد_ستون-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      لكل( ; pos.ستون < new_تعداد_ستون; pos.ستون++)
        clearcell(screen, &new_میانگیر[pos.ردیف * new_تعداد_ستون + pos.ستون]);
      new_ردیف--;

      إذا(فعال)
        statefields->pos.ردیف++;
    }
  }
  إذا(new_ردیف >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحيح moverows = new_ردیفها - new_ردیف - 1;
    memmove(&new_میانگیر[0], &new_میانگیر[(new_ردیف + 1) * new_تعداد_ستون], moverows * new_تعداد_ستون * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_ردیف + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.ردیف -= (new_ردیف + 1);

    لكل(new_ردیف = moverows; new_ردیف < new_ردیفها; new_ردیف++) {
      لكل(صحيح ستون = 0; ستون < new_تعداد_ستون; ستون++)
        clearcell(screen, &new_میانگیر[new_ردیف * new_تعداد_ستون + ستون]);
      new_lineinfo[new_ردیف] = (VTerm_سطر_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_میانگیر);
  screen->buffers[bufidx] = new_میانگیر;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  إذا(فعال)
    statefields->pos = new_cursor;

  أرجع;
}

ساكن صحيح resize(صحيح new_ردیفها, صحيح new_تعداد_ستون, VTermStateFields *fields, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح altscreen_فعال = (screen->buffers[BUFIDX_ALTSCREEN] && screen->میانگیر == screen->buffers[BUFIDX_ALTSCREEN]);

  صحيح old_ردیفها = screen->ردیفها_2;
  صحيح old_تعداد_ستون = screen->تعداد_ستون;

  إذا(new_تعداد_ستون > old_تعداد_ستون) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    إذا(screen->sb_میانگیر)
      vterm_allocator_free(screen->vt, screen->sb_میانگیر);

    إذا (new_تعداد_ستون > VTERM_MAX_تعداد_ستون)
      new_تعداد_ستون = VTERM_MAX_تعداد_ستون;

    screen->sb_میانگیر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_تعداد_ستون);
  }

  إذا (new_ردیفها > VTERM_MAX_ردیفها)
    new_ردیفها = VTERM_MAX_ردیفها;

  resize_میانگیر(screen, 0, new_ردیفها, new_تعداد_ستون, !altscreen_فعال, fields);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    resize_میانگیر(screen, 1, new_ردیفها, new_تعداد_ستون, altscreen_فعال, fields);
  وإلا إذا(new_ردیفها != old_ردیفها) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_ردیفها);
    لكل(صحيح ردیف = 0; ردیف < new_ردیفها; ردیف++)
      new_lineinfo[ردیف] = (VTerm_سطر_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->میانگیر = altscreen_فعال ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->ردیفها_2 = new_ردیفها;
  screen->تعداد_ستون = new_تعداد_ستون;

  إذا(new_تعداد_ستون <= old_تعداد_ستون) {
    إذا(screen->sb_میانگیر)
      vterm_allocator_free(screen->vt, screen->sb_میانگیر);

    screen->sb_میانگیر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_تعداد_ستون);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  إذا(screen->callbacks && screen->callbacks->resize)
    أرجع (*screen->callbacks->resize)(new_ردیفها, new_تعداد_ستون, screen->cbdata);

  أرجع 1;
}

ساكن صحيح setlineinfo(صحيح ردیف, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    لكل(صحيح ستون = 0; ستون < screen->تعداد_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, ردیف, ستون);
      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     ردیف, ستون);
	أرجع 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.شروع_ردیف = ردیف;
    rect.انتها_ردیف   = ردیف + 1;
    rect.شروع_ستون = 0;
    rect.انتها_ستون   = newinfo->doublewidth ? screen->تعداد_ستون / 2 : screen->تعداد_ستون;
    damagerect(screen, rect);

    إذا(newinfo->doublewidth) {
      rect.شروع_ستون = screen->تعداد_ستون / 2;
      rect.انتها_ستون   = screen->تعداد_ستون;

      erase_داخلی(rect, 0, user);
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
  صحيح ردیفها_2, تعداد_ستون;

  vterm_get_اندازه(vt, &ردیفها_2, &تعداد_ستون);

  screen->vt = vt;
  screen->state = state;

  screen->damage_ادغام = VTERM_DAMAGE_CELL;
  screen->damaged.شروع_ردیف = -1;
  screen->pending_scrollrect.شروع_ردیف = -1;

  screen->ردیفها_2 = ردیفها_2;
  screen->تعداد_ستون = تعداد_ستون;

  screen->global_معکوس = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = عدم;
  screen->cbdata    = عدم;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_میانگیر(screen, ردیفها_2, تعداد_ستون);

  screen->میانگیر = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_میانگیر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * تعداد_ستون);
  إذا (screen->میانگیر == عدم || screen->sb_میانگیر == عدم)
  {
    vterm_screen_free(screen);
    أرجع عدم;
  }

  vterm_state_مجموعه_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  أرجع screen;
}

داخلی فراغ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_میانگیر);

  vterm_allocator_free(screen->vt, screen);
}

فراغ vterm_screen_reset(VTermScreen *screen, صحيح hard)
{
  screen->damaged.شروع_ردیف = -1;
  screen->pending_scrollrect.شروع_ردیف = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ساكن size_t _get_chars(ثابت VTermScreen *screen, ثابت صحيح utf8, فراغ *میانگیر, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحيح padding = 0;

#تعریف_کن PUT(c)                                             \
  إذا(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    إذا(میانگیر && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (حرف *)میانگیر + outpos);   \
    وإلا                                                   \
      outpos += thislen;                                   \
  }                                                        \
  وإلا {                                                   \
    إذا(میانگیر && outpos + 1 <= len)                        \
      ((uint32_t*)میانگیر)[outpos++] = (c);                 \
    وإلا                                                   \
      outpos++;                                            \
  }

  لكل(صحيح ردیف = rect.شروع_ردیف; ردیف < rect.انتها_ردیف; ردیف++) {
    لكل(صحيح ستون = rect.شروع_ستون; ستون < rect.انتها_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, ردیف, ستون);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     ردیف, ستون);
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
        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_در_هر_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    إذا(ردیف < rect.انتها_ردیف - 1) {
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
  ScreenCell *intcell = getcell(screen, pos.ردیف, pos.ستون);

  إذا(!intcell)
    أرجع 0;

  لكل(صحيح i = 0; i < VTERM_MAX_CHARS_در_هر_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    إذا(!intcell->chars[i])
      اكسر;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.معکوس   = intcell->pen.معکوس ^ screen->global_معکوس;
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
    إذا(pos.ستون < (screen->تعداد_ستون - 1) &&
       getcell(screen, pos.ردیف, pos.ستون + 1)->chars[0] == (uint32_t)-1) {
      إذا(getcell(screen, pos.ردیف, pos.ستون)->chars[0] == 0x20) {
        getcell(screen, pos.ردیف, pos.ستون)->chars[0] = 0;
        cell->عرض = 2;
      } وإلا إذا(getcell(screen, pos.ردیف, pos.ستون)->chars[0] == 0) {
        getcell(screen, pos.ردیف, pos.ستون + 1)->chars[0] = 0;
        cell->عرض = 1;
      } وإلا {
        cell->عرض = 2;
      }
    } وإلا
      cell->عرض = 1;
  } وإلا {
    إذا(pos.ستون < (screen->تعداد_ستون - 1) &&
       getcell(screen, pos.ردیف, pos.ستون + 1)->chars[0] == (uint32_t)-1)
      cell->عرض = 2;
    وإلا
      cell->عرض = 1;
  }

  أرجع 1;
}

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  لكل(; pos.ستون < screen->تعداد_ستون; pos.ستون++) {
    ScreenCell *cell = getcell(screen, pos.ردیف, pos.ستون);
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
    صحيح ردیفها_2, تعداد_ستون;
    vterm_get_اندازه(screen->vt, &ردیفها_2, &تعداد_ستون);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_میانگیر(screen, ردیفها_2, تعداد_ستون);
  }
}

فراغ vterm_screen_مجموعه_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, فراغ *user)
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

فراغ vterm_screen_مجموعه_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  vterm_state_مجموعه_unrecognised_fallbacks(screen->state, fallbacks, user);
}

فراغ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  أرجع vterm_state_get_unrecognised_fbdata(screen->state);
}

فراغ vterm_screen_flush_damage(VTermScreen *screen)
{
  إذا(screen->pending_scrollrect.شروع_ردیف != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.شروع_ردیف = -1;
  }

  إذا(screen->damaged.شروع_ردیف != -1) {
    إذا(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.شروع_ردیف = -1;
  }
}

فراغ vterm_screen_مجموعه_damage_ادغام(VTermScreen *screen, VTerm_Damage_اندازه اندازه)
{
  vterm_screen_flush_damage(screen);
  screen->damage_ادغام = اندازه;
}

ساكن صحيح attrs_تفاوت(VTerm_Attr_ماسک attrs, ScreenCell *a, ScreenCell *b)
{
  إذا((attrs & VTERM_ATTR_BOLD_ماسک)       && (a->pen.bold != b->pen.bold))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_UNDERLINE_ماسک)  && (a->pen.underline != b->pen.underline))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_ITALIC_ماسک)     && (a->pen.italic != b->pen.italic))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BLINK_ماسک)      && (a->pen.blink != b->pen.blink))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_معکوس_ماسک)    && (a->pen.معکوس != b->pen.معکوس))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_CONCEAL_ماسک)    && (a->pen.conceal != b->pen.conceal))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_STRIKE_ماسک)     && (a->pen.strike != b->pen.strike))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_FONT_ماسک)       && (a->pen.font != b->pen.font))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_FOREGROUND_ماسک) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BACKGROUND_ماسک) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_SMALL_ماسک)    && (a->pen.small != b->pen.small))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BASELINE_ماسک)    && (a->pen.baseline != b->pen.baseline))
    أرجع 1;

  أرجع 0;
}

صحيح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ماسک attrs)
{
  ScreenCell *هدف = getcell(screen, pos.ردیف, pos.ستون);

  // TODO: bounds check
  extent->شروع_ردیف = pos.ردیف;
  extent->انتها_ردیف   = pos.ردیف + 1;

  إذا(extent->شروع_ستون < 0)
    extent->شروع_ستون = 0;
  إذا(extent->انتها_ستون < 0)
    extent->انتها_ستون = screen->تعداد_ستون;

  صحيح ستون;

  لكل(ستون = pos.ستون - 1; ستون >= extent->شروع_ستون; ستون--)
    إذا(attrs_تفاوت(attrs, هدف, getcell(screen, pos.ردیف, ستون)))
      اكسر;
  extent->شروع_ستون = ستون + 1;

  لكل(ستون = pos.ستون + 1; ستون < extent->انتها_ستون; ستون++)
    إذا(attrs_تفاوت(attrs, هدف, getcell(screen, pos.ردیف, ستون)))
      اكسر;
  extent->انتها_ستون = ستون - 1;

  أرجع 1;
}

فراغ vterm_screen_convert_color_مقصد_rgb(ثابت VTermScreen *screen, VTermColor *ستون)
{
  vterm_state_convert_color_مقصد_rgb(screen->state, ستون);
}

ساكن فراغ reset_default_colours(VTermScreen *screen, ScreenCell *میانگیر)
{
  لكل(صحيح ردیف = 0; ردیف <= screen->ردیفها_2 - 1; ردیف++)
    لكل(صحيح ستون = 0; ستون <= screen->تعداد_ستون - 1; ستون++) {
      ScreenCell *cell = &میانگیر[ردیف * screen->تعداد_ستون + ستون];
      إذا(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      إذا(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

فراغ vterm_screen_مجموعه_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_مجموعه_default_colors(screen->state, default_fg, default_bg);

  إذا(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_ماسک)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  إذا(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_ماسک)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  إذا(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
