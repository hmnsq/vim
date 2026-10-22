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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sign sign_T;

ರಚನೆ sign
{
    sign_T *sn_ಮುಂದಿನ; // next sign in list
    ಪೂರ್ಣಾಂಕ sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    ಶೂನ್ಯ *sn_image; // icon image
# endif
    char_u *sn_ಪಠ್ಯ; // text used instead of pixmap
    ಪೂರ್ಣಾಂಕ sn_ಸಾಲು_hl; // highlight ID for line
    ಪೂರ್ಣಾಂಕ sn_ಪಠ್ಯ_hl; // highlight ID for text
    ಪೂರ್ಣಾಂಕ sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    ಪೂರ್ಣಾಂಕ sn_ಅಂಕೆ_hl; // highlight ID for line number
    ಪೂರ್ಣಾಂಕ sn_ಆದ್ಯತೆ; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ಸ್ಥಿರ sign_T *ಮೊದಲ_sign = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_sign_typenr = 1;

ಸ್ಥಿರ ಶೂನ್ಯ sign_list_defined(sign_T *sp);
ಸ್ಥಿರ ಶೂನ್ಯ sign_undefine(sign_T *sp, sign_T *sp_prev);

ಸ್ಥಿರ ಅಕ್ಷರ *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_ಇರಿಸು 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        ಖಾಲಿ
# define SIGNCMD_ಕೊನೆಯ 6
};

# define FOR_ALL_SIGNS(sp) \
     ಗಾಗಿ ((sp) = ಮೊದಲ_sign; (sp) != ಖಾಲಿ; (sp) = (sp)->sn_ಮುಂದಿನ)

