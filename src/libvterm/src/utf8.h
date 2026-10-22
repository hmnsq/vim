/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_seqlen(long codepoint);

#ଯଦି defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_seqlen(long codepoint)
{
  ଯଦି(codepoint < 0x0000080) ଫେରନ୍ତୁ 1;
  ଯଦି(codepoint < 0x0000800) ଫେରନ୍ତୁ 2;
  ଯଦି(codepoint < 0x0010000) ଫେରନ୍ତୁ 3;
  ଯଦି(codepoint < 0x0200000) ଫେରନ୍ତୁ 4;
  ଯଦି(codepoint < 0x4000000) ଫେରନ୍ତୁ 5;
  ଫେରନ୍ତୁ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fill_utf8(long codepoint, ଅକ୍ଷର *str);

#ଯଦି defined(DEFINE_INLINES) || USE_INLINE
INLINE ପୂର୍ଣ୍ଣସଂଖ୍ୟା fill_utf8(long codepoint, ଅକ୍ଷର *str)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = nbytes;
  ଯେତେବେଳେ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ବାଛ(nbytes) {
    ସ୍ଥିତି 1: str[0] =        (codepoint & 0x7f); ଭାଙ୍ଗ;
    ସ୍ଥିତି 2: str[0] = 0xc0 | (codepoint & 0x1f); ଭାଙ୍ଗ;
    ସ୍ଥିତି 3: str[0] = 0xe0 | (codepoint & 0x0f); ଭାଙ୍ଗ;
    ସ୍ଥିତି 4: str[0] = 0xf0 | (codepoint & 0x07); ଭାଙ୍ଗ;
    ସ୍ଥିତି 5: str[0] = 0xf8 | (codepoint & 0x03); ଭାଙ୍ଗ;
    ସ୍ଥିତି 6: str[0] = 0xfc | (codepoint & 0x01); ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ nbytes;
}
#endif
/* end copy */
