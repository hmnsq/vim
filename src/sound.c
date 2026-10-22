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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SOUND)

સ્થિર long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

સંરચના soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

સ્થિર soundcb_T *first_callback = ખાલી;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
પૂર્ણાંક
has_any_sound_callback(શૂન્ય)
{
    પાછા first_callback != ખાલી;
}

સ્થિર soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    જો (arg->v_type == VAR_UNKNOWN)
        પાછા ખાલી;
    callback = get_callback(arg);
    જો (callback.cb_name == ખાલી)
        પાછા ખાલી;

    soundcb = ALLOC_ONE(soundcb_T);
    જો (soundcb == ખાલી)
    {
        free_callback(&callback);
        પાછા ખાલી;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    પાછા soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
શૂન્ય
call_sound_callback(soundcb_T *soundcb, long snd_id, પૂર્ણાંક result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_સંખ્યા;
    argv[0].vval.v_સંખ્યા = snd_id;
    argv[1].v_type = VAR_સંખ્યા;
    argv[1].vval.v_સંખ્યા = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
શૂન્ય
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ખાલી;

    માટે (p = first_callback; p != ખાલી; prev = p, p = p->snd_next)
        જો (p == soundcb)
        {
            જો (prev == ખાલી)
                first_callback = p->snd_next;
            નહીંતર
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            તોડો;
        }
}

# જો defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

સ્થિર ca_context *context = ખાલી;

// Structure to store info about a sound callback to be invoked soon.
પ્રકારવ્યાખ્યા સંરચના soundcb_queue_S soundcb_queue_T;

સંરચના soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    પૂર્ણાંક scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
સ્થિર soundcb_queue_T *callback_queue = ખાલી;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
સ્થિર શૂન્ય
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               પૂર્ણાંક error_code,
               શૂન્ય *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    જો (scb == ખાલી)
        પાછા;
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
પૂર્ણાંક
has_sound_callback_in_queue(શૂન્ય)
{
    પાછા callback_queue != ખાલી;
}

/*
 * Invoke queued sound callbacks.
 */
શૂન્ય
invoke_sound_callback(શૂન્ય)
{
    soundcb_queue_T *scb;

    જ્યારે (callback_queue != ખાલી)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

સ્થિર શૂન્ય
sound_play_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક playfile)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        પાછા;

    જો (context == ખાલી)
        ca_context_create(&context);
    જો (context == ખાલી)
        પાછા;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    પૂર્ણાંક res = CA_ERROR_INVALID;

    ++sound_id;
    જો (soundcb == ખાલી)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ખાલી);
    }
    નહીંતર
    {
        સ્થિર ca_proplist *proplist = ખાલી;

        ca_proplist_create(&proplist);
        જો (proplist != ખાલી)
        {
            જો (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (અક્ષર *)tv_get_string(&argvars[0]));
            નહીંતર
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (અક્ષર *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            જો (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_સંખ્યા = res == CA_SUCCESS ? sound_id : 0;
}

શૂન્ય
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
શૂન્ય
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
શૂન્ય
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
        પાછા;

    જો (context != ખાલી)
        ca_context_cancel(context, tv_get_સંખ્યા(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
શૂન્ય
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (context == ખાલી)
        પાછા;
    ca_context_destroy(context);
    context = ખાલી;
}

#  જો defined(EXITFREE)
શૂન્ય
sound_free(શૂન્ય)
{
    soundcb_queue_T *scb;

    જો (context != ખાલી)
        ca_context_destroy(context);

    જ્યારે (first_callback != ખાલી)
        delete_sound_callback(first_callback);

    જ્યારે (callback_queue != ખાલી)
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

સ્થિર HWND g_hWndSound = ખાલી;

સ્થિર LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    પસંદગી (message)
    {
        સ્થિતિ MM_MCINOTIFY:
            માટે (p = first_callback; p != ખાલી; p = p->snd_next)
                જો (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    અક્ષર buf[32];

                    vim_snprintf(buf, કદ(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ખાલી, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            તોડો;
    }

    પાછા DefWindowProc(hwnd, message, wParam, lParam);
}

સ્થિર HWND
sound_window(શૂન્ય)
{
    જો (g_hWndSound == ખાલી)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ખાલી, 0,
                              0, ખાલી,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, ખાલી, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   ખાલી, g_hinst, ખાલી);
    }

    પાછા g_hWndSound;
}

શૂન્ય
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        પાછા;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), ખાલી);
    જો (wp == ખાલી)
        પાછા;

    જો (PlaySoundW(wp, ખાલી, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_સંખ્યા = ++sound_id;
    મુક્ત(wp);
}

શૂન્ય
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    અક્ષર buf[32];
    MCIERROR err;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        પાછા;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    જો (p == ખાલી)
    {
        પાછા;
    }
    vim_snprintf((અક્ષર *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, ખાલી);
    મુક્ત(p);
    જો (wp == ખાલી)
        પાછા;

    err = mciSendStringW(wp, ખાલી, 0, sound_window());
    મુક્ત(wp);
    જો (err != 0)
        પાછા;

    vim_snprintf(buf, કદ(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ખાલી, 0, sound_window());
    જો (err != 0)
        જાઓ failure;

    sound_id = newid;
    rettv->vval.v_સંખ્યા = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    જો (soundcb != ખાલી)
    {
        vim_snprintf(buf, કદ(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    પાછા;

failure:
    vim_snprintf(buf, કદ(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ખાલી, 0, ખાલી);
}

શૂન્ય
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    અક્ષર buf[32];

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
        પાછા;

    id = tv_get_સંખ્યા(&argvars[0]);
    vim_snprintf(buf, કદ(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ખાલી, 0, ખાલી);
}

શૂન્ય
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(ખાલી, ખાલી, 0);
    mciSendStringA("close all", ખાલી, 0, ખાલી);
}

#  જો defined(EXITFREE)
શૂન્ય
sound_free(શૂન્ય)
{
    CloseWindow(g_hWndSound);

    જ્યારે (first_callback != ખાલી)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
સ્થિર શૂન્ય
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        પાછા;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    જો (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_સંખ્યા = play_success ? sound_id : 0;
}

શૂન્ય
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

શૂન્ય
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

શૂન્ય
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
        પાછા;
    sound_mch_stop(tv_get_સંખ્યા(&argvars[0]));
}

શૂન્ય
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  જો defined(EXITFREE)
શૂન્ય
sound_free(શૂન્ય)
{
    sound_mch_free();
    જ્યારે (first_callback != ખાલી)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
