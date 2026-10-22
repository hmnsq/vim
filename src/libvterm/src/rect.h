/*
 * Some utility functions on VTermRect structures
 */

#تعریف_کړه STRFrect "(%d,%d-%d,%d)"
#تعریف_کړه ARGSrect(r) (r).پیل_لیکه, (r).پیل_ستون, (r).پای_ټکی_لیکه, (r).پای_ټکی_ستون

/* Expand dst to contain src as well */
ساكن فراغ rect_expand(VTermRect *dst, VTermRect *src)
{
  إذا(dst->پیل_لیکه > src->پیل_لیکه) dst->پیل_لیکه = src->پیل_لیکه;
  إذا(dst->پیل_ستون > src->پیل_ستون) dst->پیل_ستون = src->پیل_ستون;
  إذا(dst->پای_ټکی_لیکه   < src->پای_ټکی_لیکه)   dst->پای_ټکی_لیکه   = src->پای_ټکی_لیکه;
  إذا(dst->پای_ټکی_ستون   < src->پای_ټکی_ستون)   dst->پای_ټکی_ستون   = src->پای_ټکی_ستون;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساكن فراغ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  إذا(dst->پیل_لیکه < bounds->پیل_لیکه) dst->پیل_لیکه = bounds->پیل_لیکه;
  إذا(dst->پیل_ستون < bounds->پیل_ستون) dst->پیل_ستون = bounds->پیل_ستون;
  إذا(dst->پای_ټکی_لیکه   > bounds->پای_ټکی_لیکه)   dst->پای_ټکی_لیکه   = bounds->پای_ټکی_لیکه;
  إذا(dst->پای_ټکی_ستون   > bounds->پای_ټکی_ستون)   dst->پای_ټکی_ستون   = bounds->پای_ټکی_ستون;
  /* Ensure it doesn't end up negatively-sized */
  إذا(dst->پای_ټکی_لیکه < dst->پیل_لیکه) dst->پای_ټکی_لیکه = dst->پیل_لیکه;
  إذا(dst->پای_ټکی_ستون < dst->پیل_ستون) dst->پای_ټکی_ستون = dst->پیل_ستون;
}

/* True if the two rectangles are equal */
ساكن صحيح rect_equal(VTermRect *a, VTermRect *b)
{
  أرجع (a->پیل_لیکه == b->پیل_لیکه) &&
         (a->پیل_ستون == b->پیل_ستون) &&
         (a->پای_ټکی_لیکه   == b->پای_ټکی_لیکه)   &&
         (a->پای_ټکی_ستون   == b->پای_ټکی_ستون);
}

/* True if small is contained entirely within big */
ساكن صحيح rect_لري(VTermRect *big, VTermRect *small)
{
  إذا(small->پیل_لیکه < big->پیل_لیکه) أرجع 0;
  إذا(small->پیل_ستون < big->پیل_ستون) أرجع 0;
  إذا(small->پای_ټکی_لیکه   > big->پای_ټکی_لیکه)   أرجع 0;
  إذا(small->پای_ټکی_ستون   > big->پای_ټکی_ستون)   أرجع 0;
  أرجع 1;
}

/* True if the rectangles overlap at all */
ساكن صحيح rect_intersects(VTermRect *a, VTermRect *b)
{
  إذا(a->پیل_لیکه > b->پای_ټکی_لیکه || b->پیل_لیکه > a->پای_ټکی_لیکه)
    أرجع 0;
  إذا(a->پیل_ستون > b->پای_ټکی_ستون || b->پیل_ستون > a->پای_ټکی_ستون)
    أرجع 0;
  أرجع 1;
}
