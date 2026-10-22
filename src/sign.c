/* vi:set ts=8 sts=4 sw=4 et:
 *
 * VIM - Vi IMproved by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * sign.c: functions for managing signs
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sign sign_T;

ਬਣਤਰ sign
{
    sign_T *sn_ਅਗਲਾ; // next sign in list
    ਪੂਰਨ sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    ਖਾਲੀ *sn_image; // icon image
# endif
    char_u *sn_ਪਾਠ; // text used instead of pixmap
    ਪੂਰਨ sn_ਸਤਰ_hl; // highlight ID for line
    ਪੂਰਨ sn_ਪਾਠ_hl; // highlight ID for text
    ਪੂਰਨ sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    ਪੂਰਨ sn_ਅੰਕ_hl; // highlight ID for line number
    ਪੂਰਨ sn_ਤਰਜੀਹ; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ਸਥਿਰ sign_T *ਪਹਿਲਾ_sign = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_sign_typenr = 1;

ਸਥਿਰ ਖਾਲੀ sign_list_defined(sign_T *sp);
ਸਥਿਰ ਖਾਲੀ sign_undefine(sign_T *sp, sign_T *sp_prev);

ਸਥਿਰ ਅੱਖਰ *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_ਰੱਖੋ 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        ਨਿਰਾ
# define SIGNCMD_ਆਖਰੀ 6
};

# define FOR_ALL_SIGNS(sp) \
     ਲਈ ((sp) = ਪਹਿਲਾ_sign; (sp) != ਨਿਰਾ; (sp) = (sp)->sn_ਅਗਲਾ)

ਸਥਿਰ hashtab_T sg_ਸਾਰਣੀ; // sign group (signgroup_T) hashtable
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
ਖਾਲੀ
init_signs(ਖਾਲੀ)
{
    hash_init(&sg_ਸਾਰਣੀ); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ਸਥਿਰ signgroup_T *
sign_ਸਮੂਹ_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_ਸਾਰਣੀ, groupname, hash);
    signgroup_T *ਸਮੂਹ = ਨਿਰਾ;

    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
        // new group
        ਸਮੂਹ = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
            ਵਾਪਸ ਨਿਰਾ;

        STRCPY(ਸਮੂਹ->sg_name, groupname);
        ਸਮੂਹ->sg_refcount = 1;
        ਸਮੂਹ->sg_ਅਗਲਾ_sign_id = 1;
        hash_ਜੋੜੋ_ਵਸਤੂ(&sg_ਸਾਰਣੀ, hi, ਸਮੂਹ->sg_name, hash);
    }
    ਨਹੀਂਤਾਂ
    {
        // existing group
        ਸਮੂਹ = HI2SG(hi);
        ਸਮੂਹ->sg_refcount++;
    }

    ਵਾਪਸ ਸਮੂਹ;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ਸਥਿਰ ਖਾਲੀ
sign_ਸਮੂਹ_unref(char_u *groupname)
{
    hashitem_T *hi = hash_ਲੱਭੋ(&sg_ਸਾਰਣੀ, groupname);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
        ਵਾਪਸ;

    signgroup_T *ਸਮੂਹ = HI2SG(hi);
    ਸਮੂਹ->sg_refcount--;
    ਜੇ (ਸਮੂਹ->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_ਸਾਰਣੀ, hi, "sign remove");
        vim_free(ਸਮੂਹ);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
ਸਥਿਰ ਪੂਰਨ
sign_ਵਿੱਚ_ਸਮੂਹ(sign_ਇੰਦਰਾਜ_T *sign, char_u *ਸਮੂਹ)
{
    ਵਾਪਸ ((ਸਮੂਹ != ਨਿਰਾ && STRCMP(ਸਮੂਹ, "*") == 0) ||
            (ਸਮੂਹ == ਨਿਰਾ && sign->se_ਸਮੂਹ == ਨਿਰਾ) ||
            (ਸਮੂਹ != ਨਿਰਾ && sign->se_ਸਮੂਹ != ਨਿਰਾ &&
             STRCMP(ਸਮੂਹ, sign->se_ਸਮੂਹ->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ਸਥਿਰ ਪੂਰਨ
sign_ਸਮੂਹ_for_ਖਿੜਕੀ(sign_ਇੰਦਰਾਜ_T *sign, win_T *wp)
{
    ਪੂਰਨ for_popup = sign->se_ਸਮੂਹ != ਨਿਰਾ &&
                    STRNCMP("PopUp", sign->se_ਸਮੂਹ->sg_name, 5) == 0;

    ਵਾਪਸ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ਸਥਿਰ ਪੂਰਨ
sign_ਸਮੂਹ_get_ਅਗਲਾ_signid(buf_T *buf, char_u *groupname)
{
    ਪੂਰਨ id = 1;
    signgroup_T *ਸਮੂਹ = ਨਿਰਾ;
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ;

    ਜੇ (groupname != ਨਿਰਾ)
    {
        hashitem_T *hi = hash_ਲੱਭੋ(&sg_ਸਾਰਣੀ, groupname);
        ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
            ਵਾਪਸ id;
        ਸਮੂਹ = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    ਪੂਰਨ ਮਿਲਿਆ = FALSE;
    ਜਦੋਂ (!ਮਿਲਿਆ)
    {
        ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
            id = ਅਗਲਾ_sign_id++; // global group
        ਨਹੀਂਤਾਂ
            id = ਸਮੂਹ->sg_ਅਗਲਾ_sign_id++;

        // Check whether this sign is already placed in the buffer
        ਮਿਲਿਆ = TRUE;
        FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
        {
            ਜੇ (id == sign->se_id && sign_ਵਿੱਚ_ਸਮੂਹ(sign, groupname))
            {
                ਮਿਲਿਆ = FALSE; // sign identifier is in use
                ਰੋਕੋ;
            }
        }
    }

    ਵਾਪਸ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ਸਥਿਰ ਖਾਲੀ
ਪਾਓ_sign(buf_T *buf, // buffer to store sign in
            sign_ਇੰਦਰਾਜ_T *prev, // previous sign entry
            sign_ਇੰਦਰਾਜ_T *ਅਗਲਾ, // next sign entry
            ਪੂਰਨ id, // sign ID
            char_u *ਸਮੂਹ, // sign group; NULL for global group
            ਪੂਰਨ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ਪੂਰਨ typenr) // typenr of sign we are adding
{
    sign_ਇੰਦਰਾਜ_T *newsign =
        lalloc_id(ਆਕਾਰ(sign_ਇੰਦਰਾਜ_T), FALSE, aid_ਪਾਓ_sign);
    ਜੇ (newsign == ਨਿਰਾ)
        ਵਾਪਸ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ਜੇ (ਸਮੂਹ != ਨਿਰਾ)
    {
        newsign->se_ਸਮੂਹ = sign_ਸਮੂਹ_ref(ਸਮੂਹ);
        ਜੇ (newsign->se_ਸਮੂਹ == ਨਿਰਾ)
        {
            vim_free(newsign);
            ਵਾਪਸ;
        }
    }
    ਨਹੀਂਤਾਂ
    {
        newsign->se_ਸਮੂਹ = ਨਿਰਾ;
    }

    newsign->se_ਤਰਜੀਹ = prio;
    newsign->se_ਅਗਲਾ = ਅਗਲਾ;
    newsign->se_prev = prev;
    ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
        ਅਗਲਾ->se_prev = newsign;

    ਜੇ (prev == ਨਿਰਾ)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ਜੇ (buf->b_signlist == ਨਿਰਾ)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            ਬਦਲਿਆ_ਸਤਰ_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ਜੇ (netbeans_ਸਰਗਰਮ())
            buf->b_has_sign_ਕਾਲਮ = true;
# endif
    }
    ਨਹੀਂਤਾਂ
    {
        prev->se_ਅਗਲਾ = newsign;
    }

    // Remember the newly inserted sign so a following insert on a later line
    // can resume the search from here.
    buf->b_sign_finger = newsign;
}

/*
 * Insert a new sign sorted by line number and sign priority.
 */
