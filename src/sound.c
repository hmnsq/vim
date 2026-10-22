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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_SOUND)

স্থির long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

সংরচনা soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

স্থির soundcb_T *first_callback = রিক্ত;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
পূর্ণাঙ্ক
has_any_sound_callback(শূন্য)
{
    ঘূৰাই first_callback != রিক্ত;
}

স্থির soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    যদি (arg->v_type == VAR_UNKNOWN)
        ঘূৰাই রিক্ত;
    callback = get_callback(arg);
    যদি (callback.cb_name == রিক্ত)
        ঘূৰাই রিক্ত;

    soundcb = ALLOC_ONE(soundcb_T);
    যদি (soundcb == রিক্ত)
    {
        free_callback(&callback);
        ঘূৰাই রিক্ত;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    ঘূৰাই soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
শূন্য
call_sound_callback(soundcb_T *soundcb, long snd_id, পূর্ণাঙ্ক result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_সংখ্যা;
    argv[0].vval.v_সংখ্যা = snd_id;
    argv[1].v_type = VAR_সংখ্যা;
    argv[1].vval.v_সংখ্যা = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
শূন্য
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = রিক্ত;

    বাবে (p = first_callback; p != রিক্ত; prev = p, p = p->snd_next)
        যদি (p == soundcb)
        {
            যদি (prev == রিক্ত)
                first_callback = p->snd_next;
            অন্যথা
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            ভঙ্গ;
        }
}

# যদি defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

স্থির ca_context *context = রিক্ত;

// Structure to store info about a sound callback to be invoked soon.
প্রকারনির্দেশ সংরচনা soundcb_queue_S soundcb_queue_T;

সংরচনা soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    পূর্ণাঙ্ক scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
স্থির soundcb_queue_T *callback_queue = রিক্ত;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
স্থির শূন্য
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               পূর্ণাঙ্ক error_code,
               শূন্য *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    যদি (scb == রিক্ত)
        ঘূৰাই;
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
পূর্ণাঙ্ক
has_sound_callback_in_queue(শূন্য)
{
    ঘূৰাই callback_queue != রিক্ত;
}

/*
 * Invoke queued sound callbacks.
 */
শূন্য
invoke_sound_callback(শূন্য)
{
    soundcb_queue_T *scb;

    যেতিয়ালৈ (callback_queue != রিক্ত)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

স্থির শূন্য
sound_play_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক playfile)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    যদি (context == রিক্ত)
        ca_context_create(&context);
    যদি (context == রিক্ত)
        ঘূৰাই;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    পূর্ণাঙ্ক res = CA_ERROR_INVALID;

    ++sound_id;
    যদি (soundcb == রিক্ত)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", রিক্ত);
    }
    অন্যথা
    {
        স্থির ca_proplist *proplist = রিক্ত;

        ca_proplist_create(&proplist);
        যদি (proplist != রিক্ত)
        {
            যদি (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (ৱর্ণ *)tv_get_string(&argvars[0]));
            অন্যথা
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (ৱর্ণ *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            যদি (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_সংখ্যা = res == CA_SUCCESS ? sound_id : 0;
}

শূন্য
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
শূন্য
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
শূন্য
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    যদি (context != রিক্ত)
        ca_context_cancel(context, tv_get_সংখ্যা(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
শূন্য
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (context == রিক্ত)
        ঘূৰাই;
    ca_context_destroy(context);
    context = রিক্ত;
}

#  যদি defined(EXITFREE)
শূন্য
sound_free(শূন্য)
{
    soundcb_queue_T *scb;

    যদি (context != রিক্ত)
        ca_context_destroy(context);

    যেতিয়ালৈ (first_callback != রিক্ত)
        delete_sound_callback(first_callback);

    যেতিয়ালৈ (callback_queue != রিক্ত)
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

স্থির HWND g_hWndSound = রিক্ত;

স্থির LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ৱিকল্প (message)
    {
        পক্ষ MM_MCINOTIFY:
            বাবে (p = first_callback; p != রিক্ত; p = p->snd_next)
                যদি (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    ৱর্ণ buf[32];

                    vim_snprintf(buf, পরিমাণ(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, রিক্ত, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ভঙ্গ;
    }

    ঘূৰাই DefWindowProc(hwnd, message, wParam, lParam);
}

স্থির HWND
sound_window(শূন্য)
{
    যদি (g_hWndSound == রিক্ত)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, রিক্ত, 0,
                              0, রিক্ত,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, রিক্ত, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   রিক্ত, g_hinst, রিক্ত);
    }

    ঘূৰাই g_hWndSound;
}

শূন্য
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), রিক্ত);
    যদি (wp == রিক্ত)
        ঘূৰাই;

    যদি (PlaySoundW(wp, রিক্ত, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_সংখ্যা = ++sound_id;
    মুক্ত(wp);
}

শূন্য
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    ৱর্ণ buf[32];
    MCIERROR err;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    যদি (p == রিক্ত)
    {
        ঘূৰাই;
    }
    vim_snprintf((ৱর্ণ *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, রিক্ত);
    মুক্ত(p);
    যদি (wp == রিক্ত)
        ঘূৰাই;

    err = mciSendStringW(wp, রিক্ত, 0, sound_window());
    মুক্ত(wp);
    যদি (err != 0)
        ঘূৰাই;

    vim_snprintf(buf, পরিমাণ(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, রিক্ত, 0, sound_window());
    যদি (err != 0)
        গচ্ছ failure;

    sound_id = newid;
    rettv->vval.v_সংখ্যা = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    যদি (soundcb != রিক্ত)
    {
        vim_snprintf(buf, পরিমাণ(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    ঘূৰাই;

failure:
    vim_snprintf(buf, পরিমাণ(buf), "close sound%06ld", newid);
    mciSendStringA(buf, রিক্ত, 0, রিক্ত);
}

শূন্য
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    ৱর্ণ buf[32];

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    id = tv_get_সংখ্যা(&argvars[0]);
    vim_snprintf(buf, পরিমাণ(buf), "stop sound%06ld", id);
    mciSendStringA(buf, রিক্ত, 0, রিক্ত);
}

শূন্য
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(রিক্ত, রিক্ত, 0);
    mciSendStringA("close all", রিক্ত, 0, রিক্ত);
}

#  যদি defined(EXITFREE)
শূন্য
sound_free(শূন্য)
{
    CloseWindow(g_hWndSound);

    যেতিয়ালৈ (first_callback != রিক্ত)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
স্থির শূন্য
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    যদি (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_সংখ্যা = play_success ? sound_id : 0;
}

শূন্য
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

শূন্য
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

শূন্য
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
        ঘূৰাই;
    sound_mch_stop(tv_get_সংখ্যা(&argvars[0]));
}

শূন্য
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  যদি defined(EXITFREE)
শূন্য
sound_free(শূন্য)
{
    sound_mch_free();
    যেতিয়ালৈ (first_callback != রিক্ত)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
