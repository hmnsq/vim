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
  إذا(ptr)
    memset(ptr, 0, size);
  أرجع ptr;
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
  أرجع vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح rows, صحيح cols, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  أرجع vterm_build(&builder);
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

  إذا (vt->tmpbuffer == عدم
      || vt->outbuffer == عدم
      || vt->tmpbuffer == عدم)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    أرجع عدم;
  }

  أرجع vt;
}

فراغ vterm_free(VTerm *vt)
{
  إذا(vt->screen)
    vterm_screen_free(vt->screen);

  إذا(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL فراغ *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  أرجع (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  إذا (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_size(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  إذا(rowsp)
    *rowsp = vt->rows;
  إذا(colsp)
    *colsp = vt->cols;
}

فراغ vterm_set_size(VTerm *vt, صحيح rows, صحيح cols)
{
  إذا(rows < 1 || cols < 1)
    أرجع;

  vt->rows = rows;
  vt->cols = cols;

  إذا(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  أرجع vt->mode.utf8;
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

INTERNAL فراغ vterm_پاڵنان_output_bytes(VTerm *vt, ثابت حرف *bytes, size_t len)
{
  إذا(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    أرجع;
  }

  إذا(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    أرجع;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL فراغ vterm_پاڵنان_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#وإلا
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  إذا (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_پاڵنان_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL فراغ vterm_پاڵنان_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_پاڵنان_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL فراغ vterm_پاڵنان_output_sprintf_ctrl(VTerm *vt, unsigned حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وإلا
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  إذا(cur >= vt->tmpbuffer_len)
    أرجع;
  vterm_پاڵنان_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_پاڵنان_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL فراغ vterm_پاڵنان_output_sprintf_str(VTerm *vt, unsigned حرف ctrl, صحيح term, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl) {
    إذا(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وإلا
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_پاڵنان_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_پاڵنان_output_vsprintf(vt, fmt, args);
  va_end(args);

  إذا(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_پاڵنان_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *buffer, size_t len)
{
  إذا(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  إذا(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  أرجع len;
}

VTerm_بەها_Type vterm_get_attr_type(VTermAttr attr)
{
  بدل(attr) {
    حالة VTERM_ATTR_BOLD:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_UNDERLINE:  أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_ITALIC:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BLINK:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_REVERSE:    أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_CONCEAL:    أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_STRIKE:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_FONT:       أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_FOREGROUND: أرجع VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_BACKGROUND: أرجع VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_SMALL:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BASELINE:   أرجع VTERM_VALUETYPE_INT;

    حالة VTERM_N_ATTRS: أرجع 0;
  }
  أرجع 0; /* UNREACHABLE */
}

VTerm_بەها_Type vterm_get_prop_type(VTermProp prop)
{
  بدل(prop) {
    حالة VTERM_PROP_CURSORVISIBLE: أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORBLINK:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_ALTSCREEN:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_TITLE:         أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_ICONNAME:      أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_REVERSE:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORSHAPE:   أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_MOUSE:         أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_FOCUSREPORT:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORCOLOR:   أرجع VTERM_VALUETYPE_STRING;

    حالة VTERM_N_PROPS: أرجع 0;
  }
  أرجع 0; /* UNREACHABLE */
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

  إذا(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    أرجع;
  }

  إذا(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  وإلا {
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

  إذا(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  وإلا {
    صحيح upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  إذا(moverect)
    (*moverect)(dest, src, user);

  إذا(downward > 0)
    rect.start_row = rect.end_row - downward;
  وإلا إذا(downward < 0)
    rect.end_row = rect.start_row - downward;

  إذا(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  وإلا إذا(rightward < 0)
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

  إذا(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  وإلا /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  إذا(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  وإلا /* rightward >= 0 */ {
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
  إذا(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  إذا(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
