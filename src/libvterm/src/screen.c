#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

// vim: set sw=2 :
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "rect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_SPACE 0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_LINEFEED 0x0a

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
  /* After the bitfield */
  VTermColor   fg, bg;

  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ bold      : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ underline : 2;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ italic    : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ blink     : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਲਟ   : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ conceal   : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ strike    : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ font      : 4; /* 0 to 9 */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ small     : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ protected_cell : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dwl            : 1; /* on a DECDWL or DECDHL line */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
  uint32_t chars[VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL];
  ScreenPen pen;
} ScreenCell;

ਬਣਤਰ VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ਅਚਲ VTermScreenCallbacks *callbacks;
  ਖਾਲੀ *cbdata;
  ਪੂਰਨ callbacks_has_pushline4;

  VTerm_Damage_ਮਾਪ damage_ਮਿਲਾਓ;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  ਪੂਰਨ pending_scroll_downward, pending_scroll_rightward;

  ਪੂਰਨ ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ;

  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ global_ਉਲਟ : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *ਬਫਰ;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_ਬਫਰ;

  ScreenPen pen;
};

ਸਥਿਰ ਖਾਲੀ clearcell(ਅਚਲ VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ਸਥਿਰ ScreenCell *getcell(ਅਚਲ VTermScreen *screen, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
  ਜੇ(ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= screen->ਪੰਕਤੀਆਂ_2)
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(ਖਾਨਾ < 0 || ਖਾਨਾ >= screen->ਖਾਨੇ_ਗਿਣਤੀ)
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ (screen->ਬਫਰ == ਨਿਰਾ)
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ screen->ਬਫਰ + (screen->ਖਾਨੇ_ਗਿਣਤੀ * ਪੰਕਤੀ) + ਖਾਨਾ;
}

ਸਥਿਰ ScreenCell *alloc_ਬਫਰ(VTermScreen *screen, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
  ScreenCell *new_ਬਫਰ = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(ScreenCell) * ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);

  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ < ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++) {
    ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ < ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
      clearcell(screen, &new_ਬਫਰ[ਪੰਕਤੀ * ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ]);
    }
  }

  ਵਾਪਸ new_ਬਫਰ;
}

ਸਥਿਰ ਖਾਲੀ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  ਚੋਣ(screen->damage_ਮਿਲਾਓ) {
  ਹਾਲਤ VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    ਰੋਕੋ;

  ਹਾਲਤ VTERM_DAMAGE_ਪੰਕਤੀ:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    ਜੇ(rect.ਸਿਰਾ_ਪੰਕਤੀ > rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    ਨਹੀਂਤਾਂ ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ == -1) {
      // None stored yet
      screen->damaged = rect;
      ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ == screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ) {
      // Merge with the stored line
      ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਖਾਨਾ > rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
        screen->damaged.ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ;
      ਜੇ(screen->damaged.ਸਿਰਾ_ਖਾਨਾ < rect.ਸਿਰਾ_ਖਾਨਾ)
        screen->damaged.ਸਿਰਾ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ;
      ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    ਰੋਕੋ;

  ਹਾਲਤ VTERM_DAMAGE_SCREEN:
  ਹਾਲਤ VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ == -1)
      screen->damaged = rect;
    ਨਹੀਂਤਾਂ {
      rect_expand(&screen->damaged, &rect);
    }
    ਵਾਪਸ;

  ਮੂਲ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_ਮਿਲਾਓ);
    ਵਾਪਸ;
  }

  ਜੇ(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ਸਥਿਰ ਖਾਲੀ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.ਸਿਰਾ_ਪੰਕਤੀ = screen->ਪੰਕਤੀਆਂ_2;
  rect.ਸਿਰਾ_ਖਾਨਾ = screen->ਖਾਨੇ_ਗਿਣਤੀ;

  damagerect(screen, rect);
}

