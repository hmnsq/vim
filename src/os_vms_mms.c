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
    ᬲᭂᬤᭂᬮ (argc-- > 0)
    {
	argp = *argv++;
	ᬬᬾᬦ᭄ (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    ᬬᬾᬦ᭄ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ᬲᭂᬤᭂᬮ (fgets(buf, BUFSIZ, fpi))
    {
	ᬬᬾᬦ᭄ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (err_line)
	    {
		ᬬᬾᬦ᭄ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		ᬮᬶᬬ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ᬲᭂᬤᭂᬮ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ᬯᬮᬶ(0);
}
