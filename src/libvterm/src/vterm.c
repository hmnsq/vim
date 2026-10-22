#define DEFINE_INLINES

// vim: set sw=2 :
#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "utf8.h"

/*****************
 * API functions *
 *****************/

स्थिर शून्य *default_malloc(size_t size, शून्य *allocdata UNUSED)
{
  शून्य *ptr = malloc(size);
  सचे(ptr)
    memset(ptr, 0, size);
  पटिनिवत्त ptr;
}

स्थिर शून्य default_free(शून्य *ptr, शून्य *allocdata UNUSED)
{
  मुक्त(ptr);
}

स्थिर VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(पूर्णाङ्क rows, पूर्णाङ्क cols)
{
  संरचना VTermBuilder builder;
  memset(&builder, 0, परिमाण(builder));
  builder.rows = rows;
  builder.cols = cols;
  पटिनिवत्त vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(पूर्णाङ्क rows, पूर्णाङ्क cols, VTermAllocatorFunctions *funcs, शून्य *allocdata)
{
  संरचना VTermBuilder builder;
  memset(&builder, 0, परिमाण(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  पटिनिवत्त vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ध्रुव संरचना VTermBuilder *builder)
{
  ध्रुव VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(परिमाण(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = रिक्त;
  vt->parser.cbdata    = रिक्त;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = रिक्त;
  vt->outdata = रिक्त;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  सचे (vt->tmpbuffer == रिक्त
      || vt->outbuffer == रिक्त
      || vt->tmpbuffer == रिक्त)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    पटिनिवत्त रिक्त;
  }

  पटिनिवत्त vt;
}

शून्य vterm_free(VTerm *vt)
{
  सचे(vt->screen)
    vterm_screen_free(vt->screen);

  सचे(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL शून्य *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  पटिनिवत्त (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL शून्य vterm_allocator_free(VTerm *vt, शून्य *ptr)
{
  सचे (ptr)
    (*vt->allocator->मुक्त)(ptr, vt->allocdata);
}

शून्य vterm_get_size(ध्रुव VTerm *vt, पूर्णाङ्क *rowsp, पूर्णाङ्क *colsp)
{
  सचे(rowsp)
    *rowsp = vt->rows;
  सचे(colsp)
    *colsp = vt->cols;
}

शून्य vterm_set_size(VTerm *vt, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
  सचे(rows < 1 || cols < 1)
    पटिनिवत्त;

  vt->rows = rows;
  vt->cols = cols;

  सचे(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

पूर्णाङ्क vterm_get_utf8(ध्रुव VTerm *vt)
{
  पटिनिवत्त vt->mode.utf8;
}

शून्य vterm_set_utf8(VTerm *vt, पूर्णाङ्क is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

शून्य vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, शून्य *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL शून्य vterm_प्रक्षिप_output_bytes(VTerm *vt, ध्रुव वर्ण *bytes, size_t len)
{
  सचे(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    पटिनिवत्त;
  }

  सचे(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    पटिनिवत्त;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL शून्य vterm_प्रक्षिप_output_vsprintf(VTerm *vt, ध्रुव वर्ण *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#अञ्ञथा
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  सचे (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_प्रक्षिप_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL शून्य vterm_प्रक्षिप_output_sprintf(VTerm *vt, ध्रुव वर्ण *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_प्रक्षिप_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL शून्य vterm_प्रक्षिप_output_sprintf_ctrl(VTerm *vt, unsigned वर्ण ctrl, ध्रुव वर्ण *fmt, ...)
{
  size_t cur;

  सचे(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  अञ्ञथा
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  सचे(cur >= vt->tmpbuffer_len)
    पटिनिवत्त;
  vterm_प्रक्षिप_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_प्रक्षिप_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL शून्य vterm_प्रक्षिप_output_sprintf_str(VTerm *vt, unsigned वर्ण ctrl, पूर्णाङ्क term, ध्रुव वर्ण *fmt, ...)
{
  size_t cur;

  सचे(ctrl) {
    सचे(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    अञ्ञथा
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    सचे(cur >= vt->tmpbuffer_len)
      पटिनिवत्त;
    vterm_प्रक्षिप_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_प्रक्षिप_output_vsprintf(vt, fmt, args);
  va_end(args);

  सचे(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    सचे(cur >= vt->tmpbuffer_len)
      पटिनिवत्त;
    vterm_प्रक्षिप_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ध्रुव VTerm *vt)
{
  पटिनिवत्त vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ध्रुव VTerm *vt)
{
  पटिनिवत्त vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ध्रुव VTerm *vt)
{
  पटिनिवत्त vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, वर्ण *buffer, size_t len)
{
  सचे(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  सचे(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  पटिनिवत्त len;
}

VTerm_मानम्_Type vterm_get_attr_type(VTermAttr attr)
{
  विकल्प(attr) {
    पक्ष VTERM_ATTR_BOLD:       पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_UNDERLINE:  पटिनिवत्त VTERM_VALUETYPE_INT;
    पक्ष VTERM_ATTR_ITALIC:     पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_BLINK:      पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_REVERSE:    पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_CONCEAL:    पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_STRIKE:     पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_FONT:       पटिनिवत्त VTERM_VALUETYPE_INT;
    पक्ष VTERM_ATTR_FOREGROUND: पटिनिवत्त VTERM_VALUETYPE_COLOR;
    पक्ष VTERM_ATTR_BACKGROUND: पटिनिवत्त VTERM_VALUETYPE_COLOR;
    पक्ष VTERM_ATTR_SMALL:      पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_BASELINE:   पटिनिवत्त VTERM_VALUETYPE_INT;

    पक्ष VTERM_N_ATTRS: पटिनिवत्त 0;
  }
  पटिनिवत्त 0; /* UNREACHABLE */
}

VTerm_मानम्_Type vterm_get_prop_type(VTermProp prop)
{
  विकल्प(prop) {
    पक्ष VTERM_PROP_CURSORVISIBLE: पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORBLINK:   पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_ALTSCREEN:     पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_TITLE:         पटिनिवत्त VTERM_VALUETYPE_STRING;
    पक्ष VTERM_PROP_ICONNAME:      पटिनिवत्त VTERM_VALUETYPE_STRING;
    पक्ष VTERM_PROP_REVERSE:       पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORSHAPE:   पटिनिवत्त VTERM_VALUETYPE_INT;
    पक्ष VTERM_PROP_MOUSE:         पटिनिवत्त VTERM_VALUETYPE_INT;
    पक्ष VTERM_PROP_FOCUSREPORT:   पटिनिवत्त VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORCOLOR:   पटिनिवत्त VTERM_VALUETYPE_STRING;

    पक्ष VTERM_N_PROPS: पटिनिवत्त 0;
  }
  पटिनिवत्त 0; /* UNREACHABLE */
}

शून्य vterm_scroll_rect(VTermRect rect,
    पूर्णाङ्क downward,
    पूर्णाङ्क rightward,
    पूर्णाङ्क (*moverect)(VTermRect src, VTermRect dest, शून्य *user),
    पूर्णाङ्क (*eraserect)(VTermRect rect, पूर्णाङ्क selective, शून्य *user),
    शून्य *user)
{
  VTermRect src;
  VTermRect dest;

  सचे(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    पटिनिवत्त;
  }

  सचे(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  अञ्ञथा {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    पूर्णाङ्क leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  सचे(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  अञ्ञथा {
    पूर्णाङ्क upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  सचे(moverect)
    (*moverect)(dest, src, user);

  सचे(downward > 0)
    rect.start_row = rect.end_row - downward;
  अञ्ञथा सचे(downward < 0)
    rect.end_row = rect.start_row - downward;

  सचे(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  अञ्ञथा सचे(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

शून्य vterm_copy_cells(VTermRect dest,
    VTermRect src,
    शून्य (*copycell)(VTermPos dest, VTermPos src, शून्य *user),
    शून्य *user)
{
  पूर्णाङ्क downward  = src.start_row - dest.start_row;
  पूर्णाङ्क rightward = src.start_col - dest.start_col;

  पूर्णाङ्क init_row, test_row, init_col, test_col;
  पूर्णाङ्क inc_row, inc_col;

  सचे(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  अञ्ञथा /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  सचे(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  अञ्ञथा /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  अत्थाय(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    अत्थाय(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

शून्य vterm_check_version(पूर्णाङ्क major, पूर्णाङ्क minor)
{
  सचे(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  सचे(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
