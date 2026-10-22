/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned صحیح utf8_seqlen(long codepoint);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned صحیح utf8_seqlen(long codepoint)
{
  اگر(codepoint < 0x0000080) بازگشت 1;
  اگر(codepoint < 0x0000800) بازگشت 2;
  اگر(codepoint < 0x0010000) بازگشت 3;
  اگر(codepoint < 0x0200000) بازگشت 4;
  اگر(codepoint < 0x4000000) بازگشت 5;
  بازگشت 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
صحیح fill_utf8(long codepoint, نویسه *str);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE صحیح fill_utf8(long codepoint, نویسه *str)
{
  صحیح nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  صحیح b = nbytes;
  وقتی(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  انتخاب(nbytes) {
    حالت 1: str[0] =        (codepoint & 0x7f); بشکن;
    حالت 2: str[0] = 0xc0 | (codepoint & 0x1f); بشکن;
    حالت 3: str[0] = 0xe0 | (codepoint & 0x0f); بشکن;
    حالت 4: str[0] = 0xf0 | (codepoint & 0x07); بشکن;
    حالت 5: str[0] = 0xf8 | (codepoint & 0x03); بشکن;
    حالت 6: str[0] = 0xfc | (codepoint & 0x01); بشکن;
  }

  بازگشت nbytes;
}
#endif
/* end copy */
