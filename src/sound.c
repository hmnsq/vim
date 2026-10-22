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

#إذا defined(FEAT_SOUND)

ساكن دراز sound_id = 0;

// soundcb_T is typedef'ed in vim.h

بنية soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    دراز snd_id;
# endif
    soundcb_T *snd_بعدی;
};

ساكن soundcb_T *اولی_callback = عدم;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
صحيح
has_any_sound_callback(فراغ)
{
    أرجع اولی_callback != عدم;
}

ساكن soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    إذا (arg->v_type == VAR_UNKNOWN)
        أرجع عدم;
    callback = get_callback(arg);
    إذا (callback.cb_name == عدم)
        أرجع عدم;

    soundcb = ALLOC_ONE(soundcb_T);
    إذا (soundcb == عدم)
    {
        free_callback(&callback);
        أرجع عدم;
    }

    soundcb->snd_بعدی = اولی_callback;
    اولی_callback = soundcb;
    مجموعه_callback(&soundcb->snd_callback, &callback);
    أرجع soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
فراغ
call_sound_callback(soundcb_T *soundcb, دراز snd_id, صحيح نتیجه)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = snd_id;
    argv[1].v_type = VAR_عدد;
    argv[1].vval.v_عدد = نتیجه;
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

    لكل (p = اولی_callback; p != عدم; prev = p, p = p->snd_بعدی)
        إذا (p == soundcb)
        {
            إذا (prev == عدم)
                اولی_callback = p->snd_بعدی;
            وإلا
                prev->snd_بعدی = p->snd_بعدی;
            free_callback(&p->snd_callback);
            vim_free(p);
            اكسر;
        }
}

# إذا defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ساكن ca_context *context = عدم;

// Structure to store info about a sound callback to be invoked soon.
تعريفنوع بنية soundcb_صف_S soundcb_صف_T;

بنية soundcb_صف_S
{
    soundcb_صف_T *scb_بعدی;
    uint32_t scb_id; // ID of the sound
    صحيح scb_نتیجه; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ساكن soundcb_صف_T *callback_صف = عدم;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ساكن فراغ
sound_callback(ca_context *c استفادهنشده,
               uint32_t id,
               صحيح error_کد,
               فراغ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_صف_T *scb;

    scb = ALLOC_ONE(soundcb_صف_T);
    إذا (scb == عدم)
        أرجع;
    scb->scb_بعدی = callback_صف;
    callback_صف = scb;
    scb->scb_id = id;
    scb->scb_نتیجه = error_کد == CA_SUCCESS ? 0
                      : error_کد == CA_ERROR_CANCELED ||
                              error_کد == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
صحيح
has_sound_callback_درون_صف(فراغ)
{
    أرجع callback_صف != عدم;
}

/*
 * Invoke queued sound callbacks.
 */
فراغ
invoke_sound_callback(فراغ)
{
    soundcb_صف_T *scb;

    طالما (callback_صف != عدم)
    {
        scb = callback_صف;
        callback_صف = scb->scb_بعدی;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_نتیجه);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ساكن فراغ
sound_play_common(typval_T *argvars, typval_T *rettv, صحيح playfile)
{
    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    إذا (context == عدم)
        ca_context_create(&context);
    إذا (context == عدم)
        أرجع;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    صحيح res = CA_ERROR_INVALID;

    ++sound_id;
    إذا (soundcb == عدم)
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
        إذا (proplist != عدم)
        {
            إذا (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (حرف *)tv_get_string(&argvars[0]));
            وإلا
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (حرف *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_پر(context, sound_id, proplist,
                                       sound_callback, soundcb);
            إذا (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_عدد = res == CA_SUCCESS ? sound_id : 0;
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
f_sound_stop(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        أرجع;

    إذا (context != عدم)
        ca_context_cancel(context, tv_get_عدد(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
فراغ
f_sound_clear(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
    إذا (context == عدم)
        أرجع;
    ca_context_destroy(context);
    context = عدم;
}

#  إذا defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    soundcb_صف_T *scb;

    إذا (context != عدم)
        ca_context_destroy(context);

    طالما (اولی_callback != عدم)
        delete_sound_callback(اولی_callback);

    طالما (callback_صف != عدم)
    {
        scb = callback_صف;
        callback_صف = scb->scb_بعدی;
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
sound_wndproc(HWND hwnd, UINT پیام_2, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    بدل (پیام_2)
    {
        حالة MM_MCINOTIFY:
            لكل (p = اولی_callback; p != عدم; p = p->snd_بعدی)
                إذا (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    حرف buf[32];

                    vim_snprintf(buf, حجم(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, عدم, 0, 0);

                    دراز نتیجه = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, نتیجه);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            اكسر;
    }

    أرجع Def_پنجره_Proc(hwnd, پیام_2, wParam, lParam);
}

ساكن HWND
sound_پنجره(فراغ)
{
    إذا (g_hWndSound == عدم)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, عدم, 0,
                              0, عدم,          clazz };
        ثبات_Class(&wndclass);
        g_hWndSound = Create_پنجره(clazz, عدم, 0, 0, 0, 0, 0, HWND_پیام,
                                   عدم, g_hinst, عدم);
    }

    أرجع g_hWndSound;
}

فراغ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    wp = enc_مقصد_utf16(tv_get_string(&argvars[0]), عدم);
    إذا (wp == عدم)
        أرجع;

    إذا (PlaySoundW(wp, عدم, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_عدد = ++sound_id;
    حرر(wp);
}

فراغ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    دراز newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    حرف buf[32];
    MCIERROR err;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    إذا (p == عدم)
    {
        أرجع;
    }
    vim_snprintf((حرف *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_مقصد_utf16((char_u *)p, عدم);
    حرر(p);
    إذا (wp == عدم)
        أرجع;

    err = mciSendStringW(wp, عدم, 0, sound_پنجره());
    حرر(wp);
    إذا (err != 0)
        أرجع;

    vim_snprintf(buf, حجم(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, عدم, 0, sound_پنجره());
    إذا (err != 0)
        اذهب failure;

    sound_id = newid;
    rettv->vval.v_عدد = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    إذا (soundcb != عدم)
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
f_sound_stop(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    دراز id;
    حرف buf[32];

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        أرجع;

    id = tv_get_عدد(&argvars[0]);
    vim_snprintf(buf, حجم(buf), "stop sound%06ld", id);
    mciSendStringA(buf, عدم, 0, عدم);
}

فراغ
f_sound_clear(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
    PlaySoundW(عدم, عدم, 0);
    mciSendStringA("close all", عدم, 0, عدم);
}

#  إذا defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    Close_پنجره(g_hWndSound);

    طالما (اولی_callback != عدم)
        delete_sound_callback(اولی_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ساكن فراغ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        أرجع;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    إذا (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_عدد = play_success ? sound_id : 0;
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
f_sound_stop(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
        أرجع;
    sound_mch_stop(tv_get_عدد(&argvars[0]));
}

فراغ
f_sound_clear(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
    sound_mch_clear();
}

#  إذا defined(EXITFREE)
فراغ
sound_free(فراغ)
{
    sound_mch_free();
    طالما (اولی_callback != عدم)
        delete_sound_callback(اولی_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#پایان_اگر // FEAT_SOUND
