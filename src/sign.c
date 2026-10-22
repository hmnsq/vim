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

#ಯದಿ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ sign sign_T;

ಸಂರಚನಾ sign
{
    sign_T *sn_next; // next sign in list
    ಪೂರ್ಣಾಙ್ಕ sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    ಶೂನ್ಯ *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    ಪೂರ್ಣಾಙ್ಕ sn_ಸಾಲು_hl; // highlight ID for line
    ಪೂರ್ಣಾಙ್ಕ sn_text_hl; // highlight ID for text
    ಪೂರ್ಣಾಙ್ಕ sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    ಪೂರ್ಣಾಙ್ಕ sn_ಅಂಕೆ_hl; // highlight ID for line number
    ಪೂರ್ಣಾಙ್ಕ sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ಸ್ಥಿರ sign_T *first_sign = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_sign_typenr = 1;

ಸ್ಥಿರ ಶೂನ್ಯ sign_list_defined(sign_T *sp);
ಸ್ಥಿರ ಶೂನ್ಯ sign_undefine(sign_T *sp, sign_T *sp_prev);

ಸ್ಥಿರ ವರ್ಣ *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_PLACE 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        ರಿಕ್ತ
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     ಪ್ರತಿ ((sp) = first_sign; (sp) != ರಿಕ್ತ; (sp) = (sp)->sn_next)

ಸ್ಥಿರ hashtab_T sg_table; // sign group (signgroup_T) hashtable
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
ಶೂನ್ಯ
init_signs(ಶೂನ್ಯ)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ಸ್ಥಿರ signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = ರಿಕ್ತ;

    ಯದಿ (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ಯದಿ (group == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    ಅನ್ಯಥಾ
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    ಪ್ರತ್ಯಾಗಮ group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    ಯದಿ (HASHITEM_EMPTY(hi))
        ಪ್ರತ್ಯಾಗಮ;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    ಯದಿ (group->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_table, hi, "sign remove");
        vim_free(group);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_in_group(sign_entry_T *sign, char_u *group)
{
    ಪ್ರತ್ಯಾಗಮ ((group != ರಿಕ್ತ && STRCMP(group, "*") == 0) ||
            (group == ರಿಕ್ತ && sign->se_group == ರಿಕ್ತ) ||
            (group != ರಿಕ್ತ && sign->se_group != ರಿಕ್ತ &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ for_popup = sign->se_group != ರಿಕ್ತ &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    ಪ್ರತ್ಯಾಗಮ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    ಪೂರ್ಣಾಙ್ಕ id = 1;
    signgroup_T *group = ರಿಕ್ತ;
    sign_entry_T *sign = ರಿಕ್ತ;

    ಯದಿ (groupname != ರಿಕ್ತ)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        ಯದಿ (HASHITEM_EMPTY(hi))
            ಪ್ರತ್ಯಾಗಮ id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    ಪೂರ್ಣಾಙ್ಕ found = FALSE;
    ಯಾವತ್ (!found)
    {
        ಯದಿ (group == ರಿಕ್ತ)
            id = next_sign_id++; // global group
        ಅನ್ಯಥಾ
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ಯದಿ (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                ಭಙ್ಗ;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            ಪೂರ್ಣಾಙ್ಕ id, // sign ID
            char_u *group, // sign group; NULL for global group
            ಪೂರ್ಣಾಙ್ಕ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ಪೂರ್ಣಾಙ್ಕ typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(ಪರಿಮಾಣ(sign_entry_T), FALSE, aid_ಸೇರಿಸು_sign);
    ಯದಿ (newsign == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ಯದಿ (group != ರಿಕ್ತ)
    {
        newsign->se_group = sign_group_ref(group);
        ಯದಿ (newsign->se_group == ರಿಕ್ತ)
        {
            vim_free(newsign);
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    ಅನ್ಯಥಾ
    {
        newsign->se_group = ರಿಕ್ತ;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    ಯದಿ (next != ರಿಕ್ತ)
        next->se_prev = newsign;

    ಯದಿ (prev == ರಿಕ್ತ)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ಯದಿ (buf->b_signlist == ರಿಕ್ತ)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_ಸಾಲು_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ಯದಿ (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    ಅನ್ಯಥಾ
    {
        prev->se_next = newsign;
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
                         sign_entry_T *prev, // previous sign entry
                         ಪೂರ್ಣಾಙ್ಕ id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         ಪೂರ್ಣಾಙ್ಕ prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         ಪೂರ್ಣಾಙ್ಕ typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ಯಾವತ್ (prev != ರಿಕ್ತ && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == ರಿಕ್ತ) ? buf->b_signlist : prev->se_next;

    ಸೇರಿಸು_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ಸ್ಥಿರ sign_T *
find_sign_by_typenr(ಪೂರ್ಣಾಙ್ಕ typenr)
{
    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
        ಯದಿ (sp->sn_typenr == typenr)
            ಪ್ರತ್ಯಾಗಮ sp;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get the name of a sign by its typenr.
 */
ಸ್ಥಿರ char_u *
sign_typenr2name(ಪೂರ್ಣಾಙ್ಕ typenr)
{
    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
        ಯದಿ (sp->sn_typenr == typenr)
            ಪ್ರತ್ಯಾಗಮ sp->sn_name;
    ಪ್ರತ್ಯಾಗಮ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ಸ್ಥಿರ dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ಯದಿ (d == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    dict_add_ಸಂಖ್ಯೆ(d, "id", sign->se_id);
    ಯದಿ (sign->se_group == ರಿಕ್ತ)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    ಅನ್ಯಥಾ
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_ಸಂಖ್ಯೆ(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_ಸಂಖ್ಯೆ(d, "priority", sign->se_priority);

    ಪ್ರತ್ಯಾಗಮ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_sort_by_prio_on_ಸಾಲು(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ಯದಿ ((sign->se_prev == ರಿಕ್ತ || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == ರಿಕ್ತ || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        ಪ್ರತ್ಯಾಗಮ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    ಯಾವತ್ (p->se_prev != ರಿಕ್ತ && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    ಯದಿ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        ಯಾವತ್ (p->se_next != ರಿಕ್ತ && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    ಯದಿ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    ಯದಿ (sign->se_prev != ರಿಕ್ತ)
        sign->se_prev->se_next = sign->se_next;

    ಯದಿ (sign->se_next != ರಿಕ್ತ)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = ರಿಕ್ತ;
    sign->se_next = ರಿಕ್ತ;

    // Re-insert 'sign' at the right place
    ಯದಿ (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        ಯದಿ (sign->se_prev != ರಿಕ್ತ)
            sign->se_prev->se_next = sign;

        ಯದಿ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ಅನ್ಯಥಾ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        ಯದಿ (sign->se_next != ರಿಕ್ತ)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
buf_addsign(buf_T *buf, // buffer to store sign in
            ಪೂರ್ಣಾಙ್ಕ id, // sign ID
            char_u *groupname, // sign group
            ಪೂರ್ಣಾಙ್ಕ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ಪೂರ್ಣಾಙ್ಕ typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    sign_entry_T *prev = ರಿಕ್ತ; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ಯದಿ (buf->b_sign_finger != ರಿಕ್ತ && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    ಪ್ರತಿ (sign = (prev == ರಿಕ್ತ) ? buf->b_signlist : prev->se_next; sign != ರಿಕ್ತ;
         sign = sign->se_next)
    {
        ಯದಿ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ಸಾಲು(buf, sign);
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಅನ್ಯಥಾ ಯದಿ (lnum < sign->se_lnum)
        {
            ಸೇರಿಸು_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ಪ್ರತ್ಯಾಗಮ;
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
                     ಪೂರ್ಣಾಙ್ಕ markId, // sign ID
                     char_u *group, // sign group
                     ಪೂರ್ಣಾಙ್ಕ typenr, // typenr of sign we are adding
                     ಪೂರ್ಣಾಙ್ಕ prio) // sign priority
{
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ಯದಿ (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ಸಾಲು(buf, sign);
            ಪ್ರತ್ಯಾಗಮ sign->se_lnum;
        }
    }

    ಪ್ರತ್ಯಾಗಮ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
ಪೂರ್ಣಾಙ್ಕ
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = ರಿಕ್ತ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಯದಿ (sign->se_lnum > lnum)
            ಭಙ್ಗ;

        ಯದಿ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            ಯದಿ (sp == ರಿಕ್ತ)
                ಪ್ರತ್ಯಾಗಮ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            ಯದಿ (sattr->sat_text != ರಿಕ್ತ && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            ಯದಿ (sp->sn_ಸಾಲು_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_ಸಾಲು_hl);

            ಯದಿ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ಯದಿ (sp->sn_ಅಂಕೆ_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_ಅಂಕೆ_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ಯದಿ (sign->se_next != ರಿಕ್ತ &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                ಯದಿ (next_sp == ರಿಕ್ತ)
                    ಪ್ರತ್ಯಾಗಮ FALSE;

                ಯದಿ (sattr->sat_icon == ರಿಕ್ತ && sattr->sat_text == ರಿಕ್ತ)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                ಯದಿ (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                ಯದಿ (sp->sn_ಸಾಲು_hl <= 0 && next_sp->sn_ಸಾಲು_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_ಸಾಲು_hl);

                ಯದಿ (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                ಯದಿ (sp->sn_ಅಂಕೆ_hl <= 0 && next_sp->sn_ಅಂಕೆ_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_ಅಂಕೆ_hl);
            }
            ಪ್ರತ್ಯಾಗಮ TRUE;
        }
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
            ಪೂರ್ಣಾಙ್ಕ id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = ರಿಕ್ತ; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ರಿಕ್ತ; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    ಪ್ರತಿ (sign_entry_T *sign = buf->b_signlist; sign != ರಿಕ್ತ; sign = next)
    {
        next = sign->se_next;

        ಯದಿ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            ಯದಿ (next != ರಿಕ್ತ)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ಯದಿ (sign->se_group != ರಿಕ್ತ)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_ಸಾಲು_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ಯದಿ (group == ರಿಕ್ತ || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                ಭಙ್ಗ;
        }
        ಅನ್ಯಥಾ
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ಯದಿ (buf->b_signlist == ರಿಕ್ತ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ಸಾಲು_abv_curs();
    }

    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
ಪೂರ್ಣಾಙ್ಕ
buf_findsign(buf_T *buf, // buffer to store sign in
             ಪೂರ್ಣಾಙ್ಕ id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        ಯದಿ (sign->se_id == id && sign_in_group(sign, group))
            ಪ್ರತ್ಯಾಗಮ sign->se_lnum;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ಸ್ಥಿರ sign_entry_T *
buf_getsign_at_ಸಾಲು(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಯದಿ (sign->se_lnum > lnum)
            ಭಙ್ಗ;

        ಯದಿ (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            ಪ್ರತ್ಯಾಗಮ sign;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
ಪೂರ್ಣಾಙ್ಕ
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_ಸಾಲು(buf, lnum, groupname);
    ಯದಿ (sign != ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ sign->se_id;

    ಪ್ರತ್ಯಾಗಮ 0;
}

# ಯದಿ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
ಪೂರ್ಣಾಙ್ಕ
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    ಪೂರ್ಣಾಙ್ಕ typenr) // sign type number
{
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಯದಿ (sign->se_lnum > lnum)
            ಭಙ್ಗ;

        ಯದಿ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ಪ್ರತ್ಯಾಗಮ sign->se_id;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

#  ಯದಿ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
ಪೂರ್ಣಾಙ್ಕ
buf_signcount(buf_T *buf, linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ಯದಿ (sign->se_lnum > lnum)
            ಭಙ್ಗ;

        ಯದಿ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ರಿಕ್ತ)
            ಎಣಿಕೆ++;
    }

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
ಶೂನ್ಯ
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = ರಿಕ್ತ; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ಯದಿ (buf->b_signlist != ರಿಕ್ತ && curwin != ರಿಕ್ತ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ಸಾಲು_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ರಿಕ್ತ;

    ಪ್ರತಿ (sign_entry_T *sign = buf->b_signlist; sign != ರಿಕ್ತ; sign = next)
    {
        next = sign->se_next;
        ಯದಿ (sign_in_group(sign, group))
        {
            *lastp = next;

            ಯದಿ (next != ರಿಕ್ತ)
                next->se_prev = sign->se_prev;

            ಯದಿ (sign->se_group != ರಿಕ್ತ)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        ಅನ್ಯಥಾ
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    ವರ್ಣ lbuf[ಸಂದೇಶ_BUF_LEN];
    ವರ್ಣ group[ಸಂದೇಶ_BUF_LEN];

    ಸಂದೇಶ_puts_title(_("\n--- Signs ---"));
    ಸಂದೇಶ_putchar('\n');

    buf_T *buf = (rbuf == ರಿಕ್ತ) ? firstbuf : rbuf;
    ಯಾವತ್ (buf != ರಿಕ್ತ && !got_int)
    {
        ಯದಿ (buf->b_signlist != ರಿಕ್ತ)
        {
            vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            ಸಂದೇಶ_puts_attr(lbuf, HL_ATTR(HLF_D));
            ಸಂದೇಶ_putchar('\n');
        }

        sign_entry_T *sign = ರಿಕ್ತ;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ಯದಿ (got_int)
                ಭಙ್ಗ;

            ಯದಿ (!sign_in_group(sign, sign_group))
                ಅನುವರ್ತ;

            ಯದಿ (sign->se_group != ರಿಕ್ತ)
                vim_snprintf(group, ಸಂದೇಶ_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            ಅನ್ಯಥಾ
                group[0] = '\0';

            vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            ಸಂದೇಶ_puts(lbuf);
            ಸಂದೇಶ_putchar('\n');
        }

        ಯದಿ (rbuf != ರಿಕ್ತ)
            ಭಙ್ಗ;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
ಶೂನ್ಯ
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = ರಿಕ್ತ; // line changes may reorder, drop the finger
    sign_entry_T *sign = ರಿಕ್ತ; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ಯದಿ (sign->se_lnum < line1)
            ಅನುವರ್ತ;

        linenr_T new_lnum = sign->se_lnum;

        ಯದಿ (sign->se_lnum <= line2)
        {
            ಯದಿ (amount != MAXLNUM)
                new_lnum += amount;
        }
        ಅನ್ಯಥಾ ಯದಿ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ಯದಿ (new_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    ಪೂರ್ಣಾಙ್ಕ idx = 0;
    ವರ್ಣ save = *end_cmd;
    *end_cmd = NUL;

    ಯಾವತ್ (cmds[idx] != ರಿಕ್ತ && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ಸ್ಥಿರ sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    ಯದಿ (sp_prev != ರಿಕ್ತ)
        *sp_prev = ರಿಕ್ತ;

    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
    {
        ಯದಿ (STRCMP(sp->sn_name, name) == 0)
            ಭಙ್ಗ;

        ಯದಿ (sp_prev != ರಿಕ್ತ)
            *sp_prev = sp;
    }

    ಪ್ರತ್ಯಾಗಮ sp;
}

/*
 * Allocate a new sign
 */
ಸ್ಥಿರ sign_T *
alloc_new_sign(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(ಪರಿಮಾಣ(sign_T), aid_sign_define_by_name);
    ಯದಿ (sp == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    ಯಾವತ್ (lp != ರಿಕ್ತ)
    {
        ಯದಿ (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            ಯದಿ (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            ಯದಿ (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }

            lp = first_sign; // start all over
            ಅನುವರ್ತ;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    ಯದಿ (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ಯದಿ (sp->sn_name == ರಿಕ್ತ) // out of memory
    {
        vim_free(sp);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ sp;
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
    ಯದಿ (gui.in_use)
    {
        out_flush();
        ಯದಿ (sp->sn_image != ರಿಕ್ತ)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = ರಿಕ್ತ;
    char_u *endp = text + (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text);
    ಪೂರ್ಣಾಙ್ಕ cells = 0;

    // Remove backslashes so that it is possible to use a space.
    ಪ್ರತಿ (s = text; s + 1 < endp; ++s)
    {
        ಯದಿ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ಯದಿ (has_mbyte)
    {
        ಪ್ರತಿ (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            ಯದಿ (!vim_isprintc((*mb_ptr2char)(s)))
                ಭಙ್ಗ;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ಅನ್ಯಥಾ
    {
        ಪ್ರತಿ (s = text; s < endp; ++s)
        {
            ಯದಿ (!vim_isprintc(*s))
                ಭಙ್ಗ;
        }
        cells = (ಪೂರ್ಣಾಙ್ಕ)(s - text);
    }

    // Currently sign text must be one or two display cells
    ಯದಿ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    ಯದಿ (sp->sn_text != ರಿಕ್ತ && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Define a new sign or update an existing sign
 */
ಪೂರ್ಣಾಙ್ಕ
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    ಪೂರ್ಣಾಙ್ಕ prio)
{
    sign_T *sp_prev = ರಿಕ್ತ;
    sign_T *sp = sign_find(name, &sp_prev);
    ಯದಿ (sp == ರಿಕ್ತ)
    {
        sp = alloc_new_sign(name);
        ಯದಿ (sp == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ FAIL;

        // add the new sign to the list of signs
        ಯದಿ (sp_prev == ರಿಕ್ತ)
            first_sign = sp;
        ಅನ್ಯಥಾ
            sp_prev->sn_next = sp;
    }
    ಅನ್ಯಥಾ
    {
        win_T *wp = ರಿಕ್ತ;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ಯದಿ (wp->w_buffer->b_signlist != ರಿಕ್ತ)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ಯದಿ (icon != ರಿಕ್ತ)
        sign_define_init_icon(sp, icon);

    ಯದಿ (text != ರಿಕ್ತ && (sign_define_init_text(sp, text) == FAIL))
        ಪ್ರತ್ಯಾಗಮ FAIL;

    sp->sn_priority = prio;

    ಯದಿ (linehl != ರಿಕ್ತ)
    {
        ಯದಿ (*linehl == NUL)
            sp->sn_ಸಾಲು_hl = 0;
        ಅನ್ಯಥಾ
            sp->sn_ಸಾಲು_hl = syn_check_group(linehl, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(linehl));
    }

    ಯದಿ (texthl != ರಿಕ್ತ)
    {
        ಯದಿ (*texthl == NUL)
            sp->sn_text_hl = 0;
        ಅನ್ಯಥಾ
            sp->sn_text_hl = syn_check_group(texthl, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(texthl));
    }

    ಯದಿ (culhl != ರಿಕ್ತ)
    {
        ಯದಿ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ಅನ್ಯಥಾ
            sp->sn_cul_hl = syn_check_group(culhl, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(culhl));
    }

    ಯದಿ (numhl != ರಿಕ್ತ)
    {
        ಯದಿ (*numhl == NUL)
            sp->sn_ಅಂಕೆ_hl = 0;
        ಅನ್ಯಥಾ
            sp->sn_ಅಂಕೆ_hl = syn_check_group(numhl, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(numhl));
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
ಪೂರ್ಣಾಙ್ಕ
sign_exists_by_name(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ sign_find(name, ರಿಕ್ತ) != ರಿಕ್ತ;
}

/*
 * Free the sign specified by 'name'.
 */
ಪೂರ್ಣಾಙ್ಕ
sign_undefine_by_name(char_u *name, ಪೂರ್ಣಾಙ್ಕ give_error)
{
    sign_T *sp_prev = ರಿಕ್ತ;
    sign_T *sp = sign_find(name, &sp_prev);
    ಯದಿ (sp == ರಿಕ್ತ)
    {
        ಯದಿ (give_error)
            semsg(_(e_unknown_sign_str), name);
        ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    sign_undefine(sp, sp_prev);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * List the signs matching 'name'
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, ರಿಕ್ತ);
    ಯದಿ (sp != ರಿಕ್ತ)
        sign_list_defined(sp);
    ಅನ್ಯಥಾ
        semsg(_(e_unknown_sign_str), name);
}

ಸ್ಥಿರ ಶೂನ್ಯ
may_force_numberwidth_recompute(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ unplace)
{
# ಯದಿ defined(FEAT_LINEBREAK)
    tabpage_T *tp = ರಿಕ್ತ;
    win_T *wp = ರಿಕ್ತ;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ಯದಿ (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
ಪೂರ್ಣಾಙ್ಕ
sign_place(ಪೂರ್ಣಾಙ್ಕ *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           ಪೂರ್ಣಾಙ್ಕ prio)
{
    // Check for reserved character '*' in group name
    ಯದಿ (sign_group != ರಿಕ್ತ && (*sign_group == '*' || *sign_group == '\0'))
        ಪ್ರತ್ಯಾಗಮ FAIL;

    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
    {
        ಯದಿ (STRCMP(sp->sn_name, sign_name) == 0)
            ಭಙ್ಗ;
    }

    ಯದಿ (sp == ರಿಕ್ತ)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    ಯದಿ (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    ಯದಿ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    ಅನ್ಯಥಾ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    ಯದಿ (lnum > 0)
    {
        redraw_buf_ಸಾಲು_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ಅನ್ಯಥಾ
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Unplace the specified sign
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_unplace(ಪೂರ್ಣಾಙ್ಕ sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    ಯದಿ (buf->b_signlist == ರಿಕ್ತ) // No signs in the buffer
        ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    ಅನ್ಯಥಾ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        ಯದಿ (lnum == 0)
            ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ಯದಿ (buf->b_signlist == ರಿಕ್ತ)
        may_force_numberwidth_recompute(buf, TRUE);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_unplace_at_cursor(char_u *groupname)
{
    ಪೂರ್ಣಾಙ್ಕ id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    ಯದಿ (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    ಅನ್ಯಥಾ
        emsg(_(e_missing_sign_ಸಂಖ್ಯೆ));
}

/*
 * Jump to a sign.
 */
ಸ್ಥಿರ linenr_T
sign_jump(ಪೂರ್ಣಾಙ್ಕ sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    ಯದಿ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    // goto a sign ...
    ಯದಿ (buf_jump_open_win(buf) != ರಿಕ್ತ)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ಅನ್ಯಥಾ
    { // ... not currently in a window
        ಯದಿ (buf->b_fname == ರಿಕ್ತ)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        setpcmark();
        ಯದಿ (do_ecmd(buf->b_fnum, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ಪ್ರತ್ಯಾಗಮ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * ":sign define {name} ..." command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ರಿಕ್ತ;
    char_u *p = cmdline;
    char_u *icon = ರಿಕ್ತ;
    char_u *text = ರಿಕ್ತ;
    char_u *linehl = ರಿಕ್ತ;
    char_u *texthl = ರಿಕ್ತ;
    char_u *culhl = ರಿಕ್ತ;
    char_u *numhl = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ prio = -1;
    ಪೂರ್ಣಾಙ್ಕ failed = FALSE;

    // set values for a defined sign.
    ಯಾವತ್ (TRUE)
    {
        arg = skipwhite(p);
        ಯದಿ (*arg == NUL)
            ಭಙ್ಗ;

        p = skiptowhite_esc(arg);
        ಯದಿ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((ವರ್ಣ *)arg);
        }
        ಅನ್ಯಥಾ
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            ಭಙ್ಗ;
        }
    }

    ಯದಿ (!failed)
        sign_define_by_name(sign_name, icon, linehl, text, texthl, culhl, numhl,
                            prio);

    vim_free(icon);
    vim_free(text);
    vim_free(linehl);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);
}

/*
 * ":sign place" command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               ಪೂರ್ಣಾಙ್ಕ id,
               char_u *group,
               ಪೂರ್ಣಾಙ್ಕ prio)
{
    ಯದಿ (id <= 0)
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
        ಯದಿ (lnum >= 0 || sign_name != ರಿಕ್ತ || (group != ರಿಕ್ತ && *group == '\0'))
            emsg(_(e_invalid_argument));
        ಅನ್ಯಥಾ
            sign_list_placed(buf, group);
    }
    ಅನ್ಯಥಾ
    {
        // Place a new sign
        ಯದಿ (sign_name == ರಿಕ್ತ || buf == ರಿಕ್ತ ||
            (group != ರಿಕ್ತ && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            ಪ್ರತ್ಯಾಗಮ;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 ಪೂರ್ಣಾಙ್ಕ id,
                 char_u *group)
{
    ಯದಿ (lnum >= 0 || sign_name != ರಿಕ್ತ || (group != ರಿಕ್ತ && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (id == -2)
    {
        ಯದಿ (buf != ರಿಕ್ತ)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        ಅನ್ಯಥಾ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ಯದಿ (buf->b_signlist != ರಿಕ್ತ)
                    buf_delete_signs(buf, group);
            }
        }
    }
    ಅನ್ಯಥಾ
    {
        ಯದಿ (buf != ರಿಕ್ತ)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        ಅನ್ಯಥಾ
        {
            ಯದಿ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            ಅನ್ಯಥಾ
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, group, buf, 0);
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
              ಪೂರ್ಣಾಙ್ಕ id,
              char_u *group)
{
    ಯದಿ (sign_name == ರಿಕ್ತ && group == ರಿಕ್ತ && id == -1)
    {
        emsg(_(e_argument_required));
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (buf == ರಿಕ್ತ || (group != ರಿಕ್ತ && *group == '\0') || lnum >= 0 ||
        sign_name != ರಿಕ್ತ)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        ಪ್ರತ್ಯಾಗಮ;
    }

    (ಶೂನ್ಯ)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_sign_cmd_args(ಪೂರ್ಣಾಙ್ಕ cmd,
                    char_u *arg,
                    char_u **sign_name,
                    ಪೂರ್ಣಾಙ್ಕ *signid,
                    char_u **group,
                    ಪೂರ್ಣಾಙ್ಕ *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ lnum_arg = FALSE;

    // first arg could be placed sign id
    ಯದಿ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ಯದಿ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ಅನ್ಯಥಾ
        {
            arg = skipwhite(arg);
        }
    }

    ಯಾವತ್ (*arg != NUL)
    {
        ಯದಿ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((ವರ್ಣ *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ಯದಿ (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                ಪ್ರತ್ಯಾಗಮ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ಯದಿ (*arg != NUL)
                *arg++ = NUL;

            ಯಾವತ್ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            ಯದಿ (*arg != NUL)
                *arg++ = NUL;
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((ವರ್ಣ *)arg);
            arg = skiptowhite(arg);
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ಭಙ್ಗ;
        }
        ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((ಪೂರ್ಣಾಙ್ಕ)getdigits(&arg));

            ಯದಿ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ಭಙ್ಗ;
        }
        ಅನ್ಯಥಾ
        {
            emsg(_(e_invalid_argument));
            ಪ್ರತ್ಯಾಗಮ FAIL;
        }

        arg = skipwhite(arg);
    }

    ಯದಿ (filename != ರಿಕ್ತ && *buf == ರಿಕ್ತ)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ಯದಿ (filename == ರಿಕ್ತ &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ idx = sign_cmd_idx(arg, p);
    ಯದಿ (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ಪ್ರತ್ಯಾಗಮ;
    }
    arg = skipwhite(p);

    ಯದಿ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ id = -1;
        char_u *group = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ prio = -1;
        buf_T *buf = ರಿಕ್ತ;
        linenr_T lnum = -1;

        // Parse command line arguments
        ಯದಿ (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        ಅನ್ಯಥಾ ಯದಿ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        ಅನ್ಯಥಾ ಯದಿ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        ಪ್ರತ್ಯಾಗಮ;
    }

    // Define, undefine or list signs.
    ಯದಿ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        ಪ್ರತಿ (sign_T *sp = first_sign; sp != ರಿಕ್ತ && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    ಅನ್ಯಥಾ ಯದಿ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ಅನ್ಯಥಾ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ಯದಿ (*p != NUL)
            *p++ = NUL;

        ಯಾವತ್ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ಯದಿ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ಅನ್ಯಥಾ ಯದಿ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ಅನ್ಯಥಾ
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
    dict_add_string(retdict, "name", sp->sn_name);

    ಯದಿ (sp->sn_icon != ರಿಕ್ತ)
        dict_add_string(retdict, "icon", sp->sn_icon);

    ಯದಿ (sp->sn_text != ರಿಕ್ತ)
        dict_add_string(retdict, "text", sp->sn_text);

    ಯದಿ (sp->sn_priority > 0)
        dict_add_ಸಂಖ್ಯೆ(retdict, "priority", sp->sn_priority);

    ಯದಿ (sp->sn_ಸಾಲು_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_ಸಾಲು_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಅನ್ಯಥಾ
            dict_add_string(retdict, "linehl", p);
    }

    ಯದಿ (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_text_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಅನ್ಯಥಾ
            dict_add_string(retdict, "texthl", p);
    }

    ಯದಿ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_cul_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಅನ್ಯಥಾ
            dict_add_string(retdict, "culhl", p);
    }

    ಯದಿ (sp->sn_ಅಂಕೆ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_ಅಂಕೆ_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ಅನ್ಯಥಾ
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    ಯದಿ (name != ರಿಕ್ತ)
    {
        sp = sign_find(name, ರಿಕ್ತ);
        ಯದಿ (sp == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (; sp != ರಿಕ್ತ && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ಯದಿ (dict == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ಪ್ರತ್ಯಾಗಮ;
        }

        sign_getinfo(sp, dict);

        ಯದಿ (name != ರಿಕ್ತ) // handle only the specified sign
            ಭಙ್ಗ;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
ಶೂನ್ಯ
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = ರಿಕ್ತ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ಯದಿ (d != ರಿಕ್ತ && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       ಪೂರ್ಣಾಙ್ಕ sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ಯದಿ (d == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ಪ್ರತ್ಯಾಗಮ;
    }

    dict_add_ಸಂಖ್ಯೆ(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ಯದಿ (l == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ಪ್ರತ್ಯಾಗಮ;
    }

    sign_entry_T *sign = ರಿಕ್ತ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ಯದಿ (!sign_in_group(sign, sign_group))
            ಅನುವರ್ತ;

        ಯದಿ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ಯದಿ (sdict != ರಿಕ್ತ && list_append_dict(l, sdict) == FAIL)
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
                ಪೂರ್ಣಾಙ್ಕ sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    ಯದಿ (buf != ರಿಕ್ತ)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    ಅನ್ಯಥಾ
    {
        FOR_ALL_BUFFERS(buf)
        {
            ಯದಿ (buf->b_signlist != ರಿಕ್ತ)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# ಯದಿ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
ಶೂನ್ಯ
sign_gui_started(ಶೂನ್ಯ)
{
    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
    {
        ಯದಿ (sp->sn_icon != ರಿಕ್ತ)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_list_defined(sign_T *sp)
{
    ವರ್ಣ lbuf[ಸಂದೇಶ_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ಯದಿ (sp->sn_icon != ರಿಕ್ತ)
    {
        ಸಂದೇಶ_puts(" icon=");
        ಸಂದೇಶ_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ಯದಿ (sp->sn_image == ರಿಕ್ತ)
            ಸಂದೇಶ_puts(_(" (NOT FOUND)"));
# ಅನ್ಯಥಾ
        ಸಂದೇಶ_puts(_(" (not supported)"));
# endif
    }

    ಯದಿ (sp->sn_text != ರಿಕ್ತ)
    {
        ಸಂದೇಶ_puts(" text=");
        ಸಂದೇಶ_outtrans(sp->sn_text);
    }

    ಯದಿ (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, ಸಂದೇಶ_BUF_LEN, " priority=%d", sp->sn_priority);
        ಸಂದೇಶ_puts(lbuf);
    }

    ಯದಿ (sp->sn_ಸಾಲು_hl > 0)
    {
        ಸಂದೇಶ_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_ಸಾಲು_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            ಸಂದೇಶ_puts("NONE");
        ಅನ್ಯಥಾ
            ಸಂದೇಶ_puts((ವರ್ಣ *)p);
    }

    ಯದಿ (sp->sn_text_hl > 0)
    {
        ಸಂದೇಶ_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_text_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            ಸಂದೇಶ_puts("NONE");
        ಅನ್ಯಥಾ
            ಸಂದೇಶ_puts((ವರ್ಣ *)p);
    }

    ಯದಿ (sp->sn_cul_hl > 0)
    {
        ಸಂದೇಶ_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_cul_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            ಸಂದೇಶ_puts("NONE");
        ಅನ್ಯಥಾ
            ಸಂದೇಶ_puts((ವರ್ಣ *)p);
    }

    ಯದಿ (sp->sn_ಅಂಕೆ_hl > 0)
    {
        ಸಂದೇಶ_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ರಿಕ್ತ, sp->sn_ಅಂಕೆ_hl - 1, FALSE);
        ಯದಿ (p == ರಿಕ್ತ)
            ಸಂದೇಶ_puts("NONE");
        ಅನ್ಯಥಾ
            ಸಂದೇಶ_puts((ವರ್ಣ *)p);
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
    ಯದಿ (sp->sn_image != ರಿಕ್ತ)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    ಯದಿ (sp_prev == ರಿಕ್ತ)
        first_sign = sp->sn_next;
    ಅನ್ಯಥಾ
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# ಯದಿ defined(FEAT_SIGN_ICONS)
ಶೂನ್ಯ *
sign_get_image(ಪೂರ್ಣಾಙ್ಕ typenr) // the attribute which may have a sign
{
    sign_T *sp = ರಿಕ್ತ;
    FOR_ALL_SIGNS(sp)
    {
        ಯದಿ (sp->sn_typenr == typenr)
            ಪ್ರತ್ಯಾಗಮ sp->sn_image;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
# endif

/*
 * Undefine/free all signs.
 */
ಶೂನ್ಯ
free_signs(ಶೂನ್ಯ)
{
    ಯಾವತ್ (first_sign != ರಿಕ್ತ)
        sign_undefine(first_sign, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಗಣನಾ
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_PLACE, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_NAMES, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
ಸ್ಥಿರ char_u *
get_nth_sign_name(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ current_idx = 0;
    sign_T *sp = ರಿಕ್ತ;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ಯದಿ (current_idx++ == idx)
            ಪ್ರತ್ಯಾಗಮ sp->sn_name;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ಸ್ಥಿರ char_u *
get_nth_sign_group_name(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ current_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ todo = (ಪೂರ್ಣಾಙ್ಕ)sg_table.ht_used;
    hashitem_T *hi = ರಿಕ್ತ;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&sg_table, hi, todo)
    {
        ಯದಿ (!HASHITEM_EMPTY(hi))
        {
            --todo;
            ಯದಿ (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                ಪ್ರತ್ಯಾಗಮ group->sg_name;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವಿಕಲ್ಪ (expand_what)
    {
        ಪಕ್ಷ EXP_SUBCMD:
            ಪ್ರತ್ಯಾಗಮ (char_u *)cmds[idx];
        ಪಕ್ಷ EXP_DEFINE:
        {
            ವರ್ಣ *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ರಿಕ್ತ };
            ಪ್ರತ್ಯಾಗಮ (char_u *)define_arg[idx];
        }
        ಪಕ್ಷ EXP_PLACE:
        {
            ವರ್ಣ *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ರಿಕ್ತ };
            ಪ್ರತ್ಯಾಗಮ (char_u *)place_arg[idx];
        }
        ಪಕ್ಷ EXP_LIST:
        {
            ವರ್ಣ *list_arg[] = { "group=", "file=", "buffer=", ರಿಕ್ತ };
            ಪ್ರತ್ಯಾಗಮ (char_u *)list_arg[idx];
        }
        ಪಕ್ಷ EXP_UNPLACE:
        {
            ವರ್ಣ *unplace_arg[] = { "group=", "file=", "buffer=", ರಿಕ್ತ };
            ಪ್ರತ್ಯಾಗಮ (char_u *)unplace_arg[idx];
        }
        ಪಕ್ಷ EXP_SIGN_NAMES:
            ಪ್ರತ್ಯಾಗಮ get_nth_sign_name(idx);
        ಪಕ್ಷ EXP_SIGN_GROUPS:
            ಪ್ರತ್ಯಾಗಮ get_nth_sign_group_name(idx);
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Handle command line completion for :sign command.
 */
ಶೂನ್ಯ
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    ಪೂರ್ಣಾಙ್ಕ cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ಯದಿ (*end_subcmd == NUL)
        ಪ್ರತ್ಯಾಗಮ;

    cmd_idx = sign_cmd_idx(arg, end_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_args = skipwhite(end_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_args;
    ಕುರು
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    ಯಾವತ್ (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ಯದಿ (p == ರಿಕ್ತ)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        ವಿಕಲ್ಪ (cmd_idx)
        {
            ಪಕ್ಷ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ಭಙ್ಗ;
            ಪಕ್ಷ SIGNCMD_PLACE:
                // List placed signs
                ಯದಿ (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                ಅನ್ಯಥಾ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ಭಙ್ಗ;
            ಪಕ್ಷ SIGNCMD_LIST:
            ಪಕ್ಷ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                ಭಙ್ಗ;
            ಪಕ್ಷ SIGNCMD_JUMP:
            ಪಕ್ಷ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ಅನ್ಯಥಾ
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        ವಿಕಲ್ಪ (cmd_idx)
        {
            ಪಕ್ಷ SIGNCMD_DEFINE:
                ಯದಿ (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ಅನ್ಯಥಾ ಯದಿ (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ಅನ್ಯಥಾ
                    xp->xp_context = EXPAND_NOTHING;
                ಭಙ್ಗ;
            ಪಕ್ಷ SIGNCMD_PLACE:
                ಯದಿ (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                ಅನ್ಯಥಾ ಯದಿ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ಅನ್ಯಥಾ ಯದಿ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ಅನ್ಯಥಾ
                    xp->xp_context = EXPAND_NOTHING;
                ಭಙ್ಗ;
            ಪಕ್ಷ SIGNCMD_UNPLACE:
            ಪಕ್ಷ SIGNCMD_JUMP:
                ಯದಿ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ಅನ್ಯಥಾ ಯದಿ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ಅನ್ಯಥಾ
                    xp->xp_context = EXPAND_NOTHING;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ರಿಕ್ತ;
    char_u *icon = ರಿಕ್ತ;
    char_u *linehl = ರಿಕ್ತ;
    char_u *text = ರಿಕ್ತ;
    char_u *texthl = ರಿಕ್ತ;
    char_u *culhl = ರಿಕ್ತ;
    char_u *numhl = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ prio = -1;
    ಪೂರ್ಣಾಙ್ಕ retval = -1;

    ಯದಿ (name_arg == ರಿಕ್ತ && dict == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ retval;

    ಯದಿ (name_arg == ರಿಕ್ತ)
        name = dict_get_string(dict, "name", TRUE);
    ಅನ್ಯಥಾ
        name = vim_strsave(name_arg);

    ಯದಿ (name == ರಿಕ್ತ || name[0] == NUL)
        ಗಚ್ಛ cleanup;

    ಯದಿ (dict != ರಿಕ್ತ)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_ಸಂಖ್ಯೆ_def(dict, "priority", -1);
    }

    ಯದಿ (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
                            prio) == OK)
        retval = 0;

cleanup:
    vim_free(name);
    vim_free(icon);
    vim_free(linehl);
    vim_free(text);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ರಿಕ್ತ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
        ಪೂರ್ಣಾಙ್ಕ retval = -1;

        ಯದಿ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ರಿಕ್ತ, li->li_tv.vval.v_dict);
        ಅನ್ಯಥಾ
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
    ಯದಿ (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ಯದಿ (rettv_list_alloc(rettv) == FAIL)
            ಪ್ರತ್ಯಾಗಮ;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ಪ್ರತ್ಯಾಗಮ;
    }

    // Define a single sign
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ಯದಿ (name == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ರಿಕ್ತ);
}

/*
 * "sign_getdefined()" function
 */
ಶೂನ್ಯ
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    char_u *name = ರಿಕ್ತ;
    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
ಶೂನ್ಯ
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ರಿಕ್ತ;
    linenr_T lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ sign_id = 0;
    char_u *group = ರಿಕ್ತ;

    ಯದಿ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ಯದಿ (buf == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
        {
            ಯದಿ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ಪ್ರತ್ಯಾಗಮ;

            dictitem_T *di = ರಿಕ್ತ;
            dict_T *dict = argvars[1].vval.v_dict;

            ಯದಿ ((di = dict_find(dict, (char_u *)"lnum", -1)) != ರಿಕ್ತ)
            {
                // get signs placed at this line
                ಪೂರ್ಣಾಙ್ಕ notanum = FALSE;
                tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
                ಯದಿ (notanum)
                    ಪ್ರತ್ಯಾಗಮ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ಯದಿ ((di = dict_find(dict, (char_u *)"id", -1)) != ರಿಕ್ತ)
            {
                // get sign placed with this identifier
                ಪೂರ್ಣಾಙ್ಕ notanum = FALSE;
                sign_id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
                ಯದಿ (notanum)
                    ಪ್ರತ್ಯಾಗಮ;
            }

            ಯದಿ ((di = dict_find(dict, (char_u *)"group", -1)) != ರಿಕ್ತ)
            {
                group = tv_get_string_chk(&di->di_tv);
                ಯದಿ (group == ರಿಕ್ತ)
                    ಪ್ರತ್ಯಾಗಮ;

                ಯದಿ (*group == '\0') // empty string means global group
                    group = ರಿಕ್ತ;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
ಶೂನ್ಯ
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = ರಿಕ್ತ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script() && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ notanum = FALSE;
    // Sign identifier
    ಪೂರ್ಣಾಙ್ಕ sign_id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &notanum);
    ಯದಿ (notanum)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        ಪ್ರತ್ಯಾಗಮ;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    ಯದಿ (sign_group == ರಿಕ್ತ)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (sign_group[0] == '\0')
    {
        sign_group = ರಿಕ್ತ; // global sign group
    }
    ಅನ್ಯಥಾ
    {
        sign_group = vim_strsave(sign_group);
        ಯದಿ (sign_group == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ಯದಿ (buf == ರಿಕ್ತ)
        ಗಚ್ಛ cleanup;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    ಪೂರ್ಣಾಙ್ಕ sign_id = 0;
    char_u *group = ರಿಕ್ತ;
    char_u *sign_name = ರಿಕ್ತ;
    buf_T *buf = ರಿಕ್ತ;
    dictitem_T *di = ರಿಕ್ತ;
    linenr_T lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ prio = -1;
    ಪೂರ್ಣಾಙ್ಕ ret_sign_id = -1;

    // sign identifier
    ಯದಿ (id_tv == ರಿಕ್ತ)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        ಯದಿ (di != ರಿಕ್ತ)
            id_tv = &di->di_tv;
    }

    ಯದಿ (id_tv == ರಿಕ್ತ)
    {
        sign_id = 0;
    }
    ಅನ್ಯಥಾ
    {
        ಪೂರ್ಣಾಙ್ಕ notanum = FALSE;
        sign_id = tv_get_ಸಂಖ್ಯೆ_chk(id_tv, &notanum);

        ಯದಿ (notanum)
            ಪ್ರತ್ಯಾಗಮ -1;

        ಯದಿ (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

    // sign group
    ಯದಿ (group_tv == ರಿಕ್ತ)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        ಯದಿ (di != ರಿಕ್ತ)
            group_tv = &di->di_tv;
    }

    ಯದಿ (group_tv == ರಿಕ್ತ)
    {
        group = ರಿಕ್ತ; // global group
    }
    ಅನ್ಯಥಾ
    {
        group = tv_get_string_chk(group_tv);
        ಯದಿ (group == ರಿಕ್ತ)
            ಗಚ್ಛ cleanup;

        ಯದಿ (group[0] == '\0') // global sign group
        {
            group = ರಿಕ್ತ;
        }
        ಅನ್ಯಥಾ
        {
            group = vim_strsave(group);
            ಯದಿ (group == ರಿಕ್ತ)
                ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

    // sign name
    ಯದಿ (name_tv == ರಿಕ್ತ)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        ಯದಿ (di != ರಿಕ್ತ)
            name_tv = &di->di_tv;
    }

    ಯದಿ (name_tv == ರಿಕ್ತ)
        ಗಚ್ಛ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ಯದಿ (sign_name == ರಿಕ್ತ)
        ಗಚ್ಛ cleanup;

    // buffer to place the sign
    ಯದಿ (buf_tv == ರಿಕ್ತ)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        ಯದಿ (di != ರಿಕ್ತ)
            buf_tv = &di->di_tv;
    }

    ಯದಿ (buf_tv == ರಿಕ್ತ)
        ಗಚ್ಛ cleanup;

    buf = get_buf_arg(buf_tv);
    ಯದಿ (buf == ರಿಕ್ತ)
        ಗಚ್ಛ cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ಯದಿ (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            ಗಚ್ಛ cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
        ಪೂರ್ಣಾಙ್ಕ notanum = FALSE;
        prio = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &notanum);
        ಯದಿ (notanum)
            ಗಚ್ಛ cleanup;
    }

    ಯದಿ (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    ಪ್ರತ್ಯಾಗಮ ret_sign_id;
}

/*
 * "sign_place()" function
 */
ಶೂನ್ಯ
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ರಿಕ್ತ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script() && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[4].v_type != VAR_UNKNOWN)
    {
        ಯದಿ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ಪ್ರತ್ಯಾಗಮ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
ಶೂನ್ಯ
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    // Process the List of sign attributes
    listitem_T *li = ರಿಕ್ತ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[0].vval.v_list, li)
    {
        ಪೂರ್ಣಾಙ್ಕ sign_id = -1;

        ಯದಿ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
                                           li->li_tv.vval.v_dict);
        ಅನ್ಯಥಾ
            emsg(_(e_dictionary_required));

        list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ಸ್ಥಿರ ಶೂನ್ಯ
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ರಿಕ್ತ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
        ಪೂರ್ಣಾಙ್ಕ retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ಯದಿ (name != ರಿಕ್ತ && (sign_undefine_by_name(name, TRUE) == OK))
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

    ಯದಿ (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ಯದಿ (rettv_list_alloc(rettv) == FAIL)
            ಪ್ರತ್ಯಾಗಮ;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ಯದಿ (name == ರಿಕ್ತ)
            ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    ಪೂರ್ಣಾಙ್ಕ sign_id = 0;
    buf_T *buf = ರಿಕ್ತ;
    char_u *group = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ retval = -1;

    // sign group
    group = (group_tv != ರಿಕ್ತ) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    ಯದಿ (group != ರಿಕ್ತ)
    {
        ಯದಿ (group[0] == '\0') // global sign group
        {
            group = ರಿಕ್ತ;
        }
        ಅನ್ಯಥಾ
        {
            group = vim_strsave(group);
            ಯದಿ (group == ರಿಕ್ತ)
                ಪ್ರತ್ಯಾಗಮ retval;
        }
    }

    ಯದಿ (dict != ರಿಕ್ತ)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        ಯದಿ (di != ರಿಕ್ತ)
        {
            buf = get_buf_arg(&di->di_tv);
            ಯದಿ (buf == ರಿಕ್ತ)
                ಗಚ್ಛ cleanup;
        }

        ಯದಿ (dict_has_ಕೀಲಿ(dict, "id"))
        {
            sign_id = dict_get_ಸಂಖ್ಯೆ(dict, "id");
            ಯದಿ (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                ಗಚ್ಛ cleanup;
            }
        }
    }

    ಯದಿ (buf == ರಿಕ್ತ)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ಯದಿ (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    ಪ್ರತ್ಯಾಗಮ retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    ಯಾವತ್ (sign != ರಿಕ್ತ && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    ಪ್ರತ್ಯಾಗಮ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
ಪೂರ್ಣಾಙ್ಕ
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ಯದಿ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ಪ್ರತ್ಯಾಗಮ get_first_valid_sign(wp) != ರಿಕ್ತ && !wp->w_p_nu && !wp->w_p_rnu;

    ಯದಿ (*wp->w_p_scl == 'n')
        ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (*wp->w_p_scl == 'y')
        ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ (get_first_valid_sign(wp) != ರಿಕ್ತ
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
ಶೂನ್ಯ
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ರಿಕ್ತ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_ಸಂಖ್ಯೆ = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
ಶೂನ್ಯ
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
        ಪ್ರತ್ಯಾಗಮ;

    listitem_T *li = ರಿಕ್ತ;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[0].vval.v_list, li)
    {
        ಪೂರ್ಣಾಙ್ಕ retval = -1;

        ಯದಿ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ರಿಕ್ತ, li->li_tv.vval.v_dict);
        ಅನ್ಯಥಾ
            emsg(_(e_dictionary_required));

        list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