ਸਥਿਰ ਖਾਲੀ
ਪਾਓ_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_ਇੰਦਰਾਜ_T *prev, // previous sign entry
                         ਪੂਰਨ id, // sign ID
                         char_u *ਸਮੂਹ, // sign group; NULL for global group
                         ਪੂਰਨ prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         ਪੂਰਨ typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ਜਦੋਂ (prev != ਨਿਰਾ && prev->se_lnum == lnum && prev->se_ਤਰਜੀਹ <= prio)
        prev = prev->se_prev;

    sign_ਇੰਦਰਾਜ_T *sign = (prev == ਨਿਰਾ) ? buf->b_signlist : prev->se_ਅਗਲਾ;

    ਪਾਓ_sign(buf, prev, sign, id, ਸਮੂਹ, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ਸਥਿਰ sign_T *
ਲੱਭੋ_sign_by_typenr(ਪੂਰਨ typenr)
{
    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
        ਜੇ (sp->sn_typenr == typenr)
            ਵਾਪਸ sp;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the name of a sign by its typenr.
 */
ਸਥਿਰ char_u *
sign_typenr2name(ਪੂਰਨ typenr)
{
    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
        ਜੇ (sp->sn_typenr == typenr)
            ਵਾਪਸ sp->sn_name;
    ਵਾਪਸ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ਸਥਿਰ dict_T *
sign_get_info(sign_ਇੰਦਰਾਜ_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ਜੇ (d == ਨਿਰਾ)
        ਵਾਪਸ ਨਿਰਾ;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "id", sign->se_id);
    ਜੇ (sign->se_ਸਮੂਹ == ਨਿਰਾ)
        dict_ਜੋੜੋ_string_len(d, "group", (char_u *)"", 0);
    ਨਹੀਂਤਾਂ
        dict_ਜੋੜੋ_string(d, "group", sign->se_ਸਮੂਹ->sg_name);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", sign->se_lnum);
    dict_ਜੋੜੋ_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "priority", sign->se_ਤਰਜੀਹ);

    ਵਾਪਸ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ਸਥਿਰ ਖਾਲੀ
sign_ਛਾਂਟੋ_by_prio_on_ਸਤਰ(buf_T *buf, sign_ਇੰਦਰਾਜ_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ਜੇ ((sign->se_prev == ਨਿਰਾ || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_ਤਰਜੀਹ > sign->se_ਤਰਜੀਹ) &&
        (sign->se_ਅਗਲਾ == ਨਿਰਾ || sign->se_ਅਗਲਾ->se_lnum != sign->se_lnum ||
         sign->se_ਅਗਲਾ->se_ਤਰਜੀਹ < sign->se_ਤਰਜੀਹ))
        ਵਾਪਸ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_ਇੰਦਰਾਜ_T *p = sign;
    ਜਦੋਂ (p->se_prev != ਨਿਰਾ && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_ਤਰਜੀਹ <= sign->se_ਤਰਜੀਹ)
        p = p->se_prev;

    ਜੇ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_ਅਗਲਾ;
        ਜਦੋਂ (p->se_ਅਗਲਾ != ਨਿਰਾ && p->se_ਅਗਲਾ->se_lnum == sign->se_lnum &&
               p->se_ਅਗਲਾ->se_ਤਰਜੀਹ > sign->se_ਤਰਜੀਹ)
            p = p->se_ਅਗਲਾ;
    }

    // Remove 'sign' from the list
    ਜੇ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_ਅਗਲਾ;

    ਜੇ (sign->se_prev != ਨਿਰਾ)
        sign->se_prev->se_ਅਗਲਾ = sign->se_ਅਗਲਾ;

    ਜੇ (sign->se_ਅਗਲਾ != ਨਿਰਾ)
        sign->se_ਅਗਲਾ->se_prev = sign->se_prev;

    sign->se_prev = ਨਿਰਾ;
    sign->se_ਅਗਲਾ = ਨਿਰਾ;

    // Re-insert 'sign' at the right place
    ਜੇ (p->se_ਤਰਜੀਹ <= sign->se_ਤਰਜੀਹ)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_ਅਗਲਾ = p;
        p->se_prev = sign;
        ਜੇ (sign->se_prev != ਨਿਰਾ)
            sign->se_prev->se_ਅਗਲਾ = sign;

        ਜੇ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ਨਹੀਂਤਾਂ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_ਅਗਲਾ = p->se_ਅਗਲਾ;
        p->se_ਅਗਲਾ = sign;
        ਜੇ (sign->se_ਅਗਲਾ != ਨਿਰਾ)
            sign->se_ਅਗਲਾ->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ਸਥਿਰ ਖਾਲੀ
buf_addsign(buf_T *buf, // buffer to store sign in
            ਪੂਰਨ id, // sign ID
            char_u *groupname, // sign group
            ਪੂਰਨ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ਪੂਰਨ typenr) // typenr of sign we are adding
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    sign_ਇੰਦਰਾਜ_T *prev = ਨਿਰਾ; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ਜੇ (buf->b_sign_finger != ਨਿਰਾ && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    ਲਈ (sign = (prev == ਨਿਰਾ) ? buf->b_signlist : prev->se_ਅਗਲਾ; sign != ਨਿਰਾ;
         sign = sign->se_ਅਗਲਾ)
    {
        ਜੇ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_ਵਿੱਚ_ਸਮੂਹ(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_ਤਰਜੀਹ = prio;
            sign_ਛਾਂਟੋ_by_prio_on_ਸਤਰ(buf, sign);
            ਵਾਪਸ;
        }
        ਨਹੀਂਤਾਂ ਜੇ (lnum < sign->se_lnum)
        {
            ਪਾਓ_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ਵਾਪਸ;
        }
        prev = sign;
    }

    ਪਾਓ_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
ਸਥਿਰ linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     ਪੂਰਨ markId, // sign ID
                     char_u *ਸਮੂਹ, // sign group
                     ਪੂਰਨ typenr, // typenr of sign we are adding
                     ਪੂਰਨ prio) // sign priority
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        ਜੇ (sign->se_id == markId && sign_ਵਿੱਚ_ਸਮੂਹ(sign, ਸਮੂਹ))
        {
            sign->se_typenr = typenr;
            sign->se_ਤਰਜੀਹ = prio;
            sign_ਛਾਂਟੋ_by_prio_on_ਸਤਰ(buf, sign);
            ਵਾਪਸ sign->se_lnum;
        }
    }

    ਵਾਪਸ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
ਪੂਰਨ
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_ਸੂਚਕ(sattr);

    buf_T *buf = wp->w_ਬਫਰ;
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ;
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ਜੇ (sign->se_lnum > lnum)
            ਰੋਕੋ;

        ਜੇ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_ਸਮੂਹ_for_ਖਿੜਕੀ(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = ਲੱਭੋ_sign_by_typenr(sign->se_typenr);
            ਜੇ (sp == ਨਿਰਾ)
                ਵਾਪਸ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_ਪਾਠ = sp->sn_ਪਾਠ;

            ਜੇ (sattr->sat_ਪਾਠ != ਨਿਰਾ && sp->sn_ਪਾਠ_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_ਪਾਠ_hl);

            ਜੇ (sp->sn_ਸਤਰ_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_ਸਤਰ_hl);

            ਜੇ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ਜੇ (sp->sn_ਅੰਕ_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_ਅੰਕ_hl);

            sattr->sat_ਤਰਜੀਹ = sign->se_ਤਰਜੀਹ;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ਜੇ (sign->se_ਅਗਲਾ != ਨਿਰਾ &&
                sign->se_ਅਗਲਾ->se_ਤਰਜੀਹ == sign->se_ਤਰਜੀਹ &&
                sign->se_ਅਗਲਾ->se_lnum == sign->se_lnum)
            {
                sign_T *ਅਗਲਾ_sp = ਲੱਭੋ_sign_by_typenr(sign->se_ਅਗਲਾ->se_typenr);
                ਜੇ (ਅਗਲਾ_sp == ਨਿਰਾ)
                    ਵਾਪਸ FALSE;

                ਜੇ (sattr->sat_icon == ਨਿਰਾ && sattr->sat_ਪਾਠ == ਨਿਰਾ)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = ਅਗਲਾ_sp->sn_image;
# endif
                    sattr->sat_ਪਾਠ = ਅਗਲਾ_sp->sn_ਪਾਠ;
                }

                ਜੇ (sp->sn_ਪਾਠ_hl <= 0 && ਅਗਲਾ_sp->sn_ਪਾਠ_hl > 0)
                    sattr->sat_texthl = syn_id2attr(ਅਗਲਾ_sp->sn_ਪਾਠ_hl);

                ਜੇ (sp->sn_ਸਤਰ_hl <= 0 && ਅਗਲਾ_sp->sn_ਸਤਰ_hl > 0)
                    sattr->sat_linehl = syn_id2attr(ਅਗਲਾ_sp->sn_ਸਤਰ_hl);

                ਜੇ (sp->sn_cul_hl <= 0 && ਅਗਲਾ_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(ਅਗਲਾ_sp->sn_cul_hl);

                ਜੇ (sp->sn_ਅੰਕ_hl <= 0 && ਅਗਲਾ_sp->sn_ਅੰਕ_hl > 0)
                    sattr->sat_numhl = syn_id2attr(ਅਗਲਾ_sp->sn_ਅੰਕ_hl);
            }
            ਵਾਪਸ TRUE;
        }
    }
    ਵਾਪਸ FALSE;
}

/*
 * Delete sign 'id' in group 'group' from buffer 'buf'.
 * If 'id' is zero, then delete all the signs in group 'group'. Otherwise
 * delete only the specified sign.
 * If 'group' is '*', then delete the sign in all the groups. If 'group' is
 * NULL, then delete the sign in the global group. Otherwise delete the sign in
 * the specified group.
 * Returns the line number of the deleted sign. If multiple signs are deleted,
 * then returns the line number of the last sign deleted.
 */
