#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

// vim: set sw=2 :
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "rect.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_SPACE 0x20
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_LINEFEED 0x0a

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
  /* After the bitfield */
  VTermColor   fg, bg;

  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ bold      : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ underline : 2;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ italic    : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ blink     : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಹಿಮ್ಮುಖ   : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ conceal   : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ strike    : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ font      : 4; /* 0 to 9 */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ small     : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ protected_cell : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dwl            : 1; /* on a DECDWL or DECDHL line */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
  uint32_t chars[VTERM_MAX_CHARS_ಪ್ರತಿ_CELL];
  ScreenPen pen;
} ScreenCell;

ರಚನೆ VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ಸ್ಥಿರಾಂಕ VTermScreenCallbacks *callbacks;
  ಶೂನ್ಯ *cbdata;
  ಪೂರ್ಣಾಂಕ callbacks_has_pushline4;

  VTerm_Damage_ಗಾತ್ರ damage_ವಿಲೀನ;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  ಪೂರ್ಣಾಂಕ pending_scroll_downward, pending_scroll_rightward;

  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ global_ಹಿಮ್ಮುಖ : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *ಬಫರ್;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_ಬಫರ್;

  ScreenPen pen;
};

ಸ್ಥಿರ ಶೂನ್ಯ clearcell(ಸ್ಥಿರಾಂಕ VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ಸ್ಥಿರ ScreenCell *getcell(ಸ್ಥಿರಾಂಕ VTermScreen *screen, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
  ಆದರೆ(ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= screen->ಅಡ್ಡಸಾಲುಗಳು_2)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ (screen->ಬಫರ್ == ಖಾಲಿ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ screen->ಬಫರ್ + (screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಅಡ್ಡಸಾಲು) + ಲಂಬಸಾಲು;
}

ಸ್ಥಿರ ScreenCell *alloc_ಬಫರ್(VTermScreen *screen, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
  ScreenCell *new_ಬಫರ್ = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(ScreenCell) * ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++) {
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
      clearcell(screen, &new_ಬಫರ್[ಅಡ್ಡಸಾಲು * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು]);
    }
  }

  ಹಿಂತಿರುಗಿ new_ಬಫರ್;
}

