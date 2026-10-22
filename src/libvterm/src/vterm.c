#define DEFINE_INLINES

// vim: set sw=2 :
#அந்தர்பாவ "vterm_internal.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "utf8.h"

/*****************
 * API functions *
 *****************/

நிலையான வெறுமை *default_malloc(size_t size, வெறுமை *allocdata UNUSED)
{
  வெறுமை *ptr = malloc(size);
  எனில்(ptr)
    memset(ptr, 0, size);
  திரும்பு ptr;
}

நிலையான வெறுமை default_free(வெறுமை *ptr, வெறுமை *allocdata UNUSED)
{
  விடுவி(ptr);
}

நிலையான VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(முழு rows, முழு cols)
{
  கட்டமைப்பு VTermBuilder builder;
  memset(&builder, 0, அளவு(builder));
  builder.rows = rows;
  builder.cols = cols;
  திரும்பு vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(முழு rows, முழு cols, VTermAllocatorFunctions *funcs, வெறுமை *allocdata)
{
  கட்டமைப்பு VTermBuilder builder;
  memset(&builder, 0, அளவு(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  திரும்பு vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(மாறிலி கட்டமைப்பு VTermBuilder *builder)
{
  மாறிலி VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(அளவு(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = வெற்று;
  vt->parser.cbdata    = வெற்று;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = வெற்று;
  vt->outdata = வெற்று;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  எனில் (vt->tmpbuffer == வெற்று
      || vt->outbuffer == வெற்று
      || vt->tmpbuffer == வெற்று)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    திரும்பு வெற்று;
  }

  திரும்பு vt;
}

வெறுமை vterm_free(VTerm *vt)
{
  எனில்(vt->screen)
    vterm_screen_free(vt->screen);

  எனில்(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL வெறுமை *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  திரும்பு (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL வெறுமை vterm_allocator_free(VTerm *vt, வெறுமை *ptr)
{
  எனில் (ptr)
    (*vt->allocator->விடுவி)(ptr, vt->allocdata);
}

வெறுமை vterm_get_size(மாறிலி VTerm *vt, முழு *rowsp, முழு *colsp)
{
  எனில்(rowsp)
    *rowsp = vt->rows;
  எனில்(colsp)
    *colsp = vt->cols;
}

வெறுமை vterm_set_size(VTerm *vt, முழு rows, முழு cols)
{
  எனில்(rows < 1 || cols < 1)
    திரும்பு;

  vt->rows = rows;
  vt->cols = cols;

  எனில்(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

முழு vterm_get_utf8(மாறிலி VTerm *vt)
{
  திரும்பு vt->mode.utf8;
}

வெறுமை vterm_set_utf8(VTerm *vt, முழு is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

வெறுமை vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, வெறுமை *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL வெறுமை vterm_தள்ளு_output_bytes(VTerm *vt, மாறிலி எழுத்து *bytes, size_t len)
{
  எனில்(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    திரும்பு;
  }

  எனில்(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    திரும்பு;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL வெறுமை vterm_தள்ளு_output_vsprintf(VTerm *vt, மாறிலி எழுத்து *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#இல்லையெனில்
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  எனில் (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_தள்ளு_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL வெறுமை vterm_தள்ளு_output_sprintf(VTerm *vt, மாறிலி எழுத்து *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_தள்ளு_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL வெறுமை vterm_தள்ளு_output_sprintf_ctrl(VTerm *vt, unsigned எழுத்து ctrl, மாறிலி எழுத்து *fmt, ...)
{
  size_t cur;

  எனில்(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  இல்லையெனில்
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  எனில்(cur >= vt->tmpbuffer_len)
    திரும்பு;
  vterm_தள்ளு_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_தள்ளு_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL வெறுமை vterm_தள்ளு_output_sprintf_str(VTerm *vt, unsigned எழுத்து ctrl, முழு term, மாறிலி எழுத்து *fmt, ...)
{
  size_t cur;

  எனில்(ctrl) {
    எனில்(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    இல்லையெனில்
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    எனில்(cur >= vt->tmpbuffer_len)
      திரும்பு;
    vterm_தள்ளு_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_தள்ளு_output_vsprintf(vt, fmt, args);
  va_end(args);

  எனில்(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    எனில்(cur >= vt->tmpbuffer_len)
      திரும்பு;
    vterm_தள்ளு_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(மாறிலி VTerm *vt)
{
  திரும்பு vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(மாறிலி VTerm *vt)
{
  திரும்பு vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(மாறிலி VTerm *vt)
{
  திரும்பு vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, எழுத்து *buffer, size_t len)
{
  எனில்(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  எனில்(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  திரும்பு len;
}

VTerm_மதிப்பு_Type vterm_get_attr_type(VTermAttr attr)
{
  தேர்வு(attr) {
    நிலை VTERM_ATTR_BOLD:       திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_UNDERLINE:  திரும்பு VTERM_VALUETYPE_INT;
    நிலை VTERM_ATTR_ITALIC:     திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_BLINK:      திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_REVERSE:    திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_CONCEAL:    திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_STRIKE:     திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_FONT:       திரும்பு VTERM_VALUETYPE_INT;
    நிலை VTERM_ATTR_FOREGROUND: திரும்பு VTERM_VALUETYPE_COLOR;
    நிலை VTERM_ATTR_BACKGROUND: திரும்பு VTERM_VALUETYPE_COLOR;
    நிலை VTERM_ATTR_SMALL:      திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_ATTR_BASELINE:   திரும்பு VTERM_VALUETYPE_INT;

    நிலை VTERM_N_ATTRS: திரும்பு 0;
  }
  திரும்பு 0; /* UNREACHABLE */
}

VTerm_மதிப்பு_Type vterm_get_prop_type(VTermProp prop)
{
  தேர்வு(prop) {
    நிலை VTERM_PROP_CURSORVISIBLE: திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_PROP_CURSORBLINK:   திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_PROP_ALTSCREEN:     திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_PROP_TITLE:         திரும்பு VTERM_VALUETYPE_STRING;
    நிலை VTERM_PROP_ICONNAME:      திரும்பு VTERM_VALUETYPE_STRING;
    நிலை VTERM_PROP_REVERSE:       திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_PROP_CURSORSHAPE:   திரும்பு VTERM_VALUETYPE_INT;
    நிலை VTERM_PROP_MOUSE:         திரும்பு VTERM_VALUETYPE_INT;
    நிலை VTERM_PROP_FOCUSREPORT:   திரும்பு VTERM_VALUETYPE_BOOL;
    நிலை VTERM_PROP_CURSORCOLOR:   திரும்பு VTERM_VALUETYPE_STRING;

    நிலை VTERM_N_PROPS: திரும்பு 0;
  }
  திரும்பு 0; /* UNREACHABLE */
}

வெறுமை vterm_scroll_rect(VTermRect rect,
    முழு downward,
    முழு rightward,
    முழு (*moverect)(VTermRect src, VTermRect dest, வெறுமை *user),
    முழு (*eraserect)(VTermRect rect, முழு selective, வெறுமை *user),
    வெறுமை *user)
{
  VTermRect src;
  VTermRect dest;

  எனில்(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    திரும்பு;
  }

  எனில்(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  இல்லையெனில் {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    முழு leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  எனில்(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  இல்லையெனில் {
    முழு upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  எனில்(moverect)
    (*moverect)(dest, src, user);

  எனில்(downward > 0)
    rect.start_row = rect.end_row - downward;
  இல்லையெனில் எனில்(downward < 0)
    rect.end_row = rect.start_row - downward;

  எனில்(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  இல்லையெனில் எனில்(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

வெறுமை vterm_copy_cells(VTermRect dest,
    VTermRect src,
    வெறுமை (*copycell)(VTermPos dest, VTermPos src, வெறுமை *user),
    வெறுமை *user)
{
  முழு downward  = src.start_row - dest.start_row;
  முழு rightward = src.start_col - dest.start_col;

  முழு init_row, test_row, init_col, test_col;
  முழு inc_row, inc_col;

  எனில்(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  இல்லையெனில் /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  எனில்(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  இல்லையெனில் /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  க்கு(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    க்கு(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

வெறுமை vterm_check_version(முழு major, முழு minor)
{
  எனில்(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  எனில்(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
