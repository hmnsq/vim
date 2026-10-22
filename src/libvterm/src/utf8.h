/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ຈຳນວນເຕັມ utf8_seqlen(long codepoint);

#ຖ້າ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ຈຳນວນເຕັມ utf8_seqlen(long codepoint)
{
  ຖ້າ(codepoint < 0x0000080) ສົ່ງຄືນ 1;
  ຖ້າ(codepoint < 0x0000800) ສົ່ງຄືນ 2;
  ຖ້າ(codepoint < 0x0010000) ສົ່ງຄືນ 3;
  ຖ້າ(codepoint < 0x0200000) ສົ່ງຄືນ 4;
  ຖ້າ(codepoint < 0x4000000) ສົ່ງຄືນ 5;
  ສົ່ງຄືນ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ຈຳນວນເຕັມ fill_utf8(long codepoint, var_na *str);

#ຖ້າ defined(DEFINE_INLINES) || USE_INLINE
INLINE ຈຳນວນເຕັມ fill_utf8(long codepoint, var_na *str)
{
  ຈຳນວນເຕັມ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ຈຳນວນເຕັມ b = nbytes;
  ໃນຂະນະທີ່(b > 1) {
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

  ສົ່ງຄືນ nbytes;
}
#endif
/* end copy */
