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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SOUND)

ସ୍ଥିର long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

ସଂରଚନା soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

ସ୍ଥିର soundcb_T *first_callback = ଖାଲି;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_any_sound_callback(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ first_callback != ଖାଲି;
}

ସ୍ଥିର soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    ଯଦି (arg->v_type == VAR_UNKNOWN)
        ଫେରନ୍ତୁ ଖାଲି;
    callback = get_callback(arg);
    ଯଦି (callback.cb_name == ଖାଲି)
        ଫେରନ୍ତୁ ଖାଲି;

    soundcb = ALLOC_ONE(soundcb_T);
    ଯଦି (soundcb == ଖାଲି)
    {
        free_callback(&callback);
        ଫେରନ୍ତୁ ଖାଲି;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    ଫେରନ୍ତୁ soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
ଶୂନ୍ୟ
call_sound_callback(soundcb_T *soundcb, long snd_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result)
{
    typval_T argv[3];
    typval_T rettv;

    argv[0].v_type = VAR_ସଂଖ୍ୟା;
    argv[0].vval.v_ସଂଖ୍ୟା = snd_id;
    argv[1].v_type = VAR_ସଂଖ୍ୟା;
    argv[1].vval.v_ସଂଖ୍ୟା = result;
    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&soundcb->snd_callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
}

/*
 * Delete "soundcb" from the list of pending callbacks.
 */
ଶୂନ୍ୟ
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = ଖାଲି;

    ପାଇଁ (p = first_callback; p != ଖାଲି; prev = p, p = p->snd_next)
        ଯଦି (p == soundcb)
        {
            ଯଦି (prev == ଖାଲି)
                first_callback = p->snd_next;
            ଅନ୍ୟଥା
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            ଭାଙ୍ଗ;
        }
}

# ଯଦି defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

ସ୍ଥିର ca_context *context = ଖାଲି;

// Structure to store info about a sound callback to be invoked soon.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା soundcb_queue_S soundcb_queue_T;

ସଂରଚନା soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
ସ୍ଥିର soundcb_queue_T *callback_queue = ଖାଲି;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_code,
               ଶୂନ୍ୟ *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    ଯଦି (scb == ଖାଲି)
        ଫେରନ୍ତୁ;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_sound_callback_in_queue(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ callback_queue != ଖାଲି;
}

/*
 * Invoke queued sound callbacks.
 */
