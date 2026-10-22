#define DEFINE_INLINES

// vim: set sw=2 :
#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <stdarg.h>
#شامل <string.h>

#شامل "utf8.h"

/*****************
 * API functions *
 *****************/

ایستا تهی *default_malloc(size_t size, تهی *allocdata UNUSED)
{
  تهی *ptr = malloc(size);
  اگر(ptr)
    memset(ptr, 0, size);
  بازگشت ptr;
}

ایستا تهی default_free(تهی *ptr, تهی *allocdata UNUSED)
{
  آزاد(ptr);
}

ایستا VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحیح rows, صحیح cols)
{
  ساختار VTermBuilder builder;
  memset(&builder, 0, اندازه(builder));
  builder.rows = rows;
  builder.cols = cols;
  بازگشت vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحیح rows, صحیح cols, VTermAllocatorFunctions *funcs, تهی *allocdata)
{
  ساختار VTermBuilder builder;
  memset(&builder, 0, اندازه(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  بازگشت vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت ساختار VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(اندازه(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = پوچ;
  vt->parser.cbdata    = پوچ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = پوچ;
  vt->outdata = پوچ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  اگر (vt->tmpbuffer == پوچ
      || vt->outbuffer == پوچ
      || vt->tmpbuffer == پوچ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    بازگشت پوچ;
  }

  بازگشت vt;
}

تهی vterm_free(VTerm *vt)
{
  اگر(vt->screen)
    vterm_screen_free(vt->screen);

  اگر(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL تهی *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  بازگشت (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL تهی vterm_allocator_free(VTerm *vt, تهی *ptr)
{
  اگر (ptr)
    (*vt->allocator->آزاد)(ptr, vt->allocdata);
}

تهی vterm_get_size(ثابت VTerm *vt, صحیح *rowsp, صحیح *colsp)
{
  اگر(rowsp)
    *rowsp = vt->rows;
  اگر(colsp)
    *colsp = vt->cols;
}

تهی vterm_set_size(VTerm *vt, صحیح rows, صحیح cols)
{
  اگر(rows < 1 || cols < 1)
    بازگشت;

  vt->rows = rows;
  vt->cols = cols;

  اگر(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

صحیح vterm_get_utf8(ثابت VTerm *vt)
{
  بازگشت vt->mode.utf8;
}

تهی vterm_set_utf8(VTerm *vt, صحیح is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

تهی vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, تهی *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL تهی vterm_افزودن_output_bytes(VTerm *vt, ثابت نویسه *bytes, size_t len)
{
  اگر(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    بازگشت;
  }

  اگر(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    بازگشت;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL تهی vterm_افزودن_output_vsprintf(VTerm *vt, ثابت نویسه *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#وگرنه
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  اگر (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_افزودن_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL تهی vterm_افزودن_output_sprintf(VTerm *vt, ثابت نویسه *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_افزودن_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL تهی vterm_افزودن_output_sprintf_ctrl(VTerm *vt, unsigned نویسه ctrl, ثابت نویسه *fmt, ...)
{
  size_t cur;

  اگر(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وگرنه
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  اگر(cur >= vt->tmpbuffer_len)
    بازگشت;
  vterm_افزودن_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_افزودن_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL تهی vterm_افزودن_output_sprintf_str(VTerm *vt, unsigned نویسه ctrl, صحیح term, ثابت نویسه *fmt, ...)
{
  size_t cur;

  اگر(ctrl) {
    اگر(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وگرنه
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    اگر(cur >= vt->tmpbuffer_len)
      بازگشت;
    vterm_افزودن_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_افزودن_output_vsprintf(vt, fmt, args);
  va_end(args);

  اگر(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    اگر(cur >= vt->tmpbuffer_len)
      بازگشت;
    vterm_افزودن_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ثابت VTerm *vt)
{
  بازگشت vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ثابت VTerm *vt)
{
  بازگشت vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ثابت VTerm *vt)
{
  بازگشت vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, نویسه *buffer, size_t len)
{
  اگر(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  اگر(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  بازگشت len;
}

VTerm_مقدار_Type vterm_get_attr_type(VTermAttr attr)
{
  انتخاب(attr) {
    حالت VTERM_ATTR_BOLD:       بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_UNDERLINE:  بازگشت VTERM_VALUETYPE_INT;
    حالت VTERM_ATTR_ITALIC:     بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_BLINK:      بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_REVERSE:    بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_CONCEAL:    بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_STRIKE:     بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_FONT:       بازگشت VTERM_VALUETYPE_INT;
    حالت VTERM_ATTR_FOREGROUND: بازگشت VTERM_VALUETYPE_COLOR;
    حالت VTERM_ATTR_BACKGROUND: بازگشت VTERM_VALUETYPE_COLOR;
    حالت VTERM_ATTR_SMALL:      بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_BASELINE:   بازگشت VTERM_VALUETYPE_INT;

    حالت VTERM_N_ATTRS: بازگشت 0;
  }
  بازگشت 0; /* UNREACHABLE */
}

VTerm_مقدار_Type vterm_get_prop_type(VTermProp prop)
{
  انتخاب(prop) {
    حالت VTERM_PROP_CURSORVISIBLE: بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORBLINK:   بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_ALTSCREEN:     بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_TITLE:         بازگشت VTERM_VALUETYPE_STRING;
    حالت VTERM_PROP_ICONNAME:      بازگشت VTERM_VALUETYPE_STRING;
    حالت VTERM_PROP_REVERSE:       بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORSHAPE:   بازگشت VTERM_VALUETYPE_INT;
    حالت VTERM_PROP_MOUSE:         بازگشت VTERM_VALUETYPE_INT;
    حالت VTERM_PROP_FOCUSREPORT:   بازگشت VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORCOLOR:   بازگشت VTERM_VALUETYPE_STRING;

    حالت VTERM_N_PROPS: بازگشت 0;
  }
  بازگشت 0; /* UNREACHABLE */
}

تهی vterm_scroll_rect(VTermRect rect,
    صحیح downward,
    صحیح rightward,
    صحیح (*moverect)(VTermRect src, VTermRect dest, تهی *user),
    صحیح (*eraserect)(VTermRect rect, صحیح selective, تهی *user),
    تهی *user)
{
  VTermRect src;
  VTermRect dest;

  اگر(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    بازگشت;
  }

  اگر(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  وگرنه {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحیح leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  اگر(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  وگرنه {
    صحیح upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  اگر(moverect)
    (*moverect)(dest, src, user);

  اگر(downward > 0)
    rect.start_row = rect.end_row - downward;
  وگرنه اگر(downward < 0)
    rect.end_row = rect.start_row - downward;

  اگر(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  وگرنه اگر(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

تهی vterm_copy_cells(VTermRect dest,
    VTermRect src,
    تهی (*copycell)(VTermPos dest, VTermPos src, تهی *user),
    تهی *user)
{
  صحیح downward  = src.start_row - dest.start_row;
  صحیح rightward = src.start_col - dest.start_col;

  صحیح init_row, test_row, init_col, test_col;
  صحیح inc_row, inc_col;

  اگر(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  وگرنه /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  اگر(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  وگرنه /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  برای(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    برای(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

تهی vterm_check_version(صحیح major, صحیح minor)
{
  اگر(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  اگر(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
