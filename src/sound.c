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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_SOUND)

स्थिर दीर्घ sound_id = 0;

// soundcb_T is typedef'ed in vim.h

संरचना soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    दीर्घ snd_id;
# endif
    soundcb_T *snd_next;
};

स्थिर soundcb_T *first_callback = रिक्त;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
पूर्णाङ्क
has_any_sound_callback(शून्य)
{
    फर्काऊ first_callback != रिक्त;
}

स्थिर soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    यदि (arg->v_type == VAR_UNKNOWN)
        फर्काऊ रिक्त;
    callback = get_callback(arg);
    यदि (callback.cb_name == रिक्त)
        फर्काऊ रिक्त;

    soundcb = ALLOC_ONE(soundcb_T);
    यदि (soundcb == रिक्त)
    {
        free_callback(&callback);
        फर्काऊ रिक्त;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    फर्काऊ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
शून्य
call_sound_callback(soundcb_T *soundcb, दीर्घ snd_id, पूर्णाङ्क result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_सङ्ख्या;
    argv[0].vval.v_सङ्ख्या = snd_id;
    argv[1].v_type = VAR_सङ्ख्या;
    argv[1].vval.v_सङ्ख्या = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
शून्य
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = रिक्त;

    लागि (p = first_callback; p != रिक्त; prev = p, p = p->snd_next)
        यदि (p == soundcb)
        {
            यदि (prev == रिक्त)
                first_callback = p->snd_next;
            अन्यथा
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            रोक;
        }
}

# यदि defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

स्थिर ca_context *context = रिक्त;

// Structure to store info about a sound callback to be invoked soon.
प्रकारनिर्देश संरचना soundcb_queue_S soundcb_queue_T;

संरचना soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    पूर्णाङ्क scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
स्थिर soundcb_queue_T *callback_queue = रिक्त;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
स्थिर शून्य
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               पूर्णाङ्क error_code,
               शून्य *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    यदि (scb == रिक्त)
        फर्काऊ;
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
पूर्णाङ्क
has_sound_callback_in_queue(शून्य)
{
    फर्काऊ callback_queue != रिक्त;
}

/*
 * Invoke queued sound callbacks.
 */
शून्य
invoke_sound_callback(शून्य)
{
    soundcb_queue_T *scb;

    जबसम्म (callback_queue != रिक्त)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

स्थिर शून्य
sound_play_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क playfile)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        फर्काऊ;

    यदि (context == रिक्त)
        ca_context_create(&context);
    यदि (context == रिक्त)
        फर्काऊ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    पूर्णाङ्क res = CA_ERROR_INVALID;

    ++sound_id;
    यदि (soundcb == रिक्त)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", रिक्त);
    }
    अन्यथा
    {
        स्थिर ca_proplist *proplist = रिक्त;

        ca_proplist_create(&proplist);
        यदि (proplist != रिक्त)
        {
            यदि (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (वर्ण *)tv_get_string(&argvars[0]));
            अन्यथा
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (वर्ण *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            यदि (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_सङ्ख्या = res == CA_SUCCESS ? sound_id : 0;
}

शून्य
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
शून्य
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
शून्य
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
        फर्काऊ;

    यदि (context != रिक्त)
        ca_context_cancel(context, tv_get_सङ्ख्या(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
शून्य
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    यदि (context == रिक्त)
        फर्काऊ;
    ca_context_destroy(context);
    context = रिक्त;
}

#  यदि defined(EXITFREE)
शून्य
sound_free(शून्य)
{
    soundcb_queue_T *scb;

    यदि (context != रिक्त)
        ca_context_destroy(context);

    जबसम्म (first_callback != रिक्त)
        delete_sound_callback(first_callback);

    जबसम्म (callback_queue != रिक्त)
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

स्थिर HWND g_hWndSound = रिक्त;

स्थिर LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    विकल्प (message)
    {
        पक्ष MM_MCINOTIFY:
            लागि (p = first_callback; p != रिक्त; p = p->snd_next)
                यदि (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    वर्ण buf[32];

                    vim_snprintf(buf, परिमाण(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, रिक्त, 0, 0);

                    दीर्घ result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            रोक;
    }

    फर्काऊ DefWindowProc(hwnd, message, wParam, lParam);
}

स्थिर HWND
sound_window(शून्य)
{
    यदि (g_hWndSound == रिक्त)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, रिक्त, 0,
                              0, रिक्त,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, रिक्त, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   रिक्त, g_hinst, रिक्त);
    }

    फर्काऊ g_hWndSound;
}

शून्य
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        फर्काऊ;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), रिक्त);
    यदि (wp == रिक्त)
        फर्काऊ;

    यदि (PlaySoundW(wp, रिक्त, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_सङ्ख्या = ++sound_id;
    मुक्त(wp);
}

शून्य
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    दीर्घ newid = sound_id + 1;
    माप_प्रकार len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    वर्ण buf[32];
    MCIERROR err;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        फर्काऊ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    यदि (p == रिक्त)
    {
        फर्काऊ;
    }
    vim_snprintf((वर्ण *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, रिक्त);
    मुक्त(p);
    यदि (wp == रिक्त)
        फर्काऊ;

    err = mciSendStringW(wp, रिक्त, 0, sound_window());
    मुक्त(wp);
    यदि (err != 0)
        फर्काऊ;

    vim_snprintf(buf, परिमाण(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, रिक्त, 0, sound_window());
    यदि (err != 0)
        गच्छ failure;

    sound_id = newid;
    rettv->vval.v_सङ्ख्या = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    यदि (soundcb != रिक्त)
    {
        vim_snprintf(buf, परिमाण(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    फर्काऊ;

failure:
    vim_snprintf(buf, परिमाण(buf), "close sound%06ld", newid);
    mciSendStringA(buf, रिक्त, 0, रिक्त);
}

शून्य
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    दीर्घ id;
    वर्ण buf[32];

    यदि (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
        फर्काऊ;

    id = tv_get_सङ्ख्या(&argvars[0]);
    vim_snprintf(buf, परिमाण(buf), "stop sound%06ld", id);
    mciSendStringA(buf, रिक्त, 0, रिक्त);
}

शून्य
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(रिक्त, रिक्त, 0);
    mciSendStringA("close all", रिक्त, 0, रिक्त);
}

#  यदि defined(EXITFREE)
शून्य
sound_free(शून्य)
{
    CloseWindow(g_hWndSound);

    जबसम्म (first_callback != रिक्त)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
स्थिर शून्य
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        फर्काऊ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    यदि (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_सङ्ख्या = play_success ? sound_id : 0;
}

शून्य
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

शून्य
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

शून्य
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
        फर्काऊ;
    sound_mch_stop(tv_get_सङ्ख्या(&argvars[0]));
}

शून्य
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  यदि defined(EXITFREE)
शून्य
sound_free(शून्य)
{
    sound_mch_free();
    जबसम्म (first_callback != रिक्त)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
