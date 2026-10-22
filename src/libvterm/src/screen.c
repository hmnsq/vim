#ضمن "vterm_internal.h"

// vim: set sw=2 :
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "rect.h"
#ضمن "utf8.h"

#تعریف_کړه UNICODE_SPACE 0x20
#تعریف_کړه UNICODE_LINEFEED 0x0a

#تعریف_لغوه DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
تعريفنوع بنية
{
  /* After the bitfield */
  VTermColor   fg, bg;

  بینښه صحيح bold      : 1;
  بینښه صحيح underline : 2;
  بینښه صحيح italic    : 1;
  بینښه صحيح blink     : 1;
  بینښه صحيح سرچپه   : 1;
  بینښه صحيح conceal   : 1;
  بینښه صحيح strike    : 1;
  بینښه صحيح font      : 4; /* 0 to 9 */
  بینښه صحيح small     : 1;
  بینښه صحيح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  بینښه صحيح protected_cell : 1;
  بینښه صحيح dwl            : 1; /* on a DECDWL or DECDHL line */
  بینښه صحيح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
تعريفنوع بنية
{
  uint32_t chars[VTERM_MAX_CHARS_فی_CELL];
  ScreenPen pen;
} ScreenCell;

بنية VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_pushline4;

  VTerm_Damage_اندازه damage_یوځای;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحيح pending_scroll_downward, pending_scroll_rightward;

  صحيح لیکې_2;
  صحيح ستون_شمېر;

  بینښه صحيح global_سرچپه : 1;
  بینښه صحيح reflow : 1;

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

ساكن ScreenCell *getcell(ثابت VTermScreen *screen, صحيح لیکه, صحيح ستون)
{
  إذا(لیکه < 0 || لیکه >= screen->لیکې_2)
    أرجع عدم;
  إذا(ستون < 0 || ستون >= screen->ستون_شمېر)
    أرجع عدم;
  إذا (screen->بفر == عدم)
    أرجع عدم;
  أرجع screen->بفر + (screen->ستون_شمېر * لیکه) + ستون;
}

ساكن ScreenCell *alloc_بفر(VTermScreen *screen, صحيح لیکې_2, صحيح ستون_شمېر)
{
  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * لیکې_2 * ستون_شمېر);

  لكل(صحيح لیکه = 0; لیکه < لیکې_2; لیکه++) {
    لكل(صحيح ستون = 0; ستون < ستون_شمېر; ستون++) {
      clearcell(screen, &new_بفر[لیکه * ستون_شمېر + ستون]);
    }
  }

  أرجع new_بفر;
}

