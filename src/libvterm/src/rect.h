/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
ସ୍ଥିର ଶୂନ୍ୟ rect_expand(VTermRect *dst, VTermRect *src)
{
  ଯଦି(dst->start_row > src->start_row) dst->start_row = src->start_row;
  ଯଦି(dst->start_col > src->start_col) dst->start_col = src->start_col;
  ଯଦି(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  ଯଦି(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
ସ୍ଥିର ଶୂନ୍ୟ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ଯଦି(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  ଯଦି(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  ଯଦି(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  ଯଦି(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  ଯଦି(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  ଯଦି(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rect_equal(VTermRect *a, VTermRect *b)
{
  ଫେରନ୍ତୁ (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rect_contains(VTermRect *big, VTermRect *small)
{
  ଯଦି(small->start_row < big->start_row) ଫେରନ୍ତୁ 0;
  ଯଦି(small->start_col < big->start_col) ଫେରନ୍ତୁ 0;
  ଯଦି(small->end_row   > big->end_row)   ଫେରନ୍ତୁ 0;
  ଯଦି(small->end_col   > big->end_col)   ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ 1;
}

/* True if the rectangles overlap at all */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rect_intersects(VTermRect *a, VTermRect *b)
{
  ଯଦି(a->start_row > b->end_row || b->start_row > a->end_row)
    ଫେରନ୍ତୁ 0;
  ଯଦି(a->start_col > b->end_col || b->start_col > a->end_col)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ 1;
}
