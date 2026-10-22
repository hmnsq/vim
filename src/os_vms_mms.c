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
    ᮞᮨᮓᮨᮜ (argc-- > 0)
    {
	argp = *argv++;
	ᮚᮦᮔ᮪ (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    ᮚᮦᮔ᮪ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ᮞᮨᮓᮨᮜ (fgets(buf, BUFSIZ, fpi))
    {
	ᮚᮦᮔ᮪ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (err_line)
	    {
		ᮚᮦᮔ᮪ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		ᮜᮤᮚ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ᮞᮨᮓᮨᮜ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ᮝᮜᮤ(0);
}