ಸ್ಥಿರ hashtab_T sg_ಕೋಷ್ಟಕ; // sign group (signgroup_T) hashtable
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
ಶೂನ್ಯ
init_signs(ಶೂನ್ಯ)
{
    hash_init(&sg_ಕೋಷ್ಟಕ); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ಸ್ಥಿರ signgroup_T *
sign_ಗುಂಪು_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_ಕೋಷ್ಟಕ, groupname, hash);
    signgroup_T *ಗುಂಪು = ಖಾಲಿ;

    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
        // new group
        ಗುಂಪು = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        STRCPY(ಗುಂಪು->sg_name, groupname);
        ಗುಂಪು->sg_refcount = 1;
        ಗುಂಪು->sg_ಮುಂದಿನ_sign_id = 1;
        hash_ಕೂಡಿಸು_ಅಂಶ(&sg_ಕೋಷ್ಟಕ, hi, ಗುಂಪು->sg_name, hash);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // existing group
        ಗುಂಪು = HI2SG(hi);
        ಗುಂಪು->sg_refcount++;
    }

    ಹಿಂತಿರುಗಿ ಗುಂಪು;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_ಗುಂಪು_unref(char_u *groupname)
{
    hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(&sg_ಕೋಷ್ಟಕ, groupname);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
        ಹಿಂತಿರುಗಿ;

    signgroup_T *ಗುಂಪು = HI2SG(hi);
    ಗುಂಪು->sg_refcount--;
    ಆದರೆ (ಗುಂಪು->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_ಕೋಷ್ಟಕ, hi, "sign remove");
        vim_free(ಗುಂಪು);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_ಒಳ_ಗುಂಪು(sign_ನಮೂದು_T *sign, char_u *ಗುಂಪು)
{
    ಹಿಂತಿರುಗಿ ((ಗುಂಪು != ಖಾಲಿ && STRCMP(ಗುಂಪು, "*") == 0) ||
            (ಗುಂಪು == ಖಾಲಿ && sign->se_ಗುಂಪು == ಖಾಲಿ) ||
            (ಗುಂಪು != ಖಾಲಿ && sign->se_ಗುಂಪು != ಖಾಲಿ &&
             STRCMP(ಗುಂಪು, sign->se_ಗುಂಪು->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_ಗುಂಪು_for_ಕಿಟಕಿ(sign_ನಮೂದು_T *sign, win_T *wp)
{
    ಪೂರ್ಣಾಂಕ for_popup = sign->se_ಗುಂಪು != ಖಾಲಿ &&
                    STRNCMP("PopUp", sign->se_ಗುಂಪು->sg_name, 5) == 0;

    ಹಿಂತಿರುಗಿ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_ಗುಂಪು_get_ಮುಂದಿನ_signid(buf_T *buf, char_u *groupname)
{
    ಪೂರ್ಣಾಂಕ id = 1;
    signgroup_T *ಗುಂಪು = ಖಾಲಿ;
    sign_ನಮೂದು_T *sign = ಖಾಲಿ;

    ಆದರೆ (groupname != ಖಾಲಿ)
    {
        hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(&sg_ಕೋಷ್ಟಕ, groupname);
        ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
            ಹಿಂತಿರುಗಿ id;
        ಗುಂಪು = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = FALSE;
    ಆಗ (!ಸಿಕ್ಕಿತು)
    {
        ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
            id = ಮುಂದಿನ_sign_id++; // global group
        ಇಲ್ಲದಿದ್ದರೆ
            id = ಗುಂಪು->sg_ಮುಂದಿನ_sign_id++;

        // Check whether this sign is already placed in the buffer
        ಸಿಕ್ಕಿತು = TRUE;
        FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
        {
            ಆದರೆ (id == sign->se_id && sign_ಒಳ_ಗುಂಪು(sign, groupname))
            {
                ಸಿಕ್ಕಿತು = FALSE; // sign identifier is in use
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಹಿಂತಿರುಗಿ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_sign(buf_T *buf, // buffer to store sign in
            sign_ನಮೂದು_T *prev, // previous sign entry
            sign_ನಮೂದು_T *ಮುಂದಿನ, // next sign entry
            ಪೂರ್ಣಾಂಕ id, // sign ID
            char_u *ಗುಂಪು, // sign group; NULL for global group
            ಪೂರ್ಣಾಂಕ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ಪೂರ್ಣಾಂಕ typenr) // typenr of sign we are adding
{
    sign_ನಮೂದು_T *newsign =
        lalloc_id(ಗಾತ್ರ(sign_ನಮೂದು_T), FALSE, aid_ಸೇರಿಸು_sign);
    ಆದರೆ (newsign == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ಆದರೆ (ಗುಂಪು != ಖಾಲಿ)
    {
        newsign->se_ಗುಂಪು = sign_ಗುಂಪು_ref(ಗುಂಪು);
        ಆದರೆ (newsign->se_ಗುಂಪು == ಖಾಲಿ)
        {
            vim_free(newsign);
            ಹಿಂತಿರುಗಿ;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        newsign->se_ಗುಂಪು = ಖಾಲಿ;
    }

    newsign->se_ಆದ್ಯತೆ = prio;
    newsign->se_ಮುಂದಿನ = ಮುಂದಿನ;
    newsign->se_prev = prev;
    ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
        ಮುಂದಿನ->se_prev = newsign;

    ಆದರೆ (prev == ಖಾಲಿ)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ಆದರೆ (buf->b_signlist == ಖಾಲಿ)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
            buf->b_has_sign_ಸ್ತಂಭ = true;
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        prev->se_ಮುಂದಿನ = newsign;
    }

    // Remember the newly inserted sign so a following insert on a later line
    // can resume the search from here.
    buf->b_sign_finger = newsign;
}

/*
 * Insert a new sign sorted by line number and sign priority.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_ನಮೂದು_T *prev, // previous sign entry
                         ಪೂರ್ಣಾಂಕ id, // sign ID
                         char_u *ಗುಂಪು, // sign group; NULL for global group
                         ಪೂರ್ಣಾಂಕ prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         ಪೂರ್ಣಾಂಕ typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ಆಗ (prev != ಖಾಲಿ && prev->se_lnum == lnum && prev->se_ಆದ್ಯತೆ <= prio)
        prev = prev->se_prev;

    sign_ನಮೂದು_T *sign = (prev == ಖಾಲಿ) ? buf->b_signlist : prev->se_ಮುಂದಿನ;

    ಸೇರಿಸು_sign(buf, prev, sign, id, ಗುಂಪು, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ಸ್ಥಿರ sign_T *
ಕಂಡುಹಿಡಿ_sign_by_typenr(ಪೂರ್ಣಾಂಕ typenr)
{
    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
        ಆದರೆ (sp->sn_typenr == typenr)
            ಹಿಂತಿರುಗಿ sp;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the name of a sign by its typenr.
 */
ಸ್ಥಿರ char_u *
sign_typenr2name(ಪೂರ್ಣಾಂಕ typenr)
{
    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
        ಆದರೆ (sp->sn_typenr == typenr)
            ಹಿಂತಿರುಗಿ sp->sn_name;
    ಹಿಂತಿರುಗಿ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ಸ್ಥಿರ dict_T *
sign_get_info(sign_ನಮೂದು_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ಆದರೆ (d == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "id", sign->se_id);
    ಆದರೆ (sign->se_ಗುಂಪು == ಖಾಲಿ)
        dict_ಕೂಡಿಸು_string_len(d, "group", (char_u *)"", 0);
    ಇಲ್ಲದಿದ್ದರೆ
        dict_ಕೂಡಿಸು_string(d, "group", sign->se_ಗುಂಪು->sg_name);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", sign->se_lnum);
    dict_ಕೂಡಿಸು_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "priority", sign->se_ಆದ್ಯತೆ);

    ಹಿಂತಿರುಗಿ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_ವಿಂಗಡಿಸು_by_prio_on_ಸಾಲು(buf_T *buf, sign_ನಮೂದು_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ಆದರೆ ((sign->se_prev == ಖಾಲಿ || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_ಆದ್ಯತೆ > sign->se_ಆದ್ಯತೆ) &&
        (sign->se_ಮುಂದಿನ == ಖಾಲಿ || sign->se_ಮುಂದಿನ->se_lnum != sign->se_lnum ||
         sign->se_ಮುಂದಿನ->se_ಆದ್ಯತೆ < sign->se_ಆದ್ಯತೆ))
        ಹಿಂತಿರುಗಿ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_ನಮೂದು_T *p = sign;
    ಆಗ (p->se_prev != ಖಾಲಿ && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_ಆದ್ಯತೆ <= sign->se_ಆದ್ಯತೆ)
        p = p->se_prev;

    ಆದರೆ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_ಮುಂದಿನ;
        ಆಗ (p->se_ಮುಂದಿನ != ಖಾಲಿ && p->se_ಮುಂದಿನ->se_lnum == sign->se_lnum &&
               p->se_ಮುಂದಿನ->se_ಆದ್ಯತೆ > sign->se_ಆದ್ಯತೆ)
            p = p->se_ಮುಂದಿನ;
    }

    // Remove 'sign' from the list
    ಆದರೆ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_ಮುಂದಿನ;

    ಆದರೆ (sign->se_prev != ಖಾಲಿ)
        sign->se_prev->se_ಮುಂದಿನ = sign->se_ಮುಂದಿನ;

    ಆದರೆ (sign->se_ಮುಂದಿನ != ಖಾಲಿ)
        sign->se_ಮುಂದಿನ->se_prev = sign->se_prev;

    sign->se_prev = ಖಾಲಿ;
    sign->se_ಮುಂದಿನ = ಖಾಲಿ;

    // Re-insert 'sign' at the right place
    ಆದರೆ (p->se_ಆದ್ಯತೆ <= sign->se_ಆದ್ಯತೆ)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_ಮುಂದಿನ = p;
        p->se_prev = sign;
        ಆದರೆ (sign->se_prev != ಖಾಲಿ)
            sign->se_prev->se_ಮುಂದಿನ = sign;

        ಆದರೆ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_ಮುಂದಿನ = p->se_ಮುಂದಿನ;
        p->se_ಮುಂದಿನ = sign;
        ಆದರೆ (sign->se_ಮುಂದಿನ != ಖಾಲಿ)
            sign->se_ಮುಂದಿನ->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
buf_addsign(buf_T *buf, // buffer to store sign in
            ಪೂರ್ಣಾಂಕ id, // sign ID
            char_u *groupname, // sign group
            ಪೂರ್ಣಾಂಕ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ಪೂರ್ಣಾಂಕ typenr) // typenr of sign we are adding
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    sign_ನಮೂದು_T *prev = ಖಾಲಿ; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ಆದರೆ (buf->b_sign_finger != ಖಾಲಿ && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    ಗಾಗಿ (sign = (prev == ಖಾಲಿ) ? buf->b_signlist : prev->se_ಮುಂದಿನ; sign != ಖಾಲಿ;
         sign = sign->se_ಮುಂದಿನ)
    {
        ಆದರೆ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_ಒಳ_ಗುಂಪು(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_ಆದ್ಯತೆ = prio;
            sign_ವಿಂಗಡಿಸು_by_prio_on_ಸಾಲು(buf, sign);
            ಹಿಂತಿರುಗಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum < sign->se_lnum)
        {
            ಸೇರಿಸು_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ಹಿಂತಿರುಗಿ;
        }
        prev = sign;
    }

    ಸೇರಿಸು_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
ಸ್ಥಿರ linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     ಪೂರ್ಣಾಂಕ markId, // sign ID
                     char_u *ಗುಂಪು, // sign group
                     ಪೂರ್ಣಾಂಕ typenr, // typenr of sign we are adding
                     ಪೂರ್ಣಾಂಕ prio) // sign priority
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        ಆದರೆ (sign->se_id == markId && sign_ಒಳ_ಗುಂಪು(sign, ಗುಂಪು))
        {
            sign->se_typenr = typenr;
            sign->se_ಆದ್ಯತೆ = prio;
            sign_ವಿಂಗಡಿಸು_by_prio_on_ಸಾಲು(buf, sign);
            ಹಿಂತಿರುಗಿ sign->se_lnum;
        }
    }

    ಹಿಂತಿರುಗಿ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
ಪೂರ್ಣಾಂಕ
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_ಸೂಚಕ(sattr);

    buf_T *buf = wp->w_ಬಫರ್;
    sign_ನಮೂದು_T *sign = ಖಾಲಿ;
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಆದರೆ (sign->se_lnum > lnum)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_ಗುಂಪು_for_ಕಿಟಕಿ(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = ಕಂಡುಹಿಡಿ_sign_by_typenr(sign->se_typenr);
            ಆದರೆ (sp == ಖಾಲಿ)
                ಹಿಂತಿರುಗಿ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_ಪಠ್ಯ = sp->sn_ಪಠ್ಯ;

            ಆದರೆ (sattr->sat_ಪಠ್ಯ != ಖಾಲಿ && sp->sn_ಪಠ್ಯ_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_ಪಠ್ಯ_hl);

            ಆದರೆ (sp->sn_ಸಾಲು_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_ಸಾಲು_hl);

            ಆದರೆ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ಆದರೆ (sp->sn_ಅಂಕೆ_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_ಅಂಕೆ_hl);

            sattr->sat_ಆದ್ಯತೆ = sign->se_ಆದ್ಯತೆ;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ಆದರೆ (sign->se_ಮುಂದಿನ != ಖಾಲಿ &&
                sign->se_ಮುಂದಿನ->se_ಆದ್ಯತೆ == sign->se_ಆದ್ಯತೆ &&
                sign->se_ಮುಂದಿನ->se_lnum == sign->se_lnum)
            {
                sign_T *ಮುಂದಿನ_sp = ಕಂಡುಹಿಡಿ_sign_by_typenr(sign->se_ಮುಂದಿನ->se_typenr);
                ಆದರೆ (ಮುಂದಿನ_sp == ಖಾಲಿ)
                    ಹಿಂತಿರುಗಿ FALSE;

                ಆದರೆ (sattr->sat_icon == ಖಾಲಿ && sattr->sat_ಪಠ್ಯ == ಖಾಲಿ)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = ಮುಂದಿನ_sp->sn_image;
# endif
                    sattr->sat_ಪಠ್ಯ = ಮುಂದಿನ_sp->sn_ಪಠ್ಯ;
                }

                ಆದರೆ (sp->sn_ಪಠ್ಯ_hl <= 0 && ಮುಂದಿನ_sp->sn_ಪಠ್ಯ_hl > 0)
                    sattr->sat_texthl = syn_id2attr(ಮುಂದಿನ_sp->sn_ಪಠ್ಯ_hl);

                ಆದರೆ (sp->sn_ಸಾಲು_hl <= 0 && ಮುಂದಿನ_sp->sn_ಸಾಲು_hl > 0)
                    sattr->sat_linehl = syn_id2attr(ಮುಂದಿನ_sp->sn_ಸಾಲು_hl);

                ಆದರೆ (sp->sn_cul_hl <= 0 && ಮುಂದಿನ_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(ಮುಂದಿನ_sp->sn_cul_hl);

                ಆದರೆ (sp->sn_ಅಂಕೆ_hl <= 0 && ಮುಂದಿನ_sp->sn_ಅಂಕೆ_hl > 0)
                    sattr->sat_numhl = syn_id2attr(ಮುಂದಿನ_sp->sn_ಅಂಕೆ_hl);
            }
            ಹಿಂತಿರುಗಿ TRUE;
        }
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
            ಪೂರ್ಣಾಂಕ id, // sign id
            char_u *ಗುಂಪು) // sign group
{
    buf->b_sign_finger = ಖಾಲಿ; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_ನಮೂದು_T **lastp = &buf->b_signlist;
    sign_ನಮೂದು_T *ಮುಂದಿನ = ಖಾಲಿ; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    ಗಾಗಿ (sign_ನಮೂದು_T *sign = buf->b_signlist; sign != ಖಾಲಿ; sign = ಮುಂದಿನ)
    {
        ಮುಂದಿನ = sign->se_ಮುಂದಿನ;

        ಆದರೆ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_ಒಳ_ಗುಂಪು(sign, ಗುಂಪು))
        {
            *lastp = ಮುಂದಿನ;
            ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
                ಮುಂದಿನ->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ಆದರೆ (sign->se_ಗುಂಪು != ಖಾಲಿ)
                sign_ಗುಂಪು_unref(sign->se_ಗುಂಪು->sg_name);

            vim_free(sign);
            redraw_buf_ಸಾಲು_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ಆದರೆ (ಗುಂಪು == ಖಾಲಿ || (*ಗುಂಪು != '*' && id != 0) ||
                (*ಗುಂಪು == '*' && atlnum != 0))
                ನಿಲ್ಲಿಸು;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            lastp = &sign->se_ಮುಂದಿನ;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ಆದರೆ (buf->b_signlist == ಖಾಲಿ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
    }

    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
ಪೂರ್ಣಾಂಕ
buf_findsign(buf_T *buf, // buffer to store sign in
             ಪೂರ್ಣಾಂಕ id, // sign ID
             char_u *ಗುಂಪು) // sign group
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
        ಆದರೆ (sign->se_id == id && sign_ಒಳ_ಗುಂಪು(sign, ಗುಂಪು))
            ಹಿಂತಿರುಗಿ sign->se_lnum;

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ಸ್ಥಿರ sign_ನಮೂದು_T *
buf_getsign_at_ಸಾಲು(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಆದರೆ (sign->se_lnum > lnum)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (sign->se_lnum == lnum && sign_ಒಳ_ಗುಂಪು(sign, groupname))
            ಹಿಂತಿರುಗಿ sign;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
ಪೂರ್ಣಾಂಕ
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_ನಮೂದು_T *sign = buf_getsign_at_ಸಾಲು(buf, lnum, groupname);
    ಆದರೆ (sign != ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ sign->se_id;

    ಹಿಂತಿರುಗಿ 0;
}

# ಆದರೆ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
ಪೂರ್ಣಾಂಕ
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    ಪೂರ್ಣಾಂಕ typenr) // sign type number
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಆದರೆ (sign->se_lnum > lnum)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ಹಿಂತಿರುಗಿ sign->se_id;
    }

    ಹಿಂತಿರುಗಿ 0;
}

#  ಆದರೆ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
ಪೂರ್ಣಾಂಕ
buf_signcount(buf_T *buf, linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in the signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಆದರೆ (sign->se_lnum > lnum)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ಖಾಲಿ)
            ಎಣಿಕೆ_2++;
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
ಶೂನ್ಯ
buf_delete_signs(buf_T *buf, char_u *ಗುಂಪು)
{
    buf->b_sign_finger = ಖಾಲಿ; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ಆದರೆ (buf->b_signlist != ಖಾಲಿ && curwin != ಖಾಲಿ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
    }

    // pointer to pointer to current sign
    sign_ನಮೂದು_T **lastp = &buf->b_signlist;
    sign_ನಮೂದು_T *ಮುಂದಿನ = ಖಾಲಿ;

    ಗಾಗಿ (sign_ನಮೂದು_T *sign = buf->b_signlist; sign != ಖಾಲಿ; sign = ಮುಂದಿನ)
    {
        ಮುಂದಿನ = sign->se_ಮುಂದಿನ;
        ಆದರೆ (sign_ಒಳ_ಗುಂಪು(sign, ಗುಂಪು))
        {
            *lastp = ಮುಂದಿನ;

            ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
                ಮುಂದಿನ->se_prev = sign->se_prev;

            ಆದರೆ (sign->se_ಗುಂಪು != ಖಾಲಿ)
                sign_ಗುಂಪು_unref(sign->se_ಗುಂಪು->sg_name);

            vim_free(sign);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            lastp = &sign->se_ಮುಂದಿನ;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_placed(buf_T *rbuf, char_u *sign_ಗುಂಪು)
{
    ಅಕ್ಷರ lbuf[ಸಂದೇಶ_BUF_LEN];
    ಅಕ್ಷರ ಗುಂಪು[ಸಂದೇಶ_BUF_LEN];

    ಸಂದೇಶ_puts_title(_("\n--- Signs ---"));
    ಸಂದೇಶ_putchar('\n');

    buf_T *buf = (rbuf == ಖಾಲಿ) ? firstbuf : rbuf;
    ಆಗ (buf != ಖಾಲಿ && !got_int)
    {
        ಆದರೆ (buf->b_signlist != ಖಾಲಿ)
        {
            vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            ಸಂದೇಶ_puts_attr(lbuf, HL_ATTR(HLF_D));
            ಸಂದೇಶ_putchar('\n');
        }

        sign_ನಮೂದು_T *sign = ಖಾಲಿ;
        FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
        {
            ಆದರೆ (got_int)
                ನಿಲ್ಲಿಸು;

            ಆದರೆ (!sign_ಒಳ_ಗುಂಪು(sign, sign_ಗುಂಪು))
                ಮುಂದುವರಿ;

            ಆದರೆ (sign->se_ಗುಂಪು != ಖಾಲಿ)
                vim_snprintf(ಗುಂಪು, ಸಂದೇಶ_BUF_LEN, _("  group=%s"),
                             sign->se_ಗುಂಪು->sg_name);
            ಇಲ್ಲದಿದ್ದರೆ
                ಗುಂಪು[0] = '\0';

            vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (ದೀರ್ಘ)sign->se_lnum, sign->se_id, ಗುಂಪು,
                         sign_typenr2name(sign->se_typenr), sign->se_ಆದ್ಯತೆ);

            ಸಂದೇಶ_puts(lbuf);
            ಸಂದೇಶ_putchar('\n');
        }

        ಆದರೆ (rbuf != ಖಾಲಿ)
            ನಿಲ್ಲಿಸು;

        buf = buf->b_ಮುಂದಿನ;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
ಶೂನ್ಯ
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    ದೀರ್ಘ        amount,
    ದೀರ್ಘ        amount_after)
{
    curbuf->b_sign_finger = ಖಾಲಿ; // line changes may reorder, drop the finger
    sign_ನಮೂದು_T *sign = ಖಾಲಿ; // a sign in a b_signlist
    FOR_ALL_SIGNS_ಒಳ_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ಆದರೆ (sign->se_lnum < line1)
            ಮುಂದುವರಿ;

        linenr_T new_lnum = sign->se_lnum;

        ಆದರೆ (sign->se_lnum <= line2)
        {
            ಆದರೆ (amount != MAXLNUM)
                new_lnum += amount;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ಆದರೆ (new_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *ಕೊನೆ_cmd) // just after sign subcmd
{
    ಪೂರ್ಣಾಂಕ idx = 0;
    ಅಕ್ಷರ save = *ಕೊನೆ_cmd;
    *ಕೊನೆ_cmd = NUL;

    ಆಗ (cmds[idx] != ಖಾಲಿ && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *ಕೊನೆ_cmd = save;
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ಸ್ಥಿರ sign_T *
sign_ಕಂಡುಹಿಡಿ(char_u *name, sign_T **sp_prev)
{
    ಆದರೆ (sp_prev != ಖಾಲಿ)
        *sp_prev = ಖಾಲಿ;

    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
    {
        ಆದರೆ (STRCMP(sp->sn_name, name) == 0)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (sp_prev != ಖಾಲಿ)
            *sp_prev = sp;
    }

    ಹಿಂತಿರುಗಿ sp;
}

/*
 * Allocate a new sign
 */
ಸ್ಥಿರ sign_T *
alloc_new_sign(char_u *name)
{
    ಪೂರ್ಣಾಂಕ ಆರಂಭ = ಮುಂದಿನ_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(ಗಾತ್ರ(sign_T), aid_sign_define_by_name);
    ಆದರೆ (sp == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = ಮೊದಲ_sign;
    ಆಗ (lp != ಖಾಲಿ)
    {
        ಆದರೆ (lp->sn_typenr == ಮುಂದಿನ_sign_typenr)
        {
            ++ಮುಂದಿನ_sign_typenr;

            ಆದರೆ (ಮುಂದಿನ_sign_typenr == MAX_TYPENR)
                ಮುಂದಿನ_sign_typenr = 1;

            ಆದರೆ (ಮುಂದಿನ_sign_typenr == ಆರಂಭ)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }

            lp = ಮೊದಲ_sign; // start all over
            ಮುಂದುವರಿ;
        }
        lp = lp->sn_ಮುಂದಿನ;
    }

    sp->sn_typenr = ಮುಂದಿನ_sign_typenr;

    ಆದರೆ (++ಮುಂದಿನ_sign_typenr == MAX_TYPENR)
        ಮುಂದಿನ_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ಆದರೆ (sp->sn_name == ಖಾಲಿ) // out of memory
    {
        vim_free(sp);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ sp;
}

/*
 * Initialize the icon information for a new sign
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ಆದರೆ (gui.ಒಳ_use)
    {
        ಹೊರಹೋಗು_flush();
        ಆದರೆ (sp->sn_image != ಖಾಲಿ)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_ರಿಜಿಸ್ಟರ್_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_define_init_ಪಠ್ಯ(sign_T *sp, char_u *ಪಠ್ಯ)
{
    char_u *s = ಖಾಲಿ;
    char_u *endp = ಪಠ್ಯ + (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ);
    ಪೂರ್ಣಾಂಕ cells = 0;

    // Remove backslashes so that it is possible to use a space.
    ಗಾಗಿ (s = ಪಠ್ಯ; s + 1 < endp; ++s)
    {
        ಆದರೆ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ಆದರೆ (has_mbyte)
    {
        ಗಾಗಿ (s = ಪಠ್ಯ; s < endp; s += (*mb_ptr2len)(s))
        {
            ಆದರೆ (!vim_isprintc((*mb_ptr2char)(s)))
                ನಿಲ್ಲಿಸು;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಗಾಗಿ (s = ಪಠ್ಯ; s < endp; ++s)
        {
            ಆದರೆ (!vim_isprintc(*s))
                ನಿಲ್ಲಿಸು;
        }
        cells = (ಪೂರ್ಣಾಂಕ)(s - ಪಠ್ಯ);
    }

    // Currently sign text must be one or two display cells
    ಆದರೆ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_ಪಠ್ಯ_str), ಪಠ್ಯ);
        ಹಿಂತಿರುಗಿ FAIL;
    }

    vim_free(sp->sn_ಪಠ್ಯ);
    // Allocate one byte more if we need to pad up
    // with a space.
    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)(endp - ಪಠ್ಯ + ((cells == 1) ? 1 : 0));
    sp->sn_ಪಠ್ಯ = vim_strnsave(ಪಠ್ಯ, len);

    // For single character sign text, pad with a space.
    ಆದರೆ (sp->sn_ಪಠ್ಯ != ಖಾಲಿ && cells == 1)
        STRCPY(sp->sn_ಪಠ್ಯ + len - 1, " ");

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Define a new sign or update an existing sign
 */
ಪೂರ್ಣಾಂಕ
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *ಪಠ್ಯ,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    ಪೂರ್ಣಾಂಕ prio)
{
    sign_T *sp_prev = ಖಾಲಿ;
    sign_T *sp = sign_ಕಂಡುಹಿಡಿ(name, &sp_prev);
    ಆದರೆ (sp == ಖಾಲಿ)
    {
        sp = alloc_new_sign(name);
        ಆದರೆ (sp == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ FAIL;

        // add the new sign to the list of signs
        ಆದರೆ (sp_prev == ಖಾಲಿ)
            ಮೊದಲ_sign = sp;
        ಇಲ್ಲದಿದ್ದರೆ
            sp_prev->sn_ಮುಂದಿನ = sp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        win_T *wp = ಖಾಲಿ;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ಆದರೆ (wp->w_ಬಫರ್->b_signlist != ಖಾಲಿ)
                redraw_buf_later(wp->w_ಬಫರ್, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ಆದರೆ (icon != ಖಾಲಿ)
        sign_define_init_icon(sp, icon);

    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ && (sign_define_init_ಪಠ್ಯ(sp, ಪಠ್ಯ) == FAIL))
        ಹಿಂತಿರುಗಿ FAIL;

    sp->sn_ಆದ್ಯತೆ = prio;

    ಆದರೆ (linehl != ಖಾಲಿ)
    {
        ಆದರೆ (*linehl == NUL)
            sp->sn_ಸಾಲು_hl = 0;
        ಇಲ್ಲದಿದ್ದರೆ
            sp->sn_ಸಾಲು_hl = syn_check_ಗುಂಪು(linehl, (ಪೂರ್ಣಾಂಕ)STRLEN(linehl));
    }

    ಆದರೆ (texthl != ಖಾಲಿ)
    {
        ಆದರೆ (*texthl == NUL)
            sp->sn_ಪಠ್ಯ_hl = 0;
        ಇಲ್ಲದಿದ್ದರೆ
            sp->sn_ಪಠ್ಯ_hl = syn_check_ಗುಂಪು(texthl, (ಪೂರ್ಣಾಂಕ)STRLEN(texthl));
    }

    ಆದರೆ (culhl != ಖಾಲಿ)
    {
        ಆದರೆ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ಇಲ್ಲದಿದ್ದರೆ
            sp->sn_cul_hl = syn_check_ಗುಂಪು(culhl, (ಪೂರ್ಣಾಂಕ)STRLEN(culhl));
    }

    ಆದರೆ (numhl != ಖಾಲಿ)
    {
        ಆದರೆ (*numhl == NUL)
            sp->sn_ಅಂಕೆ_hl = 0;
        ಇಲ್ಲದಿದ್ದರೆ
            sp->sn_ಅಂಕೆ_hl = syn_check_ಗುಂಪು(numhl, (ಪೂರ್ಣಾಂಕ)STRLEN(numhl));
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
ಪೂರ್ಣಾಂಕ
sign_exists_by_name(char_u *name)
{
    ಹಿಂತಿರುಗಿ sign_ಕಂಡುಹಿಡಿ(name, ಖಾಲಿ) != ಖಾಲಿ;
}

/*
 * Free the sign specified by 'name'.
 */
ಪೂರ್ಣಾಂಕ
sign_undefine_by_name(char_u *name, ಪೂರ್ಣಾಂಕ give_error)
{
    sign_T *sp_prev = ಖಾಲಿ;
    sign_T *sp = sign_ಕಂಡುಹಿಡಿ(name, &sp_prev);
    ಆದರೆ (sp == ಖಾಲಿ)
    {
        ಆದರೆ (give_error)
            semsg(_(e_unknown_sign_str), name);
        ಹಿಂತಿರುಗಿ FAIL;
    }
    sign_undefine(sp, sp_prev);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * List the signs matching 'name'
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_ಕಂಡುಹಿಡಿ(name, ಖಾಲಿ);
    ಆದರೆ (sp != ಖಾಲಿ)
        sign_list_defined(sp);
    ಇಲ್ಲದಿದ್ದರೆ
        semsg(_(e_unknown_sign_str), name);
}

ಸ್ಥಿರ ಶೂನ್ಯ
may_force_numberwidth_recompute(buf_T *buf, ಪೂರ್ಣಾಂಕ unplace)
{
# ಆದರೆ defined(FEAT_LINEBREAK)
    tabpage_T *tp = ಖಾಲಿ;
    win_T *wp = ಖಾಲಿ;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ಆದರೆ (wp->w_ಬಫರ್ == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_ಅಗಲ < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
ಪೂರ್ಣಾಂಕ
sign_ಇರಿಸು(ಪೂರ್ಣಾಂಕ *sign_id,
           char_u *sign_ಗುಂಪು,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           ಪೂರ್ಣಾಂಕ prio)
{
    // Check for reserved character '*' in group name
    ಆದರೆ (sign_ಗುಂಪು != ಖಾಲಿ && (*sign_ಗುಂಪು == '*' || *sign_ಗುಂಪು == '\0'))
        ಹಿಂತಿರುಗಿ FAIL;

    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
    {
        ಆದರೆ (STRCMP(sp->sn_name, sign_name) == 0)
            ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (sp == ಖಾಲಿ)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (*sign_id == 0)
        *sign_id = sign_ಗುಂಪು_get_ಮುಂದಿನ_signid(buf, sign_ಗುಂಪು);

    // Use the default priority value for this sign.
    ಆದರೆ (prio == -1)
        prio = (sp->sn_ಆದ್ಯತೆ != -1) ? sp->sn_ಆದ್ಯತೆ : SIGN_DEF_PRIO;

    ಆದರೆ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_ಗುಂಪು, prio, lnum, sp->sn_typenr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_ಗುಂಪು, sp->sn_typenr,
                                    prio);
    }

    ಆದರೆ (lnum > 0)
    {
        redraw_buf_ಸಾಲು_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        semsg(_(e_not_possible_ಗಮ್ಯ_change_sign_str), sign_name);
        ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Unplace the specified sign
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_unplace(ಪೂರ್ಣಾಂಕ sign_id, char_u *sign_ಗುಂಪು, buf_T *buf, linenr_T atlnum)
{
    ಆದರೆ (buf->b_signlist == ಖಾಲಿ) // No signs in the buffer
        ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_ಗುಂಪು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_ಗುಂಪು);
        ಆದರೆ (lnum == 0)
            ಹಿಂತಿರುಗಿ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ಆದರೆ (buf->b_signlist == ಖಾಲಿ)
        may_force_numberwidth_recompute(buf, TRUE);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_unplace_at_cursor(char_u *groupname)
{
    ಪೂರ್ಣಾಂಕ id =
        buf_findsign_id(curwin->w_ಬಫರ್, curwin->w_cursor.lnum, groupname);

    ಆದರೆ (id > 0)
        sign_unplace(id, groupname, curwin->w_ಬಫರ್, curwin->w_cursor.lnum);
    ಇಲ್ಲದಿದ್ದರೆ
        emsg(_(e_missing_sign_ಸಂಖ್ಯೆ));
}

/*
 * Jump to a sign.
 */
ಸ್ಥಿರ linenr_T
sign_jump(ಪೂರ್ಣಾಂಕ sign_id, char_u *sign_ಗುಂಪು, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_ಗುಂಪು);
    ಆದರೆ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ಹಿಂತಿರುಗಿ -1;
    }

    // goto a sign ...
    ಆದರೆ (buf_jump_open_win(buf) != ಖಾಲಿ)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    { // ... not currently in a window
        ಆದರೆ (buf->b_fname == ಖಾಲಿ)
        {
            emsg(_(e_cannot_jump_ಗಮ್ಯ_ಬಫರ್_that_does_not_have_name));
            ಹಿಂತಿರುಗಿ -1;
        }
        setpcmark();
        ಆದರೆ (do_ecmd(buf->b_fnum, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ಹಿಂತಿರುಗಿ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * ":sign define {name} ..." command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ಖಾಲಿ;
    char_u *p = cmdline;
    char_u *icon = ಖಾಲಿ;
    char_u *ಪಠ್ಯ = ಖಾಲಿ;
    char_u *linehl = ಖಾಲಿ;
    char_u *texthl = ಖಾಲಿ;
    char_u *culhl = ಖಾಲಿ;
    char_u *numhl = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ prio = -1;
    ಪೂರ್ಣಾಂಕ failed = FALSE;

    // set values for a defined sign.
    ಆಗ (TRUE)
    {
        arg = skipwhite(p);
        ಆದರೆ (*arg == NUL)
            ನಿಲ್ಲಿಸು;

        p = skiptowhite_esc(arg);
        ಆದರೆ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            ಪಠ್ಯ = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((ಅಕ್ಷರ *)arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            semsg(_(e_invalid_ವಾದ_str), arg);
            failed = TRUE;
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (!failed)
        sign_define_by_name(sign_name, icon, linehl, ಪಠ್ಯ, texthl, culhl, numhl,
                            prio);

    vim_free(icon);
    vim_free(ಪಠ್ಯ);
    vim_free(linehl);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);
}

/*
 * ":sign place" command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_ಇರಿಸು_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               ಪೂರ್ಣಾಂಕ id,
               char_u *ಗುಂಪು,
               ಪೂರ್ಣಾಂಕ prio)
{
    ಆದರೆ (id <= 0)
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
        ಆದರೆ (lnum >= 0 || sign_name != ಖಾಲಿ || (ಗುಂಪು != ಖಾಲಿ && *ಗುಂಪು == '\0'))
            emsg(_(e_invalid_ವಾದ));
        ಇಲ್ಲದಿದ್ದರೆ
            sign_list_placed(buf, ಗುಂಪು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // Place a new sign
        ಆದರೆ (sign_name == ಖಾಲಿ || buf == ಖಾಲಿ ||
            (ಗುಂಪು != ಖಾಲಿ && *ಗುಂಪು == '\0'))
        {
            emsg(_(e_invalid_ವಾದ));
            ಹಿಂತಿರುಗಿ;
        }

        sign_ಇರಿಸು(&id, ಗುಂಪು, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 ಪೂರ್ಣಾಂಕ id,
                 char_u *ಗುಂಪು)
{
    ಆದರೆ (lnum >= 0 || sign_name != ಖಾಲಿ || (ಗುಂಪು != ಖಾಲಿ && *ಗುಂಪು == '\0'))
    {
        emsg(_(e_invalid_ವಾದ));
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (id == -2)
    {
        ಆದರೆ (buf != ಖಾಲಿ)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, ಗುಂಪು, buf, 0);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ಆದರೆ (buf->b_signlist != ಖಾಲಿ)
                    buf_delete_signs(buf, ಗುಂಪು);
            }
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಆದರೆ (buf != ಖಾಲಿ)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, ಗುಂಪು, buf, 0);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            ಆದರೆ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(ಗುಂಪು);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, ಗುಂಪು, buf, 0);
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
ಸ್ಥಿರ ಶೂನ್ಯ
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              ಪೂರ್ಣಾಂಕ id,
              char_u *ಗುಂಪು)
{
    ಆದರೆ (sign_name == ಖಾಲಿ && ಗುಂಪು == ಖಾಲಿ && id == -1)
    {
        emsg(_(e_ವಾದ_required));
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (buf == ಖಾಲಿ || (ಗುಂಪು != ಖಾಲಿ && *ಗುಂಪು == '\0') || lnum >= 0 ||
        sign_name != ಖಾಲಿ)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_ವಾದ));
        ಹಿಂತಿರುಗಿ;
    }

    (ಶೂನ್ಯ)sign_jump(id, ಗುಂಪು, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_sign_cmd_ವಾದಗಳು(ಪೂರ್ಣಾಂಕ cmd,
                    char_u *arg,
                    char_u **sign_name,
                    ಪೂರ್ಣಾಂಕ *signid,
                    char_u **ಗುಂಪು,
                    ಪೂರ್ಣಾಂಕ *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ lnum_arg = FALSE;

    // first arg could be placed sign id
    ಆದರೆ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ಆದರೆ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            arg = skipwhite(arg);
        }
    }

    ಆಗ (*arg != NUL)
    {
        ಆದರೆ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((ಅಕ್ಷರ *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ಆದರೆ (*signid != -1)
            {
                emsg(_(e_invalid_ವಾದ));
                ಹಿಂತಿರುಗಿ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ಆದರೆ (*arg != NUL)
                *arg++ = NUL;

            ಆಗ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *ಗುಂಪು = arg;
            arg = skiptowhite(arg);
            ಆದರೆ (*arg != NUL)
                *arg++ = NUL;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((ಅಕ್ಷರ *)arg);
            arg = skiptowhite(arg);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ನಿಲ್ಲಿಸು;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((ಪೂರ್ಣಾಂಕ)getdigits(&arg));

            ಆದರೆ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ನಿಲ್ಲಿಸು;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            emsg(_(e_invalid_ವಾದ));
            ಹಿಂತಿರುಗಿ FAIL;
        }

        arg = skipwhite(arg);
    }

    ಆದರೆ (filename != ಖಾಲಿ && *buf == ಖಾಲಿ)
    {
        semsg(_(e_invalid_ಬಫರ್_name_str), filename);
        ಹಿಂತಿರುಗಿ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ಆದರೆ (filename == ಖಾಲಿ &&
        ((cmd == SIGNCMD_ಇರಿಸು && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_ಬಫರ್;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":sign" command
 */
ಶೂನ್ಯ
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    ಪೂರ್ಣಾಂಕ idx = sign_cmd_idx(arg, p);
    ಆದರೆ (idx == SIGNCMD_ಕೊನೆಯ)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ಹಿಂತಿರುಗಿ;
    }
    arg = skipwhite(p);

    ಆದರೆ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ id = -1;
        char_u *ಗುಂಪು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ prio = -1;
        buf_T *buf = ಖಾಲಿ;
        linenr_T lnum = -1;

        // Parse command line arguments
        ಆದರೆ (parse_sign_cmd_ವಾದಗಳು(idx, arg, &sign_name, &id, &ಗುಂಪು, &prio, &buf,
                                &lnum) == FAIL)
            ಹಿಂತಿರುಗಿ;

        ಆದರೆ (idx == SIGNCMD_ಇರಿಸು)
            sign_ಇರಿಸು_cmd(buf, lnum, sign_name, id, ಗುಂಪು, prio);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, ಗುಂಪು);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, ಗುಂಪು);

        ಹಿಂತಿರುಗಿ;
    }

    // Define, undefine or list signs.
    ಆದರೆ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        ಗಾಗಿ (sign_T *sp = ಮೊದಲ_sign; sp != ಖಾಲಿ && !got_int; sp = sp->sn_ಮುಂದಿನ)
            sign_list_defined(sp);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ಆದರೆ (*p != NUL)
            *p++ = NUL;

        ಆಗ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ಆದರೆ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ಇಲ್ಲದಿದ್ದರೆ
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_ಕೂಡಿಸು_string(retdict, "name", sp->sn_name);

    ಆದರೆ (sp->sn_icon != ಖಾಲಿ)
        dict_ಕೂಡಿಸು_string(retdict, "icon", sp->sn_icon);

    ಆದರೆ (sp->sn_ಪಠ್ಯ != ಖಾಲಿ)
        dict_ಕೂಡಿಸು_string(retdict, "text", sp->sn_ಪಠ್ಯ);

    ಆದರೆ (sp->sn_ಆದ್ಯತೆ > 0)
        dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "priority", sp->sn_ಆದ್ಯತೆ);

    ಆದರೆ (sp->sn_ಸಾಲು_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಸಾಲು_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            dict_ಕೂಡಿಸು_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಇಲ್ಲದಿದ್ದರೆ
            dict_ಕೂಡಿಸು_string(retdict, "linehl", p);
    }

    ಆದರೆ (sp->sn_ಪಠ್ಯ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಪಠ್ಯ_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            dict_ಕೂಡಿಸು_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಇಲ್ಲದಿದ್ದರೆ
            dict_ಕೂಡಿಸು_string(retdict, "texthl", p);
    }

    ಆದರೆ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_cul_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            dict_ಕೂಡಿಸು_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಇಲ್ಲದಿದ್ದರೆ
            dict_ಕೂಡಿಸು_string(retdict, "culhl", p);
    }

    ಆದರೆ (sp->sn_ಅಂಕೆ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಅಂಕೆ_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            dict_ಕೂಡಿಸು_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಇಲ್ಲದಿದ್ದರೆ
            dict_ಕೂಡಿಸು_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = ಮೊದಲ_sign;

    ಆದರೆ (name != ಖಾಲಿ)
    {
        sp = sign_ಕಂಡುಹಿಡಿ(name, ಖಾಲಿ);
        ಆದರೆ (sp == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (; sp != ಖಾಲಿ && !got_int; sp = sp->sn_ಮುಂದಿನ)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ಆದರೆ (dict == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ;

        ಆದರೆ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ಹಿಂತಿರುಗಿ;
        }

        sign_getinfo(sp, dict);

        ಆದರೆ (name != ಖಾಲಿ) // handle only the specified sign
            ನಿಲ್ಲಿಸು;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
ಶೂನ್ಯ
get_ಬಫರ್_signs(buf_T *buf, list_T *l)
{
    sign_ನಮೂದು_T *sign = ಖಾಲಿ;
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ಆದರೆ (d != ಖಾಲಿ && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_get_placed_ಒಳ_buf(buf_T *buf,
                       linenr_T lnum,
                       ಪೂರ್ಣಾಂಕ sign_id,
                       char_u *sign_ಗುಂಪು,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ಆದರೆ (d == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ಹಿಂತಿರುಗಿ;
    }

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "bufnr", (ದೀರ್ಘ)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ಆದರೆ (l == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ಹಿಂತಿರುಗಿ;
    }

    sign_ನಮೂದು_T *sign = ಖಾಲಿ;
    FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign)
    {
        ಆದರೆ (!sign_ಒಳ_ಗುಂಪು(sign, sign_ಗುಂಪು))
            ಮುಂದುವರಿ;

        ಆದರೆ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ಆದರೆ (sdict != ಖಾಲಿ && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                ಪೂರ್ಣಾಂಕ sign_id,
                char_u *sign_ಗುಂಪು,
                list_T *retlist)
{
    ಆದರೆ (buf != ಖಾಲಿ)
    {
        sign_get_placed_ಒಳ_buf(buf, lnum, sign_id, sign_ಗುಂಪು, retlist);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        FOR_ALL_BUFFERS(buf)
        {
            ಆದರೆ (buf->b_signlist != ಖಾಲಿ)
                sign_get_placed_ಒಳ_buf(buf, 0, sign_id, sign_ಗುಂಪು, retlist);
        }
    }
}

# ಆದರೆ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
ಶೂನ್ಯ
sign_gui_started(ಶೂನ್ಯ)
{
    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
    {
        ಆದರೆ (sp->sn_icon != ಖಾಲಿ)
            sp->sn_image = gui_mch_ರಿಜಿಸ್ಟರ್_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_defined(sign_T *sp)
{
    ಅಕ್ಷರ lbuf[ಸಂದೇಶ_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ಆದರೆ (sp->sn_icon != ಖಾಲಿ)
    {
        ಸಂದೇಶ_puts(" icon=");
        ಸಂದೇಶ_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ಆದರೆ (sp->sn_image == ಖಾಲಿ)
            ಸಂದೇಶ_puts(_(" (NOT FOUND)"));
# ಇಲ್ಲದಿದ್ದರೆ
        ಸಂದೇಶ_puts(_(" (not supported)"));
# endif
    }

    ಆದರೆ (sp->sn_ಪಠ್ಯ != ಖಾಲಿ)
    {
        ಸಂದೇಶ_puts(" text=");
        ಸಂದೇಶ_outtrans(sp->sn_ಪಠ್ಯ);
    }

    ಆದರೆ (sp->sn_ಆದ್ಯತೆ > 0)
    {
        vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN, " priority=%d", sp->sn_ಆದ್ಯತೆ);
        ಸಂದೇಶ_puts(lbuf);
    }

    ಆದರೆ (sp->sn_ಸಾಲು_hl > 0)
    {
        ಸಂದೇಶ_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಸಾಲು_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            ಸಂದೇಶ_puts("NONE");
        ಇಲ್ಲದಿದ್ದರೆ
            ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
    }

    ಆದರೆ (sp->sn_ಪಠ್ಯ_hl > 0)
    {
        ಸಂದೇಶ_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಪಠ್ಯ_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            ಸಂದೇಶ_puts("NONE");
        ಇಲ್ಲದಿದ್ದರೆ
            ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
    }

    ಆದರೆ (sp->sn_cul_hl > 0)
    {
        ಸಂದೇಶ_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_cul_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            ಸಂದೇಶ_puts("NONE");
        ಇಲ್ಲದಿದ್ದರೆ
            ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
    }

    ಆದರೆ (sp->sn_ಅಂಕೆ_hl > 0)
    {
        ಸಂದೇಶ_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ಖಾಲಿ, sp->sn_ಅಂಕೆ_hl - 1, FALSE);
        ಆದರೆ (p == ಖಾಲಿ)
            ಸಂದೇಶ_puts("NONE");
        ಇಲ್ಲದಿದ್ದರೆ
            ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ಆದರೆ (sp->sn_image != ಖಾಲಿ)
    {
        ಹೊರಹೋಗು_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_ಪಠ್ಯ);

    ಆದರೆ (sp_prev == ಖಾಲಿ)
        ಮೊದಲ_sign = sp->sn_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
        sp_prev->sn_ಮುಂದಿನ = sp->sn_ಮುಂದಿನ;

    vim_free(sp);
}

# ಆದರೆ defined(FEAT_SIGN_ICONS)
ಶೂನ್ಯ *
sign_get_image(ಪೂರ್ಣಾಂಕ typenr) // the attribute which may have a sign
{
    sign_T *sp = ಖಾಲಿ;
    FOR_ALL_SIGNS(sp)
    {
        ಆದರೆ (sp->sn_typenr == typenr)
            ಹಿಂತಿರುಗಿ sp->sn_image;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
# endif

/*
 * Undefine/free all signs.
 */
ಶೂನ್ಯ
free_signs(ಶೂನ್ಯ)
{
    ಆಗ (ಮೊದಲ_sign != ಖಾಲಿ)
        sign_undefine(ಮೊದಲ_sign, ಖಾಲಿ);
}

ಸ್ಥಿರ ಎಣಿಕೆ
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_ಇರಿಸು, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_ಹೆಸರುಗಳು, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
ಸ್ಥಿರ char_u *
get_nth_sign_name(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ current_idx = 0;
    sign_T *sp = ಖಾಲಿ;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ಆದರೆ (current_idx++ == idx)
            ಹಿಂತಿರುಗಿ sp->sn_name;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ಸ್ಥಿರ char_u *
get_nth_sign_ಗುಂಪು_name(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ current_idx = 0;
    ಪೂರ್ಣಾಂಕ todo = (ಪೂರ್ಣಾಂಕ)sg_ಕೋಷ್ಟಕ.ht_ಬಳಸಿದ;
    hashitem_T *hi = ಖಾಲಿ;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&sg_ಕೋಷ್ಟಕ, hi, todo)
    {
        ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
        {
            --todo;
            ಆದರೆ (current_idx++ == idx)
            {
                signgroup_T *ಗುಂಪು = HI2SG(hi);
                ಹಿಂತಿರುಗಿ ಗುಂಪು->sg_name;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆಯ್ಕೆ (expand_what)
    {
        ಸ್ಥಿತಿ EXP_SUBCMD:
            ಹಿಂತಿರುಗಿ (char_u *)cmds[idx];
        ಸ್ಥಿತಿ EXP_DEFINE:
        {
            ಅಕ್ಷರ *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ಖಾಲಿ };
            ಹಿಂತಿರುಗಿ (char_u *)define_arg[idx];
        }
        ಸ್ಥಿತಿ EXP_ಇರಿಸು:
        {
            ಅಕ್ಷರ *ಇರಿಸು_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ಖಾಲಿ };
            ಹಿಂತಿರುಗಿ (char_u *)ಇರಿಸು_arg[idx];
        }
        ಸ್ಥಿತಿ EXP_LIST:
        {
            ಅಕ್ಷರ *list_arg[] = { "group=", "file=", "buffer=", ಖಾಲಿ };
            ಹಿಂತಿರುಗಿ (char_u *)list_arg[idx];
        }
        ಸ್ಥಿತಿ EXP_UNPLACE:
        {
            ಅಕ್ಷರ *unplace_arg[] = { "group=", "file=", "buffer=", ಖಾಲಿ };
            ಹಿಂತಿರುಗಿ (char_u *)unplace_arg[idx];
        }
        ಸ್ಥಿತಿ EXP_SIGN_ಹೆಸರುಗಳು:
            ಹಿಂತಿರುಗಿ get_nth_sign_name(idx);
        ಸ್ಥಿತಿ EXP_SIGN_GROUPS:
            ಹಿಂತಿರುಗಿ get_nth_sign_ಗುಂಪು_name(idx);
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * Handle command line completion for :sign command.
 */
ಶೂನ್ಯ
ಗಣ_context_ಒಳ_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *ಕೊನೆ_subcmd;
    char_u *ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ cmd_idx;
    char_u *begin_subcmd_ವಾದಗಳು;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_ನಮೂನೆ = arg;

    ಕೊನೆ_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ಆದರೆ (*ಕೊನೆ_subcmd == NUL)
        ಹಿಂತಿರುಗಿ;

    cmd_idx = sign_cmd_idx(arg, ಕೊನೆ_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_ವಾದಗಳು = skipwhite(ಕೊನೆ_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_ವಾದಗಳು;
    ಮಾಡು
    {
        p = skipwhite(p);
        ಕೊನೆಯ = p;
        p = skiptowhite(p);
    }
    ಆಗ (*p != NUL);

    p = vim_strchr(ಕೊನೆಯ, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ಆದರೆ (p == ಖಾಲಿ)
    {
        // Expand last argument name (before equal sign).
        xp->xp_ನಮೂನೆ = ಕೊನೆಯ;
        ಆಯ್ಕೆ (cmd_idx)
        {
            ಸ್ಥಿತಿ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ SIGNCMD_ಇರಿಸು:
                // List placed signs
                ಆದರೆ (VIM_ISDIGIT(*begin_subcmd_ವಾದಗಳು))
                    //   :sign place {id} {args}...
                    expand_what = EXP_ಇರಿಸು;
                ಇಲ್ಲದಿದ್ದರೆ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ SIGNCMD_LIST:
            ಸ್ಥಿತಿ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_ಹೆಸರುಗಳು;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ SIGNCMD_JUMP:
            ಸ್ಥಿತಿ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // Expand last argument value (after equal sign).
        xp->xp_ನಮೂನೆ = p + 1;
        ಆಯ್ಕೆ (cmd_idx)
        {
            ಸ್ಥಿತಿ SIGNCMD_DEFINE:
                ಆದರೆ (STRNCMP(ಕೊನೆಯ, "texthl", 6) == 0 ||
                    STRNCMP(ಕೊನೆಯ, "linehl", 6) == 0 ||
                    STRNCMP(ಕೊನೆಯ, "culhl", 5) == 0 ||
                    STRNCMP(ಕೊನೆಯ, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ಕೊನೆಯ, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ಇಲ್ಲದಿದ್ದರೆ
                    xp->xp_context = EXPAND_NOTHING;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ SIGNCMD_ಇರಿಸು:
                ಆದರೆ (STRNCMP(ಕೊನೆಯ, "name", 4) == 0)
                    expand_what = EXP_SIGN_ಹೆಸರುಗಳು;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ಕೊನೆಯ, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ಕೊನೆಯ, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ಇಲ್ಲದಿದ್ದರೆ
                    xp->xp_context = EXPAND_NOTHING;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ SIGNCMD_UNPLACE:
            ಸ್ಥಿತಿ SIGNCMD_JUMP:
                ಆದರೆ (STRNCMP(ಕೊನೆಯ, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ಕೊನೆಯ, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ಇಲ್ಲದಿದ್ದರೆ
                    xp->xp_context = EXPAND_NOTHING;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ಖಾಲಿ;
    char_u *icon = ಖಾಲಿ;
    char_u *linehl = ಖಾಲಿ;
    char_u *ಪಠ್ಯ = ಖಾಲಿ;
    char_u *texthl = ಖಾಲಿ;
    char_u *culhl = ಖಾಲಿ;
    char_u *numhl = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ prio = -1;
    ಪೂರ್ಣಾಂಕ retval = -1;

    ಆದರೆ (name_arg == ಖಾಲಿ && dict == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ retval;

    ಆದರೆ (name_arg == ಖಾಲಿ)
        name = dict_get_string(dict, "name", TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
        name = vim_strsave(name_arg);

    ಆದರೆ (name == ಖಾಲಿ || name[0] == NUL)
        ಹೋಗು cleanup;

    ಆದರೆ (dict != ಖಾಲಿ)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        ಪಠ್ಯ = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_ಸಂಖ್ಯೆ_def(dict, "priority", -1);
    }

    ಆದರೆ (sign_define_by_name(name, icon, linehl, ಪಠ್ಯ, texthl, culhl, numhl,
                            prio) == OK)
        retval = 0;

cleanup:
    vim_free(name);
    vim_free(icon);
    vim_free(linehl);
    vim_free(ಪಠ್ಯ);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_define_ಅಪವರ್ತ್ಯ(list_T *l, list_T *retlist)
{
    listitem_T *li = ಖಾಲಿ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
        ಪೂರ್ಣಾಂಕ retval = -1;

        ಆದರೆ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ಖಾಲಿ, li->li_tv.vval.v_dict);
        ಇಲ್ಲದಿದ್ದರೆ
            emsg(_(e_dictionary_required));

        list_append_ಸಂಖ್ಯೆ(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
ಶೂನ್ಯ
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
            ಹಿಂತಿರುಗಿ;

        sign_define_ಅಪವರ್ತ್ಯ(argvars[0].vval.v_list, rettv->vval.v_list);
        ಹಿಂತಿರುಗಿ;
    }

    // Define a single sign
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (name == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ಖಾಲಿ);
}

/*
 * "sign_getdefined()" function
 */
ಶೂನ್ಯ
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    char_u *name = ಖಾಲಿ;
    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
ಶೂನ್ಯ
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ಖಾಲಿ;
    linenr_T lnum = 0;
    ಪೂರ್ಣಾಂಕ sign_id = 0;
    char_u *ಗುಂಪು = ಖಾಲಿ;

    ಆದರೆ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && (check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ಆದರೆ (buf == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ;

        ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
        {
            ಆದರೆ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ಹಿಂತಿರುಗಿ;

            dictitem_T *di = ಖಾಲಿ;
            dict_T *dict = argvars[1].vval.v_dict;

            ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"lnum", -1)) != ಖಾಲಿ)
            {
                // get signs placed at this line
                ಪೂರ್ಣಾಂಕ notanum = FALSE;
                tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
                ಆದರೆ (notanum)
                    ಹಿಂತಿರುಗಿ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"id", -1)) != ಖಾಲಿ)
            {
                // get sign placed with this identifier
                ಪೂರ್ಣಾಂಕ notanum = FALSE;
                sign_id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
                ಆದರೆ (notanum)
                    ಹಿಂತಿರುಗಿ;
            }

            ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"group", -1)) != ಖಾಲಿ)
            {
                ಗುಂಪು = tv_get_string_chk(&di->di_tv);
                ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
                    ಹಿಂತಿರುಗಿ;

                ಆದರೆ (*ಗುಂಪು == '\0') // empty string means global group
                    ಗುಂಪು = ಖಾಲಿ;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, ಗುಂಪು, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
ಶೂನ್ಯ
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_ಗುಂಪು = ಖಾಲಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script() && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_ಬಫರ್_arg(argvars, 2) == FAIL))
        ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ notanum = FALSE;
    // Sign identifier
    ಪೂರ್ಣಾಂಕ sign_id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &notanum);
    ಆದರೆ (notanum)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (sign_id <= 0)
    {
        emsg(_(e_invalid_ವಾದ));
        ಹಿಂತಿರುಗಿ;
    }

    // Sign group
    sign_ಗುಂಪು = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (sign_ಗುಂಪು == ಖಾಲಿ)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (sign_ಗುಂಪು[0] == '\0')
    {
        sign_ಗುಂಪು = ಖಾಲಿ; // global sign group
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        sign_ಗುಂಪು = vim_strsave(sign_ಗುಂಪು);
        ಆದರೆ (sign_ಗುಂಪು == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ಆದರೆ (buf == ಖಾಲಿ)
        ಹೋಗು cleanup;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_jump(sign_id, sign_ಗುಂಪು, buf);

cleanup:
    vim_free(sign_ಗುಂಪು);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_ಇರಿಸು_from_dict(typval_T *id_tv,
                     typval_T *ಗುಂಪು_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    ಪೂರ್ಣಾಂಕ sign_id = 0;
    char_u *ಗುಂಪು = ಖಾಲಿ;
    char_u *sign_name = ಖಾಲಿ;
    buf_T *buf = ಖಾಲಿ;
    dictitem_T *di = ಖಾಲಿ;
    linenr_T lnum = 0;
    ಪೂರ್ಣಾಂಕ prio = -1;
    ಪೂರ್ಣಾಂಕ ret_sign_id = -1;

    // sign identifier
    ಆದರೆ (id_tv == ಖಾಲಿ)
    {
        di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"id", -1);
        ಆದರೆ (di != ಖಾಲಿ)
            id_tv = &di->di_tv;
    }

    ಆದರೆ (id_tv == ಖಾಲಿ)
    {
        sign_id = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಪೂರ್ಣಾಂಕ notanum = FALSE;
        sign_id = tv_get_ಸಂಖ್ಯೆ_chk(id_tv, &notanum);

        ಆದರೆ (notanum)
            ಹಿಂತಿರುಗಿ -1;

        ಆದರೆ (sign_id < 0)
        {
            emsg(_(e_invalid_ವಾದ));
            ಹಿಂತಿರುಗಿ -1;
        }
    }

    // sign group
    ಆದರೆ (ಗುಂಪು_tv == ಖಾಲಿ)
    {
        di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"group", -1);
        ಆದರೆ (di != ಖಾಲಿ)
            ಗುಂಪು_tv = &di->di_tv;
    }

    ಆದರೆ (ಗುಂಪು_tv == ಖಾಲಿ)
    {
        ಗುಂಪು = ಖಾಲಿ; // global group
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        ಗುಂಪು = tv_get_string_chk(ಗುಂಪು_tv);
        ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
            ಹೋಗು cleanup;

        ಆದರೆ (ಗುಂಪು[0] == '\0') // global sign group
        {
            ಗುಂಪು = ಖಾಲಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            ಗುಂಪು = vim_strsave(ಗುಂಪು);
            ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
                ಹಿಂತಿರುಗಿ -1;
        }
    }

    // sign name
    ಆದರೆ (name_tv == ಖಾಲಿ)
    {
        di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"name", -1);
        ಆದರೆ (di != ಖಾಲಿ)
            name_tv = &di->di_tv;
    }

    ಆದರೆ (name_tv == ಖಾಲಿ)
        ಹೋಗು cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ಆದರೆ (sign_name == ಖಾಲಿ)
        ಹೋಗು cleanup;

    // buffer to place the sign
    ಆದರೆ (buf_tv == ಖಾಲಿ)
    {
        di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"buffer", -1);
        ಆದರೆ (di != ಖಾಲಿ)
            buf_tv = &di->di_tv;
    }

    ಆದರೆ (buf_tv == ಖಾಲಿ)
        ಹೋಗು cleanup;

    buf = get_buf_arg(buf_tv);
    ಆದರೆ (buf == ಖಾಲಿ)
        ಹೋಗು cleanup;

    // line number of the sign
    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"lnum", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ಆದರೆ (lnum <= 0)
        {
            emsg(_(e_invalid_ವಾದ));
            ಹೋಗು cleanup;
        }
    }

    // sign priority
    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"priority", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
        ಪೂರ್ಣಾಂಕ notanum = FALSE;
        prio = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
        ಆದರೆ (notanum)
            ಹೋಗು cleanup;
    }

    ಆದರೆ (sign_ಇರಿಸು(&sign_id, ಗುಂಪು, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(ಗುಂಪು);

    ಹಿಂತಿರುಗಿ ret_sign_id;
}

/*
 * "sign_place()" function
 */
ಶೂನ್ಯ
f_sign_ಇರಿಸು(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ಖಾಲಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script() && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_ಬಫರ್_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[4].v_type != VAR_UNKNOWN)
    {
        ಆದರೆ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ಹಿಂತಿರುಗಿ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_ಇರಿಸು_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
ಶೂನ್ಯ
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    // Process the List of sign attributes
    listitem_T *li = ಖಾಲಿ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[0].vval.v_list, li)
    {
        ಪೂರ್ಣಾಂಕ sign_id = -1;

        ಆದರೆ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_ಇರಿಸು_from_dict(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                           li->li_tv.vval.v_dict);
        ಇಲ್ಲದಿದ್ದರೆ
            emsg(_(e_dictionary_required));

        list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_undefine_ಅಪವರ್ತ್ಯ(list_T *l, list_T *retlist)
{
    listitem_T *li = ಖಾಲಿ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
        ಪೂರ್ಣಾಂಕ retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ಆದರೆ (name != ಖಾಲಿ && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_ಸಂಖ್ಯೆ(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
ಶೂನ್ಯ
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
            ಹಿಂತಿರುಗಿ;

        sign_undefine_ಅಪವರ್ತ್ಯ(argvars[0].vval.v_list, rettv->vval.v_list);
        ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ಆದರೆ (name == ಖಾಲಿ)
            ಹಿಂತಿರುಗಿ;

        ಆದರೆ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sign_unplace_from_dict(typval_T *ಗುಂಪು_tv, dict_T *dict)
{
    ಪೂರ್ಣಾಂಕ sign_id = 0;
    buf_T *buf = ಖಾಲಿ;
    char_u *ಗುಂಪು = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ retval = -1;

    // sign group
    ಗುಂಪು = (ಗುಂಪು_tv != ಖಾಲಿ) ? tv_get_string(ಗುಂಪು_tv)
                               : dict_get_string(dict, "group", FALSE);

    ಆದರೆ (ಗುಂಪು != ಖಾಲಿ)
    {
        ಆದರೆ (ಗುಂಪು[0] == '\0') // global sign group
        {
            ಗುಂಪು = ಖಾಲಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            ಗುಂಪು = vim_strsave(ಗುಂಪು);
            ಆದರೆ (ಗುಂಪು == ಖಾಲಿ)
                ಹಿಂತಿರುಗಿ retval;
        }
    }

    ಆದರೆ (dict != ಖಾಲಿ)
    {
        dictitem_T *di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"buffer", -1);
        ಆದರೆ (di != ಖಾಲಿ)
        {
            buf = get_buf_arg(&di->di_tv);
            ಆದರೆ (buf == ಖಾಲಿ)
                ಹೋಗು cleanup;
        }

        ಆದರೆ (dict_has_ಕೀಲಿ(dict, "id"))
        {
            sign_id = dict_get_ಸಂಖ್ಯೆ(dict, "id");
            ಆದರೆ (sign_id <= 0)
            {
                emsg(_(e_invalid_ವಾದ));
                ಹೋಗು cleanup;
            }
        }
    }

    ಆದರೆ (buf == ಖಾಲಿ)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ಆದರೆ (sign_unplace(sign_id, ಗುಂಪು, buf, 0) != OK)
                retval = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sign_unplace(sign_id, ಗುಂಪು, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(ಗುಂಪು);

    ಹಿಂತಿರುಗಿ retval;
}

sign_ನಮೂದು_T *
get_ಮೊದಲ_valid_sign(win_T *wp)
{
    sign_ನಮೂದು_T *sign = wp->w_ಬಫರ್->b_signlist;

# ifdef FEAT_PROP_POPUP
    ಆಗ (sign != ಖಾಲಿ && !sign_ಗುಂಪು_for_ಕಿಟಕಿ(sign, wp))
        sign = sign->se_ಮುಂದಿನ;
# endif
    ಹಿಂತಿರುಗಿ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
ಪೂರ್ಣಾಂಕ
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ಆದರೆ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ಹಿಂತಿರುಗಿ get_ಮೊದಲ_valid_sign(wp) != ಖಾಲಿ && !wp->w_p_nu && !wp->w_p_rnu;

    ಆದರೆ (*wp->w_p_scl == 'n')
        ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (*wp->w_p_scl == 'y')
        ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ (get_ಮೊದಲ_valid_sign(wp) != ಖಾಲಿ
# ifdef FEAT_NETBEANS_INTG
            || wp->w_ಬಫರ್->b_has_sign_ಸ್ತಂಭ
# endif
    );
}

/*
 * "sign_unplace()" function
 */
ಶೂನ್ಯ
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ಖಾಲಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
ಶೂನ್ಯ
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
        ಹಿಂತಿರುಗಿ;

    listitem_T *li = ಖಾಲಿ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[0].vval.v_list, li)
    {
        ಪೂರ್ಣಾಂಕ retval = -1;

        ಆದರೆ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ಖಾಲಿ, li->li_tv.vval.v_dict);
        ಇಲ್ಲದಿದ್ದರೆ
            emsg(_(e_dictionary_required));

        list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, retval);
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SIGNS