ಸ್ಥಿರ ಶೂನ್ಯ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  ಆಯ್ಕೆ(screen->damage_ವಿಲೀನ) {
  ಸ್ಥಿತಿ VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ VTERM_DAMAGE_ಅಡ್ಡಸಾಲು:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    ಆದರೆ(rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು > rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು == -1) {
      // None stored yet
      screen->damaged = rect;
      ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು == screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು) {
      // Merge with the stored line
      ಆದರೆ(screen->damaged.ಆರಂಭ_ಲಂಬಸಾಲು > rect.ಆರಂಭ_ಲಂಬಸಾಲು)
        screen->damaged.ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು;
      ಆದರೆ(screen->damaged.ಕೊನೆ_ಲಂಬಸಾಲು < rect.ಕೊನೆ_ಲಂಬಸಾಲು)
        screen->damaged.ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು;
      ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ VTERM_DAMAGE_SCREEN:
  ಸ್ಥಿತಿ VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು == -1)
      screen->damaged = rect;
    ಇಲ್ಲದಿದ್ದರೆ {
      rect_expand(&screen->damaged, &rect);
    }
    ಹಿಂತಿರುಗಿ;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_ವಿಲೀನ);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ಸ್ಥಿರ ಶೂನ್ಯ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = screen->ಅಡ್ಡಸಾಲುಗಳು_2;
  rect.ಕೊನೆ_ಲಂಬಸಾಲು = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  damagerect(screen, rect);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ putglyph(VTermGlyphInfo *info, VTermPos pos, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);

  ಆದರೆ(!cell)
    ಹಿಂತಿರುಗಿ 0;

  ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ಆದರೆ(i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL)
    cell->chars[i] = 0;

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 1; ಲಂಬಸಾಲು < info->ಅಗಲ; ಲಂಬಸಾಲು++)
  {
    ScreenCell *onecell = getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು + ಲಂಬಸಾಲು);
    ಆದರೆ (onecell == ಖಾಲಿ)
      ನಿಲ್ಲಿಸು;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = pos.ಅಡ್ಡಸಾಲು;
  rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = pos.ಅಡ್ಡಸಾಲು+1;
  rect.ಆರಂಭ_ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು;
  rect.ಕೊನೆ_ಲಂಬಸಾಲು   = pos.ಲಂಬಸಾಲು+info->ಅಗಲ;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ sb_pushline_from_ಅಡ್ಡಸಾಲು(VTermScreen *screen, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ continuation)
{
  VTermPos pos;
  pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
  ಗಾಗಿ(pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; pos.ಲಂಬಸಾಲು++)
    vterm_screen_get_cell(screen, pos, screen->sb_ಬಫರ್ + pos.ಲಂಬಸಾಲು);

  ಆದರೆ(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, screen->sb_ಬಫರ್, continuation, screen->cbdata);
  ಇಲ್ಲದಿದ್ದರೆ
    (screen->callbacks->sb_pushline)(screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, screen->sb_ಬಫರ್, screen->cbdata);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ premove(VTermRect rect, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು == 0 && rect.ಆರಂಭ_ಲಂಬಸಾಲು == 0 &&        // starts top-left corner
     rect.ಕೊನೆ_ಲಂಬಸಾಲು == screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ &&                      // full width
     screen->ಬಫರ್ == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++) {
      ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *lineinfo = vterm_state_get_lineinfo(screen->state, ಅಡ್ಡಸಾಲು);
      sb_pushline_from_ಅಡ್ಡಸಾಲು(screen, ಅಡ್ಡಸಾಲು, lineinfo->continuation);
    }
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ moverect_ಆಂತರಿಕ(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = src.ಕೊನೆ_ಲಂಬಸಾಲು - src.ಆರಂಭ_ಲಂಬಸಾಲು;
  ಪೂರ್ಣಾಂಕ downward = src.ಆರಂಭ_ಅಡ್ಡಸಾಲು - dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು;

  ಪೂರ್ಣಾಂಕ init_ಅಡ್ಡಸಾಲು, test_ಅಡ್ಡಸಾಲು, inc_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(downward < 0) {
    init_ಅಡ್ಡಸಾಲು = dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು - 1;
    test_ಅಡ್ಡಸಾಲು = dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು - 1;
    inc_ಅಡ್ಡಸಾಲು  = -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    init_ಅಡ್ಡಸಾಲು = dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    test_ಅಡ್ಡಸಾಲು = dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    inc_ಅಡ್ಡಸಾಲು  = +1;
  }

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = init_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು != test_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು += inc_ಅಡ್ಡಸಾಲು)
    memmove(getcell(screen, ಅಡ್ಡಸಾಲು, dest.ಆರಂಭ_ಲಂಬಸಾಲು),
            getcell(screen, ಅಡ್ಡಸಾಲು + downward, src.ಆರಂಭ_ಲಂಬಸಾಲು),
            ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(ScreenCell));

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ moverect_user(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(screen->callbacks && screen->callbacks->moverect) {
    ಆದರೆ(screen->damage_ವಿಲೀನ != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ಆದರೆ((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ಹಿಂತಿರುಗಿ 1;
  }

  damagerect(screen, dest);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ erase_ಆಂತರಿಕ(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < screen->state->ಅಡ್ಡಸಾಲುಗಳು_2 && ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++) {
    ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *info = vterm_state_get_lineinfo(screen->state, ಅಡ್ಡಸಾಲು);

    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು < rect.ಕೊನೆ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು++) {
      ScreenCell *cell = getcell(screen, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);

      ಆದರೆ (cell == ಖಾಲಿ)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ 1;
      }
      ಆದರೆ(selective && cell->pen.protected_cell)
        ಮುಂದುವರಿ;

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

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ erase_user(VTermRect rect, ಪೂರ್ಣಾಂಕ selective ಬಳಸದ, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ erase(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user)
{
  erase_ಆಂತರಿಕ(rect, selective, user);
  ಹಿಂತಿರುಗಿ erase_user(rect, 0, user);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scrollrect(VTermRect rect, ಪೂರ್ಣಾಂಕ downward, ಪೂರ್ಣಾಂಕ rightward, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(screen->damage_ವಿಲೀನ != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_ಆಂತರಿಕ, erase_ಆಂತರಿಕ, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ಹಿಂತಿರುಗಿ 1;
  }

  ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ಆದರೆ(screen->pending_scrollrect.ಆರಂಭ_ಅಡ್ಡಸಾಲು == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_ಆಂತರಿಕ, erase_ಆಂತರಿಕ, screen);

  ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು == -1)
    ಹಿಂತಿರುಗಿ 1;

  ಆದರೆ(rect_ಒಳಗೊಂಡಿದೆ(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rect.ಆರಂಭ_ಲಂಬಸಾಲು <= screen->damaged.ಆರಂಭ_ಲಂಬಸಾಲು &&
          rect.ಕೊನೆ_ಲಂಬಸಾಲು   >= screen->damaged.ಕೊನೆ_ಲಂಬಸಾಲು &&
          rightward == 0) {
    ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು >= rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು &&
       screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು  < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು) {
      screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು -= downward;
      ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು < rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು)
        screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
      ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು > rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು)
        screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    }
    ಆದರೆ(screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು >= rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು &&
       screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು  < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು) {
      screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು -= downward;
      ಆದರೆ(screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು < rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು)
        screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
      ಆದರೆ(screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು > rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು)
        screen->damaged.ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ movecursor(VTermPos pos, VTermPos oldpos, ಪೂರ್ಣಾಂಕ visible, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(screen->callbacks && screen->callbacks->movecursor)
    ಹಿಂತಿರುಗಿ (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ setpenattr(VTermAttr attr, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆಯ್ಕೆ(attr) {
  ಸ್ಥಿತಿ VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_ಹಿಮ್ಮುಖ:
    screen->pen.ಹಿಮ್ಮುಖ = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_FONT:
    screen->pen.font = val->ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_N_ATTRS:
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ settermprop(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆಯ್ಕೆ(prop) {
  ಸ್ಥಿತಿ VTERM_PROP_ALTSCREEN:
    ಆದರೆ(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ಹಿಂತಿರುಗಿ 0;

    screen->ಬಫರ್ = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ಆದರೆ(!val->boolean)
      damagescreen(screen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_PROP_ಹಿಮ್ಮುಖ:
    screen->global_ಹಿಮ್ಮುಖ = val->boolean;
    damagescreen(screen);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ; /* ignore */
  }

  ಆದರೆ(screen->callbacks && screen->callbacks->settermprop)
    ಹಿಂತಿರುಗಿ (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bell(ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(screen->callbacks && screen->callbacks->bell)
    ಹಿಂತಿರುಗಿ (*screen->callbacks->bell)(screen->cbdata);

  ಹಿಂತಿರುಗಿ 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಾಲು_popcount(ScreenCell *ಬಫರ್, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2 ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1;
  ಆಗ(ಲಂಬಸಾಲು >= 0 && ಬಫರ್[ಅಡ್ಡಸಾಲು * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು].chars[0] == 0)
    ಲಂಬಸಾಲು--;
  ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು + 1;
}

#ವ್ಯಾಖ್ಯಾನಿಸು REFLOW (screen->reflow)

ಸ್ಥಿರ ಶೂನ್ಯ resize_ಬಫರ್(VTermScreen *screen, ಪೂರ್ಣಾಂಕ bufidx, ಪೂರ್ಣಾಂಕ new_ಅಡ್ಡಸಾಲುಗಳು, ಪೂರ್ಣಾಂಕ new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ, VTermStateFields *statefields)
{
  ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲುಗಳು = screen->ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ScreenCell *old_ಬಫರ್ = screen->buffers[bufidx];
  VTerm_ಸಾಲು_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_ಬಫರ್ = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(ScreenCell) * new_ಅಡ್ಡಸಾಲುಗಳು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  VTerm_ಸಾಲು_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(new_lineinfo[0]) * new_ಅಡ್ಡಸಾಲುಗಳು);

  // Find the final row of old buffer content
  ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲುಗಳು - 1;
  ಪೂರ್ಣಾಂಕ new_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲುಗಳು - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, old_ಅಡ್ಡಸಾಲುಗಳು, new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, new_ಅಡ್ಡಸಾಲುಗಳು, old_cursor.ಲಂಬಸಾಲು, old_cursor.ಅಡ್ಡಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  ಪೂರ್ಣಾಂಕ final_blank_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲುಗಳು;

  ಆಗ(old_ಅಡ್ಡಸಾಲು >= 0) {
    ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲು_ಕೊನೆ = old_ಅಡ್ಡಸಾಲು;
    /* TODO: Stop if dwl or dhl */
    ಆಗ(REFLOW && old_lineinfo && old_ಅಡ್ಡಸಾಲು >= 0 && old_lineinfo[old_ಅಡ್ಡಸಾಲು].continuation)
      old_ಅಡ್ಡಸಾಲು--;
    ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲು_ಆರಂಭ = old_ಅಡ್ಡಸಾಲು;

    ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು_ಆರಂಭ; ಅಡ್ಡಸಾಲು <= old_ಅಡ್ಡಸಾಲು_ಕೊನೆ; ಅಡ್ಡಸಾಲು++) {
      ಆದರೆ(REFLOW && ಅಡ್ಡಸಾಲು < (old_ಅಡ್ಡಸಾಲುಗಳು - 1) && old_lineinfo[ಅಡ್ಡಸಾಲು + 1].continuation)
        ಅಗಲ += old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಇಲ್ಲದಿದ್ದರೆ
        ಅಗಲ += ಸಾಲು_popcount(old_ಬಫರ್, ಅಡ್ಡಸಾಲು, old_ಅಡ್ಡಸಾಲುಗಳು, old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    }

    ಆದರೆ(final_blank_ಅಡ್ಡಸಾಲು == (new_ಅಡ್ಡಸಾಲು + 1) && ಅಗಲ == 0)
      final_blank_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು;

    ಪೂರ್ಣಾಂಕ new_ಎತ್ತರ = REFLOW
      ? ಅಗಲ ? (ಅಗಲ + new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) / new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ : 1
      : 1;

    ಪೂರ್ಣಾಂಕ new_ಅಡ್ಡಸಾಲು_ಕೊನೆ = new_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ new_ಅಡ್ಡಸಾಲು_ಆರಂಭ = new_ಅಡ್ಡಸಾಲು - new_ಎತ್ತರ + 1;

    old_ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ old_ಲಂಬಸಾಲು = 0;

    ಪೂರ್ಣಾಂಕ ಹೆಚ್ಚುವರಿ_ಅಡ್ಡಸಾಲುಗಳು = new_ಅಡ್ಡಸಾಲುಗಳು - final_blank_ಅಡ್ಡಸಾಲು;

    ಆದರೆ(new_ಅಡ್ಡಸಾಲು_ಆರಂಭ < 0 && /* we'd fall off the top */
        ಹೆಚ್ಚುವರಿ_ಅಡ್ಡಸಾಲುಗಳು >= 0 && /* we actually have spare rows */
        (!ಸಕ್ರಿಯ || new_cursor.ಅಡ್ಡಸಾಲು == -1 || (new_cursor.ಅಡ್ಡಸಾಲು - new_ಅಡ್ಡಸಾಲು_ಆರಂಭ) < new_ಅಡ್ಡಸಾಲುಗಳು))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      ಪೂರ್ಣಾಂಕ downwards = -new_ಅಡ್ಡಸಾಲು_ಆರಂಭ;
      ಆದರೆ(downwards > ಹೆಚ್ಚುವರಿ_ಅಡ್ಡಸಾಲುಗಳು)
        downwards = ಹೆಚ್ಚುವರಿ_ಅಡ್ಡಸಾಲುಗಳು;
      ಪೂರ್ಣಾಂಕ rowcount = new_ಅಡ್ಡಸಾಲುಗಳು - downwards;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#ಆದರೆ_ಅಂತ್ಯ

      memmove(&new_ಬಫರ್[downwards * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ], &new_ಬಫರ್[0],   rowcount * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * ಗಾತ್ರ(new_lineinfo[0]));

      new_ಅಡ್ಡಸಾಲು += downwards;
      new_ಅಡ್ಡಸಾಲು_ಆರಂಭ += downwards;
      new_ಅಡ್ಡಸಾಲು_ಕೊನೆ += downwards;

      ಆದರೆ(new_cursor.ಅಡ್ಡಸಾಲು >= 0)
        new_cursor.ಅಡ್ಡಸಾಲು += downwards;

      final_blank_ಅಡ್ಡಸಾಲು += downwards;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_ಅಡ್ಡಸಾಲು_ಆರಂಭ, new_ಅಡ್ಡಸಾಲು_ಕೊನೆ, old_ಅಡ್ಡಸಾಲು_ಆರಂಭ, old_ಅಡ್ಡಸಾಲು_ಕೊನೆ, ಅಗಲ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(new_ಅಡ್ಡಸಾಲು_ಆರಂಭ < 0) {
      ಆದರೆ(old_ಅಡ್ಡಸಾಲು_ಆರಂಭ <= old_cursor.ಅಡ್ಡಸಾಲು && old_cursor.ಅಡ್ಡಸಾಲು < old_ಅಡ್ಡಸಾಲು_ಕೊನೆ) {
        new_cursor.ಅಡ್ಡಸಾಲು = 0;
        new_cursor.ಲಂಬಸಾಲು = old_cursor.ಲಂಬಸಾಲು;
        ಆದರೆ(new_cursor.ಲಂಬಸಾಲು >= new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
          new_cursor.ಲಂಬಸಾಲು = new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1;
      }
      ನಿಲ್ಲಿಸು;
    }

    ಗಾಗಿ(new_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು_ಆರಂಭ, old_ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು_ಆರಂಭ; new_ಅಡ್ಡಸಾಲು <= new_ಅಡ್ಡಸಾಲು_ಕೊನೆ; new_ಅಡ್ಡಸಾಲು++) {
      ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = ಅಗಲ >= new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ ? new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ : ಅಗಲ;
      ಅಗಲ -= ಎಣಿಕೆ_2;

      ಪೂರ್ಣಾಂಕ new_ಲಂಬಸಾಲು = 0;

      ಆಗ(ಎಣಿಕೆ_2) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_ಬಫರ್[new_ಅಡ್ಡಸಾಲು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + new_ಲಂಬಸಾಲು] = old_ಬಫರ್[old_ಅಡ್ಡಸಾಲು * old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + old_ಲಂಬಸಾಲು];

        ಆದರೆ(old_cursor.ಅಡ್ಡಸಾಲು == old_ಅಡ್ಡಸಾಲು && old_cursor.ಲಂಬಸಾಲು == old_ಲಂಬಸಾಲು)
          new_cursor.ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು, new_cursor.ಲಂಬಸಾಲು = new_ಲಂಬಸಾಲು;

        old_ಲಂಬಸಾಲು++;
        ಆದರೆ(old_ಲಂಬಸಾಲು == old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
          old_ಅಡ್ಡಸಾಲು++;

          ಆದರೆ(!REFLOW) {
            new_ಲಂಬಸಾಲು++;
            ನಿಲ್ಲಿಸು;
          }
          old_ಲಂಬಸಾಲು = 0;
        }

        new_ಲಂಬಸಾಲು++;
        ಎಣಿಕೆ_2--;
      }

      ಆದರೆ(old_cursor.ಅಡ್ಡಸಾಲು == old_ಅಡ್ಡಸಾಲು && old_cursor.ಲಂಬಸಾಲು >= old_ಲಂಬಸಾಲು) {
        new_cursor.ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು, new_cursor.ಲಂಬಸಾಲು = (old_cursor.ಲಂಬಸಾಲು - old_ಲಂಬಸಾಲು + new_ಲಂಬಸಾಲು);
        ಆದರೆ(new_cursor.ಲಂಬಸಾಲು >= new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
          new_cursor.ಲಂಬಸಾಲು = new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1;
      }

      ಆಗ(new_ಲಂಬಸಾಲು < new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
        clearcell(screen, &new_ಬಫರ್[new_ಅಡ್ಡಸಾಲು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + new_ಲಂಬಸಾಲು]);
        new_ಲಂಬಸಾಲು++;
      }

      new_lineinfo[new_ಅಡ್ಡಸಾಲು].continuation = (new_ಅಡ್ಡಸಾಲು > new_ಅಡ್ಡಸಾಲು_ಆರಂಭ);
    }

    old_ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು_ಆರಂಭ - 1;
    new_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು_ಆರಂಭ - 1;
  }

  ಆದರೆ(old_cursor.ಅಡ್ಡಸಾಲು <= old_ಅಡ್ಡಸಾಲು) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.ಅಡ್ಡಸಾಲು = 0, new_cursor.ಲಂಬಸಾಲು = old_cursor.ಲಂಬಸಾಲು;
    ಆದರೆ(new_cursor.ಲಂಬಸಾಲು >= new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
      new_cursor.ಲಂಬಸಾಲು = new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ಆದರೆ(ಸಕ್ರಿಯ && (new_cursor.ಅಡ್ಡಸಾಲು == -1 || new_cursor.ಲಂಬಸಾಲು == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ಆದರೆ (new_cursor.ಅಡ್ಡಸಾಲು < 0)
      new_cursor.ಅಡ್ಡಸಾಲು = 0;
    ಆದರೆ (new_cursor.ಲಂಬಸಾಲು < 0)
      new_cursor.ಲಂಬಸಾಲು = 0;
  }

  ಆದರೆ(old_ಅಡ್ಡಸಾಲು >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ಆದರೆ((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು <= old_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++) {
        ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *lineinfo = old_lineinfo + ಅಡ್ಡಸಾಲು;
        sb_pushline_from_ಅಡ್ಡಸಾಲು(screen, ಅಡ್ಡಸಾಲು, lineinfo->continuation);
      }
    ಆದರೆ(ಸಕ್ರಿಯ)
      statefields->pos.ಅಡ್ಡಸಾಲು -= (old_ಅಡ್ಡಸಾಲು + 1);
  }
  ಆದರೆ(new_ಅಡ್ಡಸಾಲು >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    ಆಗ(new_ಅಡ್ಡಸಾಲು >= 0) {
      VTermPos pos;
      ಆದರೆ(!(screen->callbacks->sb_popline(old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, screen->sb_ಬಫರ್, screen->cbdata)))
        ನಿಲ್ಲಿಸು;

      pos.ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು;
      ಗಾಗಿ(pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ && pos.ಲಂಬಸಾಲು < new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; pos.ಲಂಬಸಾಲು += screen->sb_ಬಫರ್[pos.ಲಂಬಸಾಲು].ಅಗಲ) {
        VTermScreenCell *src = &screen->sb_ಬಫರ್[pos.ಲಂಬಸಾಲು];
        ScreenCell *dst = &new_ಬಫರ್[pos.ಅಡ್ಡಸಾಲು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + pos.ಲಂಬಸಾಲು];

        ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ಆದರೆ(!src->chars[i])
            ನಿಲ್ಲಿಸು;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.ಹಿಮ್ಮುಖ   = src->attrs.ಹಿಮ್ಮುಖ ^ screen->global_ಹಿಮ್ಮುಖ;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        ಆದರೆ(src->ಅಗಲ == 2 && pos.ಲಂಬಸಾಲು < (new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      ಗಾಗಿ( ; pos.ಲಂಬಸಾಲು < new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; pos.ಲಂಬಸಾಲು++)
        clearcell(screen, &new_ಬಫರ್[pos.ಅಡ್ಡಸಾಲು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + pos.ಲಂಬಸಾಲು]);
      new_ಅಡ್ಡಸಾಲು--;

      ಆದರೆ(ಸಕ್ರಿಯ)
        statefields->pos.ಅಡ್ಡಸಾಲು++;
    }
  }
  ಆದರೆ(new_ಅಡ್ಡಸಾಲು >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    ಪೂರ್ಣಾಂಕ moverows = new_ಅಡ್ಡಸಾಲುಗಳು - new_ಅಡ್ಡಸಾಲು - 1;
    memmove(&new_ಬಫರ್[0], &new_ಬಫರ್[(new_ಅಡ್ಡಸಾಲು + 1) * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ], moverows * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_ಅಡ್ಡಸಾಲು + 1], moverows * ಗಾತ್ರ(new_lineinfo[0]));

    new_cursor.ಅಡ್ಡಸಾಲು -= (new_ಅಡ್ಡಸಾಲು + 1);

    ಗಾಗಿ(new_ಅಡ್ಡಸಾಲು = moverows; new_ಅಡ್ಡಸಾಲು < new_ಅಡ್ಡಸಾಲುಗಳು; new_ಅಡ್ಡಸಾಲು++) {
      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++)
        clearcell(screen, &new_ಬಫರ್[new_ಅಡ್ಡಸಾಲು * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು]);
      new_lineinfo[new_ಅಡ್ಡಸಾಲು] = (VTerm_ಸಾಲು_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_ಬಫರ್);
  screen->buffers[bufidx] = new_ಬಫರ್;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ಆದರೆ(ಸಕ್ರಿಯ)
    statefields->pos = new_cursor;

  ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ resize(ಪೂರ್ಣಾಂಕ new_ಅಡ್ಡಸಾಲುಗಳು, ಪೂರ್ಣಾಂಕ new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermStateFields *fields, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಪೂರ್ಣಾಂಕ altscreen_ಸಕ್ರಿಯ = (screen->buffers[BUFIDX_ALTSCREEN] && screen->ಬಫರ್ == screen->buffers[BUFIDX_ALTSCREEN]);

  ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲುಗಳು = screen->ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ(new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ಆದರೆ(screen->sb_ಬಫರ್)
      vterm_allocator_free(screen->vt, screen->sb_ಬಫರ್);

    ಆದರೆ (new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
      new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    screen->sb_ಬಫರ್ = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(VTermScreenCell) * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  }

  ಆದರೆ (new_ಅಡ್ಡಸಾಲುಗಳು > VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು)
    new_ಅಡ್ಡಸಾಲುಗಳು = VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು;

  resize_ಬಫರ್(screen, 0, new_ಅಡ್ಡಸಾಲುಗಳು, new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, !altscreen_ಸಕ್ರಿಯ, fields);
  ಆದರೆ(screen->buffers[BUFIDX_ALTSCREEN])
    resize_ಬಫರ್(screen, 1, new_ಅಡ್ಡಸಾಲುಗಳು, new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, altscreen_ಸಕ್ರಿಯ, fields);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಅಡ್ಡಸಾಲುಗಳು != old_ಅಡ್ಡಸಾಲುಗಳು) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_ಸಾಲು_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(new_lineinfo[0]) * new_ಅಡ್ಡಸಾಲುಗಳು);
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < new_ಅಡ್ಡಸಾಲುಗಳು; ಅಡ್ಡಸಾಲು++)
      new_lineinfo[ಅಡ್ಡಸಾಲು] = (VTerm_ಸಾಲು_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->ಬಫರ್ = altscreen_ಸಕ್ರಿಯ ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->ಅಡ್ಡಸಾಲುಗಳು_2 = new_ಅಡ್ಡಸಾಲುಗಳು;
  screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ(new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ <= old_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(screen->sb_ಬಫರ್)
      vterm_allocator_free(screen->vt, screen->sb_ಬಫರ್);

    screen->sb_ಬಫರ್ = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(VTermScreenCell) * new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ಆದರೆ(screen->callbacks && screen->callbacks->resize)
    ಹಿಂತಿರುಗಿ (*screen->callbacks->resize)(new_ಅಡ್ಡಸಾಲುಗಳು, new_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, screen->cbdata);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ setlineinfo(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *newinfo, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *oldinfo, ಶೂನ್ಯ *user)
{
  VTermScreen *screen = user;

  ಆದರೆ(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
      ScreenCell *cell = getcell(screen, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
      ಆದರೆ (cell == ಖಾಲಿ)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = ಅಡ್ಡಸಾಲು + 1;
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0;
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = newinfo->doublewidth ? screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ / 2 : screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    damagerect(screen, rect);

    ಆದರೆ(newinfo->doublewidth) {
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ / 2;
      rect.ಕೊನೆ_ಲಂಬಸಾಲು   = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

      erase_ಆಂತರಿಕ(rect, 0, user);
    }
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sb_clear(ಶೂನ್ಯ *user) {
  VTermScreen *screen = user;

  ಆದರೆ(screen->callbacks && screen->callbacks->sb_clear)
    ಆದರೆ((*screen->callbacks->sb_clear)(screen->cbdata))
      ಹಿಂತಿರುಗಿ 1;

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  ಖಾಲಿ, // moverect
  &erase, // erase
  ಖಾಲಿ, // initpen
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
ಸ್ಥಿರ VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  ಆದರೆ(!state)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  VTermScreen *screen = vterm_allocator_malloc(vt, ಗಾತ್ರ(VTermScreen));
  ಆದರೆ (screen == ಖಾಲಿ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  vterm_get_ಗಾತ್ರ(vt, &ಅಡ್ಡಸಾಲುಗಳು_2, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

  screen->vt = vt;
  screen->state = state;

  screen->damage_ವಿಲೀನ = VTERM_DAMAGE_CELL;
  screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;
  screen->pending_scrollrect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;

  screen->ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2;
  screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  screen->global_ಹಿಮ್ಮುಖ = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = ಖಾಲಿ;
  screen->cbdata    = ಖಾಲಿ;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_ಬಫರ್(screen, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

  screen->ಬಫರ್ = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_ಬಫರ್ = vterm_allocator_malloc(screen->vt, ಗಾತ್ರ(VTermScreenCell) * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  ಆದರೆ (screen->ಬಫರ್ == ಖಾಲಿ || screen->sb_ಬಫರ್ == ಖಾಲಿ)
  {
    vterm_screen_free(screen);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  vterm_state_ಗಣ_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ಹಿಂತಿರುಗಿ screen;
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ಆದರೆ(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_ಬಫರ್);

  vterm_allocator_free(screen->vt, screen);
}

ಶೂನ್ಯ vterm_screen_reset(VTermScreen *screen, ಪೂರ್ಣಾಂಕ hard)
{
  screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;
  screen->pending_scrollrect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ಸ್ಥಿರ size_t _get_chars(ಸ್ಥಿರಾಂಕ VTermScreen *screen, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ utf8, ಶೂನ್ಯ *ಬಫರ್, size_t len, ಸ್ಥಿರಾಂಕ VTermRect rect)
{
  size_t outpos = 0;
  ಪೂರ್ಣಾಂಕ padding = 0;

#ವ್ಯಾಖ್ಯಾನಿಸು PUT(c)                                             \
  ಆದರೆ(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ಆದರೆ(ಬಫರ್ && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (ಅಕ್ಷರ *)ಬಫರ್ + outpos);   \
    ಇಲ್ಲದಿದ್ದರೆ                                                   \
      outpos += thislen;                                   \
  }                                                        \
  ಇಲ್ಲದಿದ್ದರೆ {                                                   \
    ಆದರೆ(ಬಫರ್ && outpos + 1 <= len)                        \
      ((uint32_t*)ಬಫರ್)[outpos++] = (c);                 \
    ಇಲ್ಲದಿದ್ದರೆ                                                   \
      outpos++;                                            \
  }

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++) {
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು < rect.ಕೊನೆ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು++) {
      ScreenCell *cell = getcell(screen, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);

      ಆದರೆ (cell == ಖಾಲಿ)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ 1;
      }
      ಆದರೆ(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ಆದರೆ(ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ಹಿಂತಿರುಗಿ outpos;
}

size_t vterm_screen_get_chars(ಸ್ಥಿರಾಂಕ VTermScreen *screen, uint32_t *chars, size_t len, ಸ್ಥಿರಾಂಕ VTermRect rect)
{
  ಹಿಂತಿರುಗಿ _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ VTermScreen *screen, ಅಕ್ಷರ *str, size_t len, ಸ್ಥಿರಾಂಕ VTermRect rect)
{
  ಹಿಂತಿರುಗಿ _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
ಪೂರ್ಣಾಂಕ vterm_screen_get_cell(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);

  ಆದರೆ(!intcell)
    ಹಿಂತಿರುಗಿ 0;

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ಆದರೆ(!intcell->chars[i])
      ನಿಲ್ಲಿಸು;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.ಹಿಮ್ಮುಖ   = intcell->pen.ಹಿಮ್ಮುಖ ^ screen->global_ಹಿಮ್ಮುಖ;
  cell->attrs.conceal   = intcell->pen.conceal;
  cell->attrs.strike    = intcell->pen.strike;
  cell->attrs.font      = intcell->pen.font;
  cell->attrs.small     = intcell->pen.small;
  cell->attrs.baseline  = intcell->pen.baseline;

  cell->attrs.dwl = intcell->pen.dwl;
  cell->attrs.dhl = intcell->pen.dhl;

  cell->fg = intcell->pen.fg;
  cell->bg = intcell->pen.bg;

  ಆದರೆ(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ಆದರೆ(pos.ಲಂಬಸಾಲು < (screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) &&
       getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು + 1)->chars[0] == (uint32_t)-1) {
      ಆದರೆ(getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು)->chars[0] == 0x20) {
        getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು)->chars[0] = 0;
        cell->ಅಗಲ = 2;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು)->chars[0] == 0) {
        getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು + 1)->chars[0] = 0;
        cell->ಅಗಲ = 1;
      } ಇಲ್ಲದಿದ್ದರೆ {
        cell->ಅಗಲ = 2;
      }
    } ಇಲ್ಲದಿದ್ದರೆ
      cell->ಅಗಲ = 1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(pos.ಲಂಬಸಾಲು < (screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) &&
       getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು + 1)->chars[0] == (uint32_t)-1)
      cell->ಅಗಲ = 2;
    ಇಲ್ಲದಿದ್ದರೆ
      cell->ಅಗಲ = 1;
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ vterm_screen_is_eol(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  ಗಾಗಿ(; pos.ಲಂಬಸಾಲು < screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; pos.ಲಂಬಸಾಲು++) {
    ScreenCell *cell = getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);
    ಆದರೆ(cell->chars[0] != 0)
      ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ಆದರೆ(vt->screen)
    ಹಿಂತಿರುಗಿ vt->screen;

  vt->screen = screen_new(vt);
  ಹಿಂತಿರುಗಿ vt->screen;
}

ಶೂನ್ಯ vterm_screen_enable_reflow(VTermScreen *screen, ಪೂರ್ಣಾಂಕ reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

ಶೂನ್ಯ vterm_screen_enable_altscreen(VTermScreen *screen, ಪೂರ್ಣಾಂಕ altscreen)
{
  ಆದರೆ(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    vterm_get_ಗಾತ್ರ(screen->vt, &ಅಡ್ಡಸಾಲುಗಳು_2, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_ಬಫರ್(screen, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  }
}

ಶೂನ್ಯ vterm_screen_ಗಣ_callbacks(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermScreenCallbacks *callbacks, ಶೂನ್ಯ *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

ಶೂನ್ಯ *vterm_screen_get_cbdata(VTermScreen *screen)
{
  ಹಿಂತಿರುಗಿ screen->cbdata;
}

ಶೂನ್ಯ vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

ಶೂನ್ಯ vterm_screen_ಗಣ_unrecognised_fallbacks(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user)
{
  vterm_state_ಗಣ_unrecognised_fallbacks(screen->state, fallbacks, user);
}

ಶೂನ್ಯ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  ಹಿಂತಿರುಗಿ vterm_state_get_unrecognised_fbdata(screen->state);
}

ಶೂನ್ಯ vterm_screen_flush_damage(VTermScreen *screen)
{
  ಆದರೆ(screen->pending_scrollrect.ಆರಂಭ_ಅಡ್ಡಸಾಲು != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;
  }

  ಆದರೆ(screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು != -1) {
    ಆದರೆ(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.ಆರಂಭ_ಅಡ್ಡಸಾಲು = -1;
  }
}

ಶೂನ್ಯ vterm_screen_ಗಣ_damage_ವಿಲೀನ(VTermScreen *screen, VTerm_Damage_ಗಾತ್ರ ಗಾತ್ರ_2)
{
  vterm_screen_flush_damage(screen);
  screen->damage_ವಿಲೀನ = ಗಾತ್ರ_2;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ attrs_ವ್ಯತ್ಯಾಸ(VTerm_Attr_ಮುಖವಾಡ attrs, ScreenCell *a, ScreenCell *b)
{
  ಆದರೆ((attrs & VTERM_ATTR_BOLD_ಮುಖವಾಡ)       && (a->pen.bold != b->pen.bold))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_UNDERLINE_ಮುಖವಾಡ)  && (a->pen.underline != b->pen.underline))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_ITALIC_ಮುಖವಾಡ)     && (a->pen.italic != b->pen.italic))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_BLINK_ಮುಖವಾಡ)      && (a->pen.blink != b->pen.blink))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_ಹಿಮ್ಮುಖ_ಮುಖವಾಡ)    && (a->pen.ಹಿಮ್ಮುಖ != b->pen.ಹಿಮ್ಮುಖ))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_CONCEAL_ಮುಖವಾಡ)    && (a->pen.conceal != b->pen.conceal))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_STRIKE_ಮುಖವಾಡ)     && (a->pen.strike != b->pen.strike))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_FONT_ಮುಖವಾಡ)       && (a->pen.font != b->pen.font))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_FOREGROUND_ಮುಖವಾಡ) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_BACKGROUND_ಮುಖವಾಡ) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_SMALL_ಮುಖವಾಡ)    && (a->pen.small != b->pen.small))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ((attrs & VTERM_ATTR_BASELINE_ಮುಖವಾಡ)    && (a->pen.baseline != b->pen.baseline))
    ಹಿಂತಿರುಗಿ 1;

  ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ vterm_screen_get_attrs_extent(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ಮುಖವಾಡ attrs)
{
  ScreenCell *ಗುರಿ = getcell(screen, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);

  // TODO: bounds check
  extent->ಆರಂಭ_ಅಡ್ಡಸಾಲು = pos.ಅಡ್ಡಸಾಲು;
  extent->ಕೊನೆ_ಅಡ್ಡಸಾಲು   = pos.ಅಡ್ಡಸಾಲು + 1;

  ಆದರೆ(extent->ಆರಂಭ_ಲಂಬಸಾಲು < 0)
    extent->ಆರಂಭ_ಲಂಬಸಾಲು = 0;
  ಆದರೆ(extent->ಕೊನೆ_ಲಂಬಸಾಲು < 0)
    extent->ಕೊನೆ_ಲಂಬಸಾಲು = screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

  ಗಾಗಿ(ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು - 1; ಲಂಬಸಾಲು >= extent->ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು--)
    ಆದರೆ(attrs_ವ್ಯತ್ಯಾಸ(attrs, ಗುರಿ, getcell(screen, pos.ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು)))
      ನಿಲ್ಲಿಸು;
  extent->ಆರಂಭ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + 1;

  ಗಾಗಿ(ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು + 1; ಲಂಬಸಾಲು < extent->ಕೊನೆ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು++)
    ಆದರೆ(attrs_ವ್ಯತ್ಯಾಸ(attrs, ಗುರಿ, getcell(screen, pos.ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು)))
      ನಿಲ್ಲಿಸು;
  extent->ಕೊನೆ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1;

  ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ vterm_screen_convert_color_ಗಮ್ಯ_rgb(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermColor *ಲಂಬಸಾಲು)
{
  vterm_state_convert_color_ಗಮ್ಯ_rgb(screen->state, ಲಂಬಸಾಲು);
}

ಸ್ಥಿರ ಶೂನ್ಯ reset_default_colours(VTermScreen *screen, ScreenCell *ಬಫರ್)
{
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು <= screen->ಅಡ್ಡಸಾಲುಗಳು_2 - 1; ಅಡ್ಡಸಾಲು++)
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು <= screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1; ಲಂಬಸಾಲು++) {
      ScreenCell *cell = &ಬಫರ್[ಅಡ್ಡಸಾಲು * screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು];
      ಆದರೆ(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ಆದರೆ(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

ಶೂನ್ಯ vterm_screen_ಗಣ_default_colors(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermColor *default_fg, ಸ್ಥಿರಾಂಕ VTermColor *default_bg)
{
  vterm_state_ಗಣ_default_colors(screen->state, default_fg, default_bg);

  ಆದರೆ(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_ಮುಖವಾಡ)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ಆದರೆ(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_ಮುಖವಾಡ)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ಆದರೆ(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
