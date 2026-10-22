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

ساکن طویل sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ساخت soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    طویل snd_id;
# endif
    soundcb_T *snd_اگلا;
};

ساکن soundcb_T *پہلا_callback = تہی;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
صحیح
has_any_sound_callback(خالی)
{
    واپس پہلا_callback != تہی;
}

ساکن soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    اگر (arg->v_type == VAR_UNKNOWN)
        واپس تہی;
    callback = get_callback(arg);
    اگر (callback.cb_name == تہی)
        واپس تہی;

    soundcb = ALLOC_ONE(soundcb_T);
    اگر (soundcb == تہی)
    {
        free_callback(&callback);
        واپس تہی;
    }

    soundcb->snd_اگلا = پہلا_callback;
    پہلا_callback = soundcb;
    سیٹ_callback(&soundcb->snd_callback, &callback);
    واپس soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
خالی
call_sound_callback(soundcb_T *soundcb, طویل snd_id, صحیح نتیجہ)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = snd_id;
    argv[1].v_type = VAR_نمبر;
    argv[1].vval.v_نمبر = نتیجہ;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
خالی
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *پچھلا = تہی;

    برائے (p = پہلا_callback; p != تہی; پچھلا = p, p = p->snd_اگلا)
        اگر (p == soundcb)
        {
            اگر (پچھلا == تہی)
                پہلا_callback = p->snd_اگلا;
            ورنہ
                پچھلا->snd_اگلا = p->snd_اگلا;
            free_callback(&p->snd_callback);
            vim_free(p);
            توڑو;
        }
}

# اگر defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ساکن ca_context *context = تہی;

// Structure to store info about a sound callback to be invoked soon.
قسمتعریف ساخت soundcb_قطار_S soundcb_قطار_T;