linenr_T
buf_delsign(buf_T *buf, // buffer sign is stored in
            linenr_T atlnum, // sign at this line, 0 - at any line
            ਪੂਰਨ id, // sign id
            char_u *ਸਮੂਹ) // sign group
{
    buf->b_sign_finger = ਨਿਰਾ; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_ਇੰਦਰਾਜ_T **lastp = &buf->b_signlist;
    sign_ਇੰਦਰਾਜ_T *ਅਗਲਾ = ਨਿਰਾ; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    ਲਈ (sign_ਇੰਦਰਾਜ_T *sign = buf->b_signlist; sign != ਨਿਰਾ; sign = ਅਗਲਾ)
    {
        ਅਗਲਾ = sign->se_ਅਗਲਾ;

        ਜੇ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_ਵਿੱਚ_ਸਮੂਹ(sign, ਸਮੂਹ))
        {
            *lastp = ਅਗਲਾ;
            ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
                ਅਗਲਾ->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ਜੇ (sign->se_ਸਮੂਹ != ਨਿਰਾ)
                sign_ਸਮੂਹ_unref(sign->se_ਸਮੂਹ->sg_name);

            vim_free(sign);
            redraw_buf_ਸਤਰ_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ਜੇ (ਸਮੂਹ == ਨਿਰਾ || (*ਸਮੂਹ != '*' && id != 0) ||
                (*ਸਮੂਹ == '*' && atlnum != 0))
                ਰੋਕੋ;
        }
        ਨਹੀਂਤਾਂ
        {
            lastp = &sign->se_ਅਗਲਾ;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ਜੇ (buf->b_signlist == ਨਿਰਾ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        ਬਦਲਿਆ_ਸਤਰ_abv_curs();
    }

    ਵਾਪਸ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
ਪੂਰਨ
buf_findsign(buf_T *buf, // buffer to store sign in
             ਪੂਰਨ id, // sign ID
             char_u *ਸਮੂਹ) // sign group
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
        ਜੇ (sign->se_id == id && sign_ਵਿੱਚ_ਸਮੂਹ(sign, ਸਮੂਹ))
            ਵਾਪਸ sign->se_lnum;

    ਵਾਪਸ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ਸਥਿਰ sign_ਇੰਦਰਾਜ_T *
buf_getsign_at_ਸਤਰ(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ਜੇ (sign->se_lnum > lnum)
            ਰੋਕੋ;

        ਜੇ (sign->se_lnum == lnum && sign_ਵਿੱਚ_ਸਮੂਹ(sign, groupname))
            ਵਾਪਸ sign;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
ਪੂਰਨ
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_ਇੰਦਰਾਜ_T *sign = buf_getsign_at_ਸਤਰ(buf, lnum, groupname);
    ਜੇ (sign != ਨਿਰਾ)
        ਵਾਪਸ sign->se_id;

    ਵਾਪਸ 0;
}

# ਜੇ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
ਪੂਰਨ
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    ਪੂਰਨ typenr) // sign type number
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ਜੇ (sign->se_lnum > lnum)
            ਰੋਕੋ;

        ਜੇ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ਵਾਪਸ sign->se_id;
    }

    ਵਾਪਸ 0;
}

#  ਜੇ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
ਪੂਰਨ
buf_signcount(buf_T *buf, linenr_T lnum)
{
    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in the signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ਜੇ (sign->se_lnum > lnum)
            ਰੋਕੋ;

        ਜੇ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ਨਿਰਾ)
            ਗਿਣਤੀ_2++;
    }

    ਵਾਪਸ ਗਿਣਤੀ_2;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
