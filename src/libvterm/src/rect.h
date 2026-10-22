/*
 * Some utility functions on VTermRect structures
 */

#تعریف_کن STRFrect "(%d,%d-%d,%d)"
#تعریف_کن ARGSrect(r) (r).شروع_ردیف, (r).شروع_ستون, (r).انتها_ردیف, (r).انتها_ستون

/* Expand dst to contain src as well */
ساكن فراغ rect_expand(VTermRect *dst, VTermRect *src)
{
  إذا(dst->شروع_ردیف > src->شروع_ردیف) dst->شروع_ردیف = src->شروع_ردیف;
  إذا(dst->شروع_ستون > src->شروع_ستون) dst->شروع_ستون = src->شروع_ستون;
  إذا(dst->انتها_ردیف   < src->انتها_ردیف)   dst->انتها_ردیف   = src->انتها_ردیف;
  إذا(dst->انتها_ستون   < src->انتها_ستون)   dst->انتها_ستون   = src->انتها_ستون;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساكن فراغ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  إذا(dst->شروع_ردیف < bounds->شروع_ردیف) dst->شروع_ردیف = bounds->شروع_ردیف;
  إذا(dst->شروع_ستون < bounds->شروع_ستون) dst->شروع_ستون = bounds->شروع_ستون;
  إذا(dst->انتها_ردیف   > bounds->انتها_ردیف)   dst->انتها_ردیف   = bounds->انتها_ردیف;
  إذا(dst->انتها_ستون   > bounds->انتها_ستون)   dst->انتها_ستون   = bounds->انتها_ستون;
  /* Ensure it doesn't end up negatively-sized */
  إذا(dst->انتها_ردیف < dst->شروع_ردیف) dst->انتها_ردیف = dst->شروع_ردیف;
  إذا(dst->انتها_ستون < dst->شروع_ستون) dst->انتها_ستون = dst->شروع_ستون;
}

/* True if the two rectangles are equal */
ساكن صحيح rect_equal(VTermRect *a, VTermRect *b)
{
  أرجع (a->شروع_ردیف == b->شروع_ردیف) &&
         (a->شروع_ستون == b->شروع_ستون) &&
         (a->انتها_ردیف   == b->انتها_ردیف)   &&
         (a->انتها_ستون   == b->انتها_ستون);
}

/* True if small is contained entirely within big */
ساكن صحيح rect_شامل_است(VTermRect *big, VTermRect *small)
{
  إذا(small->شروع_ردیف < big->شروع_ردیف) أرجع 0;
  إذا(small->شروع_ستون < big->شروع_ستون) أرجع 0;
  إذا(small->انتها_ردیف   > big->انتها_ردیف)   أرجع 0;
  إذا(small->انتها_ستون   > big->انتها_ستون)   أرجع 0;
  أرجع 1;
}

/* True if the rectangles overlap at all */
ساكن صحيح rect_intersects(VTermRect *a, VTermRect *b)
{
  إذا(a->شروع_ردیف > b->انتها_ردیف || b->شروع_ردیف > a->انتها_ردیف)
    أرجع 0;
  إذا(a->شروع_ستون > b->انتها_ستون || b->شروع_ستون > a->انتها_ستون)
    أرجع 0;
  أرجع 1;
}
