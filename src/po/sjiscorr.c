// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
	वर्ण buffer[बफ_मान];
	वर्ण *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(मानक_निवेश), O_द्विआधारी_2);
	_setmode(_fileno(मानक_निकास), O_द्विआधारी_2);
#endif

	जबसम्म (ख_माला_लो(buffer, बफ_मान, मानक_निवेश) != रिक्त)
	{
		लागि (p = buffer; *p != 0; p++)
		{
			यदि (म_भेदन(p, "charset=utf-8", 13) == 0
				|| म_भेदन(p, "charset=UTF-8", 13) == 0)
			{
				ख_माला_दो("charset=CP932", मानक_निकास);
				p += 12;
			}
			अन्यथा यदि (म_भेदन(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				ख_माला_दो("# Generated from ja.po, DO NOT EDIT.", मानक_निकास);
			}
			अन्यथा यदि (*(अचिन्हित वर्ण *)p == 0x81 && p[1] == '_')
			{
				अक्षर_दो('\\');
				++p;
			}
			अन्यथा
			{
				यदि (*p & 0x80)
				{
					अक्षर_दो(*p++);
					यदि (*p == '\\')
						अक्षर_दो(*p);
				}
				अक्षर_दो(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