ਖਾਲੀ
buf_delete_signs(buf_T *buf, char_u *ਸਮੂਹ)
{
    buf->b_sign_finger = ਨਿਰਾ; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ਜੇ (buf->b_signlist != ਨਿਰਾ && curwin != ਨਿਰਾ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        ਬਦਲਿਆ_ਸਤਰ_abv_curs();
    }

    // pointer to pointer to current sign
    sign_ਇੰਦਰਾਜ_T **lastp = &buf->b_signlist;
    sign_ਇੰਦਰਾਜ_T *ਅਗਲਾ = ਨਿਰਾ;

    ਲਈ (sign_ਇੰਦਰਾਜ_T *sign = buf->b_signlist; sign != ਨਿਰਾ; sign = ਅਗਲਾ)
    {
        ਅਗਲਾ = sign->se_ਅਗਲਾ;
        ਜੇ (sign_ਵਿੱਚ_ਸਮੂਹ(sign, ਸਮੂਹ))
        {
            *lastp = ਅਗਲਾ;

            ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
                ਅਗਲਾ->se_prev = sign->se_prev;

            ਜੇ (sign->se_ਸਮੂਹ != ਨਿਰਾ)
                sign_ਸਮੂਹ_unref(sign->se_ਸਮੂਹ->sg_name);

            vim_free(sign);
        }
        ਨਹੀਂਤਾਂ
        {
            lastp = &sign->se_ਅਗਲਾ;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ਸਥਿਰ ਖਾਲੀ
sign_list_placed(buf_T *rbuf, char_u *sign_ਸਮੂਹ)
{
    ਅੱਖਰ lbuf[ਸੁਨੇਹਾ_BUF_LEN];
    ਅੱਖਰ ਸਮੂਹ[ਸੁਨੇਹਾ_BUF_LEN];

    ਸੁਨੇਹਾ_puts_title(_("\n--- Signs ---"));
    ਸੁਨੇਹਾ_putchar('\n');

    buf_T *buf = (rbuf == ਨਿਰਾ) ? firstbuf : rbuf;
    ਜਦੋਂ (buf != ਨਿਰਾ && !got_int)
    {
        ਜੇ (buf->b_signlist != ਨਿਰਾ)
        {
            vim_snprintf(lbuf, ਸੁਨੇਹਾ_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            ਸੁਨੇਹਾ_puts_attr(lbuf, HL_ATTR(HLF_D));
            ਸੁਨੇਹਾ_putchar('\n');
        }

        sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ;
        FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
        {
            ਜੇ (got_int)
                ਰੋਕੋ;

            ਜੇ (!sign_ਵਿੱਚ_ਸਮੂਹ(sign, sign_ਸਮੂਹ))
                ਜਾਰੀ;

            ਜੇ (sign->se_ਸਮੂਹ != ਨਿਰਾ)
                vim_snprintf(ਸਮੂਹ, ਸੁਨੇਹਾ_BUF_LEN, _("  group=%s"),
                             sign->se_ਸਮੂਹ->sg_name);
            ਨਹੀਂਤਾਂ
                ਸਮੂਹ[0] = '\0';

            vim_snprintf(lbuf, ਸੁਨੇਹਾ_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (ਲੰਮਾ)sign->se_lnum, sign->se_id, ਸਮੂਹ,
                         sign_typenr2name(sign->se_typenr), sign->se_ਤਰਜੀਹ);

            ਸੁਨੇਹਾ_puts(lbuf);
            ਸੁਨੇਹਾ_putchar('\n');
        }

        ਜੇ (rbuf != ਨਿਰਾ)
            ਰੋਕੋ;

        buf = buf->b_ਅਗਲਾ;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
ਖਾਲੀ
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    ਲੰਮਾ        amount,
    ਲੰਮਾ        amount_after)
{
    curbuf->b_sign_finger = ਨਿਰਾ; // line changes may reorder, drop the finger
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ; // a sign in a b_signlist
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ਜੇ (sign->se_lnum < line1)
            ਜਾਰੀ;

        linenr_T new_lnum = sign->se_lnum;

        ਜੇ (sign->se_lnum <= line2)
        {
            ਜੇ (amount != MAXLNUM)
                new_lnum += amount;
        }
        ਨਹੀਂਤਾਂ ਜੇ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ਜੇ (new_lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ਸਥਿਰ ਪੂਰਨ
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *ਸਿਰਾ_cmd) // just after sign subcmd
{
    ਪੂਰਨ idx = 0;
    ਅੱਖਰ save = *ਸਿਰਾ_cmd;
    *ਸਿਰਾ_cmd = NUL;

    ਜਦੋਂ (cmds[idx] != ਨਿਰਾ && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *ਸਿਰਾ_cmd = save;
    ਵਾਪਸ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ਸਥਿਰ sign_T *
sign_ਲੱਭੋ(char_u *name, sign_T **sp_prev)
{
    ਜੇ (sp_prev != ਨਿਰਾ)
        *sp_prev = ਨਿਰਾ;

    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
    {
        ਜੇ (STRCMP(sp->sn_name, name) == 0)
            ਰੋਕੋ;

        ਜੇ (sp_prev != ਨਿਰਾ)
            *sp_prev = sp;
    }

    ਵਾਪਸ sp;
}

/*
 * Allocate a new sign
 */
ਸਥਿਰ sign_T *
alloc_new_sign(char_u *name)
{
    ਪੂਰਨ ਸ਼ੁਰੂ = ਅਗਲਾ_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(ਆਕਾਰ(sign_T), aid_sign_define_by_name);
    ਜੇ (sp == ਨਿਰਾ)
        ਵਾਪਸ ਨਿਰਾ;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = ਪਹਿਲਾ_sign;
    ਜਦੋਂ (lp != ਨਿਰਾ)
    {
        ਜੇ (lp->sn_typenr == ਅਗਲਾ_sign_typenr)
        {
            ++ਅਗਲਾ_sign_typenr;

            ਜੇ (ਅਗਲਾ_sign_typenr == MAX_TYPENR)
                ਅਗਲਾ_sign_typenr = 1;

            ਜੇ (ਅਗਲਾ_sign_typenr == ਸ਼ੁਰੂ)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ਵਾਪਸ ਨਿਰਾ;
            }

            lp = ਪਹਿਲਾ_sign; // start all over
            ਜਾਰੀ;
        }
        lp = lp->sn_ਅਗਲਾ;
    }

    sp->sn_typenr = ਅਗਲਾ_sign_typenr;

    ਜੇ (++ਅਗਲਾ_sign_typenr == MAX_TYPENR)
        ਅਗਲਾ_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ਜੇ (sp->sn_name == ਨਿਰਾ) // out of memory
    {
        vim_free(sp);
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ sp;
}

/*
 * Initialize the icon information for a new sign
 */
ਸਥਿਰ ਖਾਲੀ
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ਜੇ (gui.ਵਿੱਚ_use)
    {
        ਬਾਹਰਲਾ_flush();
        ਜੇ (sp->sn_image != ਨਿਰਾ)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_ਰਜਿਸਟਰ_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ਸਥਿਰ ਪੂਰਨ
sign_define_init_ਪਾਠ(sign_T *sp, char_u *ਪਾਠ)
{
    char_u *s = ਨਿਰਾ;
    char_u *endp = ਪਾਠ + (ਪੂਰਨ)STRLEN(ਪਾਠ);
    ਪੂਰਨ cells = 0;

    // Remove backslashes so that it is possible to use a space.
    ਲਈ (s = ਪਾਠ; s + 1 < endp; ++s)
    {
        ਜੇ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ਜੇ (has_mbyte)
    {
        ਲਈ (s = ਪਾਠ; s < endp; s += (*mb_ptr2len)(s))
        {
            ਜੇ (!vim_isprintc((*mb_ptr2char)(s)))
                ਰੋਕੋ;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ਨਹੀਂਤਾਂ
    {
        ਲਈ (s = ਪਾਠ; s < endp; ++s)
        {
            ਜੇ (!vim_isprintc(*s))
                ਰੋਕੋ;
        }
        cells = (ਪੂਰਨ)(s - ਪਾਠ);
    }

    // Currently sign text must be one or two display cells
    ਜੇ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_ਪਾਠ_str), ਪਾਠ);
        ਵਾਪਸ FAIL;
    }

    vim_free(sp->sn_ਪਾਠ);
    // Allocate one byte more if we need to pad up
    // with a space.
    ਪੂਰਨ len = (ਪੂਰਨ)(endp - ਪਾਠ + ((cells == 1) ? 1 : 0));
    sp->sn_ਪਾਠ = vim_strnsave(ਪਾਠ, len);

    // For single character sign text, pad with a space.
    ਜੇ (sp->sn_ਪਾਠ != ਨਿਰਾ && cells == 1)
        STRCPY(sp->sn_ਪਾਠ + len - 1, " ");

    ਵਾਪਸ OK;
}

/*
 * Define a new sign or update an existing sign
 */
ਪੂਰਨ
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *ਪਾਠ,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    ਪੂਰਨ prio)
{
    sign_T *sp_prev = ਨਿਰਾ;
    sign_T *sp = sign_ਲੱਭੋ(name, &sp_prev);
    ਜੇ (sp == ਨਿਰਾ)
    {
        sp = alloc_new_sign(name);
        ਜੇ (sp == ਨਿਰਾ)
            ਵਾਪਸ FAIL;

        // add the new sign to the list of signs
        ਜੇ (sp_prev == ਨਿਰਾ)
            ਪਹਿਲਾ_sign = sp;
        ਨਹੀਂਤਾਂ
            sp_prev->sn_ਅਗਲਾ = sp;
    }
    ਨਹੀਂਤਾਂ
    {
        win_T *wp = ਨਿਰਾ;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ਜੇ (wp->w_ਬਫਰ->b_signlist != ਨਿਰਾ)
                redraw_buf_later(wp->w_ਬਫਰ, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ਜੇ (icon != ਨਿਰਾ)
        sign_define_init_icon(sp, icon);

    ਜੇ (ਪਾਠ != ਨਿਰਾ && (sign_define_init_ਪਾਠ(sp, ਪਾਠ) == FAIL))
        ਵਾਪਸ FAIL;

    sp->sn_ਤਰਜੀਹ = prio;

    ਜੇ (linehl != ਨਿਰਾ)
    {
        ਜੇ (*linehl == NUL)
            sp->sn_ਸਤਰ_hl = 0;
        ਨਹੀਂਤਾਂ
            sp->sn_ਸਤਰ_hl = syn_check_ਸਮੂਹ(linehl, (ਪੂਰਨ)STRLEN(linehl));
    }

    ਜੇ (texthl != ਨਿਰਾ)
    {
        ਜੇ (*texthl == NUL)
            sp->sn_ਪਾਠ_hl = 0;
        ਨਹੀਂਤਾਂ
            sp->sn_ਪਾਠ_hl = syn_check_ਸਮੂਹ(texthl, (ਪੂਰਨ)STRLEN(texthl));
    }

    ਜੇ (culhl != ਨਿਰਾ)
    {
        ਜੇ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ਨਹੀਂਤਾਂ
            sp->sn_cul_hl = syn_check_ਸਮੂਹ(culhl, (ਪੂਰਨ)STRLEN(culhl));
    }

    ਜੇ (numhl != ਨਿਰਾ)
    {
        ਜੇ (*numhl == NUL)
            sp->sn_ਅੰਕ_hl = 0;
        ਨਹੀਂਤਾਂ
            sp->sn_ਅੰਕ_hl = syn_check_ਸਮੂਹ(numhl, (ਪੂਰਨ)STRLEN(numhl));
    }

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
ਪੂਰਨ
sign_exists_by_name(char_u *name)
{
    ਵਾਪਸ sign_ਲੱਭੋ(name, ਨਿਰਾ) != ਨਿਰਾ;
}

/*
 * Free the sign specified by 'name'.
 */
ਪੂਰਨ
sign_undefine_by_name(char_u *name, ਪੂਰਨ give_error)
{
    sign_T *sp_prev = ਨਿਰਾ;
    sign_T *sp = sign_ਲੱਭੋ(name, &sp_prev);
    ਜੇ (sp == ਨਿਰਾ)
    {
        ਜੇ (give_error)
            semsg(_(e_unknown_sign_str), name);
        ਵਾਪਸ FAIL;
    }
    sign_undefine(sp, sp_prev);

    ਵਾਪਸ OK;
}

/*
 * List the signs matching 'name'
 */
ਸਥਿਰ ਖਾਲੀ
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_ਲੱਭੋ(name, ਨਿਰਾ);
    ਜੇ (sp != ਨਿਰਾ)
        sign_list_defined(sp);
    ਨਹੀਂਤਾਂ
        semsg(_(e_unknown_sign_str), name);
}

ਸਥਿਰ ਖਾਲੀ
may_force_numberwidth_recompute(buf_T *buf, ਪੂਰਨ unplace)
{
# ਜੇ defined(FEAT_LINEBREAK)
    tabpage_T *tp = ਨਿਰਾ;
    win_T *wp = ਨਿਰਾ;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ਜੇ (wp->w_ਬਫਰ == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_ਚੌੜਾਈ < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_ਸਤਰ_ਗਿਣਤੀ = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
ਪੂਰਨ
sign_ਰੱਖੋ(ਪੂਰਨ *sign_id,
           char_u *sign_ਸਮੂਹ,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           ਪੂਰਨ prio)
{
    // Check for reserved character '*' in group name
    ਜੇ (sign_ਸਮੂਹ != ਨਿਰਾ && (*sign_ਸਮੂਹ == '*' || *sign_ਸਮੂਹ == '\0'))
        ਵਾਪਸ FAIL;

    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
    {
        ਜੇ (STRCMP(sp->sn_name, sign_name) == 0)
            ਰੋਕੋ;
    }

    ਜੇ (sp == ਨਿਰਾ)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ਵਾਪਸ FAIL;
    }

    ਜੇ (*sign_id == 0)
        *sign_id = sign_ਸਮੂਹ_get_ਅਗਲਾ_signid(buf, sign_ਸਮੂਹ);

    // Use the default priority value for this sign.
    ਜੇ (prio == -1)
        prio = (sp->sn_ਤਰਜੀਹ != -1) ? sp->sn_ਤਰਜੀਹ : SIGN_DEF_PRIO;

    ਜੇ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_ਸਮੂਹ, prio, lnum, sp->sn_typenr);
    }
    ਨਹੀਂਤਾਂ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_ਸਮੂਹ, sp->sn_typenr,
                                    prio);
    }

    ਜੇ (lnum > 0)
    {
        redraw_buf_ਸਤਰ_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ਨਹੀਂਤਾਂ
    {
        semsg(_(e_not_possible_ਮੰਜ਼ਿਲ_change_sign_str), sign_name);
        ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Unplace the specified sign
 */
ਸਥਿਰ ਪੂਰਨ
sign_unplace(ਪੂਰਨ sign_id, char_u *sign_ਸਮੂਹ, buf_T *buf, linenr_T atlnum)
{
    ਜੇ (buf->b_signlist == ਨਿਰਾ) // No signs in the buffer
        ਵਾਪਸ OK;

    ਜੇ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_ਸਮੂਹ);
    }
    ਨਹੀਂਤਾਂ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_ਸਮੂਹ);
        ਜੇ (lnum == 0)
            ਵਾਪਸ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ਜੇ (buf->b_signlist == ਨਿਰਾ)
        may_force_numberwidth_recompute(buf, TRUE);

    ਵਾਪਸ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ਸਥਿਰ ਖਾਲੀ
sign_unplace_at_cursor(char_u *groupname)
{
    ਪੂਰਨ id =
        buf_findsign_id(curwin->w_ਬਫਰ, curwin->w_cursor.lnum, groupname);

    ਜੇ (id > 0)
        sign_unplace(id, groupname, curwin->w_ਬਫਰ, curwin->w_cursor.lnum);
    ਨਹੀਂਤਾਂ
        emsg(_(e_missing_sign_ਸੰਖਿਆ));
}

/*
 * Jump to a sign.
 */
ਸਥਿਰ linenr_T
sign_jump(ਪੂਰਨ sign_id, char_u *sign_ਸਮੂਹ, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_ਸਮੂਹ);
    ਜੇ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ਵਾਪਸ -1;
    }

    // goto a sign ...
    ਜੇ (buf_jump_open_win(buf) != ਨਿਰਾ)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ਨਹੀਂਤਾਂ
    { // ... not currently in a window
        ਜੇ (buf->b_fname == ਨਿਰਾ)
        {
            emsg(_(e_cannot_jump_ਮੰਜ਼ਿਲ_ਬਫਰ_that_does_not_have_name));
            ਵਾਪਸ -1;
        }
        setpcmark();
        ਜੇ (do_ecmd(buf->b_fnum, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ਵਾਪਸ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ਵਾਪਸ lnum;
}

/*
 * ":sign define {name} ..." command
 */
ਸਥਿਰ ਖਾਲੀ
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ਨਿਰਾ;
    char_u *p = cmdline;
    char_u *icon = ਨਿਰਾ;
    char_u *ਪਾਠ = ਨਿਰਾ;
    char_u *linehl = ਨਿਰਾ;
    char_u *texthl = ਨਿਰਾ;
    char_u *culhl = ਨਿਰਾ;
    char_u *numhl = ਨਿਰਾ;
    ਪੂਰਨ prio = -1;
    ਪੂਰਨ failed = FALSE;

    // set values for a defined sign.
    ਜਦੋਂ (TRUE)
    {
        arg = skipwhite(p);
        ਜੇ (*arg == NUL)
            ਰੋਕੋ;

        p = skiptowhite_esc(arg);
        ਜੇ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            ਪਾਠ = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((ਅੱਖਰ *)arg);
        }
        ਨਹੀਂਤਾਂ
        {
            semsg(_(e_invalid_ਦਲੀਲ_str), arg);
            failed = TRUE;
            ਰੋਕੋ;
        }
    }

    ਜੇ (!failed)
        sign_define_by_name(sign_name, icon, linehl, ਪਾਠ, texthl, culhl, numhl,
                            prio);

    vim_free(icon);
    vim_free(ਪਾਠ);
    vim_free(linehl);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);
}

/*
 * ":sign place" command
 */
ਸਥਿਰ ਖਾਲੀ
sign_ਰੱਖੋ_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               ਪੂਰਨ id,
               char_u *ਸਮੂਹ,
               ਪੂਰਨ prio)
{
    ਜੇ (id <= 0)
    {
        // List signs placed in a file/buffer
        //   :sign place file={fname}
        //   :sign place group={group} file={fname}
        //   :sign place group=* file={fname}
        //   :sign place buffer={nr}
        //   :sign place group={group} buffer={nr}
        //   :sign place group=* buffer={nr}
        //   :sign place
        //   :sign place group={group}
        //   :sign place group=*
        ਜੇ (lnum >= 0 || sign_name != ਨਿਰਾ || (ਸਮੂਹ != ਨਿਰਾ && *ਸਮੂਹ == '\0'))
            emsg(_(e_invalid_ਦਲੀਲ));
        ਨਹੀਂਤਾਂ
            sign_list_placed(buf, ਸਮੂਹ);
    }
    ਨਹੀਂਤਾਂ
    {
        // Place a new sign
        ਜੇ (sign_name == ਨਿਰਾ || buf == ਨਿਰਾ ||
            (ਸਮੂਹ != ਨਿਰਾ && *ਸਮੂਹ == '\0'))
        {
            emsg(_(e_invalid_ਦਲੀਲ));
            ਵਾਪਸ;
        }

        sign_ਰੱਖੋ(&id, ਸਮੂਹ, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ਸਥਿਰ ਖਾਲੀ
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 ਪੂਰਨ id,
                 char_u *ਸਮੂਹ)
{
    ਜੇ (lnum >= 0 || sign_name != ਨਿਰਾ || (ਸਮੂਹ != ਨਿਰਾ && *ਸਮੂਹ == '\0'))
    {
        emsg(_(e_invalid_ਦਲੀਲ));
        ਵਾਪਸ;
    }

    ਜੇ (id == -2)
    {
        ਜੇ (buf != ਨਿਰਾ)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, ਸਮੂਹ, buf, 0);
        }
        ਨਹੀਂਤਾਂ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ਜੇ (buf->b_signlist != ਨਿਰਾ)
                    buf_delete_signs(buf, ਸਮੂਹ);
            }
        }
    }
    ਨਹੀਂਤਾਂ
    {
        ਜੇ (buf != ਨਿਰਾ)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, ਸਮੂਹ, buf, 0);
        }
        ਨਹੀਂਤਾਂ
        {
            ਜੇ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(ਸਮੂਹ);
            }
            ਨਹੀਂਤਾਂ
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, ਸਮੂਹ, buf, 0);
            }
        }
    }
}

