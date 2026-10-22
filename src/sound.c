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

#include "vim.h"

#만약 defined(FEAT_SOUND)

static long sound_id = 0;

// soundcb_T is typedef'ed in vim.h

struct soundcb_S
{
    callback_T snd_callback;
# ifdef MSWIN
    MCIDEVICEID snd_device_id;
    long snd_id;
# endif
    soundcb_T *snd_next;
};

static soundcb_T *first_callback = NULL;

/*
 * Return TRUE when a sound callback has been created, it may be invoked when
 * the sound finishes playing.  Also see has_sound_callback_in_queue().
 */
정수
has_any_sound_callback(void)
{
    반환 first_callback != NULL;
}

static soundcb_T *
get_sound_callback(typval_T *arg)
{
    callback_T callback;
    soundcb_T *soundcb;

    만약 (arg->v_type == VAR_UNKNOWN)
        반환 NULL;
    callback = get_callback(arg);
    만약 (callback.cb_name == NULL)
        반환 NULL;

    soundcb = ALLOC_ONE(soundcb_T);
    만약 (soundcb == NULL)
    {
        free_callback(&callback);
        반환 NULL;
    }

    soundcb->snd_next = first_callback;
    first_callback = soundcb;
    set_callback(&soundcb->snd_callback, &callback);
    반환 soundcb;
}

/*
 * Call "soundcb" with proper parameters.
 */
void
call_sound_callback(soundcb_T *soundcb, long snd_id, 정수 result)
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
void
delete_sound_callback(soundcb_T *soundcb)
{
    soundcb_T *p;
    soundcb_T *prev = NULL;

    위해 (p = first_callback; p != NULL; prev = p, p = p->snd_next)
        만약 (p == soundcb)
        {
            만약 (prev == NULL)
                first_callback = p->snd_next;
            아니면
                prev->snd_next = p->snd_next;
            free_callback(&p->snd_callback);
            vim_free(p);
            break;
        }
}

# 만약 defined(HAVE_CANBERRA)

/*
 * Sound implementation for Linux/Unix using libcanberra.
 */
#  include <canberra.h>

static ca_context *context = NULL;

// Structure to store info about a sound callback to be invoked soon.
typedef struct soundcb_queue_S soundcb_queue_T;

struct soundcb_queue_S
{
    soundcb_queue_T *scb_next;
    uint32_t scb_id; // ID of the sound
    정수 scb_result; // CA_ value
    soundcb_T *scb_callback; // function to call
};

// Queue of callbacks to invoke from the main loop.
static soundcb_queue_T *callback_queue = NULL;

/*
 * Add a callback to the queue of callbacks to invoke later from the main loop.
 * That is because the callback may be called from another thread and invoking
 * another sound function may cause trouble.
 */
static void
sound_callback(ca_context *c UNUSED,
               uint32_t id,
               정수 error_code,
               void *userdata)
{
    soundcb_T *soundcb = (soundcb_T *)userdata;
    soundcb_queue_T *scb;

    scb = ALLOC_ONE(soundcb_queue_T);
    만약 (scb == NULL)
        반환;
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
정수
has_sound_callback_in_queue(void)
{
    반환 callback_queue != NULL;
}

/*
 * Invoke queued sound callbacks.
 */
void
invoke_sound_callback(void)
{
    soundcb_queue_T *scb;

    동안 (callback_queue != NULL)
    {
        scb = callback_queue;
        callback_queue = scb->scb_next;

        call_sound_callback(scb->scb_callback, scb->scb_id, scb->scb_result);

        delete_sound_callback(scb->scb_callback);
        vim_free(scb);
    }
    redraw_after_callback(TRUE, FALSE);
}

static void
sound_play_common(typval_T *argvars, typval_T *rettv, 정수 playfile)
{
    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        반환;

    만약 (context == NULL)
        ca_context_create(&context);
    만약 (context == NULL)
        반환;

    soundcb_T *soundcb = get_sound_callback(&argvars[1]);
    정수 res = CA_ERROR_INVALID;

    ++sound_id;
    만약 (soundcb == NULL)
    {
        res = ca_context_play(context, sound_id,
                              playfile ? CA_PROP_MEDIA_FILENAME
                                       : CA_PROP_EVENT_ID,
                              tv_get_string(&argvars[0]),
                              CA_PROP_CANBERRA_CACHE_CONTROL, "volatile", NULL);
    }
    아니면
    {
        static ca_proplist *proplist = NULL;

        ca_proplist_create(&proplist);
        만약 (proplist != NULL)
        {
            만약 (playfile)
                ca_proplist_sets(proplist, CA_PROP_MEDIA_FILENAME,
                                 (char *)tv_get_string(&argvars[0]));
            아니면
                ca_proplist_sets(proplist, CA_PROP_EVENT_ID,
                                 (char *)tv_get_string(&argvars[0]));
            ca_proplist_sets(proplist, CA_PROP_CANBERRA_CACHE_CONTROL,
                             "volatile");
            res = ca_context_play_full(context, sound_id, proplist,
                                       sound_callback, soundcb);
            만약 (res != CA_SUCCESS)
                delete_sound_callback(soundcb);

            ca_proplist_destroy(proplist);
        }
    }
    rettv->vval.v_number = res == CA_SUCCESS ? sound_id : 0;
}

void
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, FALSE);
}

/*
 * implementation of sound_playfile({path} [, {callback}])
 */
void
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, TRUE);
}

/*
 * implementation of sound_stop({id})
 */
void
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        반환;

    만약 (context != NULL)
        ca_context_cancel(context, tv_get_number(&argvars[0]));
}

