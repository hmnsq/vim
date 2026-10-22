/*
 * Some utility functions on VTermRect structures
 */

#ವ್ಯಾಖ್ಯಾನಿಸು STRFrect "(%d,%d-%d,%d)"
#ವ್ಯಾಖ್ಯಾನಿಸು ARGSrect(r) (r).ಆರಂಭ_ಅಡ್ಡಸಾಲು, (r).ಆರಂಭ_ಲಂಬಸಾಲು, (r).ಕೊನೆ_ಅಡ್ಡಸಾಲು, (r).ಕೊನೆ_ಲಂಬಸಾಲು

/* Expand dst to contain src as well */
ಸ್ಥಿರ ಶೂನ್ಯ rect_expand(VTermRect *dst, VTermRect *src)
{
  ಆದರೆ(dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು > src->ಆರಂಭ_ಅಡ್ಡಸಾಲು) dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು = src->ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(dst->ಆರಂಭ_ಲಂಬಸಾಲು > src->ಆರಂಭ_ಲಂಬಸಾಲು) dst->ಆರಂಭ_ಲಂಬಸಾಲು = src->ಆರಂಭ_ಲಂಬಸಾಲು;
  ಆದರೆ(dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು   < src->ಕೊನೆ_ಅಡ್ಡಸಾಲು)   dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು   = src->ಕೊನೆ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(dst->ಕೊನೆ_ಲಂಬಸಾಲು   < src->ಕೊನೆ_ಲಂಬಸಾಲು)   dst->ಕೊನೆ_ಲಂಬಸಾಲು   = src->ಕೊನೆ_ಲಂಬಸಾಲು;
}

/* Clip the dst to ensure it does not step outside of bounds */
ಸ್ಥಿರ ಶೂನ್ಯ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ಆದರೆ(dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು < bounds->ಆರಂಭ_ಅಡ್ಡಸಾಲು) dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು = bounds->ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(dst->ಆರಂಭ_ಲಂಬಸಾಲು < bounds->ಆರಂಭ_ಲಂಬಸಾಲು) dst->ಆರಂಭ_ಲಂಬಸಾಲು = bounds->ಆರಂಭ_ಲಂಬಸಾಲು;
  ಆದರೆ(dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು   > bounds->ಕೊನೆ_ಅಡ್ಡಸಾಲು)   dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು   = bounds->ಕೊನೆ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(dst->ಕೊನೆ_ಲಂಬಸಾಲು   > bounds->ಕೊನೆ_ಲಂಬಸಾಲು)   dst->ಕೊನೆ_ಲಂಬಸಾಲು   = bounds->ಕೊನೆ_ಲಂಬಸಾಲು;
  /* Ensure it doesn't end up negatively-sized */
  ಆದರೆ(dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು < dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು) dst->ಕೊನೆ_ಅಡ್ಡಸಾಲು = dst->ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(dst->ಕೊನೆ_ಲಂಬಸಾಲು < dst->ಆರಂಭ_ಲಂಬಸಾಲು) dst->ಕೊನೆ_ಲಂಬಸಾಲು = dst->ಆರಂಭ_ಲಂಬಸಾಲು;
}

/* True if the two rectangles are equal */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rect_equal(VTermRect *a, VTermRect *b)
{
  ಹಿಂತಿರುಗಿ (a->ಆರಂಭ_ಅಡ್ಡಸಾಲು == b->ಆರಂಭ_ಅಡ್ಡಸಾಲು) &&
         (a->ಆರಂಭ_ಲಂಬಸಾಲು == b->ಆರಂಭ_ಲಂಬಸಾಲು) &&
         (a->ಕೊನೆ_ಅಡ್ಡಸಾಲು   == b->ಕೊನೆ_ಅಡ್ಡಸಾಲು)   &&
         (a->ಕೊನೆ_ಲಂಬಸಾಲು   == b->ಕೊನೆ_ಲಂಬಸಾಲು);
}

/* True if small is contained entirely within big */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rect_ಒಳಗೊಂಡಿದೆ(VTermRect *big, VTermRect *small)
{
  ಆದರೆ(small->ಆರಂಭ_ಅಡ್ಡಸಾಲು < big->ಆರಂಭ_ಅಡ್ಡಸಾಲು) ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(small->ಆರಂಭ_ಲಂಬಸಾಲು < big->ಆರಂಭ_ಲಂಬಸಾಲು) ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(small->ಕೊನೆ_ಅಡ್ಡಸಾಲು   > big->ಕೊನೆ_ಅಡ್ಡಸಾಲು)   ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(small->ಕೊನೆ_ಲಂಬಸಾಲು   > big->ಕೊನೆ_ಲಂಬಸಾಲು)   ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ 1;
}

/* True if the rectangles overlap at all */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rect_intersects(VTermRect *a, VTermRect *b)
{
  ಆದರೆ(a->ಆರಂಭ_ಅಡ್ಡಸಾಲು > b->ಕೊನೆ_ಅಡ್ಡಸಾಲು || b->ಆರಂಭ_ಅಡ್ಡಸಾಲು > a->ಕೊನೆ_ಅಡ್ಡಸಾಲು)
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(a->ಆರಂಭ_ಲಂಬಸಾಲು > b->ಕೊನೆ_ಲಂಬಸಾಲು || b->ಆರಂಭ_ಲಂಬಸಾಲು > a->ಕೊನೆ_ಲಂಬಸಾಲು)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ 1;
}
