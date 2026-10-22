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

स्थिर शून्य *default_malloc(माप_प्रकार size, शून्य *allocdata UNUSED)
{
  शून्य *ptr = दो_स्मृति(size);
  जर(ptr)
    स_रखो(ptr, 0, size);
  परत ptr;
}

स्थिर शून्य default_free(शून्य *ptr, शून्य *allocdata UNUSED)
{
  मुक्त(ptr);
}

स्थिर VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(पूर्णांक rows, पूर्णांक cols)
{
  संरचना VTermBuilder builder;
  स_रखो(&builder, 0, आकार(builder));
  builder.rows = rows;
  builder.cols = cols;
  परत vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(पूर्णांक rows, पूर्णांक cols, VTermAllocatorFunctions *funcs, शून्य *allocdata)
{
  संरचना VTermBuilder builder;
  स_रखो(&builder, 0, आकार(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  परत vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(स्थिरांक संरचना VTermBuilder *builder)
{
  स्थिरांक VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->दो_स्मृति)(आकार(VTerm), builder->allocdata);

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

  जर (vt->tmpbuffer == रिक्त
      || vt->outbuffer == रिक्त
      || vt->tmpbuffer == रिक्त)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    परत रिक्त;
  }

  परत vt;
}

शून्य vterm_free(VTerm *vt)
{
  जर(vt->screen)
    vterm_screen_free(vt->screen);

  जर(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL शून्य *vterm_allocator_malloc(VTerm *vt, माप_प्रकार size)
{
  परत (*vt->allocator->दो_स्मृति)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL शून्य vterm_allocator_free(VTerm *vt, शून्य *ptr)
{
  जर (ptr)
    (*vt->allocator->मुक्त)(ptr, vt->allocdata);
}

शून्य vterm_get_size(स्थिरांक VTerm *vt, पूर्णांक *rowsp, पूर्णांक *colsp)
{
  जर(rowsp)
    *rowsp = vt->rows;
  जर(colsp)
    *colsp = vt->cols;
}

शून्य vterm_set_size(VTerm *vt, पूर्णांक rows, पूर्णांक cols)
{
  जर(rows < 1 || cols < 1)
    परत;

  vt->rows = rows;
  vt->cols = cols;

  जर(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

पूर्णांक vterm_get_utf8(स्थिरांक VTerm *vt)
{
  परत vt->mode.utf8;
}

शून्य vterm_set_utf8(VTerm *vt, पूर्णांक is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

शून्य vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, शून्य *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL शून्य vterm_ढकला_output_bytes(VTerm *vt, स्थिरांक अक्षर *bytes, माप_प्रकार len)
{
  जर(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    परत;
  }

  जर(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    परत;
  }

  स_नकल(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL शून्य vterm_ढकला_output_vsprintf(VTerm *vt, स्थिरांक अक्षर *format, बहु_सूची args)
{
  माप_प्रकार len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#अन्यथा
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = भम_लिखो(vt->tmpbuffer, format, args);
  जर (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ढकला_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL शून्य vterm_ढकला_output_sprintf(VTerm *vt, स्थिरांक अक्षर *format, ...)
{
  बहु_सूची args;
  बहु_शुरू(args, format);
  vterm_ढकला_output_vsprintf(vt, format, args);
  बहु_पूर्ण(args);
}

INTERNAL शून्य vterm_ढकला_output_sprintf_ctrl(VTerm *vt, अचिन्हित अक्षर ctrl, स्थिरांक अक्षर *fmt, ...)
{
  माप_प्रकार cur;

  जर(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  अन्यथा
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  जर(cur >= vt->tmpbuffer_len)
    परत;
  vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);

  बहु_सूची args;
  बहु_शुरू(args, fmt);
  vterm_ढकला_output_vsprintf(vt, fmt, args);
  बहु_पूर्ण(args);
}

INTERNAL शून्य vterm_ढकला_output_sprintf_str(VTerm *vt, अचिन्हित अक्षर ctrl, पूर्णांक term, स्थिरांक अक्षर *fmt, ...)
{
  माप_प्रकार cur;

  जर(ctrl) {
    जर(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    अन्यथा
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    जर(cur >= vt->tmpbuffer_len)
      परत;
    vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);
  }

  बहु_सूची args;
  बहु_शुरू(args, fmt);
  vterm_ढकला_output_vsprintf(vt, fmt, args);
  बहु_पूर्ण(args);

  जर(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    जर(cur >= vt->tmpbuffer_len)
      परत;
    vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

माप_प्रकार vterm_output_get_buffer_size(स्थिरांक VTerm *vt)
{
  परत vt->outbuffer_len;
}

माप_प्रकार vterm_output_get_buffer_current(स्थिरांक VTerm *vt)
{
  परत vt->outbuffer_cur;
}

माप_प्रकार vterm_output_get_buffer_remaining(स्थिरांक VTerm *vt)
{
  परत vt->outbuffer_len - vt->outbuffer_cur;
}

माप_प्रकार vterm_output_read(VTerm *vt, अक्षर *buffer, माप_प्रकार len)
{
  जर(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  स_नकल(buffer, vt->outbuffer, len);

  जर(len < vt->outbuffer_cur)
    स_हटाओ(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  परत len;
}

VTerm_मूल्य_Type vterm_get_attr_type(VTermAttr attr)
{
  निवड(attr) {
    स्थिती VTERM_ATTR_BOLD:       परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_UNDERLINE:  परत VTERM_VALUETYPE_INT;
    स्थिती VTERM_ATTR_ITALIC:     परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_BLINK:      परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_REVERSE:    परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_CONCEAL:    परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_STRIKE:     परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_FONT:       परत VTERM_VALUETYPE_INT;
    स्थिती VTERM_ATTR_FOREGROUND: परत VTERM_VALUETYPE_COLOR;
    स्थिती VTERM_ATTR_BACKGROUND: परत VTERM_VALUETYPE_COLOR;
    स्थिती VTERM_ATTR_SMALL:      परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_ATTR_BASELINE:   परत VTERM_VALUETYPE_INT;

    स्थिती VTERM_N_ATTRS: परत 0;
  }
  परत 0; /* UNREACHABLE */
}

VTerm_मूल्य_Type vterm_get_prop_type(VTermProp prop)
{
  निवड(prop) {
    स्थिती VTERM_PROP_CURSORVISIBLE: परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_PROP_CURSORBLINK:   परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_PROP_ALTSCREEN:     परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_PROP_TITLE:         परत VTERM_VALUETYPE_STRING;
    स्थिती VTERM_PROP_ICONNAME:      परत VTERM_VALUETYPE_STRING;
    स्थिती VTERM_PROP_REVERSE:       परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_PROP_CURSORSHAPE:   परत VTERM_VALUETYPE_INT;
    स्थिती VTERM_PROP_MOUSE:         परत VTERM_VALUETYPE_INT;
    स्थिती VTERM_PROP_FOCUSREPORT:   परत VTERM_VALUETYPE_BOOL;
    स्थिती VTERM_PROP_CURSORCOLOR:   परत VTERM_VALUETYPE_STRING;

    स्थिती VTERM_N_PROPS: परत 0;
  }
  परत 0; /* UNREACHABLE */
}

शून्य vterm_scroll_rect(VTermRect rect,
    पूर्णांक downward,
    पूर्णांक rightward,
    पूर्णांक (*moverect)(VTermRect src, VTermRect dest, शून्य *user),
    पूर्णांक (*eraserect)(VTermRect rect, पूर्णांक selective, शून्य *user),
    शून्य *user)
{
  VTermRect src;
  VTermRect dest;

  जर(असल(downward)  >= rect.end_row - rect.start_row ||
     असल(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    परत;
  }

  जर(rightward >= 0) {
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
    पूर्णांक leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  जर(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  अन्यथा {
    पूर्णांक upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  जर(moverect)
    (*moverect)(dest, src, user);

  जर(downward > 0)
    rect.start_row = rect.end_row - downward;
  अन्यथा जर(downward < 0)
    rect.end_row = rect.start_row - downward;

  जर(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  अन्यथा जर(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

शून्य vterm_copy_cells(VTermRect dest,
    VTermRect src,
    शून्य (*copycell)(VTermPos dest, VTermPos src, शून्य *user),
    शून्य *user)
{
  पूर्णांक downward  = src.start_row - dest.start_row;
  पूर्णांक rightward = src.start_col - dest.start_col;

  पूर्णांक init_row, test_row, init_col, test_col;
  पूर्णांक inc_row, inc_col;

  जर(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  अन्यथा /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  जर(rightward < 0) {
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
  साठी(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    साठी(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

शून्य vterm_check_version(पूर्णांक major, पूर्णांक minor)
{
  जर(major != VTERM_VERSION_MAJOR) {
    ख_लिखो(मानक_त्रुटि, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    निकास(1);
  }

  जर(minor > VTERM_VERSION_MINOR) {
    ख_लिखो(मानक_त्रुटि, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    निकास(1);
  }

  // Happy
}
