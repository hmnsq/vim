#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
puur_naa_nka main(puur_naa_nka argc, var_na *argv[])
{
    FILE	*fpi, *fpo;
    var_na	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    puur_naa_nka		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    ᤀᤠ᤺ (argc-- > 0)
    {
	argp = *argv++;
	ᤀᤡ᤺ (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    ᤀᤡ᤺ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    ᤀᤡ᤺ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ᤀᤠ᤺ (fgets(buf, BUFSIZ, fpi))
    {
	ᤀᤡ᤺ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    ᤀᤡ᤺ (err_line)
	    {
		ᤀᤡ᤺ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		ᤀᤡ᤺ᤆᤡ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ᤀᤠ᤺ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ᤀᤡᤴ(0);
}
