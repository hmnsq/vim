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

#ಯದಿ defined(FEAT_SOUND)

ಸ್ಥಿರ long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ಸಂರಚನಾ soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

ಸ್ಥಿರ soundcb_T *first_callback = ರಿಕ್ತ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
ಪೂರ್ಣಾಙ್ಕ
has_any_sound_callback(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ first_callback != ರಿಕ್ತ;
}

ಸ್ಥಿರ soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    ಯದಿ (arg->v_type == VAR_UNKNOWN)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    callback = get_callback(arg);
    ಯದಿ (callback.cb_name == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    soundcb = ALLOC_ONE(soundcb_T);
    ಯದಿ (soundcb == ರಿಕ್ತ)
    {
        free_callback(&callback);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    ಪ್ರತ್ಯಾಗಮ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
ಶೂನ್ಯ
call_sound_callback(soundcb_T *soundcb, long snd_id, ಪೂರ್ಣಾಙ್ಕ result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = snd_id;
    argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[1].vval.v_ಸಂಖ್ಯೆ = result;
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
    soundcb_T *prev = ರಿಕ್ತ;

    ಪ್ರತಿ (p = first_callback; p != ರಿಕ್ತ; prev = p, p = p->snd_next)
        ಯದಿ (p == soundcb)
        {
            ಯದಿ (prev == ರಿಕ್ತ)
                first_callback = p->snd_next;
            ಅನ್ಯಥಾ
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            ಭಙ್ಗ;
        }
}

# ಯದಿ defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ಸ್ಥಿರ ca_context *context = ರಿಕ್ತ;

// Structure to store info about a sound callback to be invoked soon.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ soundcb_queue_S soundcb_queue_T;

ಸಂರಚನಾ soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    ಪೂರ್ಣಾಙ್ಕ scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ಸ್ಥಿರ soundcb_queue_T *callback_queue = ರಿಕ್ತ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               ಪೂರ್ಣಾಙ್ಕ error_code,
               ಶೂನ್ಯ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    ಯದಿ (scb == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;
    scb->scb_next = callback_queue;
    callback_queue = scb;
    scb->scb_id = id;
    scb->scb_result = error_code == CA_SUCCESS ? 0
                      : error_code == CA_ERROR_CANCELED ||
                              error_code == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
ಪೂರ್ಣಾಙ್ಕ
has_sound_callback_in_queue(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ callback_queue != ರಿಕ್ತ;
}

/*
 * Invoke queued sound callbacks.
 */
ಶೂನ್ಯ
invoke_sound_callback(ಶೂನ್ಯ)
{
    soundcb_queue_T *scb;

    ಯಾವತ್ (callback_queue != ರಿಕ್ತ)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ಸ್ಥಿರ ಶೂನ್ಯ
sound_play_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ playfile)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (context == ರಿಕ್ತ)
        ca_context_create(&context);
    ಯದಿ (context == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    ಪೂರ್ಣಾಙ್ಕ res = CA_ERROR_INVALID;

    ++sound_id;
    ಯದಿ (soundcb == ರಿಕ್ತ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
        ಸ್ಥಿರ ca_proplist *proplist = ರಿಕ್ತ;

        ca_proplist_create(&proplist);
        ಯದಿ (proplist != ರಿಕ್ತ)
        {
            ಯದಿ (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (ವರ್ಣ *)tv_get_string(&argvars[0]));
            ಅನ್ಯಥಾ
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (ವರ್ಣ *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            ಯದಿ (res != CA_SUCCESS)
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
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (context != ರಿಕ್ತ)
        ca_context_cancel(context, tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
ಶೂನ್ಯ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಯದಿ (context == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;
    ca_context_destroy(context);
    context = ರಿಕ್ತ;
}

#  ಯದಿ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    soundcb_queue_T *scb;

    ಯದಿ (context != ರಿಕ್ತ)
        ca_context_destroy(context);

    ಯಾವತ್ (first_callback != ರಿಕ್ತ)
        delete_sound_callback(first_callback);

    ಯಾವತ್ (callback_queue != ರಿಕ್ತ)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;
        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
}
#  endif

# elif defined(MSWIN)

/*
 * Sound implementation for MS-Windows.
 */

ಸ್ಥಿರ HWND g_hWndSound = ರಿಕ್ತ;

ಸ್ಥಿರ LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ವಿಕಲ್ಪ (message)
    {
        ಪಕ್ಷ MM_MCINOTIFY:
            ಪ್ರತಿ (p = first_callback; p != ರಿಕ್ತ; p = p->snd_next)
                ಯದಿ (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    ವರ್ಣ buf[32];

                    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ರಿಕ್ತ, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ DefWindowProc(hwnd, message, wParam, lParam);
}

ಸ್ಥಿರ HWND
sound_window(ಶೂನ್ಯ)
{
    ಯದಿ (g_hWndSound == ರಿಕ್ತ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ರಿಕ್ತ, 0,
                              0, ರಿಕ್ತ,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, ರಿಕ್ತ, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   ರಿಕ್ತ, g_hinst, ರಿಕ್ತ);
    }

    ಪ್ರತ್ಯಾಗಮ g_hWndSound;
}

ಶೂನ್ಯ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), ರಿಕ್ತ);
    ಯದಿ (wp == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (PlaySoundW(wp, ರಿಕ್ತ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_ಸಂಖ್ಯೆ = ++sound_id;
    ಮುಕ್ತ(wp);
}

ಶೂನ್ಯ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    ವರ್ಣ buf[32];
    MCIERROR err;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    ಯದಿ (p == ರಿಕ್ತ)
    {
        ಪ್ರತ್ಯಾಗಮ;
    }
    vim_snprintf((ವರ್ಣ *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, ರಿಕ್ತ);
    ಮುಕ್ತ(p);
    ಯದಿ (wp == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    err = mciSendStringW(wp, ರಿಕ್ತ, 0, sound_window());
    ಮುಕ್ತ(wp);
    ಯದಿ (err != 0)
        ಪ್ರತ್ಯಾಗಮ;

    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ರಿಕ್ತ, 0, sound_window());
    ಯದಿ (err != 0)
        ಗಚ್ಛ failure;

    sound_id = newid;
    rettv->vval.v_ಸಂಖ್ಯೆ = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    ಯದಿ (soundcb != ರಿಕ್ತ)
    {
        vim_snprintf(buf, ಪರಿಮಾಣ(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    ಪ್ರತ್ಯಾಗಮ;

failure:
    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ರಿಕ್ತ, 0, ರಿಕ್ತ);
}

ಶೂನ್ಯ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    ವರ್ಣ buf[32];

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ರಿಕ್ತ, 0, ರಿಕ್ತ);
}

ಶೂನ್ಯ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(ರಿಕ್ತ, ರಿಕ್ತ, 0);
    mciSendStringA("close all", ರಿಕ್ತ, 0, ರಿಕ್ತ);
}

#  ಯದಿ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    CloseWindow(g_hWndSound);

    ಯಾವತ್ (first_callback != ರಿಕ್ತ)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ಸ್ಥಿರ ಶೂನ್ಯ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    ಯದಿ (!play_success && soundcb)
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
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;
    sound_mch_stop(tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
}

ಶೂನ್ಯ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  ಯದಿ defined(EXITFREE)
ಶೂನ್ಯ
sound_free(ಶೂನ್ಯ)
{
    sound_mch_free();
    ಯಾವತ್ (first_callback != ರಿಕ್ತ)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
