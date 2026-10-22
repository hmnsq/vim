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

#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_SOUND)

ساكن long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

بنية soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

ساكن soundcb_T *first_callback = عدم;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
صحيح
has_any_sound_callback(فراغ)
{
    أرجع first_callback != عدم;
}

ساكن soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    𐡄𐡍 (arg->v_type == VAR_UNKNOWN)
        أرجع عدم;
    callback = get_callback(arg);
    𐡄𐡍 (callback.cb_name == عدم)
        أرجع عدم;

    soundcb = ALLOC_ONE(soundcb_T);
    𐡄𐡍 (soundcb == عدم)
    {
        free_callback(&callback);
        أرجع عدم;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    أرجع soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
فراغ
call_sound_callback(soundcb_T *soundcb, long snd_id, صحيح result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = snd_id;
    argv[1].v_type = VAR_NUMBER;
    argv[1].vval.v_number = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
فراغ
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = عدم;

    لكل (p = first_callback; p != عدم; prev = p, p = p->snd_next)
        𐡄𐡍 (p == soundcb)
        {
            𐡄𐡍 (prev == عدم)
                first_callback = p->snd_next;
            وإلا
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            اكسر;
        }
}

# 𐡄𐡍 defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ساكن ca_context *context = عدم;

// Structure to store info about a sound callback to be invoked soon.
تعريفنوع بنية soundcb_queue_S soundcb_queue_T;

بنية soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    صحيح scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ساكن soundcb_queue_T *callback_queue = عدم;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ساكن فراغ
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               صحيح error_code,
               فراغ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    𐡄𐡍 (scb == عدم)
        أرجع;
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
صحيح
has_sound_callback_in_queue(فراغ)
{
    أرجع callback_queue != عدم;
}

/*
 * Invoke queued sound callbacks.
 */
فراغ
invoke_sound_callback(فراغ)
{
    soundcb_queue_T *scb;

    طالما (callback_queue != عدم)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ساكن فراغ
sound_play_common(typval_T *argvars, typval_T *rettv, صحيح playfile)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    𐡄𐡍 (context == عدم)
        ca_context_create(&context);
    𐡄𐡍 (context == عدم)
        أرجع;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    صحيح res = CA_ERROR_INVALID;

    ++sound_id;
    𐡄𐡍 (soundcb == عدم)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", عدم);
    }
    وإلا
    {
        ساكن ca_proplist *proplist = عدم;

        ca_proplist_create(&proplist);
        𐡄𐡍 (proplist != عدم)
        {
            𐡄𐡍 (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (حرف *)tv_get_string(&argvars[0]));
            وإلا
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (حرف *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            𐡄𐡍 (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_number = res == CA_SUCCESS ? sound_id : 0;
}

فراغ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
فراغ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
فراغ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        أرجع;

    𐡄𐡍 (context != عدم)
        ca_context_cancel(context, tv_get_number(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
فراغ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    𐡄𐡍 (context == عدم)
        أرجع;
    ca_context_destroy(context);
    context = عدم;
}

#  𐡄𐡍 defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    soundcb_queue_T *scb;

    𐡄𐡍 (context != عدم)
        ca_context_destroy(context);

    طالما (first_callback != عدم)
        delete_sound_callback(first_callback);

    طالما (callback_queue != عدم)
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

ساكن HWND g_hWndSound = عدم;

ساكن LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    بدل (message)
    {
        حالة MM_MCINOTIFY:
            لكل (p = first_callback; p != عدم; p = p->snd_next)
                𐡄𐡍 (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    حرف buf[32];

                    vim_snprintf(buf, حجم(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, عدم, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            اكسر;
    }

    أرجع DefWindowProc(hwnd, message, wParam, lParam);
}

ساكن HWND
sound_window(فراغ)
{
    𐡄𐡍 (g_hWndSound == عدم)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, عدم, 0,
                              0, عدم,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, عدم, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   عدم, g_hinst, عدم);
    }

    أرجع g_hWndSound;
}

فراغ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), عدم);
    𐡄𐡍 (wp == عدم)
        أرجع;

    𐡄𐡍 (PlaySoundW(wp, عدم, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_number = ++sound_id;
    حرر(wp);
}

فراغ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    حرف buf[32];
    MCIERROR err;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    𐡄𐡍 (p == عدم)
    {
        أرجع;
    }
    vim_snprintf((حرف *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, عدم);
    حرر(p);
    𐡄𐡍 (wp == عدم)
        أرجع;

    err = mciSendStringW(wp, عدم, 0, sound_window());
    حرر(wp);
    𐡄𐡍 (err != 0)
        أرجع;

    vim_snprintf(buf, حجم(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, عدم, 0, sound_window());
    𐡄𐡍 (err != 0)
        اذهب failure;

    sound_id = newid;
    rettv->vval.v_number = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    𐡄𐡍 (soundcb != عدم)
    {
        vim_snprintf(buf, حجم(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    أرجع;

failure:
    vim_snprintf(buf, حجم(buf), "close sound%06ld", newid);
    mciSendStringA(buf, عدم, 0, عدم);
}

فراغ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    حرف buf[32];

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        أرجع;

    id = tv_get_number(&argvars[0]);
    vim_snprintf(buf, حجم(buf), "stop sound%06ld", id);
    mciSendStringA(buf, عدم, 0, عدم);
}

فراغ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(عدم, عدم, 0);
    mciSendStringA("close all", عدم, 0, عدم);
}

#  𐡄𐡍 defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    CloseWindow(g_hWndSound);

    طالما (first_callback != عدم)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ساكن فراغ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    𐡄𐡍 (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_number = play_success ? sound_id : 0;
}

فراغ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

فراغ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

فراغ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        أرجع;
    sound_mch_stop(tv_get_number(&argvars[0]));
}

فراغ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  𐡄𐡍 defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    sound_mch_free();
    طالما (first_callback != عدم)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
