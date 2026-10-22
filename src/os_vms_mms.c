#ضمن	<stdio.h>
#ضمن	<stdlib.h>
#ضمن	<string.h>
#ضمن	<unixio.h>
#ضمن	"vim.h"
صحيح main(صحيح argc, حرف *argv[])
{
    FILE	*fpi, *fpo;
    حرف	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    صحيح		err = 0, err_لي = 0;

    mms = "mms";
    argc--;
    argv++;
    طالما (argc-- > 0)
    {
	argp = *argv++;
	إذا (*argp == '-')
	{
	    بدل (*++argp)
	    {
		حالة 'm':
		    mms = ++argp;
		    اكسر;
		حالة 'e':
		    إذا (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    اكسر;
		افتراضي:
		    اكسر;
	    }
	}
	وإلا
	{
	    إذا (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, حجم(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    طالما (fgets(buf, BUFSIZ, fpi))
    {
	إذا (!memcmp(buf, "%CC-", 4))
	{
	    err_لي++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	وإلا
	{
	    إذا (err_لي)
	    {
		إذا (strstr(buf, _("At line")))
		{
		    err_لي = 0;
		    fprintf(fpo, "@");
		}
		وإلا
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    طالما (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    أرجع(0);
}
