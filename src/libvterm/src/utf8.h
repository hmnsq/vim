/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
अचिन्हित पूर्णांक utf8_seqlen(दीर्घ codepoint);

#जर defined(DEFINE_INLINES) || USE_INLINE
INLINE अचिन्हित पूर्णांक utf8_seqlen(दीर्घ codepoint)
{
  जर(codepoint < 0x0000080) परत 1;
  जर(codepoint < 0x0000800) परत 2;
  जर(codepoint < 0x0010000) परत 3;
  जर(codepoint < 0x0200000) परत 4;
  जर(codepoint < 0x4000000) परत 5;
  परत 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
पूर्णांक fill_utf8(दीर्घ codepoint, अक्षर *str);

#जर defined(DEFINE_INLINES) || USE_INLINE
INLINE पूर्णांक fill_utf8(दीर्घ codepoint, अक्षर *str)
{
  पूर्णांक nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  पूर्णांक b = nbytes;
  पर्यंत(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  निवड(nbytes) {
    स्थिती 1: str[0] =        (codepoint & 0x7f); थांबा;
    स्थिती 2: str[0] = 0xc0 | (codepoint & 0x1f); थांबा;
    स्थिती 3: str[0] = 0xe0 | (codepoint & 0x0f); थांबा;
    स्थिती 4: str[0] = 0xf0 | (codepoint & 0x07); थांबा;
    स्थिती 5: str[0] = 0xf8 | (codepoint & 0x03); थांबा;
    स्थिती 6: str[0] = 0xfc | (codepoint & 0x01); थांबा;
  }

  परत nbytes;
}
#endif
/* end copy */
