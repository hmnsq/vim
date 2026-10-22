#ضمن	<stdio.h>
#ضمن	<stdlib.h>
#ضمن	<string.h>
#ضمن	<unixio.h>
#ضمن	"vim.h"
صحيح اصلي(صحيح argc, حرف *argv[])
{
    FILE	*fpi, *fpo;
    حرف	cmd[132], buf[BUFSIZ], *argp, *error_file, موخه[132], *mms;
    صحيح		err = 0, err_کرښه = 0;

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
	    إذا (*موخه)
		strcat(موخه, " ");
	    strcat(موخه, argp);
	}
    }
    vim_snprintf(cmd, حجم(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, موخه);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    طالما (fgets(buf, BUFSIZ, fpi))
    {
	إذا (!memcmp(buf, "%CC-", 4))
	{
	    err_کرښه++;
	    buf[مزي_اوږدوالی(buf)-1] = '\0';
	    err++;
	}
	وإلا
	{
	    إذا (err_کرښه)
	    {
		إذا (مزي_پلټنه(buf, _("At line")))
		{
		    err_کرښه = 0;
		    fprintf(fpo, "@");
		}
		وإلا
		    buf[مزي_اوږدوالی(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    طالما (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    وتل(err ? 44 : 1);
    أرجع(0);
}