ਸਥਿਰ ਪੂਰਨ putglyph(VTermGlyphInfo *info, VTermPos pos, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);

  ਜੇ(!cell)
    ਵਾਪਸ 0;

  ਪੂਰਨ i;
  ਲਈ(i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ਜੇ(i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL)
    cell->chars[i] = 0;

  ਲਈ(ਪੂਰਨ ਖਾਨਾ = 1; ਖਾਨਾ < info->ਚੌੜਾਈ; ਖਾਨਾ++)
  {
    ScreenCell *onecell = getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ + ਖਾਨਾ);
    ਜੇ (onecell == ਨਿਰਾ)
      ਰੋਕੋ;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = pos.ਪੰਕਤੀ;
  rect.ਸਿਰਾ_ਪੰਕਤੀ   = pos.ਪੰਕਤੀ+1;
  rect.ਸ਼ੁਰੂ_ਖਾਨਾ = pos.ਖਾਨਾ;
  rect.ਸਿਰਾ_ਖਾਨਾ   = pos.ਖਾਨਾ+info->ਚੌੜਾਈ;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ sb_pushline_from_ਪੰਕਤੀ(VTermScreen *screen, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ continuation)
{
  VTermPos pos;
  pos.ਪੰਕਤੀ = ਪੰਕਤੀ;
  ਲਈ(pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < screen->ਖਾਨੇ_ਗਿਣਤੀ; pos.ਖਾਨਾ++)
    vterm_screen_get_cell(screen, pos, screen->sb_ਬਫਰ + pos.ਖਾਨਾ);

  ਜੇ(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->ਖਾਨੇ_ਗਿਣਤੀ, screen->sb_ਬਫਰ, continuation, screen->cbdata);
  ਨਹੀਂਤਾਂ
    (screen->callbacks->sb_pushline)(screen->ਖਾਨੇ_ਗਿਣਤੀ, screen->sb_ਬਫਰ, screen->cbdata);
}

ਸਥਿਰ ਪੂਰਨ premove(VTermRect rect, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ == 0 && rect.ਸ਼ੁਰੂ_ਖਾਨਾ == 0 &&        // starts top-left corner
     rect.ਸਿਰਾ_ਖਾਨਾ == screen->ਖਾਨੇ_ਗਿਣਤੀ &&                      // full width
     screen->ਬਫਰ == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++) {
      ਅਚਲ VTerm_ਸਤਰ_Info *lineinfo = vterm_state_get_lineinfo(screen->state, ਪੰਕਤੀ);
      sb_pushline_from_ਪੰਕਤੀ(screen, ਪੰਕਤੀ, lineinfo->continuation);
    }
  }

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ moverect_ਅੰਦਰੂਨੀ(VTermRect dest, VTermRect src, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ = src.ਸਿਰਾ_ਖਾਨਾ - src.ਸ਼ੁਰੂ_ਖਾਨਾ;
  ਪੂਰਨ downward = src.ਸ਼ੁਰੂ_ਪੰਕਤੀ - dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ;

  ਪੂਰਨ init_ਪੰਕਤੀ, test_ਪੰਕਤੀ, inc_ਪੰਕਤੀ;
  ਜੇ(downward < 0) {
    init_ਪੰਕਤੀ = dest.ਸਿਰਾ_ਪੰਕਤੀ - 1;
    test_ਪੰਕਤੀ = dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ - 1;
    inc_ਪੰਕਤੀ  = -1;
  }
  ਨਹੀਂਤਾਂ {
    init_ਪੰਕਤੀ = dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    test_ਪੰਕਤੀ = dest.ਸਿਰਾ_ਪੰਕਤੀ;
    inc_ਪੰਕਤੀ  = +1;
  }

  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = init_ਪੰਕਤੀ; ਪੰਕਤੀ != test_ਪੰਕਤੀ; ਪੰਕਤੀ += inc_ਪੰਕਤੀ)
    memmove(getcell(screen, ਪੰਕਤੀ, dest.ਸ਼ੁਰੂ_ਖਾਨਾ),
            getcell(screen, ਪੰਕਤੀ + downward, src.ਸ਼ੁਰੂ_ਖਾਨਾ),
            ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(ScreenCell));

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ moverect_user(VTermRect dest, VTermRect src, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(screen->callbacks && screen->callbacks->moverect) {
    ਜੇ(screen->damage_ਮਿਲਾਓ != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ਜੇ((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ਵਾਪਸ 1;
  }

  damagerect(screen, dest);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ erase_ਅੰਦਰੂਨੀ(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < screen->state->ਪੰਕਤੀਆਂ_2 && ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++) {
    ਅਚਲ VTerm_ਸਤਰ_Info *info = vterm_state_get_lineinfo(screen->state, ਪੰਕਤੀ);

    ਲਈ(ਪੂਰਨ ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ < rect.ਸਿਰਾ_ਖਾਨਾ; ਖਾਨਾ++) {
      ScreenCell *cell = getcell(screen, ਪੰਕਤੀ, ਖਾਨਾ);

      ਜੇ (cell == ਨਿਰਾ)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     ਪੰਕਤੀ, ਖਾਨਾ);
	ਵਾਪਸ 1;
      }
      ਜੇ(selective && cell->pen.protected_cell)
        ਜਾਰੀ;

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

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ erase_user(VTermRect rect, ਪੂਰਨ selective ਅਣਵਰਤਿਆ, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ erase(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user)
{
  erase_ਅੰਦਰੂਨੀ(rect, selective, user);
  ਵਾਪਸ erase_user(rect, 0, user);
}

ਸਥਿਰ ਪੂਰਨ scrollrect(VTermRect rect, ਪੂਰਨ downward, ਪੂਰਨ rightward, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(screen->damage_ਮਿਲਾਓ != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_ਅੰਦਰੂਨੀ, erase_ਅੰਦਰੂਨੀ, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ਵਾਪਸ 1;
  }

  ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ਜੇ(screen->pending_scrollrect.ਸ਼ੁਰੂ_ਪੰਕਤੀ == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  ਨਹੀਂਤਾਂ {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_ਅੰਦਰੂਨੀ, erase_ਅੰਦਰੂਨੀ, screen);

  ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ == -1)
    ਵਾਪਸ 1;

  ਜੇ(rect_ਸ਼ਾਮਲ_ਹੈ(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  ਨਹੀਂਤਾਂ ਜੇ(rect.ਸ਼ੁਰੂ_ਖਾਨਾ <= screen->damaged.ਸ਼ੁਰੂ_ਖਾਨਾ &&
          rect.ਸਿਰਾ_ਖਾਨਾ   >= screen->damaged.ਸਿਰਾ_ਖਾਨਾ &&
          rightward == 0) {
    ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ >= rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ &&
       screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ  < rect.ਸਿਰਾ_ਪੰਕਤੀ) {
      screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ -= downward;
      ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ < rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ)
        screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
      ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ > rect.ਸਿਰਾ_ਪੰਕਤੀ)
        screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ;
    }
    ਜੇ(screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ >= rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ &&
       screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ  < rect.ਸਿਰਾ_ਪੰਕਤੀ) {
      screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ -= downward;
      ਜੇ(screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ < rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ)
        screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
      ਜੇ(screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ > rect.ਸਿਰਾ_ਪੰਕਤੀ)
        screen->damaged.ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ movecursor(VTermPos pos, VTermPos oldpos, ਪੂਰਨ visible, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(screen->callbacks && screen->callbacks->movecursor)
    ਵਾਪਸ (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ setpenattr(VTermAttr attr, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਚੋਣ(attr) {
  ਹਾਲਤ VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->ਸੰਖਿਆ;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_ਉਲਟ:
    screen->pen.ਉਲਟ = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_FONT:
    screen->pen.font = val->ਸੰਖਿਆ;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->ਸੰਖਿਆ;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_N_ATTRS:
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ settermprop(VTermProp prop, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਚੋਣ(prop) {
  ਹਾਲਤ VTERM_PROP_ALTSCREEN:
    ਜੇ(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ਵਾਪਸ 0;

    screen->ਬਫਰ = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ਜੇ(!val->boolean)
      damagescreen(screen);
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_PROP_ਉਲਟ:
    screen->global_ਉਲਟ = val->boolean;
    damagescreen(screen);
    ਰੋਕੋ;
  ਮੂਲ:
    ; /* ignore */
  }

  ਜੇ(screen->callbacks && screen->callbacks->settermprop)
    ਵਾਪਸ (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ bell(ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(screen->callbacks && screen->callbacks->bell)
    ਵਾਪਸ (*screen->callbacks->bell)(screen->cbdata);

  ਵਾਪਸ 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ਸਥਿਰ ਪੂਰਨ ਸਤਰ_popcount(ScreenCell *ਬਫਰ, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2 ਅਣਵਰਤਿਆ, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
  ਪੂਰਨ ਖਾਨਾ = ਖਾਨੇ_ਗਿਣਤੀ - 1;
  ਜਦੋਂ(ਖਾਨਾ >= 0 && ਬਫਰ[ਪੰਕਤੀ * ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ].chars[0] == 0)
    ਖਾਨਾ--;
  ਵਾਪਸ ਖਾਨਾ + 1;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFLOW (screen->reflow)

ਸਥਿਰ ਖਾਲੀ resize_ਬਫਰ(VTermScreen *screen, ਪੂਰਨ bufidx, ਪੂਰਨ new_ਪੰਕਤੀਆਂ, ਪੂਰਨ new_ਖਾਨੇ_ਗਿਣਤੀ, ਪੂਰਨ ਸਰਗਰਮ, VTermStateFields *statefields)
{
  ਪੂਰਨ old_ਪੰਕਤੀਆਂ = screen->ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ old_ਖਾਨੇ_ਗਿਣਤੀ = screen->ਖਾਨੇ_ਗਿਣਤੀ;

  ScreenCell *old_ਬਫਰ = screen->buffers[bufidx];
  VTerm_ਸਤਰ_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_ਬਫਰ = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(ScreenCell) * new_ਪੰਕਤੀਆਂ * new_ਖਾਨੇ_ਗਿਣਤੀ);
  VTerm_ਸਤਰ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(new_lineinfo[0]) * new_ਪੰਕਤੀਆਂ);

  // Find the final row of old buffer content
  ਪੂਰਨ old_ਪੰਕਤੀ = old_ਪੰਕਤੀਆਂ - 1;
  ਪੂਰਨ new_ਪੰਕਤੀ = new_ਪੰਕਤੀਆਂ - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_ਖਾਨੇ_ਗਿਣਤੀ, old_ਪੰਕਤੀਆਂ, new_ਖਾਨੇ_ਗਿਣਤੀ, new_ਪੰਕਤੀਆਂ, old_cursor.ਖਾਨਾ, old_cursor.ਪੰਕਤੀ);
#ਜੇ_ਅੰਤ

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  ਪੂਰਨ final_blank_ਪੰਕਤੀ = new_ਪੰਕਤੀਆਂ;

  ਜਦੋਂ(old_ਪੰਕਤੀ >= 0) {
    ਪੂਰਨ old_ਪੰਕਤੀ_ਸਿਰਾ = old_ਪੰਕਤੀ;
    /* TODO: Stop if dwl or dhl */
    ਜਦੋਂ(REFLOW && old_lineinfo && old_ਪੰਕਤੀ >= 0 && old_lineinfo[old_ਪੰਕਤੀ].continuation)
      old_ਪੰਕਤੀ--;
    ਪੂਰਨ old_ਪੰਕਤੀ_ਸ਼ੁਰੂ = old_ਪੰਕਤੀ;

    ਪੂਰਨ ਚੌੜਾਈ = 0;
    ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = old_ਪੰਕਤੀ_ਸ਼ੁਰੂ; ਪੰਕਤੀ <= old_ਪੰਕਤੀ_ਸਿਰਾ; ਪੰਕਤੀ++) {
      ਜੇ(REFLOW && ਪੰਕਤੀ < (old_ਪੰਕਤੀਆਂ - 1) && old_lineinfo[ਪੰਕਤੀ + 1].continuation)
        ਚੌੜਾਈ += old_ਖਾਨੇ_ਗਿਣਤੀ;
      ਨਹੀਂਤਾਂ
        ਚੌੜਾਈ += ਸਤਰ_popcount(old_ਬਫਰ, ਪੰਕਤੀ, old_ਪੰਕਤੀਆਂ, old_ਖਾਨੇ_ਗਿਣਤੀ);
    }

    ਜੇ(final_blank_ਪੰਕਤੀ == (new_ਪੰਕਤੀ + 1) && ਚੌੜਾਈ == 0)
      final_blank_ਪੰਕਤੀ = new_ਪੰਕਤੀ;

    ਪੂਰਨ new_ਉਚਾਈ = REFLOW
      ? ਚੌੜਾਈ ? (ਚੌੜਾਈ + new_ਖਾਨੇ_ਗਿਣਤੀ - 1) / new_ਖਾਨੇ_ਗਿਣਤੀ : 1
      : 1;

    ਪੂਰਨ new_ਪੰਕਤੀ_ਸਿਰਾ = new_ਪੰਕਤੀ;
    ਪੂਰਨ new_ਪੰਕਤੀ_ਸ਼ੁਰੂ = new_ਪੰਕਤੀ - new_ਉਚਾਈ + 1;

    old_ਪੰਕਤੀ = old_ਪੰਕਤੀ_ਸ਼ੁਰੂ;
    ਪੂਰਨ old_ਖਾਨਾ = 0;

    ਪੂਰਨ ਵਾਧੂ_ਪੰਕਤੀਆਂ = new_ਪੰਕਤੀਆਂ - final_blank_ਪੰਕਤੀ;

    ਜੇ(new_ਪੰਕਤੀ_ਸ਼ੁਰੂ < 0 && /* we'd fall off the top */
        ਵਾਧੂ_ਪੰਕਤੀਆਂ >= 0 && /* we actually have spare rows */
        (!ਸਰਗਰਮ || new_cursor.ਪੰਕਤੀ == -1 || (new_cursor.ਪੰਕਤੀ - new_ਪੰਕਤੀ_ਸ਼ੁਰੂ) < new_ਪੰਕਤੀਆਂ))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      ਪੂਰਨ downwards = -new_ਪੰਕਤੀ_ਸ਼ੁਰੂ;
      ਜੇ(downwards > ਵਾਧੂ_ਪੰਕਤੀਆਂ)
        downwards = ਵਾਧੂ_ਪੰਕਤੀਆਂ;
      ਪੂਰਨ rowcount = new_ਪੰਕਤੀਆਂ - downwards;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#ਜੇ_ਅੰਤ

      memmove(&new_ਬਫਰ[downwards * new_ਖਾਨੇ_ਗਿਣਤੀ], &new_ਬਫਰ[0],   rowcount * new_ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * ਆਕਾਰ(new_lineinfo[0]));

      new_ਪੰਕਤੀ += downwards;
      new_ਪੰਕਤੀ_ਸ਼ੁਰੂ += downwards;
      new_ਪੰਕਤੀ_ਸਿਰਾ += downwards;

      ਜੇ(new_cursor.ਪੰਕਤੀ >= 0)
        new_cursor.ਪੰਕਤੀ += downwards;

      final_blank_ਪੰਕਤੀ += downwards;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_ਪੰਕਤੀ_ਸ਼ੁਰੂ, new_ਪੰਕਤੀ_ਸਿਰਾ, old_ਪੰਕਤੀ_ਸ਼ੁਰੂ, old_ਪੰਕਤੀ_ਸਿਰਾ, ਚੌੜਾਈ);
#ਜੇ_ਅੰਤ

    ਜੇ(new_ਪੰਕਤੀ_ਸ਼ੁਰੂ < 0) {
      ਜੇ(old_ਪੰਕਤੀ_ਸ਼ੁਰੂ <= old_cursor.ਪੰਕਤੀ && old_cursor.ਪੰਕਤੀ < old_ਪੰਕਤੀ_ਸਿਰਾ) {
        new_cursor.ਪੰਕਤੀ = 0;
        new_cursor.ਖਾਨਾ = old_cursor.ਖਾਨਾ;
        ਜੇ(new_cursor.ਖਾਨਾ >= new_ਖਾਨੇ_ਗਿਣਤੀ)
          new_cursor.ਖਾਨਾ = new_ਖਾਨੇ_ਗਿਣਤੀ-1;
      }
      ਰੋਕੋ;
    }

    ਲਈ(new_ਪੰਕਤੀ = new_ਪੰਕਤੀ_ਸ਼ੁਰੂ, old_ਪੰਕਤੀ = old_ਪੰਕਤੀ_ਸ਼ੁਰੂ; new_ਪੰਕਤੀ <= new_ਪੰਕਤੀ_ਸਿਰਾ; new_ਪੰਕਤੀ++) {
      ਪੂਰਨ ਗਿਣਤੀ_2 = ਚੌੜਾਈ >= new_ਖਾਨੇ_ਗਿਣਤੀ ? new_ਖਾਨੇ_ਗਿਣਤੀ : ਚੌੜਾਈ;
      ਚੌੜਾਈ -= ਗਿਣਤੀ_2;

      ਪੂਰਨ new_ਖਾਨਾ = 0;

      ਜਦੋਂ(ਗਿਣਤੀ_2) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_ਬਫਰ[new_ਪੰਕਤੀ * new_ਖਾਨੇ_ਗਿਣਤੀ + new_ਖਾਨਾ] = old_ਬਫਰ[old_ਪੰਕਤੀ * old_ਖਾਨੇ_ਗਿਣਤੀ + old_ਖਾਨਾ];

        ਜੇ(old_cursor.ਪੰਕਤੀ == old_ਪੰਕਤੀ && old_cursor.ਖਾਨਾ == old_ਖਾਨਾ)
          new_cursor.ਪੰਕਤੀ = new_ਪੰਕਤੀ, new_cursor.ਖਾਨਾ = new_ਖਾਨਾ;

        old_ਖਾਨਾ++;
        ਜੇ(old_ਖਾਨਾ == old_ਖਾਨੇ_ਗਿਣਤੀ) {
          old_ਪੰਕਤੀ++;

          ਜੇ(!REFLOW) {
            new_ਖਾਨਾ++;
            ਰੋਕੋ;
          }
          old_ਖਾਨਾ = 0;
        }

        new_ਖਾਨਾ++;
        ਗਿਣਤੀ_2--;
      }

      ਜੇ(old_cursor.ਪੰਕਤੀ == old_ਪੰਕਤੀ && old_cursor.ਖਾਨਾ >= old_ਖਾਨਾ) {
        new_cursor.ਪੰਕਤੀ = new_ਪੰਕਤੀ, new_cursor.ਖਾਨਾ = (old_cursor.ਖਾਨਾ - old_ਖਾਨਾ + new_ਖਾਨਾ);
        ਜੇ(new_cursor.ਖਾਨਾ >= new_ਖਾਨੇ_ਗਿਣਤੀ)
          new_cursor.ਖਾਨਾ = new_ਖਾਨੇ_ਗਿਣਤੀ-1;
      }

      ਜਦੋਂ(new_ਖਾਨਾ < new_ਖਾਨੇ_ਗਿਣਤੀ) {
        clearcell(screen, &new_ਬਫਰ[new_ਪੰਕਤੀ * new_ਖਾਨੇ_ਗਿਣਤੀ + new_ਖਾਨਾ]);
        new_ਖਾਨਾ++;
      }

      new_lineinfo[new_ਪੰਕਤੀ].continuation = (new_ਪੰਕਤੀ > new_ਪੰਕਤੀ_ਸ਼ੁਰੂ);
    }

    old_ਪੰਕਤੀ = old_ਪੰਕਤੀ_ਸ਼ੁਰੂ - 1;
    new_ਪੰਕਤੀ = new_ਪੰਕਤੀ_ਸ਼ੁਰੂ - 1;
  }

  ਜੇ(old_cursor.ਪੰਕਤੀ <= old_ਪੰਕਤੀ) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.ਪੰਕਤੀ = 0, new_cursor.ਖਾਨਾ = old_cursor.ਖਾਨਾ;
    ਜੇ(new_cursor.ਖਾਨਾ >= new_ਖਾਨੇ_ਗਿਣਤੀ)
      new_cursor.ਖਾਨਾ = new_ਖਾਨੇ_ਗਿਣਤੀ-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ਜੇ(ਸਰਗਰਮ && (new_cursor.ਪੰਕਤੀ == -1 || new_cursor.ਖਾਨਾ == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ਜੇ (new_cursor.ਪੰਕਤੀ < 0)
      new_cursor.ਪੰਕਤੀ = 0;
    ਜੇ (new_cursor.ਖਾਨਾ < 0)
      new_cursor.ਖਾਨਾ = 0;
  }

  ਜੇ(old_ਪੰਕਤੀ >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ਜੇ((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ <= old_ਪੰਕਤੀ; ਪੰਕਤੀ++) {
        ਅਚਲ VTerm_ਸਤਰ_Info *lineinfo = old_lineinfo + ਪੰਕਤੀ;
        sb_pushline_from_ਪੰਕਤੀ(screen, ਪੰਕਤੀ, lineinfo->continuation);
      }
    ਜੇ(ਸਰਗਰਮ)
      statefields->pos.ਪੰਕਤੀ -= (old_ਪੰਕਤੀ + 1);
  }
  ਜੇ(new_ਪੰਕਤੀ >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    ਜਦੋਂ(new_ਪੰਕਤੀ >= 0) {
      VTermPos pos;
      ਜੇ(!(screen->callbacks->sb_popline(old_ਖਾਨੇ_ਗਿਣਤੀ, screen->sb_ਬਫਰ, screen->cbdata)))
        ਰੋਕੋ;

      pos.ਪੰਕਤੀ = new_ਪੰਕਤੀ;
      ਲਈ(pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < old_ਖਾਨੇ_ਗਿਣਤੀ && pos.ਖਾਨਾ < new_ਖਾਨੇ_ਗਿਣਤੀ; pos.ਖਾਨਾ += screen->sb_ਬਫਰ[pos.ਖਾਨਾ].ਚੌੜਾਈ) {
        VTermScreenCell *src = &screen->sb_ਬਫਰ[pos.ਖਾਨਾ];
        ScreenCell *dst = &new_ਬਫਰ[pos.ਪੰਕਤੀ * new_ਖਾਨੇ_ਗਿਣਤੀ + pos.ਖਾਨਾ];

        ਲਈ(ਪੂਰਨ i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ਜੇ(!src->chars[i])
            ਰੋਕੋ;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.ਉਲਟ   = src->attrs.ਉਲਟ ^ screen->global_ਉਲਟ;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        ਜੇ(src->ਚੌੜਾਈ == 2 && pos.ਖਾਨਾ < (new_ਖਾਨੇ_ਗਿਣਤੀ-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      ਲਈ( ; pos.ਖਾਨਾ < new_ਖਾਨੇ_ਗਿਣਤੀ; pos.ਖਾਨਾ++)
        clearcell(screen, &new_ਬਫਰ[pos.ਪੰਕਤੀ * new_ਖਾਨੇ_ਗਿਣਤੀ + pos.ਖਾਨਾ]);
      new_ਪੰਕਤੀ--;

      ਜੇ(ਸਰਗਰਮ)
        statefields->pos.ਪੰਕਤੀ++;
    }
  }
  ਜੇ(new_ਪੰਕਤੀ >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    ਪੂਰਨ moverows = new_ਪੰਕਤੀਆਂ - new_ਪੰਕਤੀ - 1;
    memmove(&new_ਬਫਰ[0], &new_ਬਫਰ[(new_ਪੰਕਤੀ + 1) * new_ਖਾਨੇ_ਗਿਣਤੀ], moverows * new_ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_ਪੰਕਤੀ + 1], moverows * ਆਕਾਰ(new_lineinfo[0]));

    new_cursor.ਪੰਕਤੀ -= (new_ਪੰਕਤੀ + 1);

    ਲਈ(new_ਪੰਕਤੀ = moverows; new_ਪੰਕਤੀ < new_ਪੰਕਤੀਆਂ; new_ਪੰਕਤੀ++) {
      ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ < new_ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++)
        clearcell(screen, &new_ਬਫਰ[new_ਪੰਕਤੀ * new_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ]);
      new_lineinfo[new_ਪੰਕਤੀ] = (VTerm_ਸਤਰ_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_ਬਫਰ);
  screen->buffers[bufidx] = new_ਬਫਰ;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ਜੇ(ਸਰਗਰਮ)
    statefields->pos = new_cursor;

  ਵਾਪਸ;
}

ਸਥਿਰ ਪੂਰਨ resize(ਪੂਰਨ new_ਪੰਕਤੀਆਂ, ਪੂਰਨ new_ਖਾਨੇ_ਗਿਣਤੀ, VTermStateFields *fields, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਪੂਰਨ altscreen_ਸਰਗਰਮ = (screen->buffers[BUFIDX_ALTSCREEN] && screen->ਬਫਰ == screen->buffers[BUFIDX_ALTSCREEN]);

  ਪੂਰਨ old_ਪੰਕਤੀਆਂ = screen->ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ old_ਖਾਨੇ_ਗਿਣਤੀ = screen->ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ(new_ਖਾਨੇ_ਗਿਣਤੀ > old_ਖਾਨੇ_ਗਿਣਤੀ) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ਜੇ(screen->sb_ਬਫਰ)
      vterm_allocator_free(screen->vt, screen->sb_ਬਫਰ);

    ਜੇ (new_ਖਾਨੇ_ਗਿਣਤੀ > VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ)
      new_ਖਾਨੇ_ਗਿਣਤੀ = VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ;

    screen->sb_ਬਫਰ = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(VTermScreenCell) * new_ਖਾਨੇ_ਗਿਣਤੀ);
  }

  ਜੇ (new_ਪੰਕਤੀਆਂ > VTERM_MAX_ਪੰਕਤੀਆਂ)
    new_ਪੰਕਤੀਆਂ = VTERM_MAX_ਪੰਕਤੀਆਂ;

  resize_ਬਫਰ(screen, 0, new_ਪੰਕਤੀਆਂ, new_ਖਾਨੇ_ਗਿਣਤੀ, !altscreen_ਸਰਗਰਮ, fields);
  ਜੇ(screen->buffers[BUFIDX_ALTSCREEN])
    resize_ਬਫਰ(screen, 1, new_ਪੰਕਤੀਆਂ, new_ਖਾਨੇ_ਗਿਣਤੀ, altscreen_ਸਰਗਰਮ, fields);
  ਨਹੀਂਤਾਂ ਜੇ(new_ਪੰਕਤੀਆਂ != old_ਪੰਕਤੀਆਂ) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_ਸਤਰ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(new_lineinfo[0]) * new_ਪੰਕਤੀਆਂ);
    ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ < new_ਪੰਕਤੀਆਂ; ਪੰਕਤੀ++)
      new_lineinfo[ਪੰਕਤੀ] = (VTerm_ਸਤਰ_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->ਬਫਰ = altscreen_ਸਰਗਰਮ ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->ਪੰਕਤੀਆਂ_2 = new_ਪੰਕਤੀਆਂ;
  screen->ਖਾਨੇ_ਗਿਣਤੀ = new_ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ(new_ਖਾਨੇ_ਗਿਣਤੀ <= old_ਖਾਨੇ_ਗਿਣਤੀ) {
    ਜੇ(screen->sb_ਬਫਰ)
      vterm_allocator_free(screen->vt, screen->sb_ਬਫਰ);

    screen->sb_ਬਫਰ = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(VTermScreenCell) * new_ਖਾਨੇ_ਗਿਣਤੀ);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ਜੇ(screen->callbacks && screen->callbacks->resize)
    ਵਾਪਸ (*screen->callbacks->resize)(new_ਪੰਕਤੀਆਂ, new_ਖਾਨੇ_ਗਿਣਤੀ, screen->cbdata);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ setlineinfo(ਪੂਰਨ ਪੰਕਤੀ, ਅਚਲ VTerm_ਸਤਰ_Info *newinfo, ਅਚਲ VTerm_ਸਤਰ_Info *oldinfo, ਖਾਲੀ *user)
{
  VTermScreen *screen = user;

  ਜੇ(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ < screen->ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
      ScreenCell *cell = getcell(screen, ਪੰਕਤੀ, ਖਾਨਾ);
      ਜੇ (cell == ਨਿਰਾ)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     ਪੰਕਤੀ, ਖਾਨਾ);
	ਵਾਪਸ 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = ਪੰਕਤੀ + 1;
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
    rect.ਸਿਰਾ_ਖਾਨਾ   = newinfo->doublewidth ? screen->ਖਾਨੇ_ਗਿਣਤੀ / 2 : screen->ਖਾਨੇ_ਗਿਣਤੀ;
    damagerect(screen, rect);

    ਜੇ(newinfo->doublewidth) {
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = screen->ਖਾਨੇ_ਗਿਣਤੀ / 2;
      rect.ਸਿਰਾ_ਖਾਨਾ   = screen->ਖਾਨੇ_ਗਿਣਤੀ;

      erase_ਅੰਦਰੂਨੀ(rect, 0, user);
    }
  }

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ sb_clear(ਖਾਲੀ *user) {
  VTermScreen *screen = user;

  ਜੇ(screen->callbacks && screen->callbacks->sb_clear)
    ਜੇ((*screen->callbacks->sb_clear)(screen->cbdata))
      ਵਾਪਸ 1;

  ਵਾਪਸ 0;
}

ਸਥਿਰ VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  ਨਿਰਾ, // moverect
  &erase, // erase
  ਨਿਰਾ, // initpen
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
ਸਥਿਰ VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  ਜੇ(!state)
    ਵਾਪਸ ਨਿਰਾ;

  VTermScreen *screen = vterm_allocator_malloc(vt, ਆਕਾਰ(VTermScreen));
  ਜੇ (screen == ਨਿਰਾ)
    ਵਾਪਸ ਨਿਰਾ;
  ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;

  vterm_get_ਮਾਪ(vt, &ਪੰਕਤੀਆਂ_2, &ਖਾਨੇ_ਗਿਣਤੀ);

  screen->vt = vt;
  screen->state = state;

  screen->damage_ਮਿਲਾਓ = VTERM_DAMAGE_CELL;
  screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;
  screen->pending_scrollrect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;

  screen->ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2;
  screen->ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;

  screen->global_ਉਲਟ = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = ਨਿਰਾ;
  screen->cbdata    = ਨਿਰਾ;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_ਬਫਰ(screen, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ);

  screen->ਬਫਰ = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_ਬਫਰ = vterm_allocator_malloc(screen->vt, ਆਕਾਰ(VTermScreenCell) * ਖਾਨੇ_ਗਿਣਤੀ);
  ਜੇ (screen->ਬਫਰ == ਨਿਰਾ || screen->sb_ਬਫਰ == ਨਿਰਾ)
  {
    vterm_screen_free(screen);
    ਵਾਪਸ ਨਿਰਾ;
  }

  vterm_state_ਸੈੱਟ_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ਵਾਪਸ screen;
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ਜੇ(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_ਬਫਰ);

  vterm_allocator_free(screen->vt, screen);
}

ਖਾਲੀ vterm_screen_reset(VTermScreen *screen, ਪੂਰਨ hard)
{
  screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;
  screen->pending_scrollrect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ਸਥਿਰ size_t _get_chars(ਅਚਲ VTermScreen *screen, ਅਚਲ ਪੂਰਨ utf8, ਖਾਲੀ *ਬਫਰ, size_t len, ਅਚਲ VTermRect rect)
{
  size_t outpos = 0;
  ਪੂਰਨ padding = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT(c)                                             \
  ਜੇ(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ਜੇ(ਬਫਰ && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (ਅੱਖਰ *)ਬਫਰ + outpos);   \
    ਨਹੀਂਤਾਂ                                                   \
      outpos += thislen;                                   \
  }                                                        \
  ਨਹੀਂਤਾਂ {                                                   \
    ਜੇ(ਬਫਰ && outpos + 1 <= len)                        \
      ((uint32_t*)ਬਫਰ)[outpos++] = (c);                 \
    ਨਹੀਂਤਾਂ                                                   \
      outpos++;                                            \
  }

  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++) {
    ਲਈ(ਪੂਰਨ ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ < rect.ਸਿਰਾ_ਖਾਨਾ; ਖਾਨਾ++) {
      ScreenCell *cell = getcell(screen, ਪੰਕਤੀ, ਖਾਨਾ);

      ਜੇ (cell == ਨਿਰਾ)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     ਪੰਕਤੀ, ਖਾਨਾ);
	ਵਾਪਸ 1;
      }
      ਜੇ(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      ਨਹੀਂਤਾਂ ਜੇ(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      ਨਹੀਂਤਾਂ {
        ਜਦੋਂ(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        ਲਈ(ਪੂਰਨ i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ਜੇ(ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ਵਾਪਸ outpos;
}

size_t vterm_screen_get_chars(ਅਚਲ VTermScreen *screen, uint32_t *chars, size_t len, ਅਚਲ VTermRect rect)
{
  ਵਾਪਸ _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_ਪਾਠ(ਅਚਲ VTermScreen *screen, ਅੱਖਰ *str, size_t len, ਅਚਲ VTermRect rect)
{
  ਵਾਪਸ _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
ਪੂਰਨ vterm_screen_get_cell(ਅਚਲ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);

  ਜੇ(!intcell)
    ਵਾਪਸ 0;

  ਲਈ(ਪੂਰਨ i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ਜੇ(!intcell->chars[i])
      ਰੋਕੋ;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.ਉਲਟ   = intcell->pen.ਉਲਟ ^ screen->global_ਉਲਟ;
  cell->attrs.conceal   = intcell->pen.conceal;
  cell->attrs.strike    = intcell->pen.strike;
  cell->attrs.font      = intcell->pen.font;
  cell->attrs.small     = intcell->pen.small;
  cell->attrs.baseline  = intcell->pen.baseline;

  cell->attrs.dwl = intcell->pen.dwl;
  cell->attrs.dhl = intcell->pen.dhl;

  cell->fg = intcell->pen.fg;
  cell->bg = intcell->pen.bg;

  ਜੇ(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ਜੇ(pos.ਖਾਨਾ < (screen->ਖਾਨੇ_ਗਿਣਤੀ - 1) &&
       getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ + 1)->chars[0] == (uint32_t)-1) {
      ਜੇ(getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ)->chars[0] == 0x20) {
        getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ)->chars[0] = 0;
        cell->ਚੌੜਾਈ = 2;
      } ਨਹੀਂਤਾਂ ਜੇ(getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ)->chars[0] == 0) {
        getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ + 1)->chars[0] = 0;
        cell->ਚੌੜਾਈ = 1;
      } ਨਹੀਂਤਾਂ {
        cell->ਚੌੜਾਈ = 2;
      }
    } ਨਹੀਂਤਾਂ
      cell->ਚੌੜਾਈ = 1;
  } ਨਹੀਂਤਾਂ {
    ਜੇ(pos.ਖਾਨਾ < (screen->ਖਾਨੇ_ਗਿਣਤੀ - 1) &&
       getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ + 1)->chars[0] == (uint32_t)-1)
      cell->ਚੌੜਾਈ = 2;
    ਨਹੀਂਤਾਂ
      cell->ਚੌੜਾਈ = 1;
  }

  ਵਾਪਸ 1;
}

ਪੂਰਨ vterm_screen_is_eol(ਅਚਲ VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  ਲਈ(; pos.ਖਾਨਾ < screen->ਖਾਨੇ_ਗਿਣਤੀ; pos.ਖਾਨਾ++) {
    ScreenCell *cell = getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);
    ਜੇ(cell->chars[0] != 0)
      ਵਾਪਸ 0;
  }

  ਵਾਪਸ 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ਜੇ(vt->screen)
    ਵਾਪਸ vt->screen;

  vt->screen = screen_new(vt);
  ਵਾਪਸ vt->screen;
}

ਖਾਲੀ vterm_screen_enable_reflow(VTermScreen *screen, ਪੂਰਨ reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

ਖਾਲੀ vterm_screen_enable_altscreen(VTermScreen *screen, ਪੂਰਨ altscreen)
{
  ਜੇ(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;
    vterm_get_ਮਾਪ(screen->vt, &ਪੰਕਤੀਆਂ_2, &ਖਾਨੇ_ਗਿਣਤੀ);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_ਬਫਰ(screen, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ);
  }
}

ਖਾਲੀ vterm_screen_ਸੈੱਟ_callbacks(VTermScreen *screen, ਅਚਲ VTermScreenCallbacks *callbacks, ਖਾਲੀ *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

ਖਾਲੀ *vterm_screen_get_cbdata(VTermScreen *screen)
{
  ਵਾਪਸ screen->cbdata;
}

ਖਾਲੀ vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

ਖਾਲੀ vterm_screen_ਸੈੱਟ_unrecognised_fallbacks(VTermScreen *screen, ਅਚਲ VTermStateFallbacks *fallbacks, ਖਾਲੀ *user)
{
  vterm_state_ਸੈੱਟ_unrecognised_fallbacks(screen->state, fallbacks, user);
}

ਖਾਲੀ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  ਵਾਪਸ vterm_state_get_unrecognised_fbdata(screen->state);
}

ਖਾਲੀ vterm_screen_flush_damage(VTermScreen *screen)
{
  ਜੇ(screen->pending_scrollrect.ਸ਼ੁਰੂ_ਪੰਕਤੀ != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;
  }

  ਜੇ(screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ != -1) {
    ਜੇ(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.ਸ਼ੁਰੂ_ਪੰਕਤੀ = -1;
  }
}

ਖਾਲੀ vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(VTermScreen *screen, VTerm_Damage_ਮਾਪ ਮਾਪ)
{
  vterm_screen_flush_damage(screen);
  screen->damage_ਮਿਲਾਓ = ਮਾਪ;
}

ਸਥਿਰ ਪੂਰਨ attrs_ਫਰਕ(VTerm_Attr_ਨਕਾਬ attrs, ScreenCell *a, ScreenCell *b)
{
  ਜੇ((attrs & VTERM_ATTR_BOLD_ਨਕਾਬ)       && (a->pen.bold != b->pen.bold))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_UNDERLINE_ਨਕਾਬ)  && (a->pen.underline != b->pen.underline))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_ITALIC_ਨਕਾਬ)     && (a->pen.italic != b->pen.italic))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_BLINK_ਨਕਾਬ)      && (a->pen.blink != b->pen.blink))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_ਉਲਟ_ਨਕਾਬ)    && (a->pen.ਉਲਟ != b->pen.ਉਲਟ))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_CONCEAL_ਨਕਾਬ)    && (a->pen.conceal != b->pen.conceal))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_STRIKE_ਨਕਾਬ)     && (a->pen.strike != b->pen.strike))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_FONT_ਨਕਾਬ)       && (a->pen.font != b->pen.font))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_FOREGROUND_ਨਕਾਬ) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_BACKGROUND_ਨਕਾਬ) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_SMALL_ਨਕਾਬ)    && (a->pen.small != b->pen.small))
    ਵਾਪਸ 1;
  ਜੇ((attrs & VTERM_ATTR_BASELINE_ਨਕਾਬ)    && (a->pen.baseline != b->pen.baseline))
    ਵਾਪਸ 1;

  ਵਾਪਸ 0;
}