/*
 * Jump to a placed sign commands:
 *   :sign jump {id} file={fname}
 *   :sign jump {id} buffer={nr}
 *   :sign jump {id} group={group} file={fname}
 *   :sign jump {id} group={group} buffer={nr}
 */
ਸਥਿਰ ਖਾਲੀ
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              ਪੂਰਨ id,
              char_u *ਸਮੂਹ)
{
    ਜੇ (sign_name == ਨਿਰਾ && ਸਮੂਹ == ਨਿਰਾ && id == -1)
    {
        emsg(_(e_ਦਲੀਲ_required));
        ਵਾਪਸ;
    }

    ਜੇ (buf == ਨਿਰਾ || (ਸਮੂਹ != ਨਿਰਾ && *ਸਮੂਹ == '\0') || lnum >= 0 ||
        sign_name != ਨਿਰਾ)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_ਦਲੀਲ));
        ਵਾਪਸ;
    }

    (ਖਾਲੀ)sign_jump(id, ਸਮੂਹ, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ਸਥਿਰ ਪੂਰਨ
parse_sign_cmd_ਦਲੀਲਾਂ(ਪੂਰਨ cmd,
                    char_u *arg,
                    char_u **sign_name,
                    ਪੂਰਨ *signid,
                    char_u **ਸਮੂਹ,
                    ਪੂਰਨ *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ਨਿਰਾ;
    ਪੂਰਨ lnum_arg = FALSE;

    // first arg could be placed sign id
    ਜੇ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ਜੇ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ਨਹੀਂਤਾਂ
        {
            arg = skipwhite(arg);
        }
    }

    ਜਦੋਂ (*arg != NUL)
    {
        ਜੇ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((ਅੱਖਰ *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ਜੇ (*signid != -1)
            {
                emsg(_(e_invalid_ਦਲੀਲ));
                ਵਾਪਸ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ਜੇ (*arg != NUL)
                *arg++ = NUL;

            ਜਦੋਂ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *ਸਮੂਹ = arg;
            arg = skiptowhite(arg);
            ਜੇ (*arg != NUL)
                *arg++ = NUL;
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((ਅੱਖਰ *)arg);
            arg = skiptowhite(arg);
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ਰੋਕੋ;
        }
        ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((ਪੂਰਨ)getdigits(&arg));

            ਜੇ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ਰੋਕੋ;
        }
        ਨਹੀਂਤਾਂ
        {
            emsg(_(e_invalid_ਦਲੀਲ));
            ਵਾਪਸ FAIL;
        }

        arg = skipwhite(arg);
    }

    ਜੇ (filename != ਨਿਰਾ && *buf == ਨਿਰਾ)
    {
        semsg(_(e_invalid_ਬਫਰ_name_str), filename);
        ਵਾਪਸ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ਜੇ (filename == ਨਿਰਾ &&
        ((cmd == SIGNCMD_ਰੱਖੋ && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_ਬਫਰ;

    ਵਾਪਸ OK;
}

/*
 * ":sign" command
 */
ਖਾਲੀ
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    ਪੂਰਨ idx = sign_cmd_idx(arg, p);
    ਜੇ (idx == SIGNCMD_ਆਖਰੀ)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ਵਾਪਸ;
    }
    arg = skipwhite(p);

    ਜੇ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ਨਿਰਾ;
        ਪੂਰਨ id = -1;
        char_u *ਸਮੂਹ = ਨਿਰਾ;
        ਪੂਰਨ prio = -1;
        buf_T *buf = ਨਿਰਾ;
        linenr_T lnum = -1;

        // Parse command line arguments
        ਜੇ (parse_sign_cmd_ਦਲੀਲਾਂ(idx, arg, &sign_name, &id, &ਸਮੂਹ, &prio, &buf,
                                &lnum) == FAIL)
            ਵਾਪਸ;

        ਜੇ (idx == SIGNCMD_ਰੱਖੋ)
            sign_ਰੱਖੋ_cmd(buf, lnum, sign_name, id, ਸਮੂਹ, prio);
        ਨਹੀਂਤਾਂ ਜੇ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, ਸਮੂਹ);
        ਨਹੀਂਤਾਂ ਜੇ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, ਸਮੂਹ);

        ਵਾਪਸ;
    }

    // Define, undefine or list signs.
    ਜੇ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        ਲਈ (sign_T *sp = ਪਹਿਲਾ_sign; sp != ਨਿਰਾ && !got_int; sp = sp->sn_ਅਗਲਾ)
            sign_list_defined(sp);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ਨਹੀਂਤਾਂ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ਜੇ (*p != NUL)
            *p++ = NUL;

        ਜਦੋਂ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ਜੇ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ਨਹੀਂਤਾਂ ਜੇ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ਨਹੀਂਤਾਂ
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
ਸਥਿਰ ਖਾਲੀ
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_ਜੋੜੋ_string(retdict, "name", sp->sn_name);

    ਜੇ (sp->sn_icon != ਨਿਰਾ)
        dict_ਜੋੜੋ_string(retdict, "icon", sp->sn_icon);

    ਜੇ (sp->sn_ਪਾਠ != ਨਿਰਾ)
        dict_ਜੋੜੋ_string(retdict, "text", sp->sn_ਪਾਠ);

    ਜੇ (sp->sn_ਤਰਜੀਹ > 0)
        dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "priority", sp->sn_ਤਰਜੀਹ);

    ਜੇ (sp->sn_ਸਤਰ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਸਤਰ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            dict_ਜੋੜੋ_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ਨਹੀਂਤਾਂ
            dict_ਜੋੜੋ_string(retdict, "linehl", p);
    }

    ਜੇ (sp->sn_ਪਾਠ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਪਾਠ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            dict_ਜੋੜੋ_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ਨਹੀਂਤਾਂ
            dict_ਜੋੜੋ_string(retdict, "texthl", p);
    }

    ਜੇ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_cul_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            dict_ਜੋੜੋ_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ਨਹੀਂਤਾਂ
            dict_ਜੋੜੋ_string(retdict, "culhl", p);
    }

    ਜੇ (sp->sn_ਅੰਕ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਅੰਕ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            dict_ਜੋੜੋ_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ਨਹੀਂਤਾਂ
            dict_ਜੋੜੋ_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ਸਥਿਰ ਖਾਲੀ
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = ਪਹਿਲਾ_sign;

    ਜੇ (name != ਨਿਰਾ)
    {
        sp = sign_ਲੱਭੋ(name, ਨਿਰਾ);
        ਜੇ (sp == ਨਿਰਾ)
            ਵਾਪਸ;
    }

    ਲਈ (; sp != ਨਿਰਾ && !got_int; sp = sp->sn_ਅਗਲਾ)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ਜੇ (dict == ਨਿਰਾ)
            ਵਾਪਸ;

        ਜੇ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ਵਾਪਸ;
        }

        sign_getinfo(sp, dict);

        ਜੇ (name != ਨਿਰਾ) // handle only the specified sign
            ਰੋਕੋ;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
