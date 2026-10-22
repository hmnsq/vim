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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SOUND)

ಸ್ಥಿರ ದೀರ್ಘ sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ರಚನೆ soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    ದೀರ್ಘ snd_id;
# endif
    soundcb_T *snd_ಮುಂದಿನ;
};

ಸ್ಥಿರ soundcb_T *ಮೊದಲ_callback = ಖಾಲಿ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
ಪೂರ್ಣಾಂಕ
has_any_sound_callback(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಮೊದಲ_callback != ಖಾಲಿ;
}

ಸ್ಥಿರ soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    ಆದರೆ (arg->v_type == VAR_UNKNOWN)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    callback = get_callback(arg);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    soundcb = ALLOC_ONE(soundcb_T);
    ಆದರೆ (soundcb == ಖಾಲಿ)
    {
        free_callback(&callback);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    soundcb->snd_ಮುಂದಿನ = ಮೊದಲ_callback;
    ಮೊದಲ_callback = soundcb;
    ಗಣ_callback(&soundcb->snd_callback, &callback);
    ಹಿಂತಿರುಗಿ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
ಶೂನ್ಯ
call_sound_callback(soundcb_T *soundcb, ದೀರ್ಘ snd_id, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = snd_id;
    argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[1].vval.v_ಸಂಖ್ಯೆ = ಫಲಿತಾಂಶ;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
ಶೂನ್ಯ
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ಖಾಲಿ;

    ಗಾಗಿ (p = ಮೊದಲ_callback; p != ಖಾಲಿ; prev = p, p = p->snd_ಮುಂದಿನ)
        ಆದರೆ (p == soundcb)
        {
            ಆದರೆ (prev == ಖಾಲಿ)
                ಮೊದಲ_callback = p->snd_ಮುಂದಿನ;
            ಇಲ್ಲದಿದ್ದರೆ
                prev->snd_ಮುಂದಿನ = p->snd_ಮುಂದಿನ;
            free_callback(&p->snd_callback);
            vim_free(p);
            ನಿಲ್ಲಿಸು;
        }
}

# ಆದರೆ defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ಸ್ಥಿರ ca_context *context = ಖಾಲಿ;

// Structure to store info about a sound callback to be invoked soon.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ soundcb_ಸರತಿ_S soundcb_ಸರತಿ_T;

ರಚನೆ soundcb_ಸರತಿ_S
{
    soundcb_ಸರತಿ_T *scb_ಮುಂದಿನ;
    uint32_t scb_id; // ID of the sound
    ಪೂರ್ಣಾಂಕ scb_ಫಲಿತಾಂಶ; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ಸ್ಥಿರ soundcb_ಸರತಿ_T *callback_ಸರತಿ = ಖಾಲಿ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sound_callback(ca_context *c ಬಳಸದ,
               uint32_t id,
               ಪೂರ್ಣಾಂಕ error_ಸಂಕೇತ,
               ಶೂನ್ಯ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_ಸರತಿ_T *scb;

    scb = ALLOC_ONE(soundcb_ಸರತಿ_T);
    ಆದರೆ (scb == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;
    scb->scb_ಮುಂದಿನ = callback_ಸರತಿ;
    callback_ಸರತಿ = scb;
    scb->scb_id = id;
    scb->scb_ಫಲಿತಾಂಶ = error_ಸಂಕೇತ == CA_SUCCESS ? 0
                      : error_ಸಂಕೇತ == CA_ERROR_CANCELED ||
                              error_ಸಂಕೇತ == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
ಪೂರ್ಣಾಂಕ
has_sound_callback_ಒಳ_ಸರತಿ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ callback_ಸರತಿ != ಖಾಲಿ;
}

/*
 * Invoke queued sound callbacks.
 */
ಶೂನ್ಯ
invoke_sound_callback(ಶೂನ್ಯ)
{
    soundcb_ಸರತಿ_T *scb;

    ಆಗ (callback_ಸರತಿ != ಖಾಲಿ)
    {
        scb = callback_ಸರತಿ;
        callback_ಸರತಿ = scb->scb_ಮುಂದಿನ;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_ಫಲಿತಾಂಶ);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ಸ್ಥಿರ ಶೂನ್ಯ
sound_play_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ playfile)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (context == ಖಾಲಿ)
        ca_context_create(&context);
    ಆದರೆ (context == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    ಪೂರ್ಣಾಂಕ res = CA_ERROR_INVALID;

    ++sound_id;
    ಆದರೆ (soundcb == ಖಾಲಿ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಸ್ಥಿರ ca_proplist *proplist = ಖಾಲಿ;

        ca_proplist_create(&proplist);
        ಆದರೆ (proplist != ಖಾಲಿ)
        {
            ಆದರೆ (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (ಅಕ್ಷರ *)tv_get_string(&argvars[0]));
            ಇಲ್ಲದಿದ್ದರೆ
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (ಅಕ್ಷರ *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_ತುಂಬಿದ(context, sound_id, proplist,
                                       sound_callback, soundcb);
            ಆದರೆ (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = res == CA_SUCCESS ? sound_id : 0;
}

ಶೂನ್ಯ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
ಶೂನ್ಯ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
ಶೂನ್ಯ
f_sound_stop(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (context != ಖಾಲಿ)
        ca_context_cancel(context, tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
ಶೂನ್ಯ
f_sound_clear(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (context == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;
    ca_context_destroy(context);
    context = ಖಾಲಿ;
}

#  ಆದರೆ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    soundcb_ಸರತಿ_T *scb;

    ಆದರೆ (context != ಖಾಲಿ)
        ca_context_destroy(context);

    ಆಗ (ಮೊದಲ_callback != ಖಾಲಿ)
        delete_sound_callback(ಮೊದಲ_callback);

    ಆಗ (callback_ಸರತಿ != ಖಾಲಿ)
    {
        scb = callback_ಸರತಿ;
        callback_ಸರತಿ = scb->scb_ಮುಂದಿನ;
        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
}
#  endif

# elif defined(MSWIN)

/*
 * Sound implementation for MS-Windows.
 */

ಸ್ಥಿರ HWND g_hWndSound = ಖಾಲಿ;

ಸ್ಥಿರ LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT ಸಂದೇಶ_2, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ಆಯ್ಕೆ (ಸಂದೇಶ_2)
    {
        ಸ್ಥಿತಿ MM_MCINOTIFY:
            ಗಾಗಿ (p = ಮೊದಲ_callback; p != ಖಾಲಿ; p = p->snd_ಮುಂದಿನ)
                ಆದರೆ (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    ಅಕ್ಷರ buf[32];

                    vim_snprintf(buf, ಗಾತ್ರ(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ಖಾಲಿ, 0, 0);

                    ದೀರ್ಘ ಫಲಿತಾಂಶ = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, ಫಲಿತಾಂಶ);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc(hwnd, ಸಂದೇಶ_2, wParam, lParam);
}

ಸ್ಥಿರ HWND
sound_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (g_hWndSound == ಖಾಲಿ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ಖಾಲಿ, 0,
                              0, ಖಾಲಿ,          clazz };
        ರಿಜಿಸ್ಟರ್_Class(&wndclass);
        g_hWndSound = Create_ಕಿಟಕಿ(clazz, ಖಾಲಿ, 0, 0, 0, 0, 0, HWND_ಸಂದೇಶ,
                                   ಖಾಲಿ, g_hinst, ಖಾಲಿ);
    }

    ಹಿಂತಿರುಗಿ g_hWndSound;
}

ಶೂನ್ಯ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    wp = enc_ಗಮ್ಯ_utf16(tv_get_string(&argvars[0]), ಖಾಲಿ);
    ಆದರೆ (wp == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (PlaySoundW(wp, ಖಾಲಿ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_ಸಂಖ್ಯೆ = ++sound_id;
    ಬಿಡುಗಡೆ(wp);
}

ಶೂನ್ಯ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    ದೀರ್ಘ newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    ಅಕ್ಷರ buf[32];
    MCIERROR err;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    ಆದರೆ (p == ಖಾಲಿ)
    {
        ಹಿಂತಿರುಗಿ;
    }
    vim_snprintf((ಅಕ್ಷರ *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_ಗಮ್ಯ_utf16((char_u *)p, ಖಾಲಿ);
    ಬಿಡುಗಡೆ(p);
    ಆದರೆ (wp == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    err = mciSendStringW(wp, ಖಾಲಿ, 0, sound_ಕಿಟಕಿ());
    ಬಿಡುಗಡೆ(wp);
    ಆದರೆ (err != 0)
        ಹಿಂತಿರುಗಿ;

    vim_snprintf(buf, ಗಾತ್ರ(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ಖಾಲಿ, 0, sound_ಕಿಟಕಿ());
    ಆದರೆ (err != 0)
        ಹೋಗು failure;

    sound_id = newid;
    rettv->vval.v_ಸಂಖ್ಯೆ = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    ಆದರೆ (soundcb != ಖಾಲಿ)
    {
        vim_snprintf(buf, ಗಾತ್ರ(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    ಹಿಂತಿರುಗಿ;

failure:
    vim_snprintf(buf, ಗಾತ್ರ(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ಖಾಲಿ, 0, ಖಾಲಿ);
}

ಶೂನ್ಯ
f_sound_stop(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ದೀರ್ಘ id;
    ಅಕ್ಷರ buf[32];

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    vim_snprintf(buf, ಗಾತ್ರ(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ಖಾಲಿ, 0, ಖಾಲಿ);
}

ಶೂನ್ಯ
f_sound_clear(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    PlaySoundW(ಖಾಲಿ, ಖಾಲಿ, 0);
    mciSendStringA("close all", ಖಾಲಿ, 0, ಖಾಲಿ);
}

#  ಆದರೆ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    Close_ಕಿಟಕಿ(g_hWndSound);

    ಆಗ (ಮೊದಲ_callback != ಖಾಲಿ)
        delete_sound_callback(ಮೊದಲ_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ಸ್ಥಿರ ಶೂನ್ಯ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    ಆದರೆ (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = play_success ? sound_id : 0;
}

ಶೂನ್ಯ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

ಶೂನ್ಯ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

ಶೂನ್ಯ
f_sound_stop(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;
    sound_mch_stop(tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
}

ಶೂನ್ಯ
f_sound_clear(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    sound_mch_clear();
}

#  ಆದರೆ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    sound_mch_free();
    ಆಗ (ಮೊದಲ_callback != ಖಾಲಿ)
        delete_sound_callback(ಮೊದಲ_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SOUND