/*
 * implementation of sound_clear()
 */
void
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    만약 (context == NULL)
        반환;
    ca_context_destroy(context);
    context = NULL;
}

#  만약 defined(EXITFREE)
void
sound_free(void)
{
    soundcb_queue_T *scb;

    만약 (context != NULL)
        ca_context_destroy(context);

    동안 (first_callback != NULL)
        delete_sound_callback(first_callback);

    동안 (callback_queue != NULL)
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

static HWND g_hWndSound = NULL;

static LRESULT CALLBACK
sound_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    soundcb_T *p;

    switch (message)
    {
        case MM_MCINOTIFY:
            위해 (p = first_callback; p != NULL; p = p->snd_next)
                만약 (p->snd_device_id == (MCIDEVICEID)lParam)
                {
                    char buf[32];

                    vim_snprintf(buf, sizeof(buf), "close sound%06ld",
                                 p->snd_id);
                    mciSendStringA(buf, NULL, 0, 0);

                    long result = wParam == MCI_NOTIFY_SUCCESSFUL ? 0
                                  : wParam == MCI_NOTIFY_ABORTED  ? 1
                                                                  : 2;
                    call_sound_callback(p, p->snd_id, result);

                    delete_sound_callback(p);
                    redraw_after_callback(TRUE, FALSE);
                }
            break;
    }

    반환 DefWindowProc(hwnd, message, wParam, lParam);
}

static HWND
sound_window(void)
{
    만약 (g_hWndSound == NULL)
    {
        LPCSTR clazz = "VimSound";
        WNDCLASS wndclass = { 0, sound_wndproc, 0,    0, g_hinst, NULL, 0,
                              0, NULL,          clazz };
        RegisterClass(&wndclass);
        g_hWndSound = CreateWindow(clazz, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   NULL, g_hinst, NULL);
    }

    반환 g_hWndSound;
}

void
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    WCHAR *wp;

    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        반환;

    wp = enc_to_utf16(tv_get_string(&argvars[0]), NULL);
    만약 (wp == NULL)
        반환;

    만약 (PlaySoundW(wp, NULL, SND_ASYNC | SND_ALIAS))
        rettv->vval.v_number = ++sound_id;
    free(wp);
}

void
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    long newid = sound_id + 1;
    size_t len;
    char_u *p, *filename;
    WCHAR *wp;
    soundcb_T *soundcb;
    char buf[32];
    MCIERROR err;

    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        반환;

    filename = tv_get_string(&argvars[0]);

    len = STRLEN(filename) + 5 + 18 + 2 + 1;
    p = alloc(len);
    만약 (p == NULL)
    {
        반환;
    }
    vim_snprintf((char *)p, len, "open \"%s\" alias sound%06ld", filename,
                 newid);

    wp = enc_to_utf16((char_u *)p, NULL);
    free(p);
    만약 (wp == NULL)
        반환;

    err = mciSendStringW(wp, NULL, 0, sound_window());
    free(wp);
    만약 (err != 0)
        반환;

    vim_snprintf(buf, sizeof(buf), "play sound%06ld notify", newid);
    err = mciSendStringA(buf, NULL, 0, sound_window());
    만약 (err != 0)
        goto failure;

    sound_id = newid;
    rettv->vval.v_number = sound_id;

    soundcb = get_sound_callback(&argvars[1]);
    만약 (soundcb != NULL)
    {
        vim_snprintf(buf, sizeof(buf), "sound%06ld", newid);
        soundcb->snd_id = newid;
        soundcb->snd_device_id = mciGetDeviceID(buf);
    }
    반환;

failure:
    vim_snprintf(buf, sizeof(buf), "close sound%06ld", newid);
    mciSendStringA(buf, NULL, 0, NULL);
}

void
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    long id;
    char buf[32];

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        반환;

    id = tv_get_number(&argvars[0]);
    vim_snprintf(buf, sizeof(buf), "stop sound%06ld", id);
    mciSendStringA(buf, NULL, 0, NULL);
}

void
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    PlaySoundW(NULL, NULL, 0);
    mciSendStringA("close all", NULL, 0, NULL);
}

#  만약 defined(EXITFREE)
void
sound_free(void)
{
    CloseWindow(g_hWndSound);

    동안 (first_callback != NULL)
        delete_sound_callback(first_callback);
}
#  endif

# elif defined(MACOS_X_DARWIN)

// Sound implementation for macOS.
static void
sound_play_common(typval_T *argvars, typval_T *rettv, bool playfile)
{
    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
        반환;

    char_u *sound_name = tv_get_string(&argvars[0]);
    soundcb_T *soundcb = get_sound_callback(&argvars[1]);

    ++sound_id;

    bool play_success = sound_mch_play(sound_name, sound_id, soundcb, playfile);
    만약 (!play_success && soundcb)
    {
        delete_sound_callback(soundcb);
    }
    rettv->vval.v_number = play_success ? sound_id : 0;
}

void
f_sound_playevent(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, false);
}

void
f_sound_playfile(typval_T *argvars, typval_T *rettv)
{
    sound_play_common(argvars, rettv, true);
}

void
f_sound_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
        반환;
    sound_mch_stop(tv_get_number(&argvars[0]));
}

void
f_sound_clear(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    sound_mch_clear();
}

#  만약 defined(EXITFREE)
void
sound_free(void)
{
    sound_mch_free();
    동안 (first_callback != NULL)
        delete_sound_callback(first_callback);
}
#  endif

# endif // MACOS_X_DARWIN

#endif // FEAT_SOUND
