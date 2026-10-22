#అన్తర్భావ "vterm_internal.h"

// vim: set sw=2 :
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "rect.h"
#అన్తర్భావ "utf8.h"

#నిర్వచించు UNICODE_SPACE 0x20
#నిర్వచించు UNICODE_LINEFEED 0x0a

#నిర్వచనం_రద్దు DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
రకనిర్వచనం నిర్మాణం
{
  /* After the bitfield */
  VTermColor   fg, bg;

  గుర్తులేని పూర్ణాంకం bold      : 1;
  గుర్తులేని పూర్ణాంకం underline : 2;
  గుర్తులేని పూర్ణాంకం italic    : 1;
  గుర్తులేని పూర్ణాంకం blink     : 1;
  గుర్తులేని పూర్ణాంకం తిరగేయి   : 1;
  గుర్తులేని పూర్ణాంకం conceal   : 1;
  గుర్తులేని పూర్ణాంకం strike    : 1;
  గుర్తులేని పూర్ణాంకం font      : 4; /* 0 to 9 */
  గుర్తులేని పూర్ణాంకం small     : 1;
  గుర్తులేని పూర్ణాంకం baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  గుర్తులేని పూర్ణాంకం protected_cell : 1;
  గుర్తులేని పూర్ణాంకం dwl            : 1; /* on a DECDWL or DECDHL line */
  గుర్తులేని పూర్ణాంకం dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
రకనిర్వచనం నిర్మాణం
{
  uint32_t chars[VTERM_MAX_CHARS_చొప్పున_CELL];
  ScreenPen pen;
} ScreenCell;

నిర్మాణం VTermScreen
{
  VTerm *vt;
  VTermState *state;

  స్థిరరాశి VTermScreenCallbacks *callbacks;
  శూన్య *cbdata;
  పూర్ణాంకం callbacks_has_pushline4;

  VTerm_Damage_కొలత damage_విలీనం;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  పూర్ణాంకం pending_scroll_downward, pending_scroll_rightward;

  పూర్ణాంకం అడ్డువరుసలు_2;
  పూర్ణాంకం నిలువుల_సంఖ్య;

  గుర్తులేని పూర్ణాంకం global_తిరగేయి : 1;
  గుర్తులేని పూర్ణాంకం reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *బఫరు;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_బఫరు;

  ScreenPen pen;
};

స్థిర శూన్య clearcell(స్థిరరాశి VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

స్థిర ScreenCell *getcell(స్థిరరాశి VTermScreen *screen, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
  ఒకవేళ(అడ్డువరుస < 0 || అడ్డువరుస >= screen->అడ్డువరుసలు_2)
    తిరిగి ఖాళీ;
  ఒకవేళ(నిలువు < 0 || నిలువు >= screen->నిలువుల_సంఖ్య)
    తిరిగి ఖాళీ;
  ఒకవేళ (screen->బఫరు == ఖాళీ)
    తిరిగి ఖాళీ;
  తిరిగి screen->బఫరు + (screen->నిలువుల_సంఖ్య * అడ్డువరుస) + నిలువు;
}

స్థిర ScreenCell *alloc_బఫరు(VTermScreen *screen, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
  ScreenCell *new_బఫరు = vterm_allocator_malloc(screen->vt, పరిమాణం(ScreenCell) * అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);

  కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస < అడ్డువరుసలు_2; అడ్డువరుస++) {
    కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు < నిలువుల_సంఖ్య; నిలువు++) {
      clearcell(screen, &new_బఫరు[అడ్డువరుస * నిలువుల_సంఖ్య + నిలువు]);
    }
  }

  తిరిగి new_బఫరు;
}

స్థిర శూన్య damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  ఎంపిక(screen->damage_విలీనం) {
  స్థితి VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    ఆపు;

  స్థితి VTERM_DAMAGE_అడ్డువరుస:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    ఒకవేళ(rect.చివర_అడ్డువరుస > rect.ప్రారంభం_అడ్డువరుస + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    లేకపోతే ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస == -1) {
      // None stored yet
      screen->damaged = rect;
      తిరిగి;
    }
    లేకపోతే ఒకవేళ(rect.ప్రారంభం_అడ్డువరుస == screen->damaged.ప్రారంభం_అడ్డువరుస) {
      // Merge with the stored line
      ఒకవేళ(screen->damaged.ప్రారంభం_నిలువు > rect.ప్రారంభం_నిలువు)
        screen->damaged.ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు;
      ఒకవేళ(screen->damaged.చివర_నిలువు < rect.చివర_నిలువు)
        screen->damaged.చివర_నిలువు = rect.చివర_నిలువు;
      తిరిగి;
    }
    లేకపోతే {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    ఆపు;

  స్థితి VTERM_DAMAGE_SCREEN:
  స్థితి VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస == -1)
      screen->damaged = rect;
    లేకపోతే {
      rect_expand(&screen->damaged, &rect);
    }
    తిరిగి;

  అప్రమేయ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_విలీనం);
    తిరిగి;
  }

  ఒకవేళ(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

స్థిర శూన్య damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.చివర_అడ్డువరుస = screen->అడ్డువరుసలు_2;
  rect.చివర_నిలువు = screen->నిలువుల_సంఖ్య;

  damagerect(screen, rect);
}

