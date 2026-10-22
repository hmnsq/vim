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

#شامل "vim.h"

#اگر defined(FEAT_SOUND)

ایستا long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ساختار soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

ایستا soundcb_T *first_callback = پوچ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
صحیح
has_any_sound_callback(تهی)
{
    بازگشت first_callback != پوچ;
}

ایستا soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    اگر (arg->v_type == VAR_UNKNOWN)
        بازگشت پوچ;
    callback = get_callback(arg);
    اگر (callback.cb_name == پوچ)
        بازگشت پوچ;

    soundcb = ALLOC_ONE(soundcb_T);
    اگر (soundcb == پوچ)
    {
        free_callback(&callback);
        بازگشت پوچ;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    بازگشت soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
تهی
call_sound_callback(soundcb_T *soundcb, long snd_id, صحیح result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = snd_id;
    argv[1].v_type = VAR_عدد;
    argv[1].vval.v_عدد = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
تهی
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = پوچ;

    برای (p = first_callback; p != پوچ; prev = p, p = p->snd_next)
        اگر (p == soundcb)
        {
            اگر (prev == پوچ)
                first_callback = p->snd_next;
            وگرنه
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            بشکن;
        }
}

# اگر defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ایستا ca_context *context = پوچ;

// Structure to store info about a sound callback to be invoked soon.
تعریفنوع ساختار soundcb_queue_S soundcb_queue_T;

ساختار soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    صحیح scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ایستا soundcb_queue_T *callback_queue = پوچ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ایستا تهی
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               صحیح error_code,
               تهی *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    اگر (scb == پوچ)
        بازگشت;
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
صحیح
has_sound_callback_in_queue(تهی)
{
    بازگشت callback_queue != پوچ;
}

/*
 * Invoke queued sound callbacks.
 */
تهی
invoke_sound_callback(تهی)
{
    soundcb_queue_T *scb;

    وقتی (callback_queue != پوچ)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ایستا تهی
sound_play_common(typval_T *argvars, typval_T *rettv, صحیح playfile)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        بازگشت;

    اگر (context == پوچ)
        ca_context_create(&context);
    اگر (context == پوچ)
        بازگشت;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    صحیح res = CA_ERROR_INVALID;

    ++sound_id;
    اگر (soundcb == پوچ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", پوچ);
    }
    وگرنه
    {
        ایستا ca_proplist *proplist = پوچ;

        ca_proplist_create(&proplist);
        اگر (proplist != پوچ)
        {
            اگر (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (نویسه *)tv_get_string(&argvars[0]));
            وگرنه
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (نویسه *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            اگر (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_عدد = res == CA_SUCCESS ? sound_id : 0;
}

تهی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
تهی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
تهی
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        بازگشت;

    اگر (context != پوچ)
        ca_context_cancel(context, tv_get_عدد(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
تهی
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    اگر (context == پوچ)
        بازگشت;
    ca_context_destroy(context);
    context = پوچ;
}

#  اگر defined(EXITFREE)
تهی
sound_free(تهی)
{
    soundcb_queue_T *scb;

    اگر (context != پوچ)
        ca_context_destroy(context);

    وقتی (first_callback != پوچ)
        delete_sound_callback(first_callback);

    وقتی (callback_queue != پوچ)
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

ایستا HWND g_hWndSound = پوچ;

ایستا LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    انتخاب (message)
    {
        حالت MM_MCINOTIFY:
            برای (p = first_callback; p != پوچ; p = p->snd_next)
                اگر (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    نویسه buf[32];

                    vim_snprintf(buf, اندازه(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, پوچ, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            بشکن;
    }

    بازگشت DefWindowProc(hwnd, message, wParam, lParam);
}

ایستا HWND
sound_window(تهی)
{
    اگر (g_hWndSound == پوچ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, پوچ, 0,
                              0, پوچ,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, پوچ, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   پوچ, g_hinst, پوچ);
    }

    بازگشت g_hWndSound;
}

تهی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        بازگشت;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), پوچ);
    اگر (wp == پوچ)
        بازگشت;

    اگر (PlaySoundW(wp, پوچ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_عدد = ++sound_id;
    آزاد(wp);
}

تهی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    نویسه buf[32];
    MCIERROR err;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        بازگشت;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    اگر (p == پوچ)
    {
        بازگشت;
    }
    vim_snprintf((نویسه *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, پوچ);
    آزاد(p);
    اگر (wp == پوچ)
        بازگشت;

    err = mciSendStringW(wp, پوچ, 0, sound_window());
    آزاد(wp);
    اگر (err != 0)
        بازگشت;

    vim_snprintf(buf, اندازه(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, پوچ, 0, sound_window());
    اگر (err != 0)
        برو failure;

    sound_id = newid;
    rettv->vval.v_عدد = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    اگر (soundcb != پوچ)
    {
        vim_snprintf(buf, اندازه(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    بازگشت;

failure:
    vim_snprintf(buf, اندازه(buf), "close sound%06ld", newid);
    mciSendStringA(buf, پوچ, 0, پوچ);
}

تهی
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    نویسه buf[32];

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        بازگشت;

    id = tv_get_عدد(&argvars[0]);
    vim_snprintf(buf, اندازه(buf), "stop sound%06ld", id);
    mciSendStringA(buf, پوچ, 0, پوچ);
}

تهی
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(پوچ, پوچ, 0);
    mciSendStringA("close all", پوچ, 0, پوچ);
}

#  اگر defined(EXITFREE)
تهی
sound_free(تهی)
{
    CloseWindow(g_hWndSound);

    وقتی (first_callback != پوچ)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ایستا تهی
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        بازگشت;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    اگر (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_عدد = play_success ? sound_id : 0;
}

تهی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

تهی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

تهی
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        بازگشت;
    sound_mch_stop(tv_get_عدد(&argvars[0]));
}

تهی
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  اگر defined(EXITFREE)
تهی
sound_free(تهی)
{
    sound_mch_free();
    وقتی (first_callback != پوچ)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
