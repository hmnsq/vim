#ಅನ್ತರ್ಭಾವ	<stdio.h>
#ಅನ್ತರ್ಭಾವ	<stdlib.h>
#ಅನ್ತರ್ಭಾವ	<string.h>
#ಅನ್ತರ್ಭಾವ	<unixio.h>
#ಅನ್ತರ್ಭಾವ	"vim.h"
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
    FILE	*fpi, *fpo;
    ಅಕ್ಷರ	cmd[132], buf[BUFSIZ], *argp, *error_file, ಗುರಿ[132], *mms;
    ಪೂರ್ಣಾಂಕ		err = 0, err_ಸಾಲು = 0;

    mms = "mms";
    argc--;
    argv++;
    ಆಗ (argc-- > 0)
    {
	argp = *argv++;
	ಆದರೆ (*argp == '-')
	{
	    ಆಯ್ಕೆ (*++argp)
	    {
		ಸ್ಥಿತಿ 'm':
		    mms = ++argp;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'e':
		    ಆದರೆ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*ಗುರಿ)
		strcat(ಗುರಿ, " ");
	    strcat(ಗುರಿ, argp);
	}
    }
    vim_snprintf(cmd, ಗಾತ್ರ(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, ಗುರಿ);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ಆಗ (fgets(buf, BUFSIZ, fpi))
    {
	ಆದರೆ (!memcmp(buf, "%CC-", 4))
	{
	    err_ಸಾಲು++;
	    buf[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf)-1] = '\0';
	    err++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (err_ಸಾಲು)
	    {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(buf, _("At line")))
		{
		    err_ಸಾಲು = 0;
		    fprintf(fpo, "@");
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    buf[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ಆಗ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    ನಿರ್ಗಮಿಸು(err ? 44 : 1);
    ಹಿಂತಿರುಗಿ(0);
}
