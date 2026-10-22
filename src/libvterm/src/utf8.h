/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned පූර්ණ_සංඛ්‍යා utf8_seqlen(long codepoint);

#නම් defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned පූර්ණ_සංඛ්‍යා utf8_seqlen(long codepoint)
{
  නම්(codepoint < 0x0000080) ආපසු 1;
  නම්(codepoint < 0x0000800) ආපසු 2;
  නම්(codepoint < 0x0010000) ආපසු 3;
  නම්(codepoint < 0x0200000) ආපසු 4;
  නම්(codepoint < 0x4000000) ආපසු 5;
  ආපසු 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
පූර්ණ_සංඛ්‍යා fill_utf8(long codepoint, var_na *str);

#නම් defined(DEFINE_INLINES) || USE_INLINE
INLINE පූර්ණ_සංඛ්‍යා fill_utf8(long codepoint, var_na *str)
{
  පූර්ණ_සංඛ්‍යා nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  පූර්ණ_සංඛ්‍යා b = nbytes;
  අතරතුර(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  switch(nbytes) {
    pak_sa 1: str[0] =        (codepoint & 0x7f); bha_nga;
    pak_sa 2: str[0] = 0xc0 | (codepoint & 0x1f); bha_nga;
    pak_sa 3: str[0] = 0xe0 | (codepoint & 0x0f); bha_nga;
    pak_sa 4: str[0] = 0xf0 | (codepoint & 0x07); bha_nga;
    pak_sa 5: str[0] = 0xf8 | (codepoint & 0x03); bha_nga;
    pak_sa 6: str[0] = 0xfc | (codepoint & 0x01); bha_nga;
  }

  ආපසු nbytes;
}
#endif
/* end copy */