ਖਾਲੀ
get_ਬਫਰ_signs(buf_T *buf, list_T *l)
{
    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ;
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ਜੇ (d != ਨਿਰਾ && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ਸਥਿਰ ਖਾਲੀ
sign_get_placed_ਵਿੱਚ_buf(buf_T *buf,
                       linenr_T lnum,
                       ਪੂਰਨ sign_id,
                       char_u *sign_ਸਮੂਹ,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ਜੇ (d == ਨਿਰਾ)
        ਵਾਪਸ;

    ਜੇ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ਵਾਪਸ;
    }

    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "bufnr", (ਲੰਮਾ)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ਜੇ (l == ਨਿਰਾ)
        ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ਵਾਪਸ;
    }

    sign_ਇੰਦਰਾਜ_T *sign = ਨਿਰਾ;
    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign)
    {
        ਜੇ (!sign_ਵਿੱਚ_ਸਮੂਹ(sign, sign_ਸਮੂਹ))
            ਜਾਰੀ;

        ਜੇ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ਜੇ (sdict != ਨਿਰਾ && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
ਸਥਿਰ ਖਾਲੀ
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                ਪੂਰਨ sign_id,
                char_u *sign_ਸਮੂਹ,
                list_T *retlist)
{
    ਜੇ (buf != ਨਿਰਾ)
    {
        sign_get_placed_ਵਿੱਚ_buf(buf, lnum, sign_id, sign_ਸਮੂਹ, retlist);
    }
    ਨਹੀਂਤਾਂ
    {
        FOR_ALL_BUFFERS(buf)
        {
            ਜੇ (buf->b_signlist != ਨਿਰਾ)
                sign_get_placed_ਵਿੱਚ_buf(buf, 0, sign_id, sign_ਸਮੂਹ, retlist);
        }
    }
}

# ਜੇ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
ਖਾਲੀ
sign_gui_started(ਖਾਲੀ)
{
    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
    {
        ਜੇ (sp->sn_icon != ਨਿਰਾ)
            sp->sn_image = gui_mch_ਰਜਿਸਟਰ_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ਸਥਿਰ ਖਾਲੀ
sign_list_defined(sign_T *sp)
{
    ਅੱਖਰ lbuf[ਸੁਨੇਹਾ_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ਜੇ (sp->sn_icon != ਨਿਰਾ)
    {
        ਸੁਨੇਹਾ_puts(" icon=");
        ਸੁਨੇਹਾ_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ਜੇ (sp->sn_image == ਨਿਰਾ)
            ਸੁਨੇਹਾ_puts(_(" (NOT FOUND)"));
# ਨਹੀਂਤਾਂ
        ਸੁਨੇਹਾ_puts(_(" (not supported)"));
# endif
    }

    ਜੇ (sp->sn_ਪਾਠ != ਨਿਰਾ)
    {
        ਸੁਨੇਹਾ_puts(" text=");
        ਸੁਨੇਹਾ_outtrans(sp->sn_ਪਾਠ);
    }

    ਜੇ (sp->sn_ਤਰਜੀਹ > 0)
    {
        vim_snprintf(lbuf, ਸੁਨੇਹਾ_BUF_LEN, " priority=%d", sp->sn_ਤਰਜੀਹ);
        ਸੁਨੇਹਾ_puts(lbuf);
    }

    ਜੇ (sp->sn_ਸਤਰ_hl > 0)
    {
        ਸੁਨੇਹਾ_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਸਤਰ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            ਸੁਨੇਹਾ_puts("NONE");
        ਨਹੀਂਤਾਂ
            ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
    }

    ਜੇ (sp->sn_ਪਾਠ_hl > 0)
    {
        ਸੁਨੇਹਾ_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਪਾਠ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            ਸੁਨੇਹਾ_puts("NONE");
        ਨਹੀਂਤਾਂ
            ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
    }

    ਜੇ (sp->sn_cul_hl > 0)
    {
        ਸੁਨੇਹਾ_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_cul_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            ਸੁਨੇਹਾ_puts("NONE");
        ਨਹੀਂਤਾਂ
            ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
    }

    ਜੇ (sp->sn_ਅੰਕ_hl > 0)
    {
        ਸੁਨੇਹਾ_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ਨਿਰਾ, sp->sn_ਅੰਕ_hl - 1, FALSE);
        ਜੇ (p == ਨਿਰਾ)
            ਸੁਨੇਹਾ_puts("NONE");
        ਨਹੀਂਤਾਂ
            ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
ਸਥਿਰ ਖਾਲੀ
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ਜੇ (sp->sn_image != ਨਿਰਾ)
    {
        ਬਾਹਰਲਾ_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_ਪਾਠ);

    ਜੇ (sp_prev == ਨਿਰਾ)
        ਪਹਿਲਾ_sign = sp->sn_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
        sp_prev->sn_ਅਗਲਾ = sp->sn_ਅਗਲਾ;

    vim_free(sp);
}

# ਜੇ defined(FEAT_SIGN_ICONS)
ਖਾਲੀ *
sign_get_image(ਪੂਰਨ typenr) // the attribute which may have a sign
{
    sign_T *sp = ਨਿਰਾ;
    FOR_ALL_SIGNS(sp)
    {
        ਜੇ (sp->sn_typenr == typenr)
            ਵਾਪਸ sp->sn_image;
    }
    ਵਾਪਸ ਨਿਰਾ;
}
# endif

/*
 * Undefine/free all signs.
 */
ਖਾਲੀ
free_signs(ਖਾਲੀ)
{
    ਜਦੋਂ (ਪਹਿਲਾ_sign != ਨਿਰਾ)
        sign_undefine(ਪਹਿਲਾ_sign, ਨਿਰਾ);
}

ਸਥਿਰ ਗਿਣਤੀ
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_ਰੱਖੋ, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_ਨਾਂ, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
ਸਥਿਰ char_u *
get_nth_sign_name(ਪੂਰਨ idx)
{
    ਪੂਰਨ current_idx = 0;
    sign_T *sp = ਨਿਰਾ;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ਜੇ (current_idx++ == idx)
            ਵਾਪਸ sp->sn_name;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ਸਥਿਰ char_u *
get_nth_sign_ਸਮੂਹ_name(ਪੂਰਨ idx)
{
    ਪੂਰਨ current_idx = 0;
    ਪੂਰਨ todo = (ਪੂਰਨ)sg_ਸਾਰਣੀ.ht_ਵਰਤਿਆ;
    hashitem_T *hi = ਨਿਰਾ;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&sg_ਸਾਰਣੀ, hi, todo)
    {
        ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
        {
            --todo;
            ਜੇ (current_idx++ == idx)
            {
                signgroup_T *ਸਮੂਹ = HI2SG(hi);
                ਵਾਪਸ ਸਮੂਹ->sg_name;
            }
        }
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਚੋਣ (expand_what)
    {
        ਹਾਲਤ EXP_SUBCMD:
            ਵਾਪਸ (char_u *)cmds[idx];
        ਹਾਲਤ EXP_DEFINE:
        {
            ਅੱਖਰ *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ਨਿਰਾ };
            ਵਾਪਸ (char_u *)define_arg[idx];
        }
        ਹਾਲਤ EXP_ਰੱਖੋ:
        {
            ਅੱਖਰ *ਰੱਖੋ_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ਨਿਰਾ };
            ਵਾਪਸ (char_u *)ਰੱਖੋ_arg[idx];
        }
        ਹਾਲਤ EXP_LIST:
        {
            ਅੱਖਰ *list_arg[] = { "group=", "file=", "buffer=", ਨਿਰਾ };
            ਵਾਪਸ (char_u *)list_arg[idx];
        }
        ਹਾਲਤ EXP_UNPLACE:
        {
            ਅੱਖਰ *unplace_arg[] = { "group=", "file=", "buffer=", ਨਿਰਾ };
            ਵਾਪਸ (char_u *)unplace_arg[idx];
        }
        ਹਾਲਤ EXP_SIGN_ਨਾਂ:
            ਵਾਪਸ get_nth_sign_name(idx);
        ਹਾਲਤ EXP_SIGN_GROUPS:
            ਵਾਪਸ get_nth_sign_ਸਮੂਹ_name(idx);
        ਮੂਲ:
            ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Handle command line completion for :sign command.
 */
ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *ਸਿਰਾ_subcmd;
    char_u *ਆਖਰੀ;
    ਪੂਰਨ cmd_idx;
    char_u *begin_subcmd_ਦਲੀਲਾਂ;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_ਨਮੂਨਾ = arg;

    ਸਿਰਾ_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ਜੇ (*ਸਿਰਾ_subcmd == NUL)
        ਵਾਪਸ;

    cmd_idx = sign_cmd_idx(arg, ਸਿਰਾ_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_ਦਲੀਲਾਂ = skipwhite(ਸਿਰਾ_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_ਦਲੀਲਾਂ;
    ਕਰੋ
    {
        p = skipwhite(p);
        ਆਖਰੀ = p;
        p = skiptowhite(p);
    }
    ਜਦੋਂ (*p != NUL);

    p = vim_strchr(ਆਖਰੀ, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ਜੇ (p == ਨਿਰਾ)
    {
        // Expand last argument name (before equal sign).
        xp->xp_ਨਮੂਨਾ = ਆਖਰੀ;
        ਚੋਣ (cmd_idx)
        {
            ਹਾਲਤ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ਰੋਕੋ;
            ਹਾਲਤ SIGNCMD_ਰੱਖੋ:
                // List placed signs
                ਜੇ (VIM_ISDIGIT(*begin_subcmd_ਦਲੀਲਾਂ))
                    //   :sign place {id} {args}...
                    expand_what = EXP_ਰੱਖੋ;
                ਨਹੀਂਤਾਂ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ਰੋਕੋ;
            ਹਾਲਤ SIGNCMD_LIST:
            ਹਾਲਤ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_ਨਾਂ;
                ਰੋਕੋ;
            ਹਾਲਤ SIGNCMD_JUMP:
            ਹਾਲਤ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ਰੋਕੋ;
            ਮੂਲ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ਨਹੀਂਤਾਂ
    {
        // Expand last argument value (after equal sign).
        xp->xp_ਨਮੂਨਾ = p + 1;
        ਚੋਣ (cmd_idx)
        {
            ਹਾਲਤ SIGNCMD_DEFINE:
                ਜੇ (STRNCMP(ਆਖਰੀ, "texthl", 6) == 0 ||
                    STRNCMP(ਆਖਰੀ, "linehl", 6) == 0 ||
                    STRNCMP(ਆਖਰੀ, "culhl", 5) == 0 ||
                    STRNCMP(ਆਖਰੀ, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ਆਖਰੀ, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ਨਹੀਂਤਾਂ
                    xp->xp_context = EXPAND_NOTHING;
                ਰੋਕੋ;
            ਹਾਲਤ SIGNCMD_ਰੱਖੋ:
                ਜੇ (STRNCMP(ਆਖਰੀ, "name", 4) == 0)
                    expand_what = EXP_SIGN_ਨਾਂ;
                ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ਆਖਰੀ, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ਆਖਰੀ, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ਨਹੀਂਤਾਂ
                    xp->xp_context = EXPAND_NOTHING;
                ਰੋਕੋ;
            ਹਾਲਤ SIGNCMD_UNPLACE:
            ਹਾਲਤ SIGNCMD_JUMP:
                ਜੇ (STRNCMP(ਆਖਰੀ, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ਆਖਰੀ, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ਨਹੀਂਤਾਂ
                    xp->xp_context = EXPAND_NOTHING;
                ਰੋਕੋ;
            ਮੂਲ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ਸਥਿਰ ਪੂਰਨ
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ਨਿਰਾ;
    char_u *icon = ਨਿਰਾ;
    char_u *linehl = ਨਿਰਾ;
    char_u *ਪਾਠ = ਨਿਰਾ;
    char_u *texthl = ਨਿਰਾ;
    char_u *culhl = ਨਿਰਾ;
    char_u *numhl = ਨਿਰਾ;
    ਪੂਰਨ prio = -1;
    ਪੂਰਨ retval = -1;

    ਜੇ (name_arg == ਨਿਰਾ && dict == ਨਿਰਾ)
        ਵਾਪਸ retval;

    ਜੇ (name_arg == ਨਿਰਾ)
        name = dict_get_string(dict, "name", TRUE);
    ਨਹੀਂਤਾਂ
        name = vim_strsave(name_arg);

    ਜੇ (name == ਨਿਰਾ || name[0] == NUL)
        ਜਾਓ cleanup;

    ਜੇ (dict != ਨਿਰਾ)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        ਪਾਠ = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_ਸੰਖਿਆ_def(dict, "priority", -1);
    }

    ਜੇ (sign_define_by_name(name, icon, linehl, ਪਾਠ, texthl, culhl, numhl,
                            prio) == OK)
        retval = 0;

cleanup:
    vim_free(name);
    vim_free(icon);
    vim_free(linehl);
    vim_free(ਪਾਠ);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);

    ਵਾਪਸ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ਸਥਿਰ ਖਾਲੀ
sign_define_ਗੁਣਜ(list_T *l, list_T *retlist)
{
    listitem_T *li = ਨਿਰਾ;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
        ਪੂਰਨ retval = -1;

        ਜੇ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ਨਿਰਾ, li->li_tv.vval.v_dict);
        ਨਹੀਂਤਾਂ
            emsg(_(e_dictionary_required));

        list_append_ਸੰਖਿਆ(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
ਖਾਲੀ
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ਜੇ (rettv_list_alloc(rettv) == FAIL)
            ਵਾਪਸ;

        sign_define_ਗੁਣਜ(argvars[0].vval.v_list, rettv->vval.v_list);
        ਵਾਪਸ;
    }

    // Define a single sign
    rettv->vval.v_ਸੰਖਿਆ = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ਜੇ (name == ਨਿਰਾ)
        ਵਾਪਸ;

    ਜੇ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ਨਿਰਾ);
}

/*
 * "sign_getdefined()" function
 */
ਖਾਲੀ
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    char_u *name = ਨਿਰਾ;
    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
ਖਾਲੀ
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ਨਿਰਾ;
    linenr_T lnum = 0;
    ਪੂਰਨ sign_id = 0;
    char_u *ਸਮੂਹ = ਨਿਰਾ;

    ਜੇ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && (check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ਜੇ (buf == ਨਿਰਾ)
            ਵਾਪਸ;

        ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
        {
            ਜੇ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ਵਾਪਸ;

            dictitem_T *di = ਨਿਰਾ;
            dict_T *dict = argvars[1].vval.v_dict;

            ਜੇ ((di = dict_ਲੱਭੋ(dict, (char_u *)"lnum", -1)) != ਨਿਰਾ)
            {
                // get signs placed at this line
                ਪੂਰਨ notanum = FALSE;
                tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &notanum);
                ਜੇ (notanum)
                    ਵਾਪਸ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ਜੇ ((di = dict_ਲੱਭੋ(dict, (char_u *)"id", -1)) != ਨਿਰਾ)
            {
                // get sign placed with this identifier
                ਪੂਰਨ notanum = FALSE;
                sign_id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &notanum);
                ਜੇ (notanum)
                    ਵਾਪਸ;
            }

            ਜੇ ((di = dict_ਲੱਭੋ(dict, (char_u *)"group", -1)) != ਨਿਰਾ)
            {
                ਸਮੂਹ = tv_get_string_chk(&di->di_tv);
                ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
                    ਵਾਪਸ;

                ਜੇ (*ਸਮੂਹ == '\0') // empty string means global group
                    ਸਮੂਹ = ਨਿਰਾ;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, ਸਮੂਹ, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
ਖਾਲੀ
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_ਸਮੂਹ = ਨਿਰਾ;
    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script() && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_ਬਫਰ_arg(argvars, 2) == FAIL))
        ਵਾਪਸ;

    ਪੂਰਨ notanum = FALSE;
    // Sign identifier
    ਪੂਰਨ sign_id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], &notanum);
    ਜੇ (notanum)
        ਵਾਪਸ;

    ਜੇ (sign_id <= 0)
    {
        emsg(_(e_invalid_ਦਲੀਲ));
        ਵਾਪਸ;
    }

    // Sign group
    sign_ਸਮੂਹ = tv_get_string_chk(&argvars[1]);
    ਜੇ (sign_ਸਮੂਹ == ਨਿਰਾ)
        ਵਾਪਸ;

    ਜੇ (sign_ਸਮੂਹ[0] == '\0')
    {
        sign_ਸਮੂਹ = ਨਿਰਾ; // global sign group
    }
    ਨਹੀਂਤਾਂ
    {
        sign_ਸਮੂਹ = vim_strsave(sign_ਸਮੂਹ);
        ਜੇ (sign_ਸਮੂਹ == ਨਿਰਾ)
            ਵਾਪਸ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ਜੇ (buf == ਨਿਰਾ)
        ਜਾਓ cleanup;

    rettv->vval.v_ਸੰਖਿਆ = sign_jump(sign_id, sign_ਸਮੂਹ, buf);

cleanup:
    vim_free(sign_ਸਮੂਹ);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ਸਥਿਰ ਪੂਰਨ
sign_ਰੱਖੋ_from_dict(typval_T *id_tv,
                     typval_T *ਸਮੂਹ_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    ਪੂਰਨ sign_id = 0;
    char_u *ਸਮੂਹ = ਨਿਰਾ;
    char_u *sign_name = ਨਿਰਾ;
    buf_T *buf = ਨਿਰਾ;
    dictitem_T *di = ਨਿਰਾ;
    linenr_T lnum = 0;
    ਪੂਰਨ prio = -1;
    ਪੂਰਨ ret_sign_id = -1;

    // sign identifier
    ਜੇ (id_tv == ਨਿਰਾ)
    {
        di = dict_ਲੱਭੋ(dict, (char_u *)"id", -1);
        ਜੇ (di != ਨਿਰਾ)
            id_tv = &di->di_tv;
    }

    ਜੇ (id_tv == ਨਿਰਾ)
    {
        sign_id = 0;
    }
    ਨਹੀਂਤਾਂ
    {
        ਪੂਰਨ notanum = FALSE;
        sign_id = tv_get_ਸੰਖਿਆ_chk(id_tv, &notanum);

        ਜੇ (notanum)
            ਵਾਪਸ -1;

        ਜੇ (sign_id < 0)
        {
            emsg(_(e_invalid_ਦਲੀਲ));
            ਵਾਪਸ -1;
        }
    }

    // sign group
    ਜੇ (ਸਮੂਹ_tv == ਨਿਰਾ)
    {
        di = dict_ਲੱਭੋ(dict, (char_u *)"group", -1);
        ਜੇ (di != ਨਿਰਾ)
            ਸਮੂਹ_tv = &di->di_tv;
    }

    ਜੇ (ਸਮੂਹ_tv == ਨਿਰਾ)
    {
        ਸਮੂਹ = ਨਿਰਾ; // global group
    }
    ਨਹੀਂਤਾਂ
    {
        ਸਮੂਹ = tv_get_string_chk(ਸਮੂਹ_tv);
        ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
            ਜਾਓ cleanup;

        ਜੇ (ਸਮੂਹ[0] == '\0') // global sign group
        {
            ਸਮੂਹ = ਨਿਰਾ;
        }
        ਨਹੀਂਤਾਂ
        {
            ਸਮੂਹ = vim_strsave(ਸਮੂਹ);
            ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
                ਵਾਪਸ -1;
        }
    }

    // sign name
    ਜੇ (name_tv == ਨਿਰਾ)
    {
        di = dict_ਲੱਭੋ(dict, (char_u *)"name", -1);
        ਜੇ (di != ਨਿਰਾ)
            name_tv = &di->di_tv;
    }

    ਜੇ (name_tv == ਨਿਰਾ)
        ਜਾਓ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ਜੇ (sign_name == ਨਿਰਾ)
        ਜਾਓ cleanup;

    // buffer to place the sign
    ਜੇ (buf_tv == ਨਿਰਾ)
    {
        di = dict_ਲੱਭੋ(dict, (char_u *)"buffer", -1);
        ਜੇ (di != ਨਿਰਾ)
            buf_tv = &di->di_tv;
    }

    ਜੇ (buf_tv == ਨਿਰਾ)
        ਜਾਓ cleanup;

    buf = get_buf_arg(buf_tv);
    ਜੇ (buf == ਨਿਰਾ)
        ਜਾਓ cleanup;

    // line number of the sign
    di = dict_ਲੱਭੋ(dict, (char_u *)"lnum", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ਜੇ (lnum <= 0)
        {
            emsg(_(e_invalid_ਦਲੀਲ));
            ਜਾਓ cleanup;
        }
    }

    // sign priority
    di = dict_ਲੱਭੋ(dict, (char_u *)"priority", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
        ਪੂਰਨ notanum = FALSE;
        prio = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &notanum);
        ਜੇ (notanum)
            ਜਾਓ cleanup;
    }

    ਜੇ (sign_ਰੱਖੋ(&sign_id, ਸਮੂਹ, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(ਸਮੂਹ);

    ਵਾਪਸ ret_sign_id;
}

/*
 * "sign_place()" function
 */
ਖਾਲੀ
f_sign_ਰੱਖੋ(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ਨਿਰਾ;
    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script() && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_ਬਫਰ_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ਵਾਪਸ;

    ਜੇ (argvars[4].v_type != VAR_UNKNOWN)
    {
        ਜੇ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ਵਾਪਸ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_ਸੰਖਿਆ = sign_ਰੱਖੋ_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
ਖਾਲੀ
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
        ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    // Process the List of sign attributes
    listitem_T *li = ਨਿਰਾ;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvars[0].vval.v_list, li)
    {
        ਪੂਰਨ sign_id = -1;

        ਜੇ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_ਰੱਖੋ_from_dict(ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                           li->li_tv.vval.v_dict);
        ਨਹੀਂਤਾਂ
            emsg(_(e_dictionary_required));

        list_append_ਸੰਖਿਆ(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ਸਥਿਰ ਖਾਲੀ
sign_undefine_ਗੁਣਜ(list_T *l, list_T *retlist)
{
    listitem_T *li = ਨਿਰਾ;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
        ਪੂਰਨ retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ਜੇ (name != ਨਿਰਾ && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_ਸੰਖਿਆ(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
ਖਾਲੀ
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ਜੇ (rettv_list_alloc(rettv) == FAIL)
            ਵਾਪਸ;

        sign_undefine_ਗੁਣਜ(argvars[0].vval.v_list, rettv->vval.v_list);
        ਵਾਪਸ;
    }

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ਜੇ (name == ਨਿਰਾ)
            ਵਾਪਸ;

        ਜੇ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_ਸੰਖਿਆ = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ਸਥਿਰ ਪੂਰਨ
sign_unplace_from_dict(typval_T *ਸਮੂਹ_tv, dict_T *dict)
{
    ਪੂਰਨ sign_id = 0;
    buf_T *buf = ਨਿਰਾ;
    char_u *ਸਮੂਹ = ਨਿਰਾ;
    ਪੂਰਨ retval = -1;

    // sign group
    ਸਮੂਹ = (ਸਮੂਹ_tv != ਨਿਰਾ) ? tv_get_string(ਸਮੂਹ_tv)
                               : dict_get_string(dict, "group", FALSE);

    ਜੇ (ਸਮੂਹ != ਨਿਰਾ)
    {
        ਜੇ (ਸਮੂਹ[0] == '\0') // global sign group
        {
            ਸਮੂਹ = ਨਿਰਾ;
        }
        ਨਹੀਂਤਾਂ
        {
            ਸਮੂਹ = vim_strsave(ਸਮੂਹ);
            ਜੇ (ਸਮੂਹ == ਨਿਰਾ)
                ਵਾਪਸ retval;
        }
    }

    ਜੇ (dict != ਨਿਰਾ)
    {
        dictitem_T *di = dict_ਲੱਭੋ(dict, (char_u *)"buffer", -1);
        ਜੇ (di != ਨਿਰਾ)
        {
            buf = get_buf_arg(&di->di_tv);
            ਜੇ (buf == ਨਿਰਾ)
                ਜਾਓ cleanup;
        }

        ਜੇ (dict_has_ਕੁੰਜੀ(dict, "id"))
        {
            sign_id = dict_get_ਸੰਖਿਆ(dict, "id");
            ਜੇ (sign_id <= 0)
            {
                emsg(_(e_invalid_ਦਲੀਲ));
                ਜਾਓ cleanup;
            }
        }
    }

    ਜੇ (buf == ਨਿਰਾ)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ਜੇ (sign_unplace(sign_id, ਸਮੂਹ, buf, 0) != OK)
                retval = -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (sign_unplace(sign_id, ਸਮੂਹ, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(ਸਮੂਹ);

    ਵਾਪਸ retval;
}

sign_ਇੰਦਰਾਜ_T *
get_ਪਹਿਲਾ_valid_sign(win_T *wp)
{
    sign_ਇੰਦਰਾਜ_T *sign = wp->w_ਬਫਰ->b_signlist;

# ifdef FEAT_PROP_POPUP
    ਜਦੋਂ (sign != ਨਿਰਾ && !sign_ਸਮੂਹ_for_ਖਿੜਕੀ(sign, wp))
        sign = sign->se_ਅਗਲਾ;
# endif
    ਵਾਪਸ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
ਪੂਰਨ
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ਜੇ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ਵਾਪਸ get_ਪਹਿਲਾ_valid_sign(wp) != ਨਿਰਾ && !wp->w_p_nu && !wp->w_p_rnu;

    ਜੇ (*wp->w_p_scl == 'n')
        ਵਾਪਸ FALSE;

    ਜੇ (*wp->w_p_scl == 'y')
        ਵਾਪਸ TRUE;

    ਵਾਪਸ (get_ਪਹਿਲਾ_valid_sign(wp) != ਨਿਰਾ
# ifdef FEAT_NETBEANS_INTG
            || wp->w_ਬਫਰ->b_has_sign_ਕਾਲਮ
# endif
    );
}

/*
 * "sign_unplace()" function
 */
ਖਾਲੀ
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ਨਿਰਾ;
    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_ਸੰਖਿਆ = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
ਖਾਲੀ
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
        ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
        ਵਾਪਸ;

    listitem_T *li = ਨਿਰਾ;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvars[0].vval.v_list, li)
    {
        ਪੂਰਨ retval = -1;

        ਜੇ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ਨਿਰਾ, li->li_tv.vval.v_dict);
        ਨਹੀਂਤਾਂ
            emsg(_(e_dictionary_required));

        list_append_ਸੰਖਿਆ(rettv->vval.v_list, retval);
    }
}

#ਜੇ_ਅੰਤ // FEAT_SIGNS
