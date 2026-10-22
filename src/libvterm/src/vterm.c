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
  यदि(ptr)
    memset(ptr, 0, size);
  प्रत्यागम ptr;
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
  प्रत्यागम vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(पूर्णाङ्क rows, पूर्णाङ्क cols, VTermAllocatorFunctions *funcs, शून्य *allocdata)
{
  संरचना VTermBuilder builder;
  memset(&builder, 0, परिमाण(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  प्रत्यागम vterm_build(&builder);
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

  यदि (vt->tmpbuffer == रिक्त
      || vt->outbuffer == रिक्त
      || vt->tmpbuffer == रिक्त)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    प्रत्यागम रिक्त;
  }

  प्रत्यागम vt;
}

शून्य vterm_free(VTerm *vt)
{
  यदि(vt->screen)
    vterm_screen_free(vt->screen);

  यदि(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL शून्य *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  प्रत्यागम (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL शून्य vterm_allocator_free(VTerm *vt, शून्य *ptr)
{
  यदि (ptr)
    (*vt->allocator->मुक्त)(ptr, vt->allocdata);
}

शून्य vterm_get_size(ध्रुव VTerm *vt, पूर्णाङ्क *rowsp, पूर्णाङ्क *colsp)
{
  यदि(rowsp)
    *rowsp = vt->rows;
  यदि(colsp)
    *colsp = vt->cols;
}

शून्य vterm_set_size(VTerm *vt, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
  यदि(rows < 1 || cols < 1)
    प्रत्यागम;

  vt->rows = rows;
  vt->cols = cols;

  यदि(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

पूर्णाङ्क vterm_get_utf8(ध्रुव VTerm *vt)
{
  प्रत्यागम vt->mode.utf8;
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

INTERNAL शून्य vterm_ढकला_output_bytes(VTerm *vt, ध्रुव वर्ण *bytes, size_t len)
{
  यदि(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    प्रत्यागम;
  }

  यदि(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    प्रत्यागम;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL शून्य vterm_ढकला_output_vsprintf(VTerm *vt, ध्रुव वर्ण *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#अन्यथा
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  यदि (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ढकला_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL शून्य vterm_ढकला_output_sprintf(VTerm *vt, ध्रुव वर्ण *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_ढकला_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL शून्य vterm_ढकला_output_sprintf_ctrl(VTerm *vt, unsigned वर्ण ctrl, ध्रुव वर्ण *fmt, ...)
{
  size_t cur;

  यदि(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  अन्यथा
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  यदि(cur >= vt->tmpbuffer_len)
    प्रत्यागम;
  vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_ढकला_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL शून्य vterm_ढकला_output_sprintf_str(VTerm *vt, unsigned वर्ण ctrl, पूर्णाङ्क term, ध्रुव वर्ण *fmt, ...)
{
  size_t cur;

  यदि(ctrl) {
    यदि(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    अन्यथा
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    यदि(cur >= vt->tmpbuffer_len)
      प्रत्यागम;
    vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_ढकला_output_vsprintf(vt, fmt, args);
  va_end(args);

  यदि(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    यदि(cur >= vt->tmpbuffer_len)
      प्रत्यागम;
    vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ध्रुव VTerm *vt)
{
  प्रत्यागम vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ध्रुव VTerm *vt)
{
  प्रत्यागम vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ध्रुव VTerm *vt)
{
  प्रत्यागम vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, वर्ण *buffer, size_t len)
{
  यदि(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  यदि(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  प्रत्यागम len;
}

VTerm_मूल्य_Type vterm_get_attr_type(VTermAttr attr)
{
  विकल्प(attr) {
    पक्ष VTERM_ATTR_BOLD:       प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_UNDERLINE:  प्रत्यागम VTERM_VALUETYPE_INT;
    पक्ष VTERM_ATTR_ITALIC:     प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_BLINK:      प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_REVERSE:    प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_CONCEAL:    प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_STRIKE:     प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_FONT:       प्रत्यागम VTERM_VALUETYPE_INT;
    पक्ष VTERM_ATTR_FOREGROUND: प्रत्यागम VTERM_VALUETYPE_COLOR;
    पक्ष VTERM_ATTR_BACKGROUND: प्रत्यागम VTERM_VALUETYPE_COLOR;
    पक्ष VTERM_ATTR_SMALL:      प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_ATTR_BASELINE:   प्रत्यागम VTERM_VALUETYPE_INT;

    पक्ष VTERM_N_ATTRS: प्रत्यागम 0;
  }
  प्रत्यागम 0; /* UNREACHABLE */
}

VTerm_मूल्य_Type vterm_get_prop_type(VTermProp prop)
{
  विकल्प(prop) {
    पक्ष VTERM_PROP_CURSORVISIBLE: प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORBLINK:   प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_ALTSCREEN:     प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_TITLE:         प्रत्यागम VTERM_VALUETYPE_STRING;
    पक्ष VTERM_PROP_ICONNAME:      प्रत्यागम VTERM_VALUETYPE_STRING;
    पक्ष VTERM_PROP_REVERSE:       प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORSHAPE:   प्रत्यागम VTERM_VALUETYPE_INT;
    पक्ष VTERM_PROP_MOUSE:         प्रत्यागम VTERM_VALUETYPE_INT;
    पक्ष VTERM_PROP_FOCUSREPORT:   प्रत्यागम VTERM_VALUETYPE_BOOL;
    पक्ष VTERM_PROP_CURSORCOLOR:   प्रत्यागम VTERM_VALUETYPE_STRING;

    पक्ष VTERM_N_PROPS: प्रत्यागम 0;
  }
  प्रत्यागम 0; /* UNREACHABLE */
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

  यदि(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    प्रत्यागम;
  }

  यदि(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  अन्यथा {
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

  यदि(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  अन्यथा {
    पूर्णाङ्क upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  यदि(moverect)
    (*moverect)(dest, src, user);

  यदि(downward > 0)
    rect.start_row = rect.end_row - downward;
  अन्यथा यदि(downward < 0)
    rect.end_row = rect.start_row - downward;

  यदि(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  अन्यथा यदि(rightward < 0)
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

  यदि(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  अन्यथा /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  यदि(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  अन्यथा /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  प्रति(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    प्रति(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

शून्य vterm_check_version(पूर्णाङ्क major, पूर्णाङ्क minor)
{
  यदि(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  यदि(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
