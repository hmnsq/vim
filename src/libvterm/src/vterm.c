#define DEFINE_INLINES

// vim: set sw=2 :
#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdarg.h>
#ضمن <string.h>

#ضمن "utf8.h"

/*****************
 * API functions *
 *****************/

ساكن فراغ *default_malloc(size_t size, فراغ *allocdata UNUSED)
{
  فراغ *ptr = malloc(size);
  ܐܢ(ptr)
    memset(ptr, 0, size);
  ܦܢܐ ptr;
}

ساكن فراغ default_free(فراغ *ptr, فراغ *allocdata UNUSED)
{
  حرر(ptr);
}

ساكن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحيح rows, صحيح cols)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.rows = rows;
  builder.cols = cols;
  ܦܢܐ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح rows, صحيح cols, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ܦܢܐ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت بنية VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = عدم;
  vt->parser.cbdata    = عدم;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = عدم;
  vt->outdata = عدم;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ܐܢ (vt->tmpbuffer == عدم
      || vt->outbuffer == عدم
      || vt->tmpbuffer == عدم)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ܦܢܐ عدم;
  }

  ܦܢܐ vt;
}

فراغ vterm_free(VTerm *vt)
{
  ܐܢ(vt->screen)
    vterm_screen_free(vt->screen);

  ܐܢ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL فراغ *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ܦܢܐ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  ܐܢ (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_size(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  ܐܢ(rowsp)
    *rowsp = vt->rows;
  ܐܢ(colsp)
    *colsp = vt->cols;
}

فراغ vterm_set_size(VTerm *vt, صحيح rows, صحيح cols)
{
  ܐܢ(rows < 1 || cols < 1)
    ܦܢܐ;

  vt->rows = rows;
  vt->cols = cols;

  ܐܢ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  ܦܢܐ vt->mode.utf8;
}

فراغ vterm_set_utf8(VTerm *vt, صحيح is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

فراغ vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL فراغ vterm_push_output_bytes(VTerm *vt, ثابت حرف *bytes, size_t len)
{
  ܐܢ(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ܦܢܐ;
  }

  ܐܢ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ܦܢܐ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL فراغ vterm_push_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#ܐܚܪܢܐ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ܐܢ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_push_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL فراغ vterm_push_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_push_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL فراغ vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  ܐܢ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ܐܚܪܢܐ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ܐܢ(cur >= vt->tmpbuffer_len)
    ܦܢܐ;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL فراغ vterm_push_output_sprintf_str(VTerm *vt, unsigned حرف ctrl, صحيح term, ثابت حرف *fmt, ...)
{
  size_t cur;

  ܐܢ(ctrl) {
    ܐܢ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ܐܚܪܢܐ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ܐܢ(cur >= vt->tmpbuffer_len)
      ܦܢܐ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  ܐܢ(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ܐܢ(cur >= vt->tmpbuffer_len)
      ܦܢܐ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ثابت VTerm *vt)
{
  ܦܢܐ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ثابت VTerm *vt)
{
  ܦܢܐ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ثابت VTerm *vt)
{
  ܦܢܐ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *buffer, size_t len)
{
  ܐܢ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ܐܢ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ܦܢܐ len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  بدل(attr) {
    حالة VTERM_ATTR_BOLD:       ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_UNDERLINE:  ܦܢܐ VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_ITALIC:     ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BLINK:      ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_REVERSE:    ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_CONCEAL:    ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_STRIKE:     ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_FONT:       ܦܢܐ VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_FOREGROUND: ܦܢܐ VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_BACKGROUND: ܦܢܐ VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_SMALL:      ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BASELINE:   ܦܢܐ VTERM_VALUETYPE_INT;

    حالة VTERM_N_ATTRS: ܦܢܐ 0;
  }
  ܦܢܐ 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  بدل(prop) {
    حالة VTERM_PROP_CURSORVISIBLE: ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORBLINK:   ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_ALTSCREEN:     ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_TITLE:         ܦܢܐ VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_ICONNAME:      ܦܢܐ VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_REVERSE:       ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORSHAPE:   ܦܢܐ VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_MOUSE:         ܦܢܐ VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_FOCUSREPORT:   ܦܢܐ VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORCOLOR:   ܦܢܐ VTERM_VALUETYPE_STRING;

    حالة VTERM_N_PROPS: ܦܢܐ 0;
  }
  ܦܢܐ 0; /* UNREACHABLE */
}

فراغ vterm_scroll_rect(VTermRect rect,
    صحيح downward,
    صحيح rightward,
    صحيح (*moverect)(VTermRect src, VTermRect dest, فراغ *user),
    صحيح (*eraserect)(VTermRect rect, صحيح selective, فراغ *user),
    فراغ *user)
{
  VTermRect src;
  VTermRect dest;

  ܐܢ(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ܦܢܐ;
  }

  ܐܢ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  ܐܚܪܢܐ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحيح leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  ܐܢ(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  ܐܚܪܢܐ {
    صحيح upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ܐܢ(moverect)
    (*moverect)(dest, src, user);

  ܐܢ(downward > 0)
    rect.start_row = rect.end_row - downward;
  ܐܚܪܢܐ ܐܢ(downward < 0)
    rect.end_row = rect.start_row - downward;

  ܐܢ(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  ܐܚܪܢܐ ܐܢ(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

فراغ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
    فراغ *user)
{
  صحيح downward  = src.start_row - dest.start_row;
  صحيح rightward = src.start_col - dest.start_col;

  صحيح init_row, test_row, init_col, test_col;
  صحيح inc_row, inc_col;

  ܐܢ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  ܐܚܪܢܐ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ܐܢ(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  ܐܚܪܢܐ /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  لكل(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    لكل(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

فراغ vterm_check_version(صحيح major, صحيح minor)
{
  ܐܢ(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ܐܢ(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
