/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
स्थिर शून्य rect_expand(VTermRect *dst, VTermRect *src)
{
  सचे(dst->start_row > src->start_row) dst->start_row = src->start_row;
  सचे(dst->start_col > src->start_col) dst->start_col = src->start_col;
  सचे(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  सचे(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
स्थिर शून्य rect_clip(VTermRect *dst, VTermRect *bounds)
{
  सचे(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  सचे(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  सचे(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  सचे(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  सचे(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  सचे(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
स्थिर पूर्णाङ्क rect_equal(VTermRect *a, VTermRect *b)
{
  पटिनिवत्त (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
स्थिर पूर्णाङ्क rect_contains(VTermRect *big, VTermRect *small)
{
  सचे(small->start_row < big->start_row) पटिनिवत्त 0;
  सचे(small->start_col < big->start_col) पटिनिवत्त 0;
  सचे(small->end_row   > big->end_row)   पटिनिवत्त 0;
  सचे(small->end_col   > big->end_col)   पटिनिवत्त 0;
  पटिनिवत्त 1;
}

/* True if the rectangles overlap at all */
स्थिर पूर्णाङ्क rect_intersects(VTermRect *a, VTermRect *b)
{
  सचे(a->start_row > b->end_row || b->start_row > a->end_row)
    पटिनिवत्त 0;
  सचे(a->start_col > b->end_col || b->start_col > a->end_col)
    पटिनिवत्त 0;
  पटिनिवत्त 1;
}
