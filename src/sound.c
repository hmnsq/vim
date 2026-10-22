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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SOUND)

ਸਥਿਰ ਲੰਮਾ sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ਬਣਤਰ soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    ਲੰਮਾ snd_id;
# endif
    soundcb_T *snd_ਅਗਲਾ;
};

ਸਥਿਰ soundcb_T *ਪਹਿਲਾ_callback = ਨਿਰਾ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
ਪੂਰਨ
has_any_sound_callback(ਖਾਲੀ)
{
    ਵਾਪਸ ਪਹਿਲਾ_callback != ਨਿਰਾ;
}

ਸਥਿਰ soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    ਜੇ (arg->v_type == VAR_UNKNOWN)
        ਵਾਪਸ ਨਿਰਾ;
    callback = get_callback(arg);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
        ਵਾਪਸ ਨਿਰਾ;

    soundcb = ALLOC_ONE(soundcb_T);
    ਜੇ (soundcb == ਨਿਰਾ)
    {
        free_callback(&callback);
        ਵਾਪਸ ਨਿਰਾ;
    }

    soundcb->snd_ਅਗਲਾ = ਪਹਿਲਾ_callback;
    ਪਹਿਲਾ_callback = soundcb;
    ਸੈੱਟ_callback(&soundcb->snd_callback, &callback);
    ਵਾਪਸ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
ਖਾਲੀ
call_sound_callback(soundcb_T *soundcb, ਲੰਮਾ snd_id, ਪੂਰਨ ਨਤੀਜਾ)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = snd_id;
    argv[1].v_type = VAR_ਸੰਖਿਆ;
    argv[1].vval.v_ਸੰਖਿਆ = ਨਤੀਜਾ;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
ਖਾਲੀ
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ਨਿਰਾ;

    ਲਈ (p = ਪਹਿਲਾ_callback; p != ਨਿਰਾ; prev = p, p = p->snd_ਅਗਲਾ)
        ਜੇ (p == soundcb)
        {
            ਜੇ (prev == ਨਿਰਾ)
                ਪਹਿਲਾ_callback = p->snd_ਅਗਲਾ;
            ਨਹੀਂਤਾਂ
                prev->snd_ਅਗਲਾ = p->snd_ਅਗਲਾ;
            free_callback(&p->snd_callback);
            vim_free(p);
            ਰੋਕੋ;
        }
}

# ਜੇ defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ਸਥਿਰ ca_context *context = ਨਿਰਾ;

// Structure to store info about a sound callback to be invoked soon.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ soundcb_ਕਤਾਰ_S soundcb_ਕਤਾਰ_T;