ਪੂਰਨ vterm_screen_get_attrs_extent(ਅਚਲ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ਨਕਾਬ attrs)
{
  ScreenCell *ਨਿਸ਼ਾਨਾ = getcell(screen, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);

  // TODO: bounds check
  extent->ਸ਼ੁਰੂ_ਪੰਕਤੀ = pos.ਪੰਕਤੀ;
  extent->ਸਿਰਾ_ਪੰਕਤੀ   = pos.ਪੰਕਤੀ + 1;

  ਜੇ(extent->ਸ਼ੁਰੂ_ਖਾਨਾ < 0)
    extent->ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
  ਜੇ(extent->ਸਿਰਾ_ਖਾਨਾ < 0)
    extent->ਸਿਰਾ_ਖਾਨਾ = screen->ਖਾਨੇ_ਗਿਣਤੀ;

  ਪੂਰਨ ਖਾਨਾ;

  ਲਈ(ਖਾਨਾ = pos.ਖਾਨਾ - 1; ਖਾਨਾ >= extent->ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ--)
    ਜੇ(attrs_ਫਰਕ(attrs, ਨਿਸ਼ਾਨਾ, getcell(screen, pos.ਪੰਕਤੀ, ਖਾਨਾ)))
      ਰੋਕੋ;
  extent->ਸ਼ੁਰੂ_ਖਾਨਾ = ਖਾਨਾ + 1;

  ਲਈ(ਖਾਨਾ = pos.ਖਾਨਾ + 1; ਖਾਨਾ < extent->ਸਿਰਾ_ਖਾਨਾ; ਖਾਨਾ++)
    ਜੇ(attrs_ਫਰਕ(attrs, ਨਿਸ਼ਾਨਾ, getcell(screen, pos.ਪੰਕਤੀ, ਖਾਨਾ)))
      ਰੋਕੋ;
  extent->ਸਿਰਾ_ਖਾਨਾ = ਖਾਨਾ - 1;

  ਵਾਪਸ 1;
}

