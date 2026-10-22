/* vi:set ts=8 sts=4 sw=4 et:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * sound.c: functions related making noise
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SOUND)

స్థిర దీర్ఘ sound_id = 0;

// soundcb_T is typedef'ed in vim.h

నిర్మాణం soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    దీర్ఘ snd_id;
# endif
    soundcb_T *snd_తదుపరి;
};

స్థిర soundcb_T *మొదటి_callback = ఖాళీ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
పూర్ణాంకం
has_any_sound_callback(శూన్య)
{
    తిరిగి మొదటి_callback != ఖాళీ;
}

స్థిర soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    ఒకవేళ (arg->v_type == VAR_UNKNOWN)
        తిరిగి ఖాళీ;
    callback = get_callback(arg);
    ఒకవేళ (callback.cb_name == ఖాళీ)
        తిరిగి ఖాళీ;

    soundcb = ALLOC_ONE(soundcb_T);
    ఒకవేళ (soundcb == ఖాళీ)
    {
        free_callback(&callback);
        తిరిగి ఖాళీ;
    }

    soundcb->snd_తదుపరి = మొదటి_callback;
    మొదటి_callback = soundcb;
    సమితి_callback(&soundcb->snd_callback, &callback);
    తిరిగి soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
శూన్య
call_sound_callback(soundcb_T *soundcb, దీర్ఘ snd_id, పూర్ణాంకం ఫలితం)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = snd_id;
    argv[1].v_type = VAR_సంఖ్య;
    argv[1].vval.v_సంఖ్య = ఫలితం;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
శూన్య
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ఖాళీ;

    కొరకు (p = మొదటి_callback; p != ఖాళీ; prev = p, p = p->snd_తదుపరి)
        ఒకవేళ (p == soundcb)
        {
            ఒకవేళ (prev == ఖాళీ)
                మొదటి_callback = p->snd_తదుపరి;
            లేకపోతే
                prev->snd_తదుపరి = p->snd_తదుపరి;
            free_callback(&p->snd_callback);
            vim_free(p);
            ఆపు;
        }
}

# ఒకవేళ defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

స్థిర ca_context *context = ఖాళీ;

// Structure to store info about a sound callback to be invoked soon.
రకనిర్వచనం నిర్మాణం soundcb_వరుస_S soundcb_వరుస_T;

నిర్మాణం soundcb_వరుస_S
{
    soundcb_వరుస_T *scb_తదుపరి;
    uint32_t scb_id; // ID of the sound
    పూర్ణాంకం scb_ఫలితం; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
స్థిర soundcb_వరుస_T *callback_వరుస = ఖాళీ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
స్థిర శూన్య
sound_callback(ca_context *c వాడని,
               uint32_t id,
               పూర్ణాంకం error_సంకేతం,
               శూన్య *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_వరుస_T *scb;

    scb = ALLOC_ONE(soundcb_వరుస_T);
    ఒకవేళ (scb == ఖాళీ)
        తిరిగి;
    scb->scb_తదుపరి = callback_వరుస;
    callback_వరుస = scb;
    scb->scb_id = id;
    scb->scb_ఫలితం = error_సంకేతం == CA_SUCCESS ? 0
                      : error_సంకేతం == CA_ERROR_CANCELED ||
                              error_సంకేతం == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
పూర్ణాంకం
has_sound_callback_లోపల_వరుస(శూన్య)
{
    తిరిగి callback_వరుస != ఖాళీ;
}

/*
 * Invoke queued sound callbacks.
 */