ساكن فراغ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  بدل(screen->damage_یوځای) {
  حالة VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    اكسر;

  حالة VTERM_DAMAGE_لیکه:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    إذا(rect.پای_ټکی_لیکه > rect.پیل_لیکه + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وإلا إذا(screen->damaged.پیل_لیکه == -1) {
      // None stored yet
      screen->damaged = rect;
      أرجع;
    }
    وإلا إذا(rect.پیل_لیکه == screen->damaged.پیل_لیکه) {
      // Merge with the stored line
      إذا(screen->damaged.پیل_ستون > rect.پیل_ستون)
        screen->damaged.پیل_ستون = rect.پیل_ستون;
      إذا(screen->damaged.پای_ټکی_ستون < rect.پای_ټکی_ستون)
        screen->damaged.پای_ټکی_ستون = rect.پای_ټکی_ستون;
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
    إذا(screen->damaged.پیل_لیکه == -1)
      screen->damaged = rect;
    وإلا {
      rect_expand(&screen->damaged, &rect);
    }
    أرجع;

  افتراضي:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_یوځای);
    أرجع;
  }

  إذا(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ساكن فراغ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.پای_ټکی_لیکه = screen->لیکې_2;
  rect.پای_ټکی_ستون = screen->ستون_شمېر;

  damagerect(screen, rect);
}

ساكن صحيح putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.لیکه, pos.ستون);

  إذا(!cell)
    أرجع 0;

  صحيح i;
  لكل(i = 0; i < VTERM_MAX_CHARS_فی_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  إذا(i < VTERM_MAX_CHARS_فی_CELL)
    cell->chars[i] = 0;

  لكل(صحيح ستون = 1; ستون < info->پلنوالی; ستون++)
  {
    ScreenCell *onecell = getcell(screen, pos.لیکه, pos.ستون + ستون);
    إذا (onecell == عدم)
      اكسر;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.پیل_لیکه = pos.لیکه;
  rect.پای_ټکی_لیکه   = pos.لیکه+1;
  rect.پیل_ستون = pos.ستون;
  rect.پای_ټکی_ستون   = pos.ستون+info->پلنوالی;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن فراغ sb_pushline_from_لیکه(VTermScreen *screen, صحيح لیکه, صحيح continuation)
{
  VTermPos pos;
  pos.لیکه = لیکه;
  لكل(pos.ستون = 0; pos.ستون < screen->ستون_شمېر; pos.ستون++)
    vterm_screen_get_cell(screen, pos, screen->sb_بفر + pos.ستون);

  إذا(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->ستون_شمېر, screen->sb_بفر, continuation, screen->cbdata);
  وإلا
    (screen->callbacks->sb_pushline)(screen->ستون_شمېر, screen->sb_بفر, screen->cbdata);
}

ساكن صحيح premove(VTermRect rect, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.پیل_لیکه == 0 && rect.پیل_ستون == 0 &&        // starts top-left corner
     rect.پای_ټکی_ستون == screen->ستون_شمېر &&                      // full width
     screen->بفر == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    لكل(صحيح لیکه = 0; لیکه < rect.پای_ټکی_لیکه; لیکه++) {
      ثابت VTerm_کرښه_Info *lineinfo = vterm_state_get_lineinfo(screen->state, لیکه);
      sb_pushline_from_لیکه(screen, لیکه, lineinfo->continuation);
    }
  }

  أرجع 1;
}

ساكن صحيح moverect_دننی(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح ستون_شمېر = src.پای_ټکی_ستون - src.پیل_ستون;
  صحيح downward = src.پیل_لیکه - dest.پیل_لیکه;

  صحيح init_لیکه, test_لیکه, inc_لیکه;
  إذا(downward < 0) {
    init_لیکه = dest.پای_ټکی_لیکه - 1;
    test_لیکه = dest.پیل_لیکه - 1;
    inc_لیکه  = -1;
  }
  وإلا {
    init_لیکه = dest.پیل_لیکه;
    test_لیکه = dest.پای_ټکی_لیکه;
    inc_لیکه  = +1;
  }

  لكل(صحيح لیکه = init_لیکه; لیکه != test_لیکه; لیکه += inc_لیکه)
    memmove(getcell(screen, لیکه, dest.پیل_ستون),
            getcell(screen, لیکه + downward, src.پیل_ستون),
            ستون_شمېر * حجم(ScreenCell));

  أرجع 1;
}

ساكن صحيح moverect_user(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->moverect) {
    إذا(screen->damage_یوځای != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    إذا((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      أرجع 1;
  }

  damagerect(screen, dest);

  أرجع 1;
}

ساكن صحيح erase_دننی(VTermRect rect, صحيح selective, فراغ *user)
{
  VTermScreen *screen = user;

  لكل(صحيح لیکه = rect.پیل_لیکه; لیکه < screen->state->لیکې_2 && لیکه < rect.پای_ټکی_لیکه; لیکه++) {
    ثابت VTerm_کرښه_Info *info = vterm_state_get_lineinfo(screen->state, لیکه);

    لكل(صحيح ستون = rect.پیل_ستون; ستون < rect.پای_ټکی_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, لیکه, ستون);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     لیکه, ستون);
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

ساكن صحيح erase_user(VTermRect rect, صحيح selective ناکارول_شوی, فراغ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن صحيح erase(VTermRect rect, صحيح selective, فراغ *user)
{
  erase_دننی(rect, selective, user);
  أرجع erase_user(rect, 0, user);
}

ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->damage_یوځای != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_دننی, erase_دننی, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    أرجع 1;
  }

  إذا(screen->damaged.پیل_لیکه != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  إذا(screen->pending_scrollrect.پیل_لیکه == -1) {
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
      moverect_دننی, erase_دننی, screen);

  إذا(screen->damaged.پیل_لیکه == -1)
    أرجع 1;

  إذا(rect_لري(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وإلا إذا(rect.پیل_ستون <= screen->damaged.پیل_ستون &&
          rect.پای_ټکی_ستون   >= screen->damaged.پای_ټکی_ستون &&
          rightward == 0) {
    إذا(screen->damaged.پیل_لیکه >= rect.پیل_لیکه &&
       screen->damaged.پیل_لیکه  < rect.پای_ټکی_لیکه) {
      screen->damaged.پیل_لیکه -= downward;
      إذا(screen->damaged.پیل_لیکه < rect.پیل_لیکه)
        screen->damaged.پیل_لیکه = rect.پیل_لیکه;
      إذا(screen->damaged.پیل_لیکه > rect.پای_ټکی_لیکه)
        screen->damaged.پیل_لیکه = rect.پای_ټکی_لیکه;
    }
    إذا(screen->damaged.پای_ټکی_لیکه >= rect.پیل_لیکه &&
       screen->damaged.پای_ټکی_لیکه  < rect.پای_ټکی_لیکه) {
      screen->damaged.پای_ټکی_لیکه -= downward;
      إذا(screen->damaged.پای_ټکی_لیکه < rect.پیل_لیکه)
        screen->damaged.پای_ټکی_لیکه = rect.پیل_لیکه;
      إذا(screen->damaged.پای_ټکی_لیکه > rect.پای_ټکی_لیکه)
        screen->damaged.پای_ټکی_لیکه = rect.پای_ټکی_لیکه;
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

ساكن صحيح setpenattr(VTermAttr attr, VTerm_ارزښت *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->شمېره;
    أرجع 1;
  حالة VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_سرچپه:
    screen->pen.سرچپه = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_FONT:
    screen->pen.font = val->شمېره;
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
    screen->pen.baseline = val->شمېره;
    أرجع 1;

  حالة VTERM_N_ATTRS:
    أرجع 0;
  }

  أرجع 0;
}

ساكن صحيح settermprop(VTermProp prop, VTerm_ارزښت *val, فراغ *user)
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
  حالة VTERM_PROP_سرچپه:
    screen->global_سرچپه = val->boolean;
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
ساكن صحيح کرښه_popcount(ScreenCell *بفر, صحيح لیکه, صحيح لیکې_2 ناکارول_شوی, صحيح ستون_شمېر)
{
  صحيح ستون = ستون_شمېر - 1;
  طالما(ستون >= 0 && بفر[لیکه * ستون_شمېر + ستون].chars[0] == 0)
    ستون--;
  أرجع ستون + 1;
}

#تعریف_کړه REFLOW (screen->reflow)

ساكن فراغ resize_بفر(VTermScreen *screen, صحيح bufidx, صحيح new_لیکې, صحيح new_ستون_شمېر, صحيح فعال, VTermStateFields *statefields)
{
  صحيح old_لیکې = screen->لیکې_2;
  صحيح old_ستون_شمېر = screen->ستون_شمېر;

  ScreenCell *old_بفر = screen->buffers[bufidx];
  VTerm_کرښه_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_لیکې * new_ستون_شمېر);
  VTerm_کرښه_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_لیکې);

  // Find the final row of old buffer content
  صحيح old_لیکه = old_لیکې - 1;
  صحيح new_لیکه = new_لیکې - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#که_تعریف_شوی DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_ستون_شمېر, old_لیکې, new_ستون_شمېر, new_لیکې, old_cursor.ستون, old_cursor.لیکه);
#د_که_پای

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحيح final_blank_لیکه = new_لیکې;

  طالما(old_لیکه >= 0) {
    صحيح old_لیکه_پای_ټکی = old_لیکه;
    /* TODO: Stop if dwl or dhl */
    طالما(REFLOW && old_lineinfo && old_لیکه >= 0 && old_lineinfo[old_لیکه].continuation)
      old_لیکه--;
    صحيح old_لیکه_پیل = old_لیکه;

    صحيح پلنوالی = 0;
    لكل(صحيح لیکه = old_لیکه_پیل; لیکه <= old_لیکه_پای_ټکی; لیکه++) {
      إذا(REFLOW && لیکه < (old_لیکې - 1) && old_lineinfo[لیکه + 1].continuation)
        پلنوالی += old_ستون_شمېر;
      وإلا
        پلنوالی += کرښه_popcount(old_بفر, لیکه, old_لیکې, old_ستون_شمېر);
    }

    إذا(final_blank_لیکه == (new_لیکه + 1) && پلنوالی == 0)
      final_blank_لیکه = new_لیکه;

    صحيح new_لوړوالی = REFLOW
      ? پلنوالی ? (پلنوالی + new_ستون_شمېر - 1) / new_ستون_شمېر : 1
      : 1;

    صحيح new_لیکه_پای_ټکی = new_لیکه;
    صحيح new_لیکه_پیل = new_لیکه - new_لوړوالی + 1;

    old_لیکه = old_لیکه_پیل;
    صحيح old_ستون = 0;

    صحيح اضافي_لیکې = new_لیکې - final_blank_لیکه;

    إذا(new_لیکه_پیل < 0 && /* we'd fall off the top */
        اضافي_لیکې >= 0 && /* we actually have spare rows */
        (!فعال || new_cursor.لیکه == -1 || (new_cursor.لیکه - new_لیکه_پیل) < new_لیکې))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحيح downwards = -new_لیکه_پیل;
      إذا(downwards > اضافي_لیکې)
        downwards = اضافي_لیکې;
      صحيح rowcount = new_لیکې - downwards;

#که_تعریف_شوی DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#د_که_پای

      memmove(&new_بفر[downwards * new_ستون_شمېر], &new_بفر[0],   rowcount * new_ستون_شمېر * حجم(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * حجم(new_lineinfo[0]));

      new_لیکه += downwards;
      new_لیکه_پیل += downwards;
      new_لیکه_پای_ټکی += downwards;

      إذا(new_cursor.لیکه >= 0)
        new_cursor.لیکه += downwards;

      final_blank_لیکه += downwards;
    }

#که_تعریف_شوی DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_لیکه_پیل, new_لیکه_پای_ټکی, old_لیکه_پیل, old_لیکه_پای_ټکی, پلنوالی);
#د_که_پای

    إذا(new_لیکه_پیل < 0) {
      إذا(old_لیکه_پیل <= old_cursor.لیکه && old_cursor.لیکه < old_لیکه_پای_ټکی) {
        new_cursor.لیکه = 0;
        new_cursor.ستون = old_cursor.ستون;
        إذا(new_cursor.ستون >= new_ستون_شمېر)
          new_cursor.ستون = new_ستون_شمېر-1;
      }
      اكسر;
    }

    لكل(new_لیکه = new_لیکه_پیل, old_لیکه = old_لیکه_پیل; new_لیکه <= new_لیکه_پای_ټکی; new_لیکه++) {
      صحيح شمېر = پلنوالی >= new_ستون_شمېر ? new_ستون_شمېر : پلنوالی;
      پلنوالی -= شمېر;

      صحيح new_ستون = 0;

      طالما(شمېر) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_بفر[new_لیکه * new_ستون_شمېر + new_ستون] = old_بفر[old_لیکه * old_ستون_شمېر + old_ستون];

        إذا(old_cursor.لیکه == old_لیکه && old_cursor.ستون == old_ستون)
          new_cursor.لیکه = new_لیکه, new_cursor.ستون = new_ستون;

        old_ستون++;
        إذا(old_ستون == old_ستون_شمېر) {
          old_لیکه++;

          إذا(!REFLOW) {
            new_ستون++;
            اكسر;
          }
          old_ستون = 0;
        }

        new_ستون++;
        شمېر--;
      }

      إذا(old_cursor.لیکه == old_لیکه && old_cursor.ستون >= old_ستون) {
        new_cursor.لیکه = new_لیکه, new_cursor.ستون = (old_cursor.ستون - old_ستون + new_ستون);
        إذا(new_cursor.ستون >= new_ستون_شمېر)
          new_cursor.ستون = new_ستون_شمېر-1;
      }

      طالما(new_ستون < new_ستون_شمېر) {
        clearcell(screen, &new_بفر[new_لیکه * new_ستون_شمېر + new_ستون]);
        new_ستون++;
      }

      new_lineinfo[new_لیکه].continuation = (new_لیکه > new_لیکه_پیل);
    }

    old_لیکه = old_لیکه_پیل - 1;
    new_لیکه = new_لیکه_پیل - 1;
  }

  إذا(old_cursor.لیکه <= old_لیکه) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.لیکه = 0, new_cursor.ستون = old_cursor.ستون;
    إذا(new_cursor.ستون >= new_ستون_شمېر)
      new_cursor.ستون = new_ستون_شمېر-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  إذا(فعال && (new_cursor.لیکه == -1 || new_cursor.ستون == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    إذا (new_cursor.لیکه < 0)
      new_cursor.لیکه = 0;
    إذا (new_cursor.ستون < 0)
      new_cursor.ستون = 0;
  }

  إذا(old_لیکه >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    إذا((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      لكل(صحيح لیکه = 0; لیکه <= old_لیکه; لیکه++) {
        ثابت VTerm_کرښه_Info *lineinfo = old_lineinfo + لیکه;
        sb_pushline_from_لیکه(screen, لیکه, lineinfo->continuation);
      }
    إذا(فعال)
      statefields->pos.لیکه -= (old_لیکه + 1);
  }
  إذا(new_لیکه >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    طالما(new_لیکه >= 0) {
      VTermPos pos;
      إذا(!(screen->callbacks->sb_popline(old_ستون_شمېر, screen->sb_بفر, screen->cbdata)))
        اكسر;

      pos.لیکه = new_لیکه;
      لكل(pos.ستون = 0; pos.ستون < old_ستون_شمېر && pos.ستون < new_ستون_شمېر; pos.ستون += screen->sb_بفر[pos.ستون].پلنوالی) {
        VTermScreenCell *src = &screen->sb_بفر[pos.ستون];
        ScreenCell *dst = &new_بفر[pos.لیکه * new_ستون_شمېر + pos.ستون];

        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_فی_CELL; i++) {
          dst->chars[i] = src->chars[i];
          إذا(!src->chars[i])
            اكسر;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.سرچپه   = src->attrs.سرچپه ^ screen->global_سرچپه;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        إذا(src->پلنوالی == 2 && pos.ستون < (new_ستون_شمېر-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      لكل( ; pos.ستون < new_ستون_شمېر; pos.ستون++)
        clearcell(screen, &new_بفر[pos.لیکه * new_ستون_شمېر + pos.ستون]);
      new_لیکه--;

      إذا(فعال)
        statefields->pos.لیکه++;
    }
  }
  إذا(new_لیکه >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحيح moverows = new_لیکې - new_لیکه - 1;
    memmove(&new_بفر[0], &new_بفر[(new_لیکه + 1) * new_ستون_شمېر], moverows * new_ستون_شمېر * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_لیکه + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.لیکه -= (new_لیکه + 1);

    لكل(new_لیکه = moverows; new_لیکه < new_لیکې; new_لیکه++) {
      لكل(صحيح ستون = 0; ستون < new_ستون_شمېر; ستون++)
        clearcell(screen, &new_بفر[new_لیکه * new_ستون_شمېر + ستون]);
      new_lineinfo[new_لیکه] = (VTerm_کرښه_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_بفر);
  screen->buffers[bufidx] = new_بفر;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  إذا(فعال)
    statefields->pos = new_cursor;

  أرجع;
}

ساكن صحيح resize(صحيح new_لیکې, صحيح new_ستون_شمېر, VTermStateFields *fields, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح altscreen_فعال = (screen->buffers[BUFIDX_ALTSCREEN] && screen->بفر == screen->buffers[BUFIDX_ALTSCREEN]);

  صحيح old_لیکې = screen->لیکې_2;
  صحيح old_ستون_شمېر = screen->ستون_شمېر;

  إذا(new_ستون_شمېر > old_ستون_شمېر) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    إذا(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    إذا (new_ستون_شمېر > VTERM_MAX_ستون_شمېر)
      new_ستون_شمېر = VTERM_MAX_ستون_شمېر;

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_ستون_شمېر);
  }

  إذا (new_لیکې > VTERM_MAX_لیکې)
    new_لیکې = VTERM_MAX_لیکې;

  resize_بفر(screen, 0, new_لیکې, new_ستون_شمېر, !altscreen_فعال, fields);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    resize_بفر(screen, 1, new_لیکې, new_ستون_شمېر, altscreen_فعال, fields);
  وإلا إذا(new_لیکې != old_لیکې) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_کرښه_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_لیکې);
    لكل(صحيح لیکه = 0; لیکه < new_لیکې; لیکه++)
      new_lineinfo[لیکه] = (VTerm_کرښه_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->بفر = altscreen_فعال ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->لیکې_2 = new_لیکې;
  screen->ستون_شمېر = new_ستون_شمېر;

  إذا(new_ستون_شمېر <= old_ستون_شمېر) {
    إذا(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_ستون_شمېر);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  إذا(screen->callbacks && screen->callbacks->resize)
    أرجع (*screen->callbacks->resize)(new_لیکې, new_ستون_شمېر, screen->cbdata);

  أرجع 1;
}

ساكن صحيح setlineinfo(صحيح لیکه, ثابت VTerm_کرښه_Info *newinfo, ثابت VTerm_کرښه_Info *oldinfo, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    لكل(صحيح ستون = 0; ستون < screen->ستون_شمېر; ستون++) {
      ScreenCell *cell = getcell(screen, لیکه, ستون);
      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     لیکه, ستون);
	أرجع 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.پیل_لیکه = لیکه;
    rect.پای_ټکی_لیکه   = لیکه + 1;
    rect.پیل_ستون = 0;
    rect.پای_ټکی_ستون   = newinfo->doublewidth ? screen->ستون_شمېر / 2 : screen->ستون_شمېر;
    damagerect(screen, rect);

    إذا(newinfo->doublewidth) {
      rect.پیل_ستون = screen->ستون_شمېر / 2;
      rect.پای_ټکی_ستون   = screen->ستون_شمېر;

      erase_دننی(rect, 0, user);
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
  صحيح لیکې_2, ستون_شمېر;

  vterm_get_اندازه(vt, &لیکې_2, &ستون_شمېر);

  screen->vt = vt;
  screen->state = state;

  screen->damage_یوځای = VTERM_DAMAGE_CELL;
  screen->damaged.پیل_لیکه = -1;
  screen->pending_scrollrect.پیل_لیکه = -1;

  screen->لیکې_2 = لیکې_2;
  screen->ستون_شمېر = ستون_شمېر;

  screen->global_سرچپه = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = عدم;
  screen->cbdata    = عدم;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_بفر(screen, لیکې_2, ستون_شمېر);

  screen->بفر = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * ستون_شمېر);
  إذا (screen->بفر == عدم || screen->sb_بفر == عدم)
  {
    vterm_screen_free(screen);
    أرجع عدم;
  }

  vterm_state_سېټ_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  أرجع screen;
}

دننی فراغ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_بفر);

  vterm_allocator_free(screen->vt, screen);
}

فراغ vterm_screen_reset(VTermScreen *screen, صحيح hard)
{
  screen->damaged.پیل_لیکه = -1;
  screen->pending_scrollrect.پیل_لیکه = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ساكن size_t _get_chars(ثابت VTermScreen *screen, ثابت صحيح utf8, فراغ *بفر, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحيح padding = 0;

#تعریف_کړه PUT(c)                                             \
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

  لكل(صحيح لیکه = rect.پیل_لیکه; لیکه < rect.پای_ټکی_لیکه; لیکه++) {
    لكل(صحيح ستون = rect.پیل_ستون; ستون < rect.پای_ټکی_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, لیکه, ستون);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     لیکه, ستون);
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
        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_فی_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    إذا(لیکه < rect.پای_ټکی_لیکه - 1) {
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
  ScreenCell *intcell = getcell(screen, pos.لیکه, pos.ستون);

  إذا(!intcell)
    أرجع 0;

  لكل(صحيح i = 0; i < VTERM_MAX_CHARS_فی_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    إذا(!intcell->chars[i])
      اكسر;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.سرچپه   = intcell->pen.سرچپه ^ screen->global_سرچپه;
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
    إذا(pos.ستون < (screen->ستون_شمېر - 1) &&
       getcell(screen, pos.لیکه, pos.ستون + 1)->chars[0] == (uint32_t)-1) {
      إذا(getcell(screen, pos.لیکه, pos.ستون)->chars[0] == 0x20) {
        getcell(screen, pos.لیکه, pos.ستون)->chars[0] = 0;
        cell->پلنوالی = 2;
      } وإلا إذا(getcell(screen, pos.لیکه, pos.ستون)->chars[0] == 0) {
        getcell(screen, pos.لیکه, pos.ستون + 1)->chars[0] = 0;
        cell->پلنوالی = 1;
      } وإلا {
        cell->پلنوالی = 2;
      }
    } وإلا
      cell->پلنوالی = 1;
  } وإلا {
    إذا(pos.ستون < (screen->ستون_شمېر - 1) &&
       getcell(screen, pos.لیکه, pos.ستون + 1)->chars[0] == (uint32_t)-1)
      cell->پلنوالی = 2;
    وإلا
      cell->پلنوالی = 1;
  }

  أرجع 1;
}

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  لكل(; pos.ستون < screen->ستون_شمېر; pos.ستون++) {
    ScreenCell *cell = getcell(screen, pos.لیکه, pos.ستون);
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
    صحيح لیکې_2, ستون_شمېر;
    vterm_get_اندازه(screen->vt, &لیکې_2, &ستون_شمېر);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_بفر(screen, لیکې_2, ستون_شمېر);
  }
}

فراغ vterm_screen_سېټ_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, فراغ *user)
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

فراغ vterm_screen_سېټ_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  vterm_state_سېټ_unrecognised_fallbacks(screen->state, fallbacks, user);
}

فراغ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  أرجع vterm_state_get_unrecognised_fbdata(screen->state);
}

فراغ vterm_screen_flush_damage(VTermScreen *screen)
{
  إذا(screen->pending_scrollrect.پیل_لیکه != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.پیل_لیکه = -1;
  }

  إذا(screen->damaged.پیل_لیکه != -1) {
    إذا(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.پیل_لیکه = -1;
  }
}

فراغ vterm_screen_سېټ_damage_یوځای(VTermScreen *screen, VTerm_Damage_اندازه اندازه)
{
  vterm_screen_flush_damage(screen);
  screen->damage_یوځای = اندازه;
}

ساكن صحيح attrs_توپیر(VTerm_Attr_ماسک attrs, ScreenCell *a, ScreenCell *b)
{
  إذا((attrs & VTERM_ATTR_BOLD_ماسک)       && (a->pen.bold != b->pen.bold))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_UNDERLINE_ماسک)  && (a->pen.underline != b->pen.underline))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_ITALIC_ماسک)     && (a->pen.italic != b->pen.italic))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_BLINK_ماسک)      && (a->pen.blink != b->pen.blink))
    أرجع 1;
  إذا((attrs & VTERM_ATTR_سرچپه_ماسک)    && (a->pen.سرچپه != b->pen.سرچپه))
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
  ScreenCell *موخه = getcell(screen, pos.لیکه, pos.ستون);

  // TODO: bounds check
  extent->پیل_لیکه = pos.لیکه;
  extent->پای_ټکی_لیکه   = pos.لیکه + 1;

  إذا(extent->پیل_ستون < 0)
    extent->پیل_ستون = 0;
  إذا(extent->پای_ټکی_ستون < 0)
    extent->پای_ټکی_ستون = screen->ستون_شمېر;

  صحيح ستون;

  لكل(ستون = pos.ستون - 1; ستون >= extent->پیل_ستون; ستون--)
    إذا(attrs_توپیر(attrs, موخه, getcell(screen, pos.لیکه, ستون)))
      اكسر;
  extent->پیل_ستون = ستون + 1;

  لكل(ستون = pos.ستون + 1; ستون < extent->پای_ټکی_ستون; ستون++)
    إذا(attrs_توپیر(attrs, موخه, getcell(screen, pos.لیکه, ستون)))
      اكسر;
  extent->پای_ټکی_ستون = ستون - 1;

  أرجع 1;
}

فراغ vterm_screen_convert_color_منزل_rgb(ثابت VTermScreen *screen, VTermColor *ستون)
{
  vterm_state_convert_color_منزل_rgb(screen->state, ستون);
}

ساكن فراغ reset_default_colours(VTermScreen *screen, ScreenCell *بفر)
{
  لكل(صحيح لیکه = 0; لیکه <= screen->لیکې_2 - 1; لیکه++)
    لكل(صحيح ستون = 0; ستون <= screen->ستون_شمېر - 1; ستون++) {
      ScreenCell *cell = &بفر[لیکه * screen->ستون_شمېر + ستون];
      إذا(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      إذا(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

فراغ vterm_screen_سېټ_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_سېټ_default_colors(screen->state, default_fg, default_bg);

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