ਖਾਲੀ vterm_screen_convert_color_ਮੰਜ਼ਿਲ_rgb(ਅਚਲ VTermScreen *screen, VTermColor *ਖਾਨਾ)
{
  vterm_state_convert_color_ਮੰਜ਼ਿਲ_rgb(screen->state, ਖਾਨਾ);
}

ਸਥਿਰ ਖਾਲੀ reset_default_colours(VTermScreen *screen, ScreenCell *ਬਫਰ)
{
  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ <= screen->ਪੰਕਤੀਆਂ_2 - 1; ਪੰਕਤੀ++)
    ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ <= screen->ਖਾਨੇ_ਗਿਣਤੀ - 1; ਖਾਨਾ++) {
      ScreenCell *cell = &ਬਫਰ[ਪੰਕਤੀ * screen->ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ];
      ਜੇ(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ਜੇ(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

ਖਾਲੀ vterm_screen_ਸੈੱਟ_default_colors(VTermScreen *screen, ਅਚਲ VTermColor *default_fg, ਅਚਲ VTermColor *default_bg)
{
  vterm_state_ਸੈੱਟ_default_colors(screen->state, default_fg, default_bg);

  ਜੇ(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_ਨਕਾਬ)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ਜੇ(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_ਨਕਾਬ)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ਜੇ(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
