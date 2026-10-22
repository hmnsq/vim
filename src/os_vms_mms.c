#ضمن	<stdio.h>
#ضمن	<stdlib.h>
#ضمن	<string.h>
#ضمن	<unixio.h>
#ضمن	"vim.h"
صحيح مکيه(صحيح argc, حرف *argv[])
{
    FILE	*fpi, *fpo;
    حرف	cmd[132], buf[BUFSIZ], *argp, *error_file, هدف[132], *mms;
    صحيح		err = 0, err_سٽ = 0;

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
	    إذا (*هدف)
		strcat(هدف, " ");
	    strcat(هدف, argp);
	}
    }
    vim_snprintf(cmd, حجم(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, هدف);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    طالما (fgets(buf, BUFSIZ, fpi))
    {
	إذا (!memcmp(buf, "%CC-", 4))
	{
	    err_سٽ++;
	    buf[لڙي_ڊيگهه(buf)-1] = '\0';
	    err++;
	}
	وإلا
	{
	    إذا (err_سٽ)
	    {
		إذا (لڙي_ڳولا(buf, _("At line")))
		{
		    err_سٽ = 0;
		    fprintf(fpo, "@");
		}
		وإلا
		    buf[لڙي_ڊيگهه(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    طالما (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    ٻاهر_نڪرو(err ? 44 : 1);
    أرجع(0);
}