ਬਣਤਰ soundcb_ਕਤਾਰ_S
{
    soundcb_ਕਤਾਰ_T *scb_ਅਗਲਾ;
    uint32_t scb_id; // ID of the sound
    ਪੂਰਨ scb_ਨਤੀਜਾ; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ਸਥਿਰ soundcb_ਕਤਾਰ_T *callback_ਕਤਾਰ = ਨਿਰਾ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ਸਥਿਰ ਖਾਲੀ
sound_callback(ca_context *c ਅਣਵਰਤਿਆ,
               uint32_t id,
               ਪੂਰਨ error_ਕੋਡ,
               ਖਾਲੀ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_ਕਤਾਰ_T *scb;

    scb = ALLOC_ONE(soundcb_ਕਤਾਰ_T);
    ਜੇ (scb == ਨਿਰਾ)
        ਵਾਪਸ;
    scb->scb_ਅਗਲਾ = callback_ਕਤਾਰ;
    callback_ਕਤਾਰ = scb;
    scb->scb_id = id;
    scb->scb_ਨਤੀਜਾ = error_ਕੋਡ == CA_SUCCESS ? 0
                      : error_ਕੋਡ == CA_ERROR_CANCELED ||
                              error_ਕੋਡ == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
ਪੂਰਨ
has_sound_callback_ਵਿੱਚ_ਕਤਾਰ(ਖਾਲੀ)
{
    ਵਾਪਸ callback_ਕਤਾਰ != ਨਿਰਾ;
}

/*
 * Invoke queued sound callbacks.
 */
ਖਾਲੀ
invoke_sound_callback(ਖਾਲੀ)
{
    soundcb_ਕਤਾਰ_T *scb;

    ਜਦੋਂ (callback_ਕਤਾਰ != ਨਿਰਾ)
    {
        scb = callback_ਕਤਾਰ;
        callback_ਕਤਾਰ = scb->scb_ਅਗਲਾ;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_ਨਤੀਜਾ);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ਸਥਿਰ ਖਾਲੀ
sound_play_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ playfile)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    ਜੇ (context == ਨਿਰਾ)
        ca_context_create(&context);
    ਜੇ (context == ਨਿਰਾ)
        ਵਾਪਸ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    ਪੂਰਨ res = CA_ERROR_INVALID;

    ++sound_id;
    ਜੇ (soundcb == ਨਿਰਾ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
        ਸਥਿਰ ca_proplist *proplist = ਨਿਰਾ;

        ca_proplist_create(&proplist);
        ਜੇ (proplist != ਨਿਰਾ)
        {
            ਜੇ (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (ਅੱਖਰ *)tv_get_string(&argvars[0]));
            ਨਹੀਂਤਾਂ
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (ਅੱਖਰ *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_ਭਰਿਆ(context, sound_id, proplist,
                                       sound_callback, soundcb);
            ਜੇ (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_ਸੰਖਿਆ = res == CA_SUCCESS ? sound_id : 0;
}

ਖਾਲੀ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
ਖਾਲੀ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
ਖਾਲੀ
f_sound_stop(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    ਜੇ (context != ਨਿਰਾ)
        ca_context_cancel(context, tv_get_ਸੰਖਿਆ(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
ਖਾਲੀ
f_sound_clear(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (context == ਨਿਰਾ)
        ਵਾਪਸ;
    ca_context_destroy(context);
    context = ਨਿਰਾ;
}

#  ਜੇ defined(EXITFREE)
ਖਾਲੀ
sound_free(ਖਾਲੀ)
{
    soundcb_ਕਤਾਰ_T *scb;

    ਜੇ (context != ਨਿਰਾ)
        ca_context_destroy(context);

    ਜਦੋਂ (ਪਹਿਲਾ_callback != ਨਿਰਾ)
        delete_sound_callback(ਪਹਿਲਾ_callback);

    ਜਦੋਂ (callback_ਕਤਾਰ != ਨਿਰਾ)
    {
        scb = callback_ਕਤਾਰ;
        callback_ਕਤਾਰ = scb->scb_ਅਗਲਾ;
        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
}
#  endif

# elif defined(MSWIN)

/*
 * Sound implementation for MS-Windows.
 */

ਸਥਿਰ HWND g_hWndSound = ਨਿਰਾ;

ਸਥਿਰ LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT ਸੁਨੇਹਾ_2, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ਚੋਣ (ਸੁਨੇਹਾ_2)
    {
        ਹਾਲਤ MM_MCINOTIFY:
            ਲਈ (p = ਪਹਿਲਾ_callback; p != ਨਿਰਾ; p = p->snd_ਅਗਲਾ)
                ਜੇ (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    ਅੱਖਰ buf[32];

                    vim_snprintf(buf, ਆਕਾਰ(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ਨਿਰਾ, 0, 0);

                    ਲੰਮਾ ਨਤੀਜਾ = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, ਨਤੀਜਾ);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ਰੋਕੋ;
    }

    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc(hwnd, ਸੁਨੇਹਾ_2, wParam, lParam);
}

ਸਥਿਰ HWND
sound_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਜੇ (g_hWndSound == ਨਿਰਾ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ਨਿਰਾ, 0,
                              0, ਨਿਰਾ,          clazz };
        ਰਜਿਸਟਰ_Class(&wndclass);
        g_hWndSound = Create_ਖਿੜਕੀ(clazz, ਨਿਰਾ, 0, 0, 0, 0, 0, HWND_ਸੁਨੇਹਾ,
                                   ਨਿਰਾ, g_hinst, ਨਿਰਾ);
    }

    ਵਾਪਸ g_hWndSound;
}

ਖਾਲੀ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    wp = enc_ਮੰਜ਼ਿਲ_utf16(tv_get_string(&argvars[0]), ਨਿਰਾ);
    ਜੇ (wp == ਨਿਰਾ)
        ਵਾਪਸ;

    ਜੇ (PlaySoundW(wp, ਨਿਰਾ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_ਸੰਖਿਆ = ++sound_id;
    ਮੁਕਤ(wp);
}

ਖਾਲੀ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    ਲੰਮਾ newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    ਅੱਖਰ buf[32];
    MCIERROR err;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    ਜੇ (p == ਨਿਰਾ)
    {
        ਵਾਪਸ;
    }
    vim_snprintf((ਅੱਖਰ *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)p, ਨਿਰਾ);
    ਮੁਕਤ(p);
    ਜੇ (wp == ਨਿਰਾ)
        ਵਾਪਸ;

    err = mciSendStringW(wp, ਨਿਰਾ, 0, sound_ਖਿੜਕੀ());
    ਮੁਕਤ(wp);
    ਜੇ (err != 0)
        ਵਾਪਸ;

    vim_snprintf(buf, ਆਕਾਰ(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ਨਿਰਾ, 0, sound_ਖਿੜਕੀ());
    ਜੇ (err != 0)
        ਜਾਓ failure;

    sound_id = newid;
    rettv->vval.v_ਸੰਖਿਆ = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    ਜੇ (soundcb != ਨਿਰਾ)
    {
        vim_snprintf(buf, ਆਕਾਰ(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    ਵਾਪਸ;

failure:
    vim_snprintf(buf, ਆਕਾਰ(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ਨਿਰਾ, 0, ਨਿਰਾ);
}

ਖਾਲੀ
f_sound_stop(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਲੰਮਾ id;
    ਅੱਖਰ buf[32];

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    id = tv_get_ਸੰਖਿਆ(&argvars[0]);
    vim_snprintf(buf, ਆਕਾਰ(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ਨਿਰਾ, 0, ਨਿਰਾ);
}

ਖਾਲੀ
f_sound_clear(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    PlaySoundW(ਨਿਰਾ, ਨਿਰਾ, 0);
    mciSendStringA("close all", ਨਿਰਾ, 0, ਨਿਰਾ);
}

#  ਜੇ defined(EXITFREE)
ਖਾਲੀ
sound_free(ਖਾਲੀ)
{
    Close_ਖਿੜਕੀ(g_hWndSound);

    ਜਦੋਂ (ਪਹਿਲਾ_callback != ਨਿਰਾ)
        delete_sound_callback(ਪਹਿਲਾ_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ਸਥਿਰ ਖਾਲੀ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    ਜੇ (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_ਸੰਖਿਆ = play_success ? sound_id : 0;
}

ਖਾਲੀ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

ਖਾਲੀ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

ਖਾਲੀ
f_sound_stop(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;
    sound_mch_stop(tv_get_ਸੰਖਿਆ(&argvars[0]));
}

ਖਾਲੀ
f_sound_clear(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    sound_mch_clear();
}

#  ਜੇ defined(EXITFREE)
ਖਾਲੀ
sound_free(ਖਾਲੀ)
{
    sound_mch_free();
    ਜਦੋਂ (ਪਹਿਲਾ_callback != ਨਿਰਾ)
        delete_sound_callback(ਪਹਿਲਾ_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#ਜੇ_ਅੰਤ // FEAT_SOUND
