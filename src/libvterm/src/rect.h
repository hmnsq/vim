/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
स्थिर शून्य rect_expand(VTermRect *dst, VTermRect *src)
{
  यदि(dst->start_row > src->start_row) dst->start_row = src->start_row;
  यदि(dst->start_col > src->start_col) dst->start_col = src->start_col;
  यदि(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  यदि(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
स्थिर शून्य rect_clip(VTermRect *dst, VTermRect *bounds)
{
  यदि(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  यदि(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  यदि(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  यदि(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  यदि(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  यदि(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
स्थिर पूर्णाङ्क rect_equal(VTermRect *a, VTermRect *b)
{
  प्रत्यागम (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
स्थिर पूर्णाङ्क rect_contains(VTermRect *big, VTermRect *small)
{
  यदि(small->start_row < big->start_row) प्रत्यागम 0;
  यदि(small->start_col < big->start_col) प्रत्यागम 0;
  यदि(small->end_row   > big->end_row)   प्रत्यागम 0;
  यदि(small->end_col   > big->end_col)   प्रत्यागम 0;
  प्रत्यागम 1;
}

/* True if the rectangles overlap at all */
स्थिर पूर्णाङ्क rect_intersects(VTermRect *a, VTermRect *b)
{
  यदि(a->start_row > b->end_row || b->start_row > a->end_row)
    प्रत्यागम 0;
  यदि(a->start_col > b->end_col || b->start_col > a->end_col)
    प्रत्यागम 0;
  प्रत्यागम 1;
}
