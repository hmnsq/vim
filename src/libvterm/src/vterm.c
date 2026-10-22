#define DEFINE_INLINES

// vim: set sw=2 :
#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "utf8.h"

/*****************
 * API functions *
 *****************/

സ്ഥിതം ശൂന്യം *default_malloc(size_t size, ശൂന്യം *allocdata UNUSED)
{
  ശൂന്യം *ptr = malloc(size);
  എങ്കിൽ(ptr)
    memset(ptr, 0, size);
  തിരികെ ptr;
}

സ്ഥിതം ശൂന്യം default_free(ശൂന്യം *ptr, ശൂന്യം *allocdata UNUSED)
{
  സ്വതന്ത്രം(ptr);
}

സ്ഥിതം VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
  ഘടന VTermBuilder builder;
  memset(&builder, 0, വലുപ്പം(builder));
  builder.rows = rows;
  builder.cols = cols;
  തിരികെ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, VTermAllocatorFunctions *funcs, ശൂന്യം *allocdata)
{
  ഘടന VTermBuilder builder;
  memset(&builder, 0, വലുപ്പം(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  തിരികെ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(സ്ഥിരം ഘടന VTermBuilder *builder)
{
  സ്ഥിരം VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(വലുപ്പം(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = ശൂന്യ;
  vt->parser.cbdata    = ശൂന്യ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ശൂന്യ;
  vt->outdata = ശൂന്യ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  എങ്കിൽ (vt->tmpbuffer == ശൂന്യ
      || vt->outbuffer == ശൂന്യ
      || vt->tmpbuffer == ശൂന്യ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    തിരികെ ശൂന്യ;
  }

  തിരികെ vt;
}

ശൂന്യം vterm_free(VTerm *vt)
{
  എങ്കിൽ(vt->screen)
    vterm_screen_free(vt->screen);

  എങ്കിൽ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL ശൂന്യം *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  തിരികെ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL ശൂന്യം vterm_allocator_free(VTerm *vt, ശൂന്യം *ptr)
{
  എങ്കിൽ (ptr)
    (*vt->allocator->സ്വതന്ത്രം)(ptr, vt->allocdata);
}

ശൂന്യം vterm_get_size(സ്ഥിരം VTerm *vt, പൂർണ്ണസംഖ്യ *rowsp, പൂർണ്ണസംഖ്യ *colsp)
{
  എങ്കിൽ(rowsp)
    *rowsp = vt->rows;
  എങ്കിൽ(colsp)
    *colsp = vt->cols;
}

ശൂന്യം vterm_set_size(VTerm *vt, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
  എങ്കിൽ(rows < 1 || cols < 1)
    തിരികെ;

  vt->rows = rows;
  vt->cols = cols;

  എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

പൂർണ്ണസംഖ്യ vterm_get_utf8(സ്ഥിരം VTerm *vt)
{
  തിരികെ vt->mode.utf8;
}

ശൂന്യം vterm_set_utf8(VTerm *vt, പൂർണ്ണസംഖ്യ is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

ശൂന്യം vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ശൂന്യം *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL ശൂന്യം vterm_തള്ളുക_output_bytes(VTerm *vt, സ്ഥിരം അക്ഷരം *bytes, size_t len)
{
  എങ്കിൽ(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    തിരികെ;
  }

  എങ്കിൽ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    തിരികെ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL ശൂന്യം vterm_തള്ളുക_output_vsprintf(VTerm *vt, സ്ഥിരം അക്ഷരം *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#അല്ലെങ്കിൽ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  എങ്കിൽ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_തള്ളുക_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL ശൂന്യം vterm_തള്ളുക_output_sprintf(VTerm *vt, സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_തള്ളുക_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL ശൂന്യം vterm_തള്ളുക_output_sprintf_ctrl(VTerm *vt, unsigned അക്ഷരം ctrl, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  size_t cur;

  എങ്കിൽ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  അല്ലെങ്കിൽ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  എങ്കിൽ(cur >= vt->tmpbuffer_len)
    തിരികെ;
  vterm_തള്ളുക_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_തള്ളുക_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL ശൂന്യം vterm_തള്ളുക_output_sprintf_str(VTerm *vt, unsigned അക്ഷരം ctrl, പൂർണ്ണസംഖ്യ term, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  size_t cur;

  എങ്കിൽ(ctrl) {
    എങ്കിൽ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    അല്ലെങ്കിൽ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    എങ്കിൽ(cur >= vt->tmpbuffer_len)
      തിരികെ;
    vterm_തള്ളുക_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_തള്ളുക_output_vsprintf(vt, fmt, args);
  va_end(args);

  എങ്കിൽ(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    എങ്കിൽ(cur >= vt->tmpbuffer_len)
      തിരികെ;
    vterm_തള്ളുക_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(സ്ഥിരം VTerm *vt)
{
  തിരികെ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(സ്ഥിരം VTerm *vt)
{
  തിരികെ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(സ്ഥിരം VTerm *vt)
{
  തിരികെ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, അക്ഷരം *buffer, size_t len)
{
  എങ്കിൽ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  എങ്കിൽ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  തിരികെ len;
}

VTerm_മൂല്യം_Type vterm_get_attr_type(VTermAttr attr)
{
  തിരഞ്ഞെടുപ്പ്(attr) {
    അവസ്ഥ VTERM_ATTR_BOLD:       തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_UNDERLINE:  തിരികെ VTERM_VALUETYPE_INT;
    അവസ്ഥ VTERM_ATTR_ITALIC:     തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_BLINK:      തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_REVERSE:    തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_CONCEAL:    തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_STRIKE:     തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_FONT:       തിരികെ VTERM_VALUETYPE_INT;
    അവസ്ഥ VTERM_ATTR_FOREGROUND: തിരികെ VTERM_VALUETYPE_COLOR;
    അവസ്ഥ VTERM_ATTR_BACKGROUND: തിരികെ VTERM_VALUETYPE_COLOR;
    അവസ്ഥ VTERM_ATTR_SMALL:      തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_ATTR_BASELINE:   തിരികെ VTERM_VALUETYPE_INT;

    അവസ്ഥ VTERM_N_ATTRS: തിരികെ 0;
  }
  തിരികെ 0; /* UNREACHABLE */
}

VTerm_മൂല്യം_Type vterm_get_prop_type(VTermProp prop)
{
  തിരഞ്ഞെടുപ്പ്(prop) {
    അവസ്ഥ VTERM_PROP_CURSORVISIBLE: തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_PROP_CURSORBLINK:   തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_PROP_ALTSCREEN:     തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_PROP_TITLE:         തിരികെ VTERM_VALUETYPE_STRING;
    അവസ്ഥ VTERM_PROP_ICONNAME:      തിരികെ VTERM_VALUETYPE_STRING;
    അവസ്ഥ VTERM_PROP_REVERSE:       തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_PROP_CURSORSHAPE:   തിരികെ VTERM_VALUETYPE_INT;
    അവസ്ഥ VTERM_PROP_MOUSE:         തിരികെ VTERM_VALUETYPE_INT;
    അവസ്ഥ VTERM_PROP_FOCUSREPORT:   തിരികെ VTERM_VALUETYPE_BOOL;
    അവസ്ഥ VTERM_PROP_CURSORCOLOR:   തിരികെ VTERM_VALUETYPE_STRING;

    അവസ്ഥ VTERM_N_PROPS: തിരികെ 0;
  }
  തിരികെ 0; /* UNREACHABLE */
}

ശൂന്യം vterm_scroll_rect(VTermRect rect,
    പൂർണ്ണസംഖ്യ downward,
    പൂർണ്ണസംഖ്യ rightward,
    പൂർണ്ണസംഖ്യ (*moverect)(VTermRect src, VTermRect dest, ശൂന്യം *user),
    പൂർണ്ണസംഖ്യ (*eraserect)(VTermRect rect, പൂർണ്ണസംഖ്യ selective, ശൂന്യം *user),
    ശൂന്യം *user)
{
  VTermRect src;
  VTermRect dest;

  എങ്കിൽ(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    തിരികെ;
  }

  എങ്കിൽ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  അല്ലെങ്കിൽ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    പൂർണ്ണസംഖ്യ leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  എങ്കിൽ(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  അല്ലെങ്കിൽ {
    പൂർണ്ണസംഖ്യ upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  എങ്കിൽ(moverect)
    (*moverect)(dest, src, user);

  എങ്കിൽ(downward > 0)
    rect.start_row = rect.end_row - downward;
  അല്ലെങ്കിൽ എങ്കിൽ(downward < 0)
    rect.end_row = rect.start_row - downward;

  എങ്കിൽ(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  അല്ലെങ്കിൽ എങ്കിൽ(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

ശൂന്യം vterm_copy_cells(VTermRect dest,
    VTermRect src,
    ശൂന്യം (*copycell)(VTermPos dest, VTermPos src, ശൂന്യം *user),
    ശൂന്യം *user)
{
  പൂർണ്ണസംഖ്യ downward  = src.start_row - dest.start_row;
  പൂർണ്ണസംഖ്യ rightward = src.start_col - dest.start_col;

  പൂർണ്ണസംഖ്യ init_row, test_row, init_col, test_col;
  പൂർണ്ണസംഖ്യ inc_row, inc_col;

  എങ്കിൽ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  അല്ലെങ്കിൽ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  എങ്കിൽ(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  അല്ലെങ്കിൽ /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  വേണ്ടി(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    വേണ്ടി(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

ശൂന്യം vterm_check_version(പൂർണ്ണസംഖ്യ major, പൂർണ്ണസംഖ്യ minor)
{
  എങ്കിൽ(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  എങ്കിൽ(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