ଶୂନ୍ୟ
invoke_sound_callback(ଶୂନ୍ୟ)
{
    soundcb_queue_T *scb;

    ଯେତେବେଳେ (callback_queue != ଖାଲି)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

ସ୍ଥିର ଶୂନ୍ୟ
sound_play_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା playfile)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (context == ଖାଲି)
        ca_context_create(&context);
    ଯଦି (context == ଖାଲି)
        ଫେରନ୍ତୁ;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = CA_ERROR_INVALID;

    ++sound_id;
    ଯଦି (soundcb == ଖାଲି)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
        ସ୍ଥିର ca_proplist *proplist = ଖାଲି;

        ca_proplist_create(&proplist);
        ଯଦି (proplist != ଖାଲି)
        {
            ଯଦି (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (ଅକ୍ଷର *)tv_get_string(&argvars[0]));
            ଅନ୍ୟଥା
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (ଅକ୍ଷର *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            ଯଦି (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_ସଂଖ୍ୟା = res == CA_SUCCESS ? sound_id : 0;
}

ଶୂନ୍ୟ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
ଶୂନ୍ୟ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
ଶୂନ୍ୟ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (context != ଖାଲି)
        ca_context_cancel(context, tv_get_ସଂଖ୍ୟା(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
ଶୂନ୍ୟ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (context == ଖାଲି)
        ଫେରନ୍ତୁ;
    ca_context_destroy(context);
    context = ଖାଲି;
}

#  ଯଦି defined(EXITFREE)
ଶୂନ୍ୟ
sound_free(ଶୂନ୍ୟ)
{
    soundcb_queue_T *scb;

    ଯଦି (context != ଖାଲି)
        ca_context_destroy(context);

    ଯେତେବେଳେ (first_callback != ଖାଲି)
        delete_sound_callback(first_callback);

    ଯେତେବେଳେ (callback_queue != ଖାଲି)
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

ସ୍ଥିର HWND g_hWndSound = ଖାଲି;

ସ୍ଥିର LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    ବାଛ (message)
    {
        ସ୍ଥିତି MM_MCINOTIFY:
            ପାଇଁ (p = first_callback; p != ଖାଲି; p = p->snd_next)
                ଯଦି (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    ଅକ୍ଷର buf[32];

                    vim_snprintf(buf, ଆକାର(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, ଖାଲି, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ DefWindowProc(hwnd, message, wParam, lParam);
}

ସ୍ଥିର HWND
sound_window(ଶୂନ୍ୟ)
{
    ଯଦି (g_hWndSound == ଖାଲି)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, ଖାଲି, 0,
                              0, ଖାଲି,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, ଖାଲି, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   ଖାଲି, g_hinst, ଖାଲି);
    }

    ଫେରନ୍ତୁ g_hWndSound;
}

ଶୂନ୍ୟ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), ଖାଲି);
    ଯଦି (wp == ଖାଲି)
        ଫେରନ୍ତୁ;

    ଯଦି (PlaySoundW(wp, ଖାଲି, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_ସଂଖ୍ୟା = ++sound_id;
    ମୁକ୍ତ(wp);
}

ଶୂନ୍ୟ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    ଅକ୍ଷର buf[32];
    MCIERROR err;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    ଯଦି (p == ଖାଲି)
    {
        ଫେରନ୍ତୁ;
    }
    vim_snprintf((ଅକ୍ଷର *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, ଖାଲି);
    ମୁକ୍ତ(p);
    ଯଦି (wp == ଖାଲି)
        ଫେରନ୍ତୁ;

    err = mciSendStringW(wp, ଖାଲି, 0, sound_window());
    ମୁକ୍ତ(wp);
    ଯଦି (err != 0)
        ଫେରନ୍ତୁ;

    vim_snprintf(buf, ଆକାର(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, ଖାଲି, 0, sound_window());
    ଯଦି (err != 0)
        ଯାଅ failure;

    sound_id = newid;
    rettv->vval.v_ସଂଖ୍ୟା = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    ଯଦି (soundcb != ଖାଲି)
    {
        vim_snprintf(buf, ଆକାର(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    ଫେରନ୍ତୁ;

failure:
    vim_snprintf(buf, ଆକାର(buf), "close sound%06ld", newid);
    mciSendStringA(buf, ଖାଲି, 0, ଖାଲି);
}

ଶୂନ୍ୟ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    ଅକ୍ଷର buf[32];

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    id = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    vim_snprintf(buf, ଆକାର(buf), "stop sound%06ld", id);
    mciSendStringA(buf, ଖାଲି, 0, ଖାଲି);
}

ଶୂନ୍ୟ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(ଖାଲି, ଖାଲି, 0);
    mciSendStringA("close all", ଖାଲି, 0, ଖାଲି);
}

#  ଯଦି defined(EXITFREE)
ଶୂନ୍ୟ
sound_free(ଶୂନ୍ୟ)
{
    CloseWindow(g_hWndSound);

    ଯେତେବେଳେ (first_callback != ଖାଲି)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
ସ୍ଥିର ଶୂନ୍ୟ
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    ଯଦି (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_ସଂଖ୍ୟା = play_success ? sound_id : 0;
}

ଶୂନ୍ୟ
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

ଶୂନ୍ୟ
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

ଶୂନ୍ୟ
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;
    sound_mch_stop(tv_get_ସଂଖ୍ୟା(&argvars[0]));
}

ଶୂନ୍ୟ
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  ଯଦି defined(EXITFREE)
ଶୂନ୍ୟ
sound_free(ଶୂନ୍ୟ)
{
    sound_mch_free();
    ଯେତେବେଳେ (first_callback != ଖାଲି)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
