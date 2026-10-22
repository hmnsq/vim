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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SOUND)

நிலையான long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

கட்டமைப்பு soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

நிலையான soundcb_T *first_callback = வெற்று;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
முழு
has_any_sound_callback(வெறுமை)
{
    திரும்பு first_callback != வெற்று;
}

நிலையான soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    எனில் (arg->v_type == VAR_UNKNOWN)
        திரும்பு வெற்று;
    callback = get_callback(arg);
    எனில் (callback.cb_name == வெற்று)
        திரும்பு வெற்று;

    soundcb = ALLOC_ONE(soundcb_T);
    எனில் (soundcb == வெற்று)
    {
        free_callback(&callback);
        திரும்பு வெற்று;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    திரும்பு soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
வெறுமை
call_sound_callback(soundcb_T *soundcb, long snd_id, முழு result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_எண்;
    argv[0].vval.v_எண் = snd_id;
    argv[1].v_type = VAR_எண்;
    argv[1].vval.v_எண் = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
வெறுமை
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = வெற்று;

    க்கு (p = first_callback; p != வெற்று; prev = p, p = p->snd_next)
        எனில் (p == soundcb)
        {
            எனில் (prev == வெற்று)
                first_callback = p->snd_next;
            இல்லையெனில்
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            நிறுத்து;
        }
}

# எனில் defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

நிலையான ca_context *context = வெற்று;

// Structure to store info about a sound callback to be invoked soon.
வகைவரை கட்டமைப்பு soundcb_queue_S soundcb_queue_T;

கட்டமைப்பு soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    முழு scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
நிலையான soundcb_queue_T *callback_queue = வெற்று;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
நிலையான வெறுமை
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               முழு error_code,
               வெறுமை *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    எனில் (scb == வெற்று)
        திரும்பு;
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
முழு
has_sound_callback_in_queue(வெறுமை)
{
    திரும்பு callback_queue != வெற்று;
}

/*
 * Invoke queued sound callbacks.
 */
வெறுமை
invoke_sound_callback(வெறுமை)
{
    soundcb_queue_T *scb;

    போது (callback_queue != வெற்று)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

நிலையான வெறுமை
sound_play_common(typval_T *argvars, typval_T *rettv, முழு playfile)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        திரும்பு;

    எனில் (context == வெற்று)
        ca_context_create(&context);
    எனில் (context == வெற்று)
        திரும்பு;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    முழு res = CA_ERROR_INVALID;

    ++sound_id;
    எனில் (soundcb == வெற்று)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", வெற்று);
    }
    இல்லையெனில்
    {
        நிலையான ca_proplist *proplist = வெற்று;

        ca_proplist_create(&proplist);
        எனில் (proplist != வெற்று)
        {
            எனில் (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (எழுத்து *)tv_get_string(&argvars[0]));
            இல்லையெனில்
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (எழுத்து *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            எனில் (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_எண் = res == CA_SUCCESS ? sound_id : 0;
}

வெறுமை
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
வெறுமை
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
வெறுமை
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
        திரும்பு;

    எனில் (context != வெற்று)
        ca_context_cancel(context, tv_get_எண்(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
வெறுமை
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (context == வெற்று)
        திரும்பு;
    ca_context_destroy(context);
    context = வெற்று;
}

#  எனில் defined(EXITFREE)
வெறுமை
sound_free(வெறுமை)
{
    soundcb_queue_T *scb;

    எனில் (context != வெற்று)
        ca_context_destroy(context);

    போது (first_callback != வெற்று)
        delete_sound_callback(first_callback);

    போது (callback_queue != வெற்று)
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

நிலையான HWND g_hWndSound = வெற்று;

நிலையான LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    தேர்வு (message)
    {
        நிலை MM_MCINOTIFY:
            க்கு (p = first_callback; p != வெற்று; p = p->snd_next)
                எனில் (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    எழுத்து buf[32];

                    vim_snprintf(buf, அளவு(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, வெற்று, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            நிறுத்து;
    }

    திரும்பு DefWindowProc(hwnd, message, wParam, lParam);
}

நிலையான HWND
sound_window(வெறுமை)
{
    எனில் (g_hWndSound == வெற்று)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, வெற்று, 0,
                              0, வெற்று,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, வெற்று, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   வெற்று, g_hinst, வெற்று);
    }

    திரும்பு g_hWndSound;
}

வெறுமை
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        திரும்பு;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), வெற்று);
    எனில் (wp == வெற்று)
        திரும்பு;

    எனில் (PlaySoundW(wp, வெற்று, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_எண் = ++sound_id;
    விடுவி(wp);
}

வெறுமை
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    எழுத்து buf[32];
    MCIERROR err;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        திரும்பு;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    எனில் (p == வெற்று)
    {
        திரும்பு;
    }
    vim_snprintf((எழுத்து *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, வெற்று);
    விடுவி(p);
    எனில் (wp == வெற்று)
        திரும்பு;

    err = mciSendStringW(wp, வெற்று, 0, sound_window());
    விடுவி(wp);
    எனில் (err != 0)
        திரும்பு;

    vim_snprintf(buf, அளவு(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, வெற்று, 0, sound_window());
    எனில் (err != 0)
        செல் failure;

    sound_id = newid;
    rettv->vval.v_எண் = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    எனில் (soundcb != வெற்று)
    {
        vim_snprintf(buf, அளவு(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    திரும்பு;

failure:
    vim_snprintf(buf, அளவு(buf), "close sound%06ld", newid);
    mciSendStringA(buf, வெற்று, 0, வெற்று);
}

வெறுமை
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    எழுத்து buf[32];

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
        திரும்பு;

    id = tv_get_எண்(&argvars[0]);
    vim_snprintf(buf, அளவு(buf), "stop sound%06ld", id);
    mciSendStringA(buf, வெற்று, 0, வெற்று);
}

வெறுமை
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(வெற்று, வெற்று, 0);
    mciSendStringA("close all", வெற்று, 0, வெற்று);
}

#  எனில் defined(EXITFREE)
வெறுமை
sound_free(வெறுமை)
{
    CloseWindow(g_hWndSound);

    போது (first_callback != வெற்று)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
நிலையான வெறுமை
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        திரும்பு;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    எனில் (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_எண் = play_success ? sound_id : 0;
}

வெறுமை
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

வெறுமை
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

வெறுமை
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
        திரும்பு;
    sound_mch_stop(tv_get_எண்(&argvars[0]));
}

வெறுமை
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  எனில் defined(EXITFREE)
வெறுமை
sound_free(வெறுமை)
{
    sound_mch_free();
    போது (first_callback != வெற்று)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
