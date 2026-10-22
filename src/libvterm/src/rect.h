/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
ಸ್ಥಿರ ಶೂನ್ಯ rect_expand(VTermRect *dst, VTermRect *src)
{
  ಯದಿ(dst->start_row > src->start_row) dst->start_row = src->start_row;
  ಯದಿ(dst->start_col > src->start_col) dst->start_col = src->start_col;
  ಯದಿ(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  ಯದಿ(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
ಸ್ಥಿರ ಶೂನ್ಯ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ಯದಿ(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  ಯದಿ(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  ಯದಿ(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  ಯದಿ(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  ಯದಿ(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  ಯದಿ(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rect_equal(VTermRect *a, VTermRect *b)
{
  ಪ್ರತ್ಯಾಗಮ (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rect_contains(VTermRect *big, VTermRect *small)
{
  ಯದಿ(small->start_row < big->start_row) ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(small->start_col < big->start_col) ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(small->end_row   > big->end_row)   ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(small->end_col   > big->end_col)   ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ 1;
}

/* True if the rectangles overlap at all */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rect_intersects(VTermRect *a, VTermRect *b)
{
  ಯದಿ(a->start_row > b->end_row || b->start_row > a->end_row)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(a->start_col > b->end_col || b->start_col > a->end_col)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ 1;
}