స్థిర పూర్ణాంకం putglyph(VTermGlyphInfo *info, VTermPos pos, శూన్య *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.అడ్డువరుస, pos.నిలువు);

  ఒకవేళ(!cell)
    తిరిగి 0;

  పూర్ణాంకం i;
  కొరకు(i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ఒకవేళ(i < VTERM_MAX_CHARS_చొప్పున_CELL)
    cell->chars[i] = 0;

  కొరకు(పూర్ణాంకం నిలువు = 1; నిలువు < info->వెడల్పు; నిలువు++)
  {
    ScreenCell *onecell = getcell(screen, pos.అడ్డువరుస, pos.నిలువు + నిలువు);
    ఒకవేళ (onecell == ఖాళీ)
      ఆపు;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.ప్రారంభం_అడ్డువరుస = pos.అడ్డువరుస;
  rect.చివర_అడ్డువరుస   = pos.అడ్డువరుస+1;
  rect.ప్రారంభం_నిలువు = pos.నిలువు;
  rect.చివర_నిలువు   = pos.నిలువు+info->వెడల్పు;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  తిరిగి 1;
}

స్థిర శూన్య sb_pushline_from_అడ్డువరుస(VTermScreen *screen, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం continuation)
{
  VTermPos pos;
  pos.అడ్డువరుస = అడ్డువరుస;
  కొరకు(pos.నిలువు = 0; pos.నిలువు < screen->నిలువుల_సంఖ్య; pos.నిలువు++)
    vterm_screen_get_cell(screen, pos, screen->sb_బఫరు + pos.నిలువు);

  ఒకవేళ(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->నిలువుల_సంఖ్య, screen->sb_బఫరు, continuation, screen->cbdata);
  లేకపోతే
    (screen->callbacks->sb_pushline)(screen->నిలువుల_సంఖ్య, screen->sb_బఫరు, screen->cbdata);
}

స్థిర పూర్ణాంకం premove(VTermRect rect, శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.ప్రారంభం_అడ్డువరుస == 0 && rect.ప్రారంభం_నిలువు == 0 &&        // starts top-left corner
     rect.చివర_నిలువు == screen->నిలువుల_సంఖ్య &&                      // full width
     screen->బఫరు == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++) {
      స్థిరరాశి VTerm_పంక్తి_Info *lineinfo = vterm_state_get_lineinfo(screen->state, అడ్డువరుస);
      sb_pushline_from_అడ్డువరుస(screen, అడ్డువరుస, lineinfo->continuation);
    }
  }

  తిరిగి 1;
}

స్థిర పూర్ణాంకం moverect_అంతర్గత(VTermRect dest, VTermRect src, శూన్య *user)
{
  VTermScreen *screen = user;

  పూర్ణాంకం నిలువుల_సంఖ్య = src.చివర_నిలువు - src.ప్రారంభం_నిలువు;
  పూర్ణాంకం downward = src.ప్రారంభం_అడ్డువరుస - dest.ప్రారంభం_అడ్డువరుస;

  పూర్ణాంకం init_అడ్డువరుస, test_అడ్డువరుస, inc_అడ్డువరుస;
  ఒకవేళ(downward < 0) {
    init_అడ్డువరుస = dest.చివర_అడ్డువరుస - 1;
    test_అడ్డువరుస = dest.ప్రారంభం_అడ్డువరుస - 1;
    inc_అడ్డువరుస  = -1;
  }
  లేకపోతే {
    init_అడ్డువరుస = dest.ప్రారంభం_అడ్డువరుస;
    test_అడ్డువరుస = dest.చివర_అడ్డువరుస;
    inc_అడ్డువరుస  = +1;
  }

  కొరకు(పూర్ణాంకం అడ్డువరుస = init_అడ్డువరుస; అడ్డువరుస != test_అడ్డువరుస; అడ్డువరుస += inc_అడ్డువరుస)
    memmove(getcell(screen, అడ్డువరుస, dest.ప్రారంభం_నిలువు),
            getcell(screen, అడ్డువరుస + downward, src.ప్రారంభం_నిలువు),
            నిలువుల_సంఖ్య * పరిమాణం(ScreenCell));

  తిరిగి 1;
}

