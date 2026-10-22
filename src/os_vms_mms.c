#అన్తర్భావ	<stdio.h>
#అన్తర్భావ	<stdlib.h>
#అన్తర్భావ	<string.h>
#అన్తర్భావ	<unixio.h>
#అన్తర్భావ	"vim.h"
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
    FILE	*fpi, *fpo;
    అక్షరం	cmd[132], buf[BUFSIZ], *argp, *error_file, లక్ష్యం[132], *mms;
    పూర్ణాంకం		err = 0, err_పంక్తి = 0;

    mms = "mms";
    argc--;
    argv++;
    అయితే (argc-- > 0)
    {
	argp = *argv++;
	ఒకవేళ (*argp == '-')
	{
	    ఎంపిక (*++argp)
	    {
		స్థితి 'm':
		    mms = ++argp;
		    ఆపు;
		స్థితి 'e':
		    ఒకవేళ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ఆపు;
		అప్రమేయ:
		    ఆపు;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (*లక్ష్యం)
		strcat(లక్ష్యం, " ");
	    strcat(లక్ష్యం, argp);
	}
    }
    vim_snprintf(cmd, పరిమాణం(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, లక్ష్యం);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    అయితే (fgets(buf, BUFSIZ, fpi))
    {
	ఒకవేళ (!memcmp(buf, "%CC-", 4))
	{
	    err_పంక్తి++;
	    buf[అక్షరశ్రేణి_పొడవు(buf)-1] = '\0';
	    err++;
	}
	లేకపోతే
	{
	    ఒకవేళ (err_పంక్తి)
	    {
		ఒకవేళ (అక్షరశ్రేణి_శోధన(buf, _("At line")))
		{
		    err_పంక్తి = 0;
		    fprintf(fpo, "@");
		}
		లేకపోతే
		    buf[అక్షరశ్రేణి_పొడవు(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    అయితే (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    నిష్క్రమించు(err ? 44 : 1);
    తిరిగి(0);
}
