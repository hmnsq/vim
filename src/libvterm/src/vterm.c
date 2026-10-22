#define DEFINE_INLINES

// vim: set sw=2 :
#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "utf8.h"

/*****************
 * API functions *
 *****************/

ಸ್ಥಿರ ಶೂನ್ಯ *default_malloc(size_t size, ಶೂನ್ಯ *allocdata UNUSED)
{
  ಶೂನ್ಯ *ptr = malloc(size);
  ಯದಿ(ptr)
    memset(ptr, 0, size);
  ಪ್ರತ್ಯಾಗಮ ptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ default_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *allocdata UNUSED)
{
  ಮುಕ್ತ(ptr);
}

ಸ್ಥಿರ VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
  ಸಂರಚನಾ VTermBuilder builder;
  memset(&builder, 0, ಪರಿಮಾಣ(builder));
  builder.rows = rows;
  builder.cols = cols;
  ಪ್ರತ್ಯಾಗಮ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, VTermAllocatorFunctions *funcs, ಶೂನ್ಯ *allocdata)
{
  ಸಂರಚನಾ VTermBuilder builder;
  memset(&builder, 0, ಪರಿಮಾಣ(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ಪ್ರತ್ಯಾಗಮ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ಧ್ರುವ ಸಂರಚನಾ VTermBuilder *builder)
{
  ಧ್ರುವ VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(ಪರಿಮಾಣ(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = ರಿಕ್ತ;
  vt->parser.cbdata    = ರಿಕ್ತ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ರಿಕ್ತ;
  vt->outdata = ರಿಕ್ತ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ಯದಿ (vt->tmpbuffer == ರಿಕ್ತ
      || vt->outbuffer == ರಿಕ್ತ
      || vt->tmpbuffer == ರಿಕ್ತ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ vt;
}

ಶೂನ್ಯ vterm_free(VTerm *vt)
{
  ಯದಿ(vt->screen)
    vterm_screen_free(vt->screen);

  ಯದಿ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL ಶೂನ್ಯ *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ಪ್ರತ್ಯಾಗಮ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL ಶೂನ್ಯ vterm_allocator_free(VTerm *vt, ಶೂನ್ಯ *ptr)
{
  ಯದಿ (ptr)
    (*vt->allocator->ಮುಕ್ತ)(ptr, vt->allocdata);
}

ಶೂನ್ಯ vterm_get_size(ಧ್ರುವ VTerm *vt, ಪೂರ್ಣಾಙ್ಕ *rowsp, ಪೂರ್ಣಾಙ್ಕ *colsp)
{
  ಯದಿ(rowsp)
    *rowsp = vt->rows;
  ಯದಿ(colsp)
    *colsp = vt->cols;
}

ಶೂನ್ಯ vterm_set_size(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
  ಯದಿ(rows < 1 || cols < 1)
    ಪ್ರತ್ಯಾಗಮ;

  vt->rows = rows;
  vt->cols = cols;

  ಯದಿ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

ಪೂರ್ಣಾಙ್ಕ vterm_get_utf8(ಧ್ರುವ VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->mode.utf8;
}

ಶೂನ್ಯ vterm_set_utf8(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

ಶೂನ್ಯ vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ಶೂನ್ಯ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL ಶೂನ್ಯ vterm_ತಳ್ಳು_output_bytes(VTerm *vt, ಧ್ರುವ ವರ್ಣ *bytes, size_t len)
{
  ಯದಿ(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ಪ್ರತ್ಯಾಗಮ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL ಶೂನ್ಯ vterm_ತಳ್ಳು_output_vsprintf(VTerm *vt, ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#ಅನ್ಯಥಾ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ಯದಿ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ತಳ್ಳು_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf(VTerm *vt, ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_ತಳ್ಳು_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_ctrl(VTerm *vt, unsigned ವರ್ಣ ctrl, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  size_t cur;

  ಯದಿ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ಅನ್ಯಥಾ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ಯದಿ(cur >= vt->tmpbuffer_len)
    ಪ್ರತ್ಯಾಗಮ;
  vterm_ತಳ್ಳು_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_ತಳ್ಳು_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_str(VTerm *vt, unsigned ವರ್ಣ ctrl, ಪೂರ್ಣಾಙ್ಕ term, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  size_t cur;

  ಯದಿ(ctrl) {
    ಯದಿ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ಅನ್ಯಥಾ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ಯದಿ(cur >= vt->tmpbuffer_len)
      ಪ್ರತ್ಯಾಗಮ;
    vterm_ತಳ್ಳು_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_ತಳ್ಳು_output_vsprintf(vt, fmt, args);
  va_end(args);

  ಯದಿ(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ಯದಿ(cur >= vt->tmpbuffer_len)
      ಪ್ರತ್ಯಾಗಮ;
    vterm_ತಳ್ಳು_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ಧ್ರುವ VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ಧ್ರುವ VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ಧ್ರುವ VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, ವರ್ಣ *buffer, size_t len)
{
  ಯದಿ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ಯದಿ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ಪ್ರತ್ಯಾಗಮ len;
}

VTerm_ಮೌಲ್ಯ_Type vterm_get_attr_type(VTermAttr attr)
{
  ವಿಕಲ್ಪ(attr) {
    ಪಕ್ಷ VTERM_ATTR_BOLD:       ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_UNDERLINE:  ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_INT;
    ಪಕ್ಷ VTERM_ATTR_ITALIC:     ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_BLINK:      ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_REVERSE:    ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_CONCEAL:    ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_STRIKE:     ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_FONT:       ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_INT;
    ಪಕ್ಷ VTERM_ATTR_FOREGROUND: ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_COLOR;
    ಪಕ್ಷ VTERM_ATTR_BACKGROUND: ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_COLOR;
    ಪಕ್ಷ VTERM_ATTR_SMALL:      ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_ATTR_BASELINE:   ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_INT;

    ಪಕ್ಷ VTERM_N_ATTRS: ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 0; /* UNREACHABLE */
}

VTerm_ಮೌಲ್ಯ_Type vterm_get_prop_type(VTermProp prop)
{
  ವಿಕಲ್ಪ(prop) {
    ಪಕ್ಷ VTERM_PROP_CURSORVISIBLE: ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_PROP_CURSORBLINK:   ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_PROP_ALTSCREEN:     ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_PROP_TITLE:         ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_STRING;
    ಪಕ್ಷ VTERM_PROP_ICONNAME:      ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_STRING;
    ಪಕ್ಷ VTERM_PROP_REVERSE:       ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_PROP_CURSORSHAPE:   ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_INT;
    ಪಕ್ಷ VTERM_PROP_MOUSE:         ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_INT;
    ಪಕ್ಷ VTERM_PROP_FOCUSREPORT:   ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_BOOL;
    ಪಕ್ಷ VTERM_PROP_CURSORCOLOR:   ಪ್ರತ್ಯಾಗಮ VTERM_VALUETYPE_STRING;

    ಪಕ್ಷ VTERM_N_PROPS: ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 0; /* UNREACHABLE */
}

ಶೂನ್ಯ vterm_scroll_rect(VTermRect rect,
    ಪೂರ್ಣಾಙ್ಕ downward,
    ಪೂರ್ಣಾಙ್ಕ rightward,
    ಪೂರ್ಣಾಙ್ಕ (*moverect)(VTermRect src, VTermRect dest, ಶೂನ್ಯ *user),
    ಪೂರ್ಣಾಙ್ಕ (*eraserect)(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ selective, ಶೂನ್ಯ *user),
    ಶೂನ್ಯ *user)
{
  VTermRect src;
  VTermRect dest;

  ಯದಿ(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  ಅನ್ಯಥಾ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ಪೂರ್ಣಾಙ್ಕ leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  ಯದಿ(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  ಅನ್ಯಥಾ {
    ಪೂರ್ಣಾಙ್ಕ upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ಯದಿ(moverect)
    (*moverect)(dest, src, user);

  ಯದಿ(downward > 0)
    rect.start_row = rect.end_row - downward;
  ಅನ್ಯಥಾ ಯದಿ(downward < 0)
    rect.end_row = rect.start_row - downward;

  ಯದಿ(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  ಅನ್ಯಥಾ ಯದಿ(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

ಶೂನ್ಯ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    ಶೂನ್ಯ (*copycell)(VTermPos dest, VTermPos src, ಶೂನ್ಯ *user),
    ಶೂನ್ಯ *user)
{
  ಪೂರ್ಣಾಙ್ಕ downward  = src.start_row - dest.start_row;
  ಪೂರ್ಣಾಙ್ಕ rightward = src.start_col - dest.start_col;

  ಪೂರ್ಣಾಙ್ಕ init_row, test_row, init_col, test_col;
  ಪೂರ್ಣಾಙ್ಕ inc_row, inc_col;

  ಯದಿ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  ಅನ್ಯಥಾ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ಯದಿ(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  ಅನ್ಯಥಾ /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  ಪ್ರತಿ(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    ಪ್ರತಿ(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

ಶೂನ್ಯ vterm_check_version(ಪೂರ್ಣಾಙ್ಕ major, ಪೂರ್ಣಾಙ್ಕ minor)
{
  ಯದಿ(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ಯದಿ(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