స్థిర పూర్ణాంకం moverect_user(VTermRect dest, VTermRect src, శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(screen->callbacks && screen->callbacks->moverect) {
    ఒకవేళ(screen->damage_విలీనం != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ఒకవేళ((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      తిరిగి 1;
  }

  damagerect(screen, dest);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం erase_అంతర్గత(VTermRect rect, పూర్ణాంకం selective, శూన్య *user)
{
  VTermScreen *screen = user;

  కొరకు(పూర్ణాంకం అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < screen->state->అడ్డువరుసలు_2 && అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++) {
    స్థిరరాశి VTerm_పంక్తి_Info *info = vterm_state_get_lineinfo(screen->state, అడ్డువరుస);

    కొరకు(పూర్ణాంకం నిలువు = rect.ప్రారంభం_నిలువు; నిలువు < rect.చివర_నిలువు; నిలువు++) {
      ScreenCell *cell = getcell(screen, అడ్డువరుస, నిలువు);

      ఒకవేళ (cell == ఖాళీ)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     అడ్డువరుస, నిలువు);
	తిరిగి 1;
      }
      ఒకవేళ(selective && cell->pen.protected_cell)
        కొనసాగు;

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

  తిరిగి 1;
}

స్థిర పూర్ణాంకం erase_user(VTermRect rect, పూర్ణాంకం selective వాడని, శూన్య *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం erase(VTermRect rect, పూర్ణాంకం selective, శూన్య *user)
{
  erase_అంతర్గత(rect, selective, user);
  తిరిగి erase_user(rect, 0, user);
}

స్థిర పూర్ణాంకం scrollrect(VTermRect rect, పూర్ణాంకం downward, పూర్ణాంకం rightward, శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(screen->damage_విలీనం != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_అంతర్గత, erase_అంతర్గత, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    తిరిగి 1;
  }

  ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ఒకవేళ(screen->pending_scrollrect.ప్రారంభం_అడ్డువరుస == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  లేకపోతే ఒకవేళ(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  లేకపోతే {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_అంతర్గత, erase_అంతర్గత, screen);

  ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస == -1)
    తిరిగి 1;

  ఒకవేళ(rect_కలిగియుంది(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  లేకపోతే ఒకవేళ(rect.ప్రారంభం_నిలువు <= screen->damaged.ప్రారంభం_నిలువు &&
          rect.చివర_నిలువు   >= screen->damaged.చివర_నిలువు &&
          rightward == 0) {
    ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస >= rect.ప్రారంభం_అడ్డువరుస &&
       screen->damaged.ప్రారంభం_అడ్డువరుస  < rect.చివర_అడ్డువరుస) {
      screen->damaged.ప్రారంభం_అడ్డువరుస -= downward;
      ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస < rect.ప్రారంభం_అడ్డువరుస)
        screen->damaged.ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస;
      ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస > rect.చివర_అడ్డువరుస)
        screen->damaged.ప్రారంభం_అడ్డువరుస = rect.చివర_అడ్డువరుస;
    }
    ఒకవేళ(screen->damaged.చివర_అడ్డువరుస >= rect.ప్రారంభం_అడ్డువరుస &&
       screen->damaged.చివర_అడ్డువరుస  < rect.చివర_అడ్డువరుస) {
      screen->damaged.చివర_అడ్డువరుస -= downward;
      ఒకవేళ(screen->damaged.చివర_అడ్డువరుస < rect.ప్రారంభం_అడ్డువరుస)
        screen->damaged.చివర_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస;
      ఒకవేళ(screen->damaged.చివర_అడ్డువరుస > rect.చివర_అడ్డువరుస)
        screen->damaged.చివర_అడ్డువరుస = rect.చివర_అడ్డువరుస;
    }
  }
  లేకపోతే {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  తిరిగి 1;
}

స్థిర పూర్ణాంకం movecursor(VTermPos pos, VTermPos oldpos, పూర్ణాంకం visible, శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(screen->callbacks && screen->callbacks->movecursor)
    తిరిగి (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  తిరిగి 0;
}

స్థిర పూర్ణాంకం setpenattr(VTermAttr attr, VTerm_విలువ *val, శూన్య *user)
{
  VTermScreen *screen = user;

  ఎంపిక(attr) {
  స్థితి VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->సంఖ్య;
    తిరిగి 1;
  స్థితి VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_తిరగేయి:
    screen->pen.తిరగేయి = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_FONT:
    screen->pen.font = val->సంఖ్య;
    తిరిగి 1;
  స్థితి VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    తిరిగి 1;
  స్థితి VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    తిరిగి 1;
  స్థితి VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->సంఖ్య;
    తిరిగి 1;

  స్థితి VTERM_N_ATTRS:
    తిరిగి 0;
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం settermprop(VTermProp prop, VTerm_విలువ *val, శూన్య *user)
{
  VTermScreen *screen = user;

  ఎంపిక(prop) {
  స్థితి VTERM_PROP_ALTSCREEN:
    ఒకవేళ(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      తిరిగి 0;

    screen->బఫరు = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ఒకవేళ(!val->boolean)
      damagescreen(screen);
    ఆపు;
  స్థితి VTERM_PROP_తిరగేయి:
    screen->global_తిరగేయి = val->boolean;
    damagescreen(screen);
    ఆపు;
  అప్రమేయ:
    ; /* ignore */
  }

  ఒకవేళ(screen->callbacks && screen->callbacks->settermprop)
    తిరిగి (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం bell(శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(screen->callbacks && screen->callbacks->bell)
    తిరిగి (*screen->callbacks->bell)(screen->cbdata);

  తిరిగి 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
స్థిర పూర్ణాంకం పంక్తి_popcount(ScreenCell *బఫరు, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అడ్డువరుసలు_2 వాడని, పూర్ణాంకం నిలువుల_సంఖ్య)
{
  పూర్ణాంకం నిలువు = నిలువుల_సంఖ్య - 1;
  అయితే(నిలువు >= 0 && బఫరు[అడ్డువరుస * నిలువుల_సంఖ్య + నిలువు].chars[0] == 0)
    నిలువు--;
  తిరిగి నిలువు + 1;
}

#నిర్వచించు REFLOW (screen->reflow)

స్థిర శూన్య resize_బఫరు(VTermScreen *screen, పూర్ణాంకం bufidx, పూర్ణాంకం new_అడ్డువరుసలు, పూర్ణాంకం new_నిలువుల_సంఖ్య, పూర్ణాంకం చురుకైన, VTermStateFields *statefields)
{
  పూర్ణాంకం old_అడ్డువరుసలు = screen->అడ్డువరుసలు_2;
  పూర్ణాంకం old_నిలువుల_సంఖ్య = screen->నిలువుల_సంఖ్య;

  ScreenCell *old_బఫరు = screen->buffers[bufidx];
  VTerm_పంక్తి_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_బఫరు = vterm_allocator_malloc(screen->vt, పరిమాణం(ScreenCell) * new_అడ్డువరుసలు * new_నిలువుల_సంఖ్య);
  VTerm_పంక్తి_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, పరిమాణం(new_lineinfo[0]) * new_అడ్డువరుసలు);

  // Find the final row of old buffer content
  పూర్ణాంకం old_అడ్డువరుస = old_అడ్డువరుసలు - 1;
  పూర్ణాంకం new_అడ్డువరుస = new_అడ్డువరుసలు - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#నిర్వచితమైతే DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_నిలువుల_సంఖ్య, old_అడ్డువరుసలు, new_నిలువుల_సంఖ్య, new_అడ్డువరుసలు, old_cursor.నిలువు, old_cursor.అడ్డువరుస);
#అయితే_ముగింపు

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  పూర్ణాంకం final_blank_అడ్డువరుస = new_అడ్డువరుసలు;

  అయితే(old_అడ్డువరుస >= 0) {
    పూర్ణాంకం old_అడ్డువరుస_చివర = old_అడ్డువరుస;
    /* TODO: Stop if dwl or dhl */
    అయితే(REFLOW && old_lineinfo && old_అడ్డువరుస >= 0 && old_lineinfo[old_అడ్డువరుస].continuation)
      old_అడ్డువరుస--;
    పూర్ణాంకం old_అడ్డువరుస_ప్రారంభం = old_అడ్డువరుస;

    పూర్ణాంకం వెడల్పు = 0;
    కొరకు(పూర్ణాంకం అడ్డువరుస = old_అడ్డువరుస_ప్రారంభం; అడ్డువరుస <= old_అడ్డువరుస_చివర; అడ్డువరుస++) {
      ఒకవేళ(REFLOW && అడ్డువరుస < (old_అడ్డువరుసలు - 1) && old_lineinfo[అడ్డువరుస + 1].continuation)
        వెడల్పు += old_నిలువుల_సంఖ్య;
      లేకపోతే
        వెడల్పు += పంక్తి_popcount(old_బఫరు, అడ్డువరుస, old_అడ్డువరుసలు, old_నిలువుల_సంఖ్య);
    }

    ఒకవేళ(final_blank_అడ్డువరుస == (new_అడ్డువరుస + 1) && వెడల్పు == 0)
      final_blank_అడ్డువరుస = new_అడ్డువరుస;

    పూర్ణాంకం new_ఎత్తు = REFLOW
      ? వెడల్పు ? (వెడల్పు + new_నిలువుల_సంఖ్య - 1) / new_నిలువుల_సంఖ్య : 1
      : 1;

    పూర్ణాంకం new_అడ్డువరుస_చివర = new_అడ్డువరుస;
    పూర్ణాంకం new_అడ్డువరుస_ప్రారంభం = new_అడ్డువరుస - new_ఎత్తు + 1;

    old_అడ్డువరుస = old_అడ్డువరుస_ప్రారంభం;
    పూర్ణాంకం old_నిలువు = 0;

    పూర్ణాంకం అదనపు_అడ్డువరుసలు = new_అడ్డువరుసలు - final_blank_అడ్డువరుస;

    ఒకవేళ(new_అడ్డువరుస_ప్రారంభం < 0 && /* we'd fall off the top */
        అదనపు_అడ్డువరుసలు >= 0 && /* we actually have spare rows */
        (!చురుకైన || new_cursor.అడ్డువరుస == -1 || (new_cursor.అడ్డువరుస - new_అడ్డువరుస_ప్రారంభం) < new_అడ్డువరుసలు))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      పూర్ణాంకం downwards = -new_అడ్డువరుస_ప్రారంభం;
      ఒకవేళ(downwards > అదనపు_అడ్డువరుసలు)
        downwards = అదనపు_అడ్డువరుసలు;
      పూర్ణాంకం rowcount = new_అడ్డువరుసలు - downwards;

#నిర్వచితమైతే DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#అయితే_ముగింపు

      memmove(&new_బఫరు[downwards * new_నిలువుల_సంఖ్య], &new_బఫరు[0],   rowcount * new_నిలువుల_సంఖ్య * పరిమాణం(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * పరిమాణం(new_lineinfo[0]));

      new_అడ్డువరుస += downwards;
      new_అడ్డువరుస_ప్రారంభం += downwards;
      new_అడ్డువరుస_చివర += downwards;

      ఒకవేళ(new_cursor.అడ్డువరుస >= 0)
        new_cursor.అడ్డువరుస += downwards;

      final_blank_అడ్డువరుస += downwards;
    }

#నిర్వచితమైతే DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_అడ్డువరుస_ప్రారంభం, new_అడ్డువరుస_చివర, old_అడ్డువరుస_ప్రారంభం, old_అడ్డువరుస_చివర, వెడల్పు);
#అయితే_ముగింపు

    ఒకవేళ(new_అడ్డువరుస_ప్రారంభం < 0) {
      ఒకవేళ(old_అడ్డువరుస_ప్రారంభం <= old_cursor.అడ్డువరుస && old_cursor.అడ్డువరుస < old_అడ్డువరుస_చివర) {
        new_cursor.అడ్డువరుస = 0;
        new_cursor.నిలువు = old_cursor.నిలువు;
        ఒకవేళ(new_cursor.నిలువు >= new_నిలువుల_సంఖ్య)
          new_cursor.నిలువు = new_నిలువుల_సంఖ్య-1;
      }
      ఆపు;
    }

    కొరకు(new_అడ్డువరుస = new_అడ్డువరుస_ప్రారంభం, old_అడ్డువరుస = old_అడ్డువరుస_ప్రారంభం; new_అడ్డువరుస <= new_అడ్డువరుస_చివర; new_అడ్డువరుస++) {
      పూర్ణాంకం లెక్క = వెడల్పు >= new_నిలువుల_సంఖ్య ? new_నిలువుల_సంఖ్య : వెడల్పు;
      వెడల్పు -= లెక్క;

      పూర్ణాంకం new_నిలువు = 0;

      అయితే(లెక్క) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_బఫరు[new_అడ్డువరుస * new_నిలువుల_సంఖ్య + new_నిలువు] = old_బఫరు[old_అడ్డువరుస * old_నిలువుల_సంఖ్య + old_నిలువు];

        ఒకవేళ(old_cursor.అడ్డువరుస == old_అడ్డువరుస && old_cursor.నిలువు == old_నిలువు)
          new_cursor.అడ్డువరుస = new_అడ్డువరుస, new_cursor.నిలువు = new_నిలువు;

        old_నిలువు++;
        ఒకవేళ(old_నిలువు == old_నిలువుల_సంఖ్య) {
          old_అడ్డువరుస++;

          ఒకవేళ(!REFLOW) {
            new_నిలువు++;
            ఆపు;
          }
          old_నిలువు = 0;
        }

        new_నిలువు++;
        లెక్క--;
      }

      ఒకవేళ(old_cursor.అడ్డువరుస == old_అడ్డువరుస && old_cursor.నిలువు >= old_నిలువు) {
        new_cursor.అడ్డువరుస = new_అడ్డువరుస, new_cursor.నిలువు = (old_cursor.నిలువు - old_నిలువు + new_నిలువు);
        ఒకవేళ(new_cursor.నిలువు >= new_నిలువుల_సంఖ్య)
          new_cursor.నిలువు = new_నిలువుల_సంఖ్య-1;
      }

      అయితే(new_నిలువు < new_నిలువుల_సంఖ్య) {
        clearcell(screen, &new_బఫరు[new_అడ్డువరుస * new_నిలువుల_సంఖ్య + new_నిలువు]);
        new_నిలువు++;
      }

      new_lineinfo[new_అడ్డువరుస].continuation = (new_అడ్డువరుస > new_అడ్డువరుస_ప్రారంభం);
    }

    old_అడ్డువరుస = old_అడ్డువరుస_ప్రారంభం - 1;
    new_అడ్డువరుస = new_అడ్డువరుస_ప్రారంభం - 1;
  }

  ఒకవేళ(old_cursor.అడ్డువరుస <= old_అడ్డువరుస) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.అడ్డువరుస = 0, new_cursor.నిలువు = old_cursor.నిలువు;
    ఒకవేళ(new_cursor.నిలువు >= new_నిలువుల_సంఖ్య)
      new_cursor.నిలువు = new_నిలువుల_సంఖ్య-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ఒకవేళ(చురుకైన && (new_cursor.అడ్డువరుస == -1 || new_cursor.నిలువు == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ఒకవేళ (new_cursor.అడ్డువరుస < 0)
      new_cursor.అడ్డువరుస = 0;
    ఒకవేళ (new_cursor.నిలువు < 0)
      new_cursor.నిలువు = 0;
  }

  ఒకవేళ(old_అడ్డువరుస >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ఒకవేళ((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస <= old_అడ్డువరుస; అడ్డువరుస++) {
        స్థిరరాశి VTerm_పంక్తి_Info *lineinfo = old_lineinfo + అడ్డువరుస;
        sb_pushline_from_అడ్డువరుస(screen, అడ్డువరుస, lineinfo->continuation);
      }
    ఒకవేళ(చురుకైన)
      statefields->pos.అడ్డువరుస -= (old_అడ్డువరుస + 1);
  }
  ఒకవేళ(new_అడ్డువరుస >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    అయితే(new_అడ్డువరుస >= 0) {
      VTermPos pos;
      ఒకవేళ(!(screen->callbacks->sb_popline(old_నిలువుల_సంఖ్య, screen->sb_బఫరు, screen->cbdata)))
        ఆపు;

      pos.అడ్డువరుస = new_అడ్డువరుస;
      కొరకు(pos.నిలువు = 0; pos.నిలువు < old_నిలువుల_సంఖ్య && pos.నిలువు < new_నిలువుల_సంఖ్య; pos.నిలువు += screen->sb_బఫరు[pos.నిలువు].వెడల్పు) {
        VTermScreenCell *src = &screen->sb_బఫరు[pos.నిలువు];
        ScreenCell *dst = &new_బఫరు[pos.అడ్డువరుస * new_నిలువుల_సంఖ్య + pos.నిలువు];

        కొరకు(పూర్ణాంకం i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ఒకవేళ(!src->chars[i])
            ఆపు;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.తిరగేయి   = src->attrs.తిరగేయి ^ screen->global_తిరగేయి;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        ఒకవేళ(src->వెడల్పు == 2 && pos.నిలువు < (new_నిలువుల_సంఖ్య-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      కొరకు( ; pos.నిలువు < new_నిలువుల_సంఖ్య; pos.నిలువు++)
        clearcell(screen, &new_బఫరు[pos.అడ్డువరుస * new_నిలువుల_సంఖ్య + pos.నిలువు]);
      new_అడ్డువరుస--;

      ఒకవేళ(చురుకైన)
        statefields->pos.అడ్డువరుస++;
    }
  }
  ఒకవేళ(new_అడ్డువరుస >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    పూర్ణాంకం moverows = new_అడ్డువరుసలు - new_అడ్డువరుస - 1;
    memmove(&new_బఫరు[0], &new_బఫరు[(new_అడ్డువరుస + 1) * new_నిలువుల_సంఖ్య], moverows * new_నిలువుల_సంఖ్య * పరిమాణం(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_అడ్డువరుస + 1], moverows * పరిమాణం(new_lineinfo[0]));

    new_cursor.అడ్డువరుస -= (new_అడ్డువరుస + 1);

    కొరకు(new_అడ్డువరుస = moverows; new_అడ్డువరుస < new_అడ్డువరుసలు; new_అడ్డువరుస++) {
      కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు < new_నిలువుల_సంఖ్య; నిలువు++)
        clearcell(screen, &new_బఫరు[new_అడ్డువరుస * new_నిలువుల_సంఖ్య + నిలువు]);
      new_lineinfo[new_అడ్డువరుస] = (VTerm_పంక్తి_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_బఫరు);
  screen->buffers[bufidx] = new_బఫరు;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ఒకవేళ(చురుకైన)
    statefields->pos = new_cursor;

  తిరిగి;
}

స్థిర పూర్ణాంకం resize(పూర్ణాంకం new_అడ్డువరుసలు, పూర్ణాంకం new_నిలువుల_సంఖ్య, VTermStateFields *fields, శూన్య *user)
{
  VTermScreen *screen = user;

  పూర్ణాంకం altscreen_చురుకైన = (screen->buffers[BUFIDX_ALTSCREEN] && screen->బఫరు == screen->buffers[BUFIDX_ALTSCREEN]);

  పూర్ణాంకం old_అడ్డువరుసలు = screen->అడ్డువరుసలు_2;
  పూర్ణాంకం old_నిలువుల_సంఖ్య = screen->నిలువుల_సంఖ్య;

  ఒకవేళ(new_నిలువుల_సంఖ్య > old_నిలువుల_సంఖ్య) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ఒకవేళ(screen->sb_బఫరు)
      vterm_allocator_free(screen->vt, screen->sb_బఫరు);

    ఒకవేళ (new_నిలువుల_సంఖ్య > VTERM_MAX_నిలువుల_సంఖ్య)
      new_నిలువుల_సంఖ్య = VTERM_MAX_నిలువుల_సంఖ్య;

    screen->sb_బఫరు = vterm_allocator_malloc(screen->vt, పరిమాణం(VTermScreenCell) * new_నిలువుల_సంఖ్య);
  }

  ఒకవేళ (new_అడ్డువరుసలు > VTERM_MAX_అడ్డువరుసలు)
    new_అడ్డువరుసలు = VTERM_MAX_అడ్డువరుసలు;

  resize_బఫరు(screen, 0, new_అడ్డువరుసలు, new_నిలువుల_సంఖ్య, !altscreen_చురుకైన, fields);
  ఒకవేళ(screen->buffers[BUFIDX_ALTSCREEN])
    resize_బఫరు(screen, 1, new_అడ్డువరుసలు, new_నిలువుల_సంఖ్య, altscreen_చురుకైన, fields);
  లేకపోతే ఒకవేళ(new_అడ్డువరుసలు != old_అడ్డువరుసలు) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_పంక్తి_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, పరిమాణం(new_lineinfo[0]) * new_అడ్డువరుసలు);
    కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస < new_అడ్డువరుసలు; అడ్డువరుస++)
      new_lineinfo[అడ్డువరుస] = (VTerm_పంక్తి_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->బఫరు = altscreen_చురుకైన ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->అడ్డువరుసలు_2 = new_అడ్డువరుసలు;
  screen->నిలువుల_సంఖ్య = new_నిలువుల_సంఖ్య;

  ఒకవేళ(new_నిలువుల_సంఖ్య <= old_నిలువుల_సంఖ్య) {
    ఒకవేళ(screen->sb_బఫరు)
      vterm_allocator_free(screen->vt, screen->sb_బఫరు);

    screen->sb_బఫరు = vterm_allocator_malloc(screen->vt, పరిమాణం(VTermScreenCell) * new_నిలువుల_సంఖ్య);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ఒకవేళ(screen->callbacks && screen->callbacks->resize)
    తిరిగి (*screen->callbacks->resize)(new_అడ్డువరుసలు, new_నిలువుల_సంఖ్య, screen->cbdata);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం setlineinfo(పూర్ణాంకం అడ్డువరుస, స్థిరరాశి VTerm_పంక్తి_Info *newinfo, స్థిరరాశి VTerm_పంక్తి_Info *oldinfo, శూన్య *user)
{
  VTermScreen *screen = user;

  ఒకవేళ(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు < screen->నిలువుల_సంఖ్య; నిలువు++) {
      ScreenCell *cell = getcell(screen, అడ్డువరుస, నిలువు);
      ఒకవేళ (cell == ఖాళీ)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     అడ్డువరుస, నిలువు);
	తిరిగి 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.ప్రారంభం_అడ్డువరుస = అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = అడ్డువరుస + 1;
    rect.ప్రారంభం_నిలువు = 0;
    rect.చివర_నిలువు   = newinfo->doublewidth ? screen->నిలువుల_సంఖ్య / 2 : screen->నిలువుల_సంఖ్య;
    damagerect(screen, rect);

    ఒకవేళ(newinfo->doublewidth) {
      rect.ప్రారంభం_నిలువు = screen->నిలువుల_సంఖ్య / 2;
      rect.చివర_నిలువు   = screen->నిలువుల_సంఖ్య;

      erase_అంతర్గత(rect, 0, user);
    }
  }

  తిరిగి 1;
}

స్థిర పూర్ణాంకం sb_clear(శూన్య *user) {
  VTermScreen *screen = user;

  ఒకవేళ(screen->callbacks && screen->callbacks->sb_clear)
    ఒకవేళ((*screen->callbacks->sb_clear)(screen->cbdata))
      తిరిగి 1;

  తిరిగి 0;
}

స్థిర VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  ఖాళీ, // moverect
  &erase, // erase
  ఖాళీ, // initpen
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
స్థిర VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  ఒకవేళ(!state)
    తిరిగి ఖాళీ;

  VTermScreen *screen = vterm_allocator_malloc(vt, పరిమాణం(VTermScreen));
  ఒకవేళ (screen == ఖాళీ)
    తిరిగి ఖాళీ;
  పూర్ణాంకం అడ్డువరుసలు_2, నిలువుల_సంఖ్య;

  vterm_get_కొలత(vt, &అడ్డువరుసలు_2, &నిలువుల_సంఖ్య);

  screen->vt = vt;
  screen->state = state;

  screen->damage_విలీనం = VTERM_DAMAGE_CELL;
  screen->damaged.ప్రారంభం_అడ్డువరుస = -1;
  screen->pending_scrollrect.ప్రారంభం_అడ్డువరుస = -1;

  screen->అడ్డువరుసలు_2 = అడ్డువరుసలు_2;
  screen->నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;

  screen->global_తిరగేయి = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = ఖాళీ;
  screen->cbdata    = ఖాళీ;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_బఫరు(screen, అడ్డువరుసలు_2, నిలువుల_సంఖ్య);

  screen->బఫరు = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_బఫరు = vterm_allocator_malloc(screen->vt, పరిమాణం(VTermScreenCell) * నిలువుల_సంఖ్య);
  ఒకవేళ (screen->బఫరు == ఖాళీ || screen->sb_బఫరు == ఖాళీ)
  {
    vterm_screen_free(screen);
    తిరిగి ఖాళీ;
  }

  vterm_state_సమితి_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  తిరిగి screen;
}

అంతర్గత శూన్య vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ఒకవేళ(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_బఫరు);

  vterm_allocator_free(screen->vt, screen);
}

శూన్య vterm_screen_reset(VTermScreen *screen, పూర్ణాంకం hard)
{
  screen->damaged.ప్రారంభం_అడ్డువరుస = -1;
  screen->pending_scrollrect.ప్రారంభం_అడ్డువరుస = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

స్థిర size_t _get_chars(స్థిరరాశి VTermScreen *screen, స్థిరరాశి పూర్ణాంకం utf8, శూన్య *బఫరు, size_t len, స్థిరరాశి VTermRect rect)
{
  size_t outpos = 0;
  పూర్ణాంకం padding = 0;

#నిర్వచించు PUT(c)                                             \
  ఒకవేళ(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ఒకవేళ(బఫరు && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (అక్షరం *)బఫరు + outpos);   \
    లేకపోతే                                                   \
      outpos += thislen;                                   \
  }                                                        \
  లేకపోతే {                                                   \
    ఒకవేళ(బఫరు && outpos + 1 <= len)                        \
      ((uint32_t*)బఫరు)[outpos++] = (c);                 \
    లేకపోతే                                                   \
      outpos++;                                            \
  }

  కొరకు(పూర్ణాంకం అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++) {
    కొరకు(పూర్ణాంకం నిలువు = rect.ప్రారంభం_నిలువు; నిలువు < rect.చివర_నిలువు; నిలువు++) {
      ScreenCell *cell = getcell(screen, అడ్డువరుస, నిలువు);

      ఒకవేళ (cell == ఖాళీ)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     అడ్డువరుస, నిలువు);
	తిరిగి 1;
      }
      ఒకవేళ(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      లేకపోతే ఒకవేళ(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      లేకపోతే {
        అయితే(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        కొరకు(పూర్ణాంకం i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ఒకవేళ(అడ్డువరుస < rect.చివర_అడ్డువరుస - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  తిరిగి outpos;
}

size_t vterm_screen_get_chars(స్థిరరాశి VTermScreen *screen, uint32_t *chars, size_t len, స్థిరరాశి VTermRect rect)
{
  తిరిగి _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_పాఠ్యం(స్థిరరాశి VTermScreen *screen, అక్షరం *str, size_t len, స్థిరరాశి VTermRect rect)
{
  తిరిగి _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
పూర్ణాంకం vterm_screen_get_cell(స్థిరరాశి VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.అడ్డువరుస, pos.నిలువు);

  ఒకవేళ(!intcell)
    తిరిగి 0;

  కొరకు(పూర్ణాంకం i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ఒకవేళ(!intcell->chars[i])
      ఆపు;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.తిరగేయి   = intcell->pen.తిరగేయి ^ screen->global_తిరగేయి;
  cell->attrs.conceal   = intcell->pen.conceal;
  cell->attrs.strike    = intcell->pen.strike;
  cell->attrs.font      = intcell->pen.font;
  cell->attrs.small     = intcell->pen.small;
  cell->attrs.baseline  = intcell->pen.baseline;

  cell->attrs.dwl = intcell->pen.dwl;
  cell->attrs.dhl = intcell->pen.dhl;

  cell->fg = intcell->pen.fg;
  cell->bg = intcell->pen.bg;

  ఒకవేళ(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ఒకవేళ(pos.నిలువు < (screen->నిలువుల_సంఖ్య - 1) &&
       getcell(screen, pos.అడ్డువరుస, pos.నిలువు + 1)->chars[0] == (uint32_t)-1) {
      ఒకవేళ(getcell(screen, pos.అడ్డువరుస, pos.నిలువు)->chars[0] == 0x20) {
        getcell(screen, pos.అడ్డువరుస, pos.నిలువు)->chars[0] = 0;
        cell->వెడల్పు = 2;
      } లేకపోతే ఒకవేళ(getcell(screen, pos.అడ్డువరుస, pos.నిలువు)->chars[0] == 0) {
        getcell(screen, pos.అడ్డువరుస, pos.నిలువు + 1)->chars[0] = 0;
        cell->వెడల్పు = 1;
      } లేకపోతే {
        cell->వెడల్పు = 2;
      }
    } లేకపోతే
      cell->వెడల్పు = 1;
  } లేకపోతే {
    ఒకవేళ(pos.నిలువు < (screen->నిలువుల_సంఖ్య - 1) &&
       getcell(screen, pos.అడ్డువరుస, pos.నిలువు + 1)->chars[0] == (uint32_t)-1)
      cell->వెడల్పు = 2;
    లేకపోతే
      cell->వెడల్పు = 1;
  }

  తిరిగి 1;
}

పూర్ణాంకం vterm_screen_is_eol(స్థిరరాశి VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  కొరకు(; pos.నిలువు < screen->నిలువుల_సంఖ్య; pos.నిలువు++) {
    ScreenCell *cell = getcell(screen, pos.అడ్డువరుస, pos.నిలువు);
    ఒకవేళ(cell->chars[0] != 0)
      తిరిగి 0;
  }

  తిరిగి 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ఒకవేళ(vt->screen)
    తిరిగి vt->screen;

  vt->screen = screen_new(vt);
  తిరిగి vt->screen;
}

శూన్య vterm_screen_enable_reflow(VTermScreen *screen, పూర్ణాంకం reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

శూన్య vterm_screen_enable_altscreen(VTermScreen *screen, పూర్ణాంకం altscreen)
{
  ఒకవేళ(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    పూర్ణాంకం అడ్డువరుసలు_2, నిలువుల_సంఖ్య;
    vterm_get_కొలత(screen->vt, &అడ్డువరుసలు_2, &నిలువుల_సంఖ్య);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_బఫరు(screen, అడ్డువరుసలు_2, నిలువుల_సంఖ్య);
  }
}

శూన్య vterm_screen_సమితి_callbacks(VTermScreen *screen, స్థిరరాశి VTermScreenCallbacks *callbacks, శూన్య *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

శూన్య *vterm_screen_get_cbdata(VTermScreen *screen)
{
  తిరిగి screen->cbdata;
}

శూన్య vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

శూన్య vterm_screen_సమితి_unrecognised_fallbacks(VTermScreen *screen, స్థిరరాశి VTermStateFallbacks *fallbacks, శూన్య *user)
{
  vterm_state_సమితి_unrecognised_fallbacks(screen->state, fallbacks, user);
}

శూన్య *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  తిరిగి vterm_state_get_unrecognised_fbdata(screen->state);
}

శూన్య vterm_screen_flush_damage(VTermScreen *screen)
{
  ఒకవేళ(screen->pending_scrollrect.ప్రారంభం_అడ్డువరుస != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.ప్రారంభం_అడ్డువరుస = -1;
  }

  ఒకవేళ(screen->damaged.ప్రారంభం_అడ్డువరుస != -1) {
    ఒకవేళ(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.ప్రారంభం_అడ్డువరుస = -1;
  }
}

శూన్య vterm_screen_సమితి_damage_విలీనం(VTermScreen *screen, VTerm_Damage_కొలత కొలత)
{
  vterm_screen_flush_damage(screen);
  screen->damage_విలీనం = కొలత;
}

స్థిర పూర్ణాంకం attrs_తేడా(VTerm_Attr_ముసుగు attrs, ScreenCell *a, ScreenCell *b)
{
  ఒకవేళ((attrs & VTERM_ATTR_BOLD_ముసుగు)       && (a->pen.bold != b->pen.bold))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_UNDERLINE_ముసుగు)  && (a->pen.underline != b->pen.underline))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_ITALIC_ముసుగు)     && (a->pen.italic != b->pen.italic))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_BLINK_ముసుగు)      && (a->pen.blink != b->pen.blink))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_తిరగేయి_ముసుగు)    && (a->pen.తిరగేయి != b->pen.తిరగేయి))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_CONCEAL_ముసుగు)    && (a->pen.conceal != b->pen.conceal))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_STRIKE_ముసుగు)     && (a->pen.strike != b->pen.strike))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_FONT_ముసుగు)       && (a->pen.font != b->pen.font))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_FOREGROUND_ముసుగు) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_BACKGROUND_ముసుగు) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_SMALL_ముసుగు)    && (a->pen.small != b->pen.small))
    తిరిగి 1;
  ఒకవేళ((attrs & VTERM_ATTR_BASELINE_ముసుగు)    && (a->pen.baseline != b->pen.baseline))
    తిరిగి 1;

  తిరిగి 0;
}

పూర్ణాంకం vterm_screen_get_attrs_extent(స్థిరరాశి VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ముసుగు attrs)
{
  ScreenCell *లక్ష్యం = getcell(screen, pos.అడ్డువరుస, pos.నిలువు);

  // TODO: bounds check
  extent->ప్రారంభం_అడ్డువరుస = pos.అడ్డువరుస;
  extent->చివర_అడ్డువరుస   = pos.అడ్డువరుస + 1;

  ఒకవేళ(extent->ప్రారంభం_నిలువు < 0)
    extent->ప్రారంభం_నిలువు = 0;
  ఒకవేళ(extent->చివర_నిలువు < 0)
    extent->చివర_నిలువు = screen->నిలువుల_సంఖ్య;

  పూర్ణాంకం నిలువు;

  కొరకు(నిలువు = pos.నిలువు - 1; నిలువు >= extent->ప్రారంభం_నిలువు; నిలువు--)
    ఒకవేళ(attrs_తేడా(attrs, లక్ష్యం, getcell(screen, pos.అడ్డువరుస, నిలువు)))
      ఆపు;
  extent->ప్రారంభం_నిలువు = నిలువు + 1;

  కొరకు(నిలువు = pos.నిలువు + 1; నిలువు < extent->చివర_నిలువు; నిలువు++)
    ఒకవేళ(attrs_తేడా(attrs, లక్ష్యం, getcell(screen, pos.అడ్డువరుస, నిలువు)))
      ఆపు;
  extent->చివర_నిలువు = నిలువు - 1;

  తిరిగి 1;
}

శూన్య vterm_screen_convert_color_గమ్యం_rgb(స్థిరరాశి VTermScreen *screen, VTermColor *నిలువు)
{
  vterm_state_convert_color_గమ్యం_rgb(screen->state, నిలువు);
}

స్థిర శూన్య reset_default_colours(VTermScreen *screen, ScreenCell *బఫరు)
{
  కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస <= screen->అడ్డువరుసలు_2 - 1; అడ్డువరుస++)
    కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు <= screen->నిలువుల_సంఖ్య - 1; నిలువు++) {
      ScreenCell *cell = &బఫరు[అడ్డువరుస * screen->నిలువుల_సంఖ్య + నిలువు];
      ఒకవేళ(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ఒకవేళ(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

శూన్య vterm_screen_సమితి_default_colors(VTermScreen *screen, స్థిరరాశి VTermColor *default_fg, స్థిరరాశి VTermColor *default_bg)
{
  vterm_state_సమితి_default_colors(screen->state, default_fg, default_bg);

  ఒకవేళ(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_ముసుగు)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ఒకవేళ(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_ముసుగు)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ఒకవేళ(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
