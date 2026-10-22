#define DEFINE_INLINES

// vim: set sw=2 :
#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "utf8.h"

/*****************
 * API functions *
 *****************/

સ્થિર શૂન્ય *default_malloc(size_t size, શૂન્ય *allocdata UNUSED)
{
  શૂન્ય *ptr = malloc(size);
  જો(ptr)
    memset(ptr, 0, size);
  પાછા ptr;
}

સ્થિર શૂન્ય default_free(શૂન્ય *ptr, શૂન્ય *allocdata UNUSED)
{
  મુક્ત(ptr);
}

સ્થિર VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(પૂર્ણાંક rows, પૂર્ણાંક cols)
{
  સંરચના VTermBuilder builder;
  memset(&builder, 0, કદ(builder));
  builder.rows = rows;
  builder.cols = cols;
  પાછા vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(પૂર્ણાંક rows, પૂર્ણાંક cols, VTermAllocatorFunctions *funcs, શૂન્ય *allocdata)
{
  સંરચના VTermBuilder builder;
  memset(&builder, 0, કદ(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  પાછા vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(અચલ સંરચના VTermBuilder *builder)
{
  અચલ VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(કદ(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = ખાલી;
  vt->parser.cbdata    = ખાલી;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ખાલી;
  vt->outdata = ખાલી;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  જો (vt->tmpbuffer == ખાલી
      || vt->outbuffer == ખાલી
      || vt->tmpbuffer == ખાલી)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    પાછા ખાલી;
  }

  પાછા vt;
}

શૂન્ય vterm_free(VTerm *vt)
{
  જો(vt->screen)
    vterm_screen_free(vt->screen);

  જો(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL શૂન્ય *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  પાછા (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL શૂન્ય vterm_allocator_free(VTerm *vt, શૂન્ય *ptr)
{
  જો (ptr)
    (*vt->allocator->મુક્ત)(ptr, vt->allocdata);
}

શૂન્ય vterm_get_size(અચલ VTerm *vt, પૂર્ણાંક *rowsp, પૂર્ણાંક *colsp)
{
  જો(rowsp)
    *rowsp = vt->rows;
  જો(colsp)
    *colsp = vt->cols;
}

શૂન્ય vterm_set_size(VTerm *vt, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
  જો(rows < 1 || cols < 1)
    પાછા;

  vt->rows = rows;
  vt->cols = cols;

  જો(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

પૂર્ણાંક vterm_get_utf8(અચલ VTerm *vt)
{
  પાછા vt->mode.utf8;
}

શૂન્ય vterm_set_utf8(VTerm *vt, પૂર્ણાંક is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

શૂન્ય vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, શૂન્ય *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL શૂન્ય vterm_ધકેલો_output_bytes(VTerm *vt, અચલ અક્ષર *bytes, size_t len)
{
  જો(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    પાછા;
  }

  જો(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    પાછા;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL શૂન્ય vterm_ધકેલો_output_vsprintf(VTerm *vt, અચલ અક્ષર *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#નહીંતર
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  જો (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ધકેલો_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL શૂન્ય vterm_ધકેલો_output_sprintf(VTerm *vt, અચલ અક્ષર *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_ધકેલો_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL શૂન્ય vterm_ધકેલો_output_sprintf_ctrl(VTerm *vt, unsigned અક્ષર ctrl, અચલ અક્ષર *fmt, ...)
{
  size_t cur;

  જો(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  નહીંતર
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  જો(cur >= vt->tmpbuffer_len)
    પાછા;
  vterm_ધકેલો_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_ધકેલો_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL શૂન્ય vterm_ધકેલો_output_sprintf_str(VTerm *vt, unsigned અક્ષર ctrl, પૂર્ણાંક term, અચલ અક્ષર *fmt, ...)
{
  size_t cur;

  જો(ctrl) {
    જો(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    નહીંતર
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    જો(cur >= vt->tmpbuffer_len)
      પાછા;
    vterm_ધકેલો_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_ધકેલો_output_vsprintf(vt, fmt, args);
  va_end(args);

  જો(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    જો(cur >= vt->tmpbuffer_len)
      પાછા;
    vterm_ધકેલો_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(અચલ VTerm *vt)
{
  પાછા vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(અચલ VTerm *vt)
{
  પાછા vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(અચલ VTerm *vt)
{
  પાછા vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, અક્ષર *buffer, size_t len)
{
  જો(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  જો(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  પાછા len;
}

VTerm_કિંમત_Type vterm_get_attr_type(VTermAttr attr)
{
  પસંદગી(attr) {
    સ્થિતિ VTERM_ATTR_BOLD:       પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_UNDERLINE:  પાછા VTERM_VALUETYPE_INT;
    સ્થિતિ VTERM_ATTR_ITALIC:     પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_BLINK:      પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_REVERSE:    પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_CONCEAL:    પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_STRIKE:     પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_FONT:       પાછા VTERM_VALUETYPE_INT;
    સ્થિતિ VTERM_ATTR_FOREGROUND: પાછા VTERM_VALUETYPE_COLOR;
    સ્થિતિ VTERM_ATTR_BACKGROUND: પાછા VTERM_VALUETYPE_COLOR;
    સ્થિતિ VTERM_ATTR_SMALL:      પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_ATTR_BASELINE:   પાછા VTERM_VALUETYPE_INT;

    સ્થિતિ VTERM_N_ATTRS: પાછા 0;
  }
  પાછા 0; /* UNREACHABLE */
}

VTerm_કિંમત_Type vterm_get_prop_type(VTermProp prop)
{
  પસંદગી(prop) {
    સ્થિતિ VTERM_PROP_CURSORVISIBLE: પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_PROP_CURSORBLINK:   પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_PROP_ALTSCREEN:     પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_PROP_TITLE:         પાછા VTERM_VALUETYPE_STRING;
    સ્થિતિ VTERM_PROP_ICONNAME:      પાછા VTERM_VALUETYPE_STRING;
    સ્થિતિ VTERM_PROP_REVERSE:       પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_PROP_CURSORSHAPE:   પાછા VTERM_VALUETYPE_INT;
    સ્થિતિ VTERM_PROP_MOUSE:         પાછા VTERM_VALUETYPE_INT;
    સ્થિતિ VTERM_PROP_FOCUSREPORT:   પાછા VTERM_VALUETYPE_BOOL;
    સ્થિતિ VTERM_PROP_CURSORCOLOR:   પાછા VTERM_VALUETYPE_STRING;

    સ્થિતિ VTERM_N_PROPS: પાછા 0;
  }
  પાછા 0; /* UNREACHABLE */
}

શૂન્ય vterm_scroll_rect(VTermRect rect,
    પૂર્ણાંક downward,
    પૂર્ણાંક rightward,
    પૂર્ણાંક (*moverect)(VTermRect src, VTermRect dest, શૂન્ય *user),
    પૂર્ણાંક (*eraserect)(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user),
    શૂન્ય *user)
{
  VTermRect src;
  VTermRect dest;

  જો(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    પાછા;
  }

  જો(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  નહીંતર {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    પૂર્ણાંક leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  જો(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  નહીંતર {
    પૂર્ણાંક upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  જો(moverect)
    (*moverect)(dest, src, user);

  જો(downward > 0)
    rect.start_row = rect.end_row - downward;
  નહીંતર જો(downward < 0)
    rect.end_row = rect.start_row - downward;

  જો(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  નહીંતર જો(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

શૂન્ય vterm_copy_cells(VTermRect dest,
    VTermRect src,
    શૂન્ય (*copycell)(VTermPos dest, VTermPos src, શૂન્ય *user),
    શૂન્ય *user)
{
  પૂર્ણાંક downward  = src.start_row - dest.start_row;
  પૂર્ણાંક rightward = src.start_col - dest.start_col;

  પૂર્ણાંક init_row, test_row, init_col, test_col;
  પૂર્ણાંક inc_row, inc_col;

  જો(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  નહીંતર /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  જો(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  નહીંતર /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  માટે(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    માટે(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

શૂન્ય vterm_check_version(પૂર્ણાંક major, પૂર્ણાંક minor)
{
  જો(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  જો(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
