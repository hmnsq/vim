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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_SOUND)

സ്ഥിതം long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ഘടന soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

സ്ഥിതം soundcb_T *first_callback = ശൂന്യ;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
പൂർണ്ണസംഖ്യ
has_any_sound_callback(ശൂന്യം)
{
    തിരികെ first_callback != ശൂന്യ;
}

സ്ഥിതം soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    എങ്കിൽ (arg->v_type == VAR_UNKNOWN)
        തിരികെ ശൂന്യ;
    callback = get_callback(arg);
    എങ്കിൽ (callback.cb_name == ശൂന്യ)
        തിരികെ ശൂന്യ;

    soundcb = ALLOC_ONE(soundcb_T);
    എങ്കിൽ (soundcb == ശൂന്യ)
    {
        free_callback(&callback);
        തിരികെ ശൂന്യ;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    തിരികെ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
ശൂന്യം
call_sound_callback(soundcb_T *soundcb, long snd_id, പൂർണ്ണസംഖ്യ result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_സംഖ്യ;
    argv[0].vval.v_സംഖ്യ = snd_id;
    argv[1].v_type = VAR_സംഖ്യ;
    argv[1].vval.v_സംഖ്യ = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
ശൂന്യം
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ശൂന്യ;

    വേണ്ടി (p = first_callback; p != ശൂന്യ; prev = p, p = p->snd_next)
        എങ്കിൽ (p == soundcb)
        {
            എങ്കിൽ (prev == ശൂന്യ)
                first_callback = p->snd_next;
            അല്ലെങ്കിൽ
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            നിർത്തുക;
        }
}

# എങ്കിൽ defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

സ്ഥിതം ca_context *context = ശൂന്യ;

// Structure to store info about a sound callback to be invoked soon.
തരനിർവചനം ഘടന soundcb_queue_S soundcb_queue_T;

ഘടന soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    പൂർണ്ണസംഖ്യ scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
സ്ഥിതം soundcb_queue_T *callback_queue = ശൂന്യ;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
സ്ഥിതം ശൂന്യം
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               പൂർണ്ണസംഖ്യ error_code,
               ശൂന്യം *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    എങ്കിൽ (scb == ശൂന്യ)
        തിരികെ;
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
പൂർണ്ണസംഖ്യ
has_sound_callback_in_queue(ശൂന്യം)
{
    തിരികെ callback_queue != ശൂന്യ;
}

/*
 * Invoke queued sound callbacks.
 */
ശൂന്യം
invoke_sound_callback(ശൂന്യം)
{
    soundcb_queue_T *scb;

    ആകുമ്പോൾ (callback_queue != ശൂന്യ)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

സ്ഥിതം ശൂന്യം
sound_play_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ playfile)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        തിരികെ;

    എങ്കിൽ (context == ശൂന്യ)
        ca_context_create(&context);
    എങ്കിൽ (context == ശൂന്യ)
        തിരികെ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    പൂർണ്ണസംഖ്യ res = CA_ERROR_INVALID;

    ++sound_id;
    എങ്കിൽ (soundcb == ശൂന്യ)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
        സ്ഥിതം ca_proplist *proplist = ശൂന്യ;

        ca_proplist_create(&proplist);
        എങ്കിൽ (proplist != ശൂന്യ)
        {
            എങ്കിൽ (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (അക്ഷരം *)tv_get_string(&argvars[0]));
            അല്ലെങ്കിൽ
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (അക്ഷരം *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            എങ്കിൽ (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_സംഖ്യ = res == CA_SUCCESS ? sound_id : 0;
}

ശൂന്യം
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
ശൂന്യം
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
ശൂന്യം
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
        തിരികെ;

    എങ്കിൽ (context != ശൂന്യ)
        ca_context_cancel(context, tv_get_സംഖ്യ(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
ശൂന്യം
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (context == ശൂന്യ)
        തിരികെ;
    ca_context_destroy(context);
    context = ശൂന്യ;
}

#  എങ്കിൽ defined(EXITFREE)
ശൂന്യം
sound_free(ശൂന്യം)
{
    soundcb_queue_T *scb;

    എങ്കിൽ (context != ശൂന്യ)
        ca_context_destroy(context);

    ആകുമ്പോൾ (first_callback != ശൂന്യ)
        delete_sound_callback(first_callback);

    ആകുമ്പോൾ (callback_queue != ശൂന്യ)
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

സ്ഥിതം HWND g_hWndSound = ശൂന്യ;

സ്ഥിതം LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    തിരഞ്ഞെടുപ്പ് (message)
    {
        അവസ്ഥ MM_MCINOTIFY:
            വേണ്ടി (p = first_callback; p != ശൂന്യ; p = p->snd_next)
                എങ്കിൽ (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    അക്ഷരം buf[32];

                    vim_snprintf(buf, വലുപ്പം(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ശൂന്യ, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            നിർത്തുക;
    }

    തിരികെ DefWindowProc(hwnd, message, wParam, lParam);
}

സ്ഥിതം HWND
sound_window(ശൂന്യം)
{
    എങ്കിൽ (g_hWndSound == ശൂന്യ)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ശൂന്യ, 0,
                              0, ശൂന്യ,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, ശൂന്യ, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   ശൂന്യ, g_hinst, ശൂന്യ);
    }

    തിരികെ g_hWndSound;
}

ശൂന്യം
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        തിരികെ;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), ശൂന്യ);
    എങ്കിൽ (wp == ശൂന്യ)
        തിരികെ;

    എങ്കിൽ (PlaySoundW(wp, ശൂന്യ, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_സംഖ്യ = ++sound_id;
    സ്വതന്ത്രം(wp);
}

ശൂന്യം
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    അക്ഷരം buf[32];
    MCIERROR err;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        തിരികെ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    എങ്കിൽ (p == ശൂന്യ)
    {
        തിരികെ;
    }
    vim_snprintf((അക്ഷരം *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, ശൂന്യ);
    സ്വതന്ത്രം(p);
    എങ്കിൽ (wp == ശൂന്യ)
        തിരികെ;

    err = mciSendStringW(wp, ശൂന്യ, 0, sound_window());
    സ്വതന്ത്രം(wp);
    എങ്കിൽ (err != 0)
        തിരികെ;

    vim_snprintf(buf, വലുപ്പം(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ശൂന്യ, 0, sound_window());
    എങ്കിൽ (err != 0)
        പോകുക failure;

    sound_id = newid;
    rettv->vval.v_സംഖ്യ = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    എങ്കിൽ (soundcb != ശൂന്യ)
    {
        vim_snprintf(buf, വലുപ്പം(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    തിരികെ;

failure:
    vim_snprintf(buf, വലുപ്പം(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ശൂന്യ, 0, ശൂന്യ);
}

ശൂന്യം
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    അക്ഷരം buf[32];

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
        തിരികെ;

    id = tv_get_സംഖ്യ(&argvars[0]);
    vim_snprintf(buf, വലുപ്പം(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ശൂന്യ, 0, ശൂന്യ);
}

ശൂന്യം
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(ശൂന്യ, ശൂന്യ, 0);
    mciSendStringA("close all", ശൂന്യ, 0, ശൂന്യ);
}

#  എങ്കിൽ defined(EXITFREE)
ശൂന്യം
sound_free(ശൂന്യം)
{
    CloseWindow(g_hWndSound);

    ആകുമ്പോൾ (first_callback != ശൂന്യ)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
സ്ഥിതം ശൂന്യം
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        തിരികെ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    എങ്കിൽ (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_സംഖ്യ = play_success ? sound_id : 0;
}

ശൂന്യം
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

ശൂന്യം
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

ശൂന്യം
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
        തിരികെ;
    sound_mch_stop(tv_get_സംഖ്യ(&argvars[0]));
}

ശൂന്യം
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  എങ്കിൽ defined(EXITFREE)
ശൂന്യം
sound_free(ശൂന്യം)
{
    sound_mch_free();
    ആകുമ്പോൾ (first_callback != ശൂന്യ)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
