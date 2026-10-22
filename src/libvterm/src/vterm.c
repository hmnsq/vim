#నిర్వచించు DEFINE_INLINES

// vim: set sw=2 :
#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "utf8.h"

/*****************
 * API functions *
 *****************/

స్థిర శూన్య *default_malloc(size_t కొలత, శూన్య *allocdata వాడని)
{
  శూన్య *ptr = జ్ఞాపకం_ఇవ్వు(కొలత);
  ఒకవేళ(ptr)
    memset(ptr, 0, కొలత);
  తిరిగి ptr;
}

స్థిర శూన్య default_free(శూన్య *ptr, శూన్య *allocdata వాడని)
{
  విడుదల(ptr);
}

స్థిర VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
  నిర్మాణం VTermBuilder builder;
  memset(&builder, 0, పరిమాణం(builder));
  builder.అడ్డువరుసలు_2 = అడ్డువరుసలు_2;
  builder.నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
  తిరిగి vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, VTermAllocatorFunctions *funcs, శూన్య *allocdata)
{
  నిర్మాణం VTermBuilder builder;
  memset(&builder, 0, పరిమాణం(builder));
  builder.అడ్డువరుసలు_2 = అడ్డువరుసలు_2;
  builder.నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  తిరిగి vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#నిర్వచించు DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(స్థిరరాశి నిర్మాణం VTermBuilder *builder)
{
  స్థిరరాశి VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->జ్ఞాపకం_ఇవ్వు)(పరిమాణం(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->అడ్డువరుసలు_2 = builder->అడ్డువరుసలు_2;
  vt->నిలువుల_సంఖ్య = builder->నిలువుల_సంఖ్య;

  vt->parser.state = సాధారణ_2;

  vt->parser.callbacks = ఖాళీ;
  vt->parser.cbdata    = ఖాళీ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ఖాళీ;
  vt->outdata = ఖాళీ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ఒకవేళ (vt->tmpbuffer == ఖాళీ
      || vt->outbuffer == ఖాళీ
      || vt->tmpbuffer == ఖాళీ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    తిరిగి ఖాళీ;
  }

  తిరిగి vt;
}

శూన్య vterm_free(VTerm *vt)
{
  ఒకవేళ(vt->screen)
    vterm_screen_free(vt->screen);

  ఒకవేళ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

అంతర్గత శూన్య *vterm_allocator_malloc(VTerm *vt, size_t కొలత)
{
  తిరిగి (*vt->allocator->జ్ఞాపకం_ఇవ్వు)(కొలత, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
అంతర్గత శూన్య vterm_allocator_free(VTerm *vt, శూన్య *ptr)
{
  ఒకవేళ (ptr)
    (*vt->allocator->విడుదల)(ptr, vt->allocdata);
}

శూన్య vterm_get_కొలత(స్థిరరాశి VTerm *vt, పూర్ణాంకం *rowsp, పూర్ణాంకం *colsp)
{
  ఒకవేళ(rowsp)
    *rowsp = vt->అడ్డువరుసలు_2;
  ఒకవేళ(colsp)
    *colsp = vt->నిలువుల_సంఖ్య;
}

శూన్య vterm_సమితి_కొలత(VTerm *vt, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
  ఒకవేళ(అడ్డువరుసలు_2 < 1 || నిలువుల_సంఖ్య < 1)
    తిరిగి;

  vt->అడ్డువరుసలు_2 = అడ్డువరుసలు_2;
  vt->నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;

  ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(అడ్డువరుసలు_2, నిలువుల_సంఖ్య, vt->parser.cbdata);
}

పూర్ణాంకం vterm_get_utf8(స్థిరరాశి VTerm *vt)
{
  తిరిగి vt->బహుళకం.utf8;
}

శూన్య vterm_సమితి_utf8(VTerm *vt, పూర్ణాంకం is_utf8)
{
  vt->బహుళకం.utf8 = is_utf8;
}

శూన్య vterm_output_సమితి_callback(VTerm *vt, VTermOutputCallback *func, శూన్య *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

అంతర్గత శూన్య vterm_నెట్టు_output_బైట్లు(VTerm *vt, స్థిరరాశి అక్షరం *బైట్లు, size_t len)
{
  ఒకవేళ(vt->outfunc) {
    (vt->outfunc)(బైట్లు, len, vt->outdata);
    తిరిగి;
  }

  ఒకవేళ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    తిరిగి;
  }

  జ్ఞాపకం_నకలు(vt->outbuffer + vt->outbuffer_cur, బైట్లు, len);
  vt->outbuffer_cur += len;
}

అంతర్గత శూన్య vterm_నెట్టు_output_vsprintf(VTerm *vt, స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  size_t len;
#నిర్వచితమైతే VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, వాదనలు);
#లేకపోతే
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, వాదనలు);
  ఒకవేళ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#అయితే_ముగింపు
  vterm_నెట్టు_output_బైట్లు(vt, vt->tmpbuffer, len);
}

అంతర్గత శూన్య vterm_నెట్టు_output_sprintf(VTerm *vt, స్థిరరాశి అక్షరం *format, ...)
{
  va_list వాదనలు;
  va_start(వాదనలు, format);
  vterm_నెట్టు_output_vsprintf(vt, format, వాదనలు);
  va_end(వాదనలు);
}

అంతర్గత శూన్య vterm_నెట్టు_output_sprintf_ctrl(VTerm *vt, గుర్తులేని అక్షరం ctrl, స్థిరరాశి అక్షరం *fmt, ...)
{
  size_t cur;

  ఒకవేళ(ctrl >= 0x80 && !vt->బహుళకం.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  లేకపోతే
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ఒకవేళ(cur >= vt->tmpbuffer_len)
    తిరిగి;
  vterm_నెట్టు_output_బైట్లు(vt, vt->tmpbuffer, cur);

  va_list వాదనలు;
  va_start(వాదనలు, fmt);
  vterm_నెట్టు_output_vsprintf(vt, fmt, వాదనలు);
  va_end(వాదనలు);
}

అంతర్గత శూన్య vterm_నెట్టు_output_sprintf_str(VTerm *vt, గుర్తులేని అక్షరం ctrl, పూర్ణాంకం పదం, స్థిరరాశి అక్షరం *fmt, ...)
{
  size_t cur;

  ఒకవేళ(ctrl) {
    ఒకవేళ(ctrl >= 0x80 && !vt->బహుళకం.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    లేకపోతే
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ఒకవేళ(cur >= vt->tmpbuffer_len)
      తిరిగి;
    vterm_నెట్టు_output_బైట్లు(vt, vt->tmpbuffer, cur);
  }

  va_list వాదనలు;
  va_start(వాదనలు, fmt);
  vterm_నెట్టు_output_vsprintf(vt, fmt, వాదనలు);
  va_end(వాదనలు);

  ఒకవేళ(పదం) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->బహుళకం.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ఒకవేళ(cur >= vt->tmpbuffer_len)
      తిరిగి;
    vterm_నెట్టు_output_బైట్లు(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_బఫరు_కొలత(స్థిరరాశి VTerm *vt)
{
  తిరిగి vt->outbuffer_len;
}

size_t vterm_output_get_బఫరు_current(స్థిరరాశి VTerm *vt)
{
  తిరిగి vt->outbuffer_cur;
}

size_t vterm_output_get_బఫరు_మిగిలిన(స్థిరరాశి VTerm *vt)
{
  తిరిగి vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, అక్షరం *బఫరు, size_t len)
{
  ఒకవేళ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  జ్ఞాపకం_నకలు(బఫరు, vt->outbuffer, len);

  ఒకవేళ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  తిరిగి len;
}

VTerm_విలువ_Type vterm_get_attr_type(VTermAttr attr)
{
  ఎంపిక(attr) {
    స్థితి VTERM_ATTR_BOLD:       తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_UNDERLINE:  తిరిగి VTERM_VALUETYPE_INT;
    స్థితి VTERM_ATTR_ITALIC:     తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_BLINK:      తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_తిరగేయి:    తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_CONCEAL:    తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_STRIKE:     తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_FONT:       తిరిగి VTERM_VALUETYPE_INT;
    స్థితి VTERM_ATTR_FOREGROUND: తిరిగి VTERM_VALUETYPE_COLOR;
    స్థితి VTERM_ATTR_BACKGROUND: తిరిగి VTERM_VALUETYPE_COLOR;
    స్థితి VTERM_ATTR_SMALL:      తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_ATTR_BASELINE:   తిరిగి VTERM_VALUETYPE_INT;

    స్థితి VTERM_N_ATTRS: తిరిగి 0;
  }
  తిరిగి 0; /* UNREACHABLE */
}

VTerm_విలువ_Type vterm_get_prop_type(VTermProp prop)
{
  ఎంపిక(prop) {
    స్థితి VTERM_PROP_CURSORVISIBLE: తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_PROP_CURSORBLINK:   తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_PROP_ALTSCREEN:     తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_PROP_TITLE:         తిరిగి VTERM_VALUETYPE_STRING;
    స్థితి VTERM_PROP_ICONNAME:      తిరిగి VTERM_VALUETYPE_STRING;
    స్థితి VTERM_PROP_తిరగేయి:       తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_PROP_CURSORSHAPE:   తిరిగి VTERM_VALUETYPE_INT;
    స్థితి VTERM_PROP_MOUSE:         తిరిగి VTERM_VALUETYPE_INT;
    స్థితి VTERM_PROP_FOCUSREPORT:   తిరిగి VTERM_VALUETYPE_BOOL;
    స్థితి VTERM_PROP_CURSORCOLOR:   తిరిగి VTERM_VALUETYPE_STRING;

    స్థితి VTERM_N_PROPS: తిరిగి 0;
  }
  తిరిగి 0; /* UNREACHABLE */
}

శూన్య vterm_scroll_rect(VTermRect rect,
    పూర్ణాంకం downward,
    పూర్ణాంకం rightward,
    పూర్ణాంకం (*moverect)(VTermRect src, VTermRect dest, శూన్య *user),
    పూర్ణాంకం (*eraserect)(VTermRect rect, పూర్ణాంకం selective, శూన్య *user),
    శూన్య *user)
{
  VTermRect src;
  VTermRect dest;

  ఒకవేళ(నిరపేక్ష(downward)  >= rect.చివర_అడ్డువరుస - rect.ప్రారంభం_అడ్డువరుస ||
     నిరపేక్ష(rightward) >= rect.చివర_నిలువు - rect.ప్రారంభం_నిలువు) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    తిరిగి;
  }

  ఒకవేళ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు;
    dest.చివర_నిలువు   = rect.చివర_నిలువు   - rightward;
    src.ప్రారంభం_నిలువు  = rect.ప్రారంభం_నిలువు + rightward;
    src.చివర_నిలువు    = rect.చివర_నిలువు;
  }
  లేకపోతే {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    పూర్ణాంకం leftward = -rightward;
    dest.ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు + leftward;
    dest.చివర_నిలువు   = rect.చివర_నిలువు;
    src.ప్రారంభం_నిలువు  = rect.ప్రారంభం_నిలువు;
    src.చివర_నిలువు    = rect.చివర_నిలువు - leftward;
  }

  ఒకవేళ(downward >= 0) {
    dest.ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస;
    dest.చివర_అడ్డువరుస   = rect.చివర_అడ్డువరుస   - downward;
    src.ప్రారంభం_అడ్డువరుస  = rect.ప్రారంభం_అడ్డువరుస + downward;
    src.చివర_అడ్డువరుస    = rect.చివర_అడ్డువరుస;
  }
  లేకపోతే {
    పూర్ణాంకం upward = -downward;
    dest.ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస + upward;
    dest.చివర_అడ్డువరుస   = rect.చివర_అడ్డువరుస;
    src.ప్రారంభం_అడ్డువరుస  = rect.ప్రారంభం_అడ్డువరుస;
    src.చివర_అడ్డువరుస    = rect.చివర_అడ్డువరుస - upward;
  }

  ఒకవేళ(moverect)
    (*moverect)(dest, src, user);

  ఒకవేళ(downward > 0)
    rect.ప్రారంభం_అడ్డువరుస = rect.చివర_అడ్డువరుస - downward;
  లేకపోతే ఒకవేళ(downward < 0)
    rect.చివర_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస - downward;

  ఒకవేళ(rightward > 0)
    rect.ప్రారంభం_నిలువు = rect.చివర_నిలువు - rightward;
  లేకపోతే ఒకవేళ(rightward < 0)
    rect.చివర_నిలువు = rect.ప్రారంభం_నిలువు - rightward;

  (*eraserect)(rect, 0, user);
}

శూన్య vterm_copy_cells(VTermRect dest,
    VTermRect src,
    శూన్య (*copycell)(VTermPos dest, VTermPos src, శూన్య *user),
    శూన్య *user)
{
  పూర్ణాంకం downward  = src.ప్రారంభం_అడ్డువరుస - dest.ప్రారంభం_అడ్డువరుస;
  పూర్ణాంకం rightward = src.ప్రారంభం_నిలువు - dest.ప్రారంభం_నిలువు;

  పూర్ణాంకం init_అడ్డువరుస, test_అడ్డువరుస, init_నిలువు, test_నిలువు;
  పూర్ణాంకం inc_అడ్డువరుస, inc_నిలువు;

  ఒకవేళ(downward < 0) {
    init_అడ్డువరుస = dest.చివర_అడ్డువరుస - 1;
    test_అడ్డువరుస = dest.ప్రారంభం_అడ్డువరుస - 1;
    inc_అడ్డువరుస = -1;
  }
  లేకపోతే /* downward >= 0 */ {
    init_అడ్డువరుస = dest.ప్రారంభం_అడ్డువరుస;
    test_అడ్డువరుస = dest.చివర_అడ్డువరుస;
    inc_అడ్డువరుస = +1;
  }

  ఒకవేళ(rightward < 0) {
    init_నిలువు = dest.చివర_నిలువు - 1;
    test_నిలువు = dest.ప్రారంభం_నిలువు - 1;
    inc_నిలువు = -1;
  }
  లేకపోతే /* rightward >= 0 */ {
    init_నిలువు = dest.ప్రారంభం_నిలువు;
    test_నిలువు = dest.చివర_నిలువు;
    inc_నిలువు = +1;
  }

  VTermPos pos;
  కొరకు(pos.అడ్డువరుస = init_అడ్డువరుస; pos.అడ్డువరుస != test_అడ్డువరుస; pos.అడ్డువరుస += inc_అడ్డువరుస)
    కొరకు(pos.నిలువు = init_నిలువు; pos.నిలువు != test_నిలువు; pos.నిలువు += inc_నిలువు) {
      VTermPos srcpos;
      srcpos.అడ్డువరుస = pos.అడ్డువరుస + downward;
      srcpos.నిలువు = pos.నిలువు + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

శూన్య vterm_check_సంస్కరణ(పూర్ణాంకం major, పూర్ణాంకం minor)
{
  ఒకవేళ(major != VTERM_సంస్కరణ_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_సంస్కరణ_MAJOR);
    నిష్క్రమించు(1);
  }

  ఒకవేళ(minor > VTERM_సంస్కరణ_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_సంస్కరణ_MINOR);
    నిష్క్రమించు(1);
  }

  // Happy
}