శూన్య
invoke_sound_callback(శూన్య)
{
    soundcb_వరుస_T *scb;

    అయితే (callback_వరుస != ఖాళీ)
    {
        scb = callback_వరుస;
        callback_వరుస = scb->scb_తదుపరి;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_ఫలితం);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

స్థిర శూన్య
sound_play_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం playfile)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        తిరిగి;

    ఒకవేళ (context == ఖాళీ)
        ca_context_create(&context);
    ఒకవేళ (context == ఖాళీ)
        తిరిగి;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    పూర్ణాంకం res = CA_ERROR_INVALID;

    ++sound_id;
    ఒకవేళ (soundcb == ఖాళీ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ఖాళీ);
    }
    లేకపోతే
    {
        స్థిర ca_proplist *proplist = ఖాళీ;

        ca_proplist_create(&proplist);
        ఒకవేళ (proplist != ఖాళీ)
        {
            ఒకవేళ (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (అక్షరం *)tv_get_string(&argvars[0]));
            లేకపోతే
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (అక్షరం *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_నిండిన(context, sound_id, proplist,
                                       sound_callback, soundcb);
            ఒకవేళ (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_సంఖ్య = res == CA_SUCCESS ? sound_id : 0;
}

శూన్య
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
శూన్య
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
శూన్య
f_sound_stop(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
        తిరిగి;

    ఒకవేళ (context != ఖాళీ)
        ca_context_cancel(context, tv_get_సంఖ్య(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
శూన్య
f_sound_clear(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (context == ఖాళీ)
        తిరిగి;
    ca_context_destroy(context);
    context = ఖాళీ;
}

#  ఒకవేళ defined(EXITFREE)
శూన్య
sound_free(శూన్య)
{
    soundcb_వరుస_T *scb;

    ఒకవేళ (context != ఖాళీ)
        ca_context_destroy(context);

    అయితే (మొదటి_callback != ఖాళీ)
        delete_sound_callback(మొదటి_callback);

    అయితే (callback_వరుస != ఖాళీ)
    {
        scb = callback_వరుస;
        callback_వరుస = scb->scb_తదుపరి;
        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
}
#  endif

# elif defined(MSWIN)

/*
 * Sound implementation for MS-Windows.
 */

స్థిర HWND g_hWndSound = ఖాళీ;

స్థిర LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT సందేశం_2, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ఎంపిక (సందేశం_2)
    {
        స్థితి MM_MCINOTIFY:
            కొరకు (p = మొదటి_callback; p != ఖాళీ; p = p->snd_తదుపరి)
                ఒకవేళ (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    అక్షరం buf[32];

                    vim_snprintf(buf, పరిమాణం(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ఖాళీ, 0, 0);

                    దీర్ఘ ఫలితం = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, ఫలితం);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ఆపు;
    }

    తిరిగి Def_కిటికీ_Proc(hwnd, సందేశం_2, wParam, lParam);
}

స్థిర HWND
sound_కిటికీ(శూన్య)
{
    ఒకవేళ (g_hWndSound == ఖాళీ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ఖాళీ, 0,
                              0, ఖాళీ,          clazz };
        రిజిస్టరు_Class(&wndclass);
        g_hWndSound = Create_కిటికీ(clazz, ఖాళీ, 0, 0, 0, 0, 0, HWND_సందేశం,
                                   ఖాళీ, g_hinst, ఖాళీ);
    }

    తిరిగి g_hWndSound;
}

శూన్య
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        తిరిగి;

    wp = enc_గమ్యం_utf16(tv_get_string(&argvars[0]), ఖాళీ);
    ఒకవేళ (wp == ఖాళీ)
        తిరిగి;

    ఒకవేళ (PlaySoundW(wp, ఖాళీ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_సంఖ్య = ++sound_id;
    విడుదల(wp);
}

శూన్య
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    దీర్ఘ newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    అక్షరం buf[32];
    MCIERROR err;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        తిరిగి;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    ఒకవేళ (p == ఖాళీ)
    {
        తిరిగి;
    }
    vim_snprintf((అక్షరం *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_గమ్యం_utf16((char_u *)p, ఖాళీ);
    విడుదల(p);
    ఒకవేళ (wp == ఖాళీ)
        తిరిగి;

    err = mciSendStringW(wp, ఖాళీ, 0, sound_కిటికీ());
    విడుదల(wp);
    ఒకవేళ (err != 0)
        తిరిగి;

    vim_snprintf(buf, పరిమాణం(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ఖాళీ, 0, sound_కిటికీ());
    ఒకవేళ (err != 0)
        వెళ్ళు failure;

    sound_id = newid;
    rettv->vval.v_సంఖ్య = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    ఒకవేళ (soundcb != ఖాళీ)
    {
        vim_snprintf(buf, పరిమాణం(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    తిరిగి;

failure:
    vim_snprintf(buf, పరిమాణం(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ఖాళీ, 0, ఖాళీ);
}

శూన్య
f_sound_stop(typval_T *argvars, typval_T *rettv వాడని)
{
    దీర్ఘ id;
    అక్షరం buf[32];

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
        తిరిగి;

    id = tv_get_సంఖ్య(&argvars[0]);
    vim_snprintf(buf, పరిమాణం(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ఖాళీ, 0, ఖాళీ);
}

శూన్య
f_sound_clear(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    PlaySoundW(ఖాళీ, ఖాళీ, 0);
    mciSendStringA("close all", ఖాళీ, 0, ఖాళీ);
}

#  ఒకవేళ defined(EXITFREE)
శూన్య
sound_free(శూన్య)
{
    Close_కిటికీ(g_hWndSound);

    అయితే (మొదటి_callback != ఖాళీ)
        delete_sound_callback(మొదటి_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
స్థిర శూన్య
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        తిరిగి;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    ఒకవేళ (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_సంఖ్య = play_success ? sound_id : 0;
}

శూన్య
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

శూన్య
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

శూన్య
f_sound_stop(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
        తిరిగి;
    sound_mch_stop(tv_get_సంఖ్య(&argvars[0]));
}

శూన్య
f_sound_clear(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    sound_mch_clear();
}

#  ఒకవేళ defined(EXITFREE)
శూన్య
sound_free(శూన్య)
{
    sound_mch_free();
    అయితే (మొదటి_callback != ఖాళీ)
        delete_sound_callback(మొదటి_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#అయితే_ముగింపు // FEAT_SOUND