ساخت soundcb_قطار_S
{
    soundcb_قطار_T *scb_اگلا;
    uint32_t scb_id; // ID of the sound
    صحیح scb_نتیجہ; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ساکن soundcb_قطار_T *callback_قطار = تہی;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ساکن خالی
sound_callback(ca_context *c غیر_مستعمل,
               uint32_t id,
               صحیح error_رمز,
               خالی *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_قطار_T *scb;

    scb = ALLOC_ONE(soundcb_قطار_T);
    اگر (scb == تہی)
        واپس;
    scb->scb_اگلا = callback_قطار;
    callback_قطار = scb;
    scb->scb_id = id;
    scb->scb_نتیجہ = error_رمز == CA_SUCCESS ? 0
                      : error_رمز == CA_ERROR_CANCELED ||
                              error_رمز == CA_ERROR_DESTROYED
                          ? 1
                          : 2;
    scb->scb_callback = soundcb;
}

/*
 * Return TRUE if there is a sound callback to be called.
 */
صحیح
has_sound_callback_اندر_قطار(خالی)
{
    واپس callback_قطار != تہی;
}

/*
 * Invoke queued sound callbacks.
 */
خالی
invoke_sound_callback(خالی)
{
    soundcb_قطار_T *scb;

    جب (callback_قطار != تہی)
    {
        scb = callback_قطار;
        callback_قطار = scb->scb_اگلا;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_نتیجہ);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ساکن خالی
sound_play_common(typval_T *argvars, typval_T *rettv, صحیح playfile)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        واپس;

    اگر (context == تہی)
        ca_context_create(&context);
    اگر (context == تہی)
        واپس;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    صحیح res = CA_ERROR_INVALID;

    ++sound_id;
    اگر (soundcb == تہی)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", تہی);
    }
    ورنہ
    {
        ساکن ca_proplist *proplist = تہی;

        ca_proplist_create(&proplist);
        اگر (proplist != تہی)
        {
            اگر (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (حرف *)tv_get_string(&argvars[0]));
            ورنہ
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (حرف *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_بھرا(context, sound_id, proplist,
                                       sound_callback, soundcb);
            اگر (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_نمبر = res == CA_SUCCESS ? sound_id : 0;
}

خالی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
خالی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
خالی
f_sound_stop(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
        واپس;

    اگر (context != تہی)
        ca_context_cancel(context, tv_get_نمبر(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
خالی
f_sound_clear(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (context == تہی)
        واپس;
    ca_context_destroy(context);
    context = تہی;
}

#  اگر defined(EXITFREE)
خالی
sound_free(خالی)
{
    soundcb_قطار_T *scb;

    اگر (context != تہی)
        ca_context_destroy(context);

    جب (پہلا_callback != تہی)
        delete_sound_callback(پہلا_callback);

    جب (callback_قطار != تہی)
    {
        scb = callback_قطار;
        callback_قطار = scb->scb_اگلا;
        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
}
#  endif

# elif defined(MSWIN)

/*
 * Sound implementation for MS-Windows.
 */

ساکن HWND g_hWndSound = تہی;

ساکن LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT پیغام_2, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    انتخاب (پیغام_2)
    {
        حالت MM_MCINOTIFY:
            برائے (p = پہلا_callback; p != تہی; p = p->snd_اگلا)
                اگر (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    حرف buf[32];

                    vim_snprintf(buf, حجم(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, تہی, 0, 0);

                    طویل نتیجہ = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, نتیجہ);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            توڑو;
    }

    واپس Def_کھڑکی_Proc(hwnd, پیغام_2, wParam, lParam);
}

ساکن HWND
sound_کھڑکی(خالی)
{
    اگر (g_hWndSound == تہی)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, تہی, 0,
                              0, تہی,          clazz };
        رجسٹر_Class(&wndclass);
        g_hWndSound = Create_کھڑکی(clazz, تہی, 0, 0, 0, 0, 0, HWND_پیغام,
                                   تہی, g_hinst, تہی);
    }

    واپس g_hWndSound;
}

خالی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        واپس;

    wp = enc_مقصد_utf16(tv_get_string(&argvars[0]), تہی);
    اگر (wp == تہی)
        واپس;

    اگر (PlaySoundW(wp, تہی, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_نمبر = ++sound_id;
    آزاد(wp);
}

خالی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    طویل newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    حرف buf[32];
    MCIERROR err;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        واپس;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    اگر (p == تہی)
    {
        واپس;
    }
    vim_snprintf((حرف *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_مقصد_utf16((char_u *)p, تہی);
    آزاد(p);
    اگر (wp == تہی)
        واپس;

    err = mciSendStringW(wp, تہی, 0, sound_کھڑکی());
    آزاد(wp);
    اگر (err != 0)
        واپس;

    vim_snprintf(buf, حجم(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, تہی, 0, sound_کھڑکی());
    اگر (err != 0)
        جاؤ failure;

    sound_id = newid;
    rettv->vval.v_نمبر = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    اگر (soundcb != تہی)
    {
        vim_snprintf(buf, حجم(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    واپس;

failure:
    vim_snprintf(buf, حجم(buf), "close sound%06ld", newid);
    mciSendStringA(buf, تہی, 0, تہی);
}

خالی
f_sound_stop(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    طویل id;
    حرف buf[32];

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
        واپس;

    id = tv_get_نمبر(&argvars[0]);
    vim_snprintf(buf, حجم(buf), "stop sound%06ld", id);
    mciSendStringA(buf, تہی, 0, تہی);
}

خالی
f_sound_clear(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    PlaySoundW(تہی, تہی, 0);
    mciSendStringA("close all", تہی, 0, تہی);
}

#  اگر defined(EXITFREE)
خالی
sound_free(خالی)
{
    Close_کھڑکی(g_hWndSound);

    جب (پہلا_callback != تہی)
        delete_sound_callback(پہلا_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ساکن خالی
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        واپس;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    اگر (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_نمبر = play_success ? sound_id : 0;
}

خالی
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

خالی
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

خالی
f_sound_stop(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
        واپس;
    sound_mch_stop(tv_get_نمبر(&argvars[0]));
}

خالی
f_sound_clear(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    sound_mch_clear();
}

#  اگر defined(EXITFREE)
خالی
sound_free(خالی)
{
    sound_mch_free();
    جب (پہلا_callback != تہی)
        delete_sound_callback(پہلا_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#اگر_ختم // FEAT_SOUND
