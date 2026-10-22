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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
வகைவரை கட்டமைப்பு sign sign_T;

கட்டமைப்பு sign
{
    sign_T *sn_next; // next sign in list
    முழு sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    வெறுமை *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    முழு sn_வரி_hl; // highlight ID for line
    முழு sn_text_hl; // highlight ID for text
    முழு sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    முழு sn_இலக்கம்_hl; // highlight ID for line number
    முழு sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

நிலையான sign_T *first_sign = வெற்று;
நிலையான முழு next_sign_typenr = 1;

நிலையான வெறுமை sign_list_defined(sign_T *sp);
நிலையான வெறுமை sign_undefine(sign_T *sp, sign_T *sp_prev);

நிலையான எழுத்து *cmds[] = { "define",
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
                        வெற்று
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     க்கு ((sp) = first_sign; (sp) != வெற்று; (sp) = (sp)->sn_next)

நிலையான hashtab_T sg_table; // sign group (signgroup_T) hashtable
நிலையான முழு next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
வெறுமை
init_signs(வெறுமை)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
நிலையான signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = வெற்று;

    எனில் (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        எனில் (group == வெற்று)
            திரும்பு வெற்று;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    இல்லையெனில்
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    திரும்பு group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
நிலையான வெறுமை
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    எனில் (HASHITEM_EMPTY(hi))
        திரும்பு;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    எனில் (group->sg_refcount == 0)
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
நிலையான முழு
sign_in_group(sign_entry_T *sign, char_u *group)
{
    திரும்பு ((group != வெற்று && STRCMP(group, "*") == 0) ||
            (group == வெற்று && sign->se_group == வெற்று) ||
            (group != வெற்று && sign->se_group != வெற்று &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
நிலையான முழு
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    முழு for_popup = sign->se_group != வெற்று &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    திரும்பு WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
நிலையான முழு
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    முழு id = 1;
    signgroup_T *group = வெற்று;
    sign_entry_T *sign = வெற்று;

    எனில் (groupname != வெற்று)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        எனில் (HASHITEM_EMPTY(hi))
            திரும்பு id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    முழு found = FALSE;
    போது (!found)
    {
        எனில் (group == வெற்று)
            id = next_sign_id++; // global group
        இல்லையெனில்
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            எனில் (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                நிறுத்து;
            }
        }
    }

    திரும்பு id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
நிலையான வெறுமை
செருகு_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            முழு id, // sign ID
            char_u *group, // sign group; NULL for global group
            முழு prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            முழு typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(அளவு(sign_entry_T), FALSE, aid_செருகு_sign);
    எனில் (newsign == வெற்று)
        திரும்பு;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    எனில் (group != வெற்று)
    {
        newsign->se_group = sign_group_ref(group);
        எனில் (newsign->se_group == வெற்று)
        {
            vim_free(newsign);
            திரும்பு;
        }
    }
    இல்லையெனில்
    {
        newsign->se_group = வெற்று;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    எனில் (next != வெற்று)
        next->se_prev = newsign;

    எனில் (prev == வெற்று)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        எனில் (buf->b_signlist == வெற்று)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_வரி_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        எனில் (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    இல்லையெனில்
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
நிலையான வெறுமை
செருகு_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         முழு id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         முழு prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         முழு typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    போது (prev != வெற்று && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == வெற்று) ? buf->b_signlist : prev->se_next;

    செருகு_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
நிலையான sign_T *
find_sign_by_typenr(முழு typenr)
{
    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
        எனில் (sp->sn_typenr == typenr)
            திரும்பு sp;
    திரும்பு வெற்று;
}

/*
 * Get the name of a sign by its typenr.
 */
நிலையான char_u *
sign_typenr2name(முழு typenr)
{
    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
        எனில் (sp->sn_typenr == typenr)
            திரும்பு sp->sn_name;
    திரும்பு (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
நிலையான dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    எனில் (d == வெற்று)
        திரும்பு வெற்று;

    dict_add_எண்(d, "id", sign->se_id);
    எனில் (sign->se_group == வெற்று)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    இல்லையெனில்
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_எண்(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_எண்(d, "priority", sign->se_priority);

    திரும்பு d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
நிலையான வெறுமை
sign_sort_by_prio_on_வரி(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    எனில் ((sign->se_prev == வெற்று || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == வெற்று || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        திரும்பு;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    போது (p->se_prev != வெற்று && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    எனில் (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        போது (p->se_next != வெற்று && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    எனில் (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    எனில் (sign->se_prev != வெற்று)
        sign->se_prev->se_next = sign->se_next;

    எனில் (sign->se_next != வெற்று)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = வெற்று;
    sign->se_next = வெற்று;

    // Re-insert 'sign' at the right place
    எனில் (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        எனில் (sign->se_prev != வெற்று)
            sign->se_prev->se_next = sign;

        எனில் (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    இல்லையெனில்
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        எனில் (sign->se_next != வெற்று)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
நிலையான வெறுமை
buf_addsign(buf_T *buf, // buffer to store sign in
            முழு id, // sign ID
            char_u *groupname, // sign group
            முழு prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            முழு typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    sign_entry_T *prev = வெற்று; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    எனில் (buf->b_sign_finger != வெற்று && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    க்கு (sign = (prev == வெற்று) ? buf->b_signlist : prev->se_next; sign != வெற்று;
         sign = sign->se_next)
    {
        எனில் (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_வரி(buf, sign);
            திரும்பு;
        }
        இல்லையெனில் எனில் (lnum < sign->se_lnum)
        {
            செருகு_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            திரும்பு;
        }
        prev = sign;
    }

    செருகு_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
நிலையான linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     முழு markId, // sign ID
                     char_u *group, // sign group
                     முழு typenr, // typenr of sign we are adding
                     முழு prio) // sign priority
{
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        எனில் (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_வரி(buf, sign);
            திரும்பு sign->se_lnum;
        }
    }

    திரும்பு (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
முழு
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = வெற்று;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        எனில் (sign->se_lnum > lnum)
            நிறுத்து;

        எனில் (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            எனில் (sp == வெற்று)
                திரும்பு FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            எனில் (sattr->sat_text != வெற்று && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            எனில் (sp->sn_வரி_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_வரி_hl);

            எனில் (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            எனில் (sp->sn_இலக்கம்_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_இலக்கம்_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            எனில் (sign->se_next != வெற்று &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                எனில் (next_sp == வெற்று)
                    திரும்பு FALSE;

                எனில் (sattr->sat_icon == வெற்று && sattr->sat_text == வெற்று)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                எனில் (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                எனில் (sp->sn_வரி_hl <= 0 && next_sp->sn_வரி_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_வரி_hl);

                எனில் (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                எனில் (sp->sn_இலக்கம்_hl <= 0 && next_sp->sn_இலக்கம்_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_இலக்கம்_hl);
            }
            திரும்பு TRUE;
        }
    }
    திரும்பு FALSE;
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
            முழு id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = வெற்று; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = வெற்று; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    க்கு (sign_entry_T *sign = buf->b_signlist; sign != வெற்று; sign = next)
    {
        next = sign->se_next;

        எனில் ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            எனில் (next != வெற்று)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            எனில் (sign->se_group != வெற்று)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_வரி_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            எனில் (group == வெற்று || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                நிறுத்து;
        }
        இல்லையெனில்
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    எனில் (buf->b_signlist == வெற்று)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_வரி_abv_curs();
    }

    திரும்பு lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
முழு
buf_findsign(buf_T *buf, // buffer to store sign in
             முழு id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        எனில் (sign->se_id == id && sign_in_group(sign, group))
            திரும்பு sign->se_lnum;

    திரும்பு 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
நிலையான sign_entry_T *
buf_getsign_at_வரி(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        எனில் (sign->se_lnum > lnum)
            நிறுத்து;

        எனில் (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            திரும்பு sign;
    }

    திரும்பு வெற்று;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
முழு
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_வரி(buf, lnum, groupname);
    எனில் (sign != வெற்று)
        திரும்பு sign->se_id;

    திரும்பு 0;
}

# எனில் defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
முழு
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    முழு typenr) // sign type number
{
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        எனில் (sign->se_lnum > lnum)
            நிறுத்து;

        எனில் (sign->se_lnum == lnum && sign->se_typenr == typenr)
            திரும்பு sign->se_id;
    }

    திரும்பு 0;
}

#  எனில் defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
முழு
buf_signcount(buf_T *buf, linenr_T lnum)
{
    முழு மொத்தம் = 0;
    sign_entry_T *sign = வெற்று; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        எனில் (sign->se_lnum > lnum)
            நிறுத்து;

        எனில் (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != வெற்று)
            மொத்தம்++;
    }

    திரும்பு மொத்தம்;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
வெறுமை
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = வெற்று; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    எனில் (buf->b_signlist != வெற்று && curwin != வெற்று)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_வரி_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = வெற்று;

    க்கு (sign_entry_T *sign = buf->b_signlist; sign != வெற்று; sign = next)
    {
        next = sign->se_next;
        எனில் (sign_in_group(sign, group))
        {
            *lastp = next;

            எனில் (next != வெற்று)
                next->se_prev = sign->se_prev;

            எனில் (sign->se_group != வெற்று)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        இல்லையெனில்
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
நிலையான வெறுமை
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    எழுத்து lbuf[செய்தி_BUF_LEN];
    எழுத்து group[செய்தி_BUF_LEN];

    செய்தி_puts_title(_("\n--- Signs ---"));
    செய்தி_putchar('\n');

    buf_T *buf = (rbuf == வெற்று) ? firstbuf : rbuf;
    போது (buf != வெற்று && !got_int)
    {
        எனில் (buf->b_signlist != வெற்று)
        {
            vim_snprintf(lbuf, செய்தி_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            செய்தி_puts_attr(lbuf, HL_ATTR(HLF_D));
            செய்தி_putchar('\n');
        }

        sign_entry_T *sign = வெற்று;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            எனில் (got_int)
                நிறுத்து;

            எனில் (!sign_in_group(sign, sign_group))
                தொடர்;

            எனில் (sign->se_group != வெற்று)
                vim_snprintf(group, செய்தி_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            இல்லையெனில்
                group[0] = '\0';

            vim_snprintf(lbuf, செய்தி_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            செய்தி_puts(lbuf);
            செய்தி_putchar('\n');
        }

        எனில் (rbuf != வெற்று)
            நிறுத்து;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
வெறுமை
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = வெற்று; // line changes may reorder, drop the finger
    sign_entry_T *sign = வெற்று; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        எனில் (sign->se_lnum < line1)
            தொடர்;

        linenr_T new_lnum = sign->se_lnum;

        எனில் (sign->se_lnum <= line2)
        {
            எனில் (amount != MAXLNUM)
                new_lnum += amount;
        }
        இல்லையெனில் எனில் (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        எனில் (new_lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
நிலையான முழு
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    முழு idx = 0;
    எழுத்து save = *end_cmd;
    *end_cmd = NUL;

    போது (cmds[idx] != வெற்று && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    திரும்பு idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
நிலையான sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    எனில் (sp_prev != வெற்று)
        *sp_prev = வெற்று;

    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
    {
        எனில் (STRCMP(sp->sn_name, name) == 0)
            நிறுத்து;

        எனில் (sp_prev != வெற்று)
            *sp_prev = sp;
    }

    திரும்பு sp;
}

/*
 * Allocate a new sign
 */
நிலையான sign_T *
alloc_new_sign(char_u *name)
{
    முழு start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(அளவு(sign_T), aid_sign_define_by_name);
    எனில் (sp == வெற்று)
        திரும்பு வெற்று;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    போது (lp != வெற்று)
    {
        எனில் (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            எனில் (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            எனில் (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                திரும்பு வெற்று;
            }

            lp = first_sign; // start all over
            தொடர்;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    எனில் (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    எனில் (sp->sn_name == வெற்று) // out of memory
    {
        vim_free(sp);
        திரும்பு வெற்று;
    }

    திரும்பு sp;
}

/*
 * Initialize the icon information for a new sign
 */
நிலையான வெறுமை
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    எனில் (gui.in_use)
    {
        out_flush();
        எனில் (sp->sn_image != வெற்று)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
நிலையான முழு
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = வெற்று;
    char_u *endp = text + (முழு)STRLEN(text);
    முழு cells = 0;

    // Remove backslashes so that it is possible to use a space.
    க்கு (s = text; s + 1 < endp; ++s)
    {
        எனில் (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    எனில் (has_mbyte)
    {
        க்கு (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            எனில் (!vim_isprintc((*mb_ptr2char)(s)))
                நிறுத்து;

            cells += (*mb_ptr2cells)(s);
        }
    }
    இல்லையெனில்
    {
        க்கு (s = text; s < endp; ++s)
        {
            எனில் (!vim_isprintc(*s))
                நிறுத்து;
        }
        cells = (முழு)(s - text);
    }

    // Currently sign text must be one or two display cells
    எனில் (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        திரும்பு FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    முழு len = (முழு)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    எனில் (sp->sn_text != வெற்று && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    திரும்பு OK;
}

/*
 * Define a new sign or update an existing sign
 */
முழு
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    முழு prio)
{
    sign_T *sp_prev = வெற்று;
    sign_T *sp = sign_find(name, &sp_prev);
    எனில் (sp == வெற்று)
    {
        sp = alloc_new_sign(name);
        எனில் (sp == வெற்று)
            திரும்பு FAIL;

        // add the new sign to the list of signs
        எனில் (sp_prev == வெற்று)
            first_sign = sp;
        இல்லையெனில்
            sp_prev->sn_next = sp;
    }
    இல்லையெனில்
    {
        win_T *wp = வெற்று;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            எனில் (wp->w_buffer->b_signlist != வெற்று)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    எனில் (icon != வெற்று)
        sign_define_init_icon(sp, icon);

    எனில் (text != வெற்று && (sign_define_init_text(sp, text) == FAIL))
        திரும்பு FAIL;

    sp->sn_priority = prio;

    எனில் (linehl != வெற்று)
    {
        எனில் (*linehl == NUL)
            sp->sn_வரி_hl = 0;
        இல்லையெனில்
            sp->sn_வரி_hl = syn_check_group(linehl, (முழு)STRLEN(linehl));
    }

    எனில் (texthl != வெற்று)
    {
        எனில் (*texthl == NUL)
            sp->sn_text_hl = 0;
        இல்லையெனில்
            sp->sn_text_hl = syn_check_group(texthl, (முழு)STRLEN(texthl));
    }

    எனில் (culhl != வெற்று)
    {
        எனில் (*culhl == NUL)
            sp->sn_cul_hl = 0;
        இல்லையெனில்
            sp->sn_cul_hl = syn_check_group(culhl, (முழு)STRLEN(culhl));
    }

    எனில் (numhl != வெற்று)
    {
        எனில் (*numhl == NUL)
            sp->sn_இலக்கம்_hl = 0;
        இல்லையெனில்
            sp->sn_இலக்கம்_hl = syn_check_group(numhl, (முழு)STRLEN(numhl));
    }

    திரும்பு OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
முழு
sign_exists_by_name(char_u *name)
{
    திரும்பு sign_find(name, வெற்று) != வெற்று;
}

/*
 * Free the sign specified by 'name'.
 */
முழு
sign_undefine_by_name(char_u *name, முழு give_error)
{
    sign_T *sp_prev = வெற்று;
    sign_T *sp = sign_find(name, &sp_prev);
    எனில் (sp == வெற்று)
    {
        எனில் (give_error)
            semsg(_(e_unknown_sign_str), name);
        திரும்பு FAIL;
    }
    sign_undefine(sp, sp_prev);

    திரும்பு OK;
}

/*
 * List the signs matching 'name'
 */
நிலையான வெறுமை
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, வெற்று);
    எனில் (sp != வெற்று)
        sign_list_defined(sp);
    இல்லையெனில்
        semsg(_(e_unknown_sign_str), name);
}

நிலையான வெறுமை
may_force_numberwidth_recompute(buf_T *buf, முழு unplace)
{
# எனில் defined(FEAT_LINEBREAK)
    tabpage_T *tp = வெற்று;
    win_T *wp = வெற்று;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        எனில் (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_வரி_மொத்தம் = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
முழு
sign_place(முழு *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           முழு prio)
{
    // Check for reserved character '*' in group name
    எனில் (sign_group != வெற்று && (*sign_group == '*' || *sign_group == '\0'))
        திரும்பு FAIL;

    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
    {
        எனில் (STRCMP(sp->sn_name, sign_name) == 0)
            நிறுத்து;
    }

    எனில் (sp == வெற்று)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        திரும்பு FAIL;
    }

    எனில் (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    எனில் (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    எனில் (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    இல்லையெனில்
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    எனில் (lnum > 0)
    {
        redraw_buf_வரி_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    இல்லையெனில்
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Unplace the specified sign
 */
நிலையான முழு
sign_unplace(முழு sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    எனில் (buf->b_signlist == வெற்று) // No signs in the buffer
        திரும்பு OK;

    எனில் (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    இல்லையெனில்
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        எனில் (lnum == 0)
            திரும்பு FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    எனில் (buf->b_signlist == வெற்று)
        may_force_numberwidth_recompute(buf, TRUE);

    திரும்பு OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
நிலையான வெறுமை
sign_unplace_at_cursor(char_u *groupname)
{
    முழு id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    எனில் (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    இல்லையெனில்
        emsg(_(e_missing_sign_எண்));
}

/*
 * Jump to a sign.
 */
நிலையான linenr_T
sign_jump(முழு sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    எனில் (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        திரும்பு -1;
    }

    // goto a sign ...
    எனில் (buf_jump_open_win(buf) != வெற்று)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    இல்லையெனில்
    { // ... not currently in a window
        எனில் (buf->b_fname == வெற்று)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            திரும்பு -1;
        }
        setpcmark();
        எனில் (do_ecmd(buf->b_fnum, வெற்று, வெற்று, வெற்று, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            திரும்பு -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    திரும்பு lnum;
}

/*
 * ":sign define {name} ..." command
 */
நிலையான வெறுமை
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = வெற்று;
    char_u *p = cmdline;
    char_u *icon = வெற்று;
    char_u *text = வெற்று;
    char_u *linehl = வெற்று;
    char_u *texthl = வெற்று;
    char_u *culhl = வெற்று;
    char_u *numhl = வெற்று;
    முழு prio = -1;
    முழு failed = FALSE;

    // set values for a defined sign.
    போது (TRUE)
    {
        arg = skipwhite(p);
        எனில் (*arg == NUL)
            நிறுத்து;

        p = skiptowhite_esc(arg);
        எனில் (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((எழுத்து *)arg);
        }
        இல்லையெனில்
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            நிறுத்து;
        }
    }

    எனில் (!failed)
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
நிலையான வெறுமை
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               முழு id,
               char_u *group,
               முழு prio)
{
    எனில் (id <= 0)
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
        எனில் (lnum >= 0 || sign_name != வெற்று || (group != வெற்று && *group == '\0'))
            emsg(_(e_invalid_argument));
        இல்லையெனில்
            sign_list_placed(buf, group);
    }
    இல்லையெனில்
    {
        // Place a new sign
        எனில் (sign_name == வெற்று || buf == வெற்று ||
            (group != வெற்று && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            திரும்பு;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
நிலையான வெறுமை
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 முழு id,
                 char_u *group)
{
    எனில் (lnum >= 0 || sign_name != வெற்று || (group != வெற்று && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        திரும்பு;
    }

    எனில் (id == -2)
    {
        எனில் (buf != வெற்று)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        இல்லையெனில்
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                எனில் (buf->b_signlist != வெற்று)
                    buf_delete_signs(buf, group);
            }
        }
    }
    இல்லையெனில்
    {
        எனில் (buf != வெற்று)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        இல்லையெனில்
        {
            எனில் (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            இல்லையெனில்
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
நிலையான வெறுமை
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              முழு id,
              char_u *group)
{
    எனில் (sign_name == வெற்று && group == வெற்று && id == -1)
    {
        emsg(_(e_argument_required));
        திரும்பு;
    }

    எனில் (buf == வெற்று || (group != வெற்று && *group == '\0') || lnum >= 0 ||
        sign_name != வெற்று)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        திரும்பு;
    }

    (வெறுமை)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
நிலையான முழு
parse_sign_cmd_args(முழு cmd,
                    char_u *arg,
                    char_u **sign_name,
                    முழு *signid,
                    char_u **group,
                    முழு *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = வெற்று;
    முழு lnum_arg = FALSE;

    // first arg could be placed sign id
    எனில் (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        எனில் (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        இல்லையெனில்
        {
            arg = skipwhite(arg);
        }
    }

    போது (*arg != NUL)
    {
        எனில் (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((எழுத்து *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            எனில் (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                திரும்பு FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            எனில் (*arg != NUL)
                *arg++ = NUL;

            போது (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            எனில் (*arg != NUL)
                *arg++ = NUL;
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((எழுத்து *)arg);
            arg = skiptowhite(arg);
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            நிறுத்து;
        }
        இல்லையெனில் எனில் (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((முழு)getdigits(&arg));

            எனில் (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            நிறுத்து;
        }
        இல்லையெனில்
        {
            emsg(_(e_invalid_argument));
            திரும்பு FAIL;
        }

        arg = skipwhite(arg);
    }

    எனில் (filename != வெற்று && *buf == வெற்று)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        திரும்பு FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    எனில் (filename == வெற்று &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    திரும்பு OK;
}

/*
 * ":sign" command
 */
வெறுமை
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    முழு idx = sign_cmd_idx(arg, p);
    எனில் (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        திரும்பு;
    }
    arg = skipwhite(p);

    எனில் (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = வெற்று;
        முழு id = -1;
        char_u *group = வெற்று;
        முழு prio = -1;
        buf_T *buf = வெற்று;
        linenr_T lnum = -1;

        // Parse command line arguments
        எனில் (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            திரும்பு;

        எனில் (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        இல்லையெனில் எனில் (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        இல்லையெனில் எனில் (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        திரும்பு;
    }

    // Define, undefine or list signs.
    எனில் (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        க்கு (sign_T *sp = first_sign; sp != வெற்று && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    இல்லையெனில் எனில் (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    இல்லையெனில்
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        எனில் (*p != NUL)
            *p++ = NUL;

        போது (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        எனில் (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        இல்லையெனில் எனில் (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        இல்லையெனில்
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
நிலையான வெறுமை
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    எனில் (sp->sn_icon != வெற்று)
        dict_add_string(retdict, "icon", sp->sn_icon);

    எனில் (sp->sn_text != வெற்று)
        dict_add_string(retdict, "text", sp->sn_text);

    எனில் (sp->sn_priority > 0)
        dict_add_எண்(retdict, "priority", sp->sn_priority);

    எனில் (sp->sn_வரி_hl > 0)
    {
        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_வரி_hl - 1, FALSE);
        எனில் (p == வெற்று)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        இல்லையெனில்
            dict_add_string(retdict, "linehl", p);
    }

    எனில் (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_text_hl - 1, FALSE);
        எனில் (p == வெற்று)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        இல்லையெனில்
            dict_add_string(retdict, "texthl", p);
    }

    எனில் (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_cul_hl - 1, FALSE);
        எனில் (p == வெற்று)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        இல்லையெனில்
            dict_add_string(retdict, "culhl", p);
    }

    எனில் (sp->sn_இலக்கம்_hl > 0)
    {
        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_இலக்கம்_hl - 1, FALSE);
        எனில் (p == வெற்று)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        இல்லையெனில்
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
நிலையான வெறுமை
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    எனில் (name != வெற்று)
    {
        sp = sign_find(name, வெற்று);
        எனில் (sp == வெற்று)
            திரும்பு;
    }

    க்கு (; sp != வெற்று && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        எனில் (dict == வெற்று)
            திரும்பு;

        எனில் (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            திரும்பு;
        }

        sign_getinfo(sp, dict);

        எனில் (name != வெற்று) // handle only the specified sign
            நிறுத்து;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
வெறுமை
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = வெற்று;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        எனில் (d != வெற்று && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
நிலையான வெறுமை
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       முழு sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    எனில் (d == வெற்று)
        திரும்பு;

    எனில் (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        திரும்பு;
    }

    dict_add_எண்(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    எனில் (l == வெற்று)
        திரும்பு;

    எனில் (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        திரும்பு;
    }

    sign_entry_T *sign = வெற்று;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        எனில் (!sign_in_group(sign, sign_group))
            தொடர்;

        எனில் ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            எனில் (sdict != வெற்று && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
நிலையான வெறுமை
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                முழு sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    எனில் (buf != வெற்று)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    இல்லையெனில்
    {
        FOR_ALL_BUFFERS(buf)
        {
            எனில் (buf->b_signlist != வெற்று)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# எனில் defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
வெறுமை
sign_gui_started(வெறுமை)
{
    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
    {
        எனில் (sp->sn_icon != வெற்று)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
நிலையான வெறுமை
sign_list_defined(sign_T *sp)
{
    எழுத்து lbuf[செய்தி_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    எனில் (sp->sn_icon != வெற்று)
    {
        செய்தி_puts(" icon=");
        செய்தி_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        எனில் (sp->sn_image == வெற்று)
            செய்தி_puts(_(" (NOT FOUND)"));
# இல்லையெனில்
        செய்தி_puts(_(" (not supported)"));
# endif
    }

    எனில் (sp->sn_text != வெற்று)
    {
        செய்தி_puts(" text=");
        செய்தி_outtrans(sp->sn_text);
    }

    எனில் (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, செய்தி_BUF_LEN, " priority=%d", sp->sn_priority);
        செய்தி_puts(lbuf);
    }

    எனில் (sp->sn_வரி_hl > 0)
    {
        செய்தி_puts(" linehl=");

        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_வரி_hl - 1, FALSE);
        எனில் (p == வெற்று)
            செய்தி_puts("NONE");
        இல்லையெனில்
            செய்தி_puts((எழுத்து *)p);
    }

    எனில் (sp->sn_text_hl > 0)
    {
        செய்தி_puts(" texthl=");

        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_text_hl - 1, FALSE);
        எனில் (p == வெற்று)
            செய்தி_puts("NONE");
        இல்லையெனில்
            செய்தி_puts((எழுத்து *)p);
    }

    எனில் (sp->sn_cul_hl > 0)
    {
        செய்தி_puts(" culhl=");

        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_cul_hl - 1, FALSE);
        எனில் (p == வெற்று)
            செய்தி_puts("NONE");
        இல்லையெனில்
            செய்தி_puts((எழுத்து *)p);
    }

    எனில் (sp->sn_இலக்கம்_hl > 0)
    {
        செய்தி_puts(" numhl=");

        char_u *p = get_highlight_name_ext(வெற்று, sp->sn_இலக்கம்_hl - 1, FALSE);
        எனில் (p == வெற்று)
            செய்தி_puts("NONE");
        இல்லையெனில்
            செய்தி_puts((எழுத்து *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
நிலையான வெறுமை
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    எனில் (sp->sn_image != வெற்று)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    எனில் (sp_prev == வெற்று)
        first_sign = sp->sn_next;
    இல்லையெனில்
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# எனில் defined(FEAT_SIGN_ICONS)
வெறுமை *
sign_get_image(முழு typenr) // the attribute which may have a sign
{
    sign_T *sp = வெற்று;
    FOR_ALL_SIGNS(sp)
    {
        எனில் (sp->sn_typenr == typenr)
            திரும்பு sp->sn_image;
    }
    திரும்பு வெற்று;
}
# endif

/*
 * Undefine/free all signs.
 */
வெறுமை
free_signs(வெறுமை)
{
    போது (first_sign != வெற்று)
        sign_undefine(first_sign, வெற்று);
}

நிலையான எண்ணிக்கை
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
நிலையான char_u *
get_nth_sign_name(முழு idx)
{
    முழு current_idx = 0;
    sign_T *sp = வெற்று;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        எனில் (current_idx++ == idx)
            திரும்பு sp->sn_name;
    }
    திரும்பு வெற்று;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
நிலையான char_u *
get_nth_sign_group_name(முழு idx)
{
    முழு current_idx = 0;
    முழு todo = (முழு)sg_table.ht_used;
    hashitem_T *hi = வெற்று;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_பொருட்கள்(&sg_table, hi, todo)
    {
        எனில் (!HASHITEM_EMPTY(hi))
        {
            --todo;
            எனில் (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                திரும்பு group->sg_name;
            }
        }
    }
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, முழு idx)
{
    தேர்வு (expand_what)
    {
        நிலை EXP_SUBCMD:
            திரும்பு (char_u *)cmds[idx];
        நிலை EXP_DEFINE:
        {
            எழுத்து *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", வெற்று };
            திரும்பு (char_u *)define_arg[idx];
        }
        நிலை EXP_PLACE:
        {
            எழுத்து *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", வெற்று };
            திரும்பு (char_u *)place_arg[idx];
        }
        நிலை EXP_LIST:
        {
            எழுத்து *list_arg[] = { "group=", "file=", "buffer=", வெற்று };
            திரும்பு (char_u *)list_arg[idx];
        }
        நிலை EXP_UNPLACE:
        {
            எழுத்து *unplace_arg[] = { "group=", "file=", "buffer=", வெற்று };
            திரும்பு (char_u *)unplace_arg[idx];
        }
        நிலை EXP_SIGN_NAMES:
            திரும்பு get_nth_sign_name(idx);
        நிலை EXP_SIGN_GROUPS:
            திரும்பு get_nth_sign_group_name(idx);
        இயல்பு:
            திரும்பு வெற்று;
    }
}

/*
 * Handle command line completion for :sign command.
 */
வெறுமை
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    முழு cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    எனில் (*end_subcmd == NUL)
        திரும்பு;

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
    செய்
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    போது (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    எனில் (p == வெற்று)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        தேர்வு (cmd_idx)
        {
            நிலை SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                நிறுத்து;
            நிலை SIGNCMD_PLACE:
                // List placed signs
                எனில் (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                இல்லையெனில்
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                நிறுத்து;
            நிலை SIGNCMD_LIST:
            நிலை SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                நிறுத்து;
            நிலை SIGNCMD_JUMP:
            நிலை SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                நிறுத்து;
            இயல்பு:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    இல்லையெனில்
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        தேர்வு (cmd_idx)
        {
            நிலை SIGNCMD_DEFINE:
                எனில் (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                இல்லையெனில் எனில் (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                இல்லையெனில்
                    xp->xp_context = EXPAND_NOTHING;
                நிறுத்து;
            நிலை SIGNCMD_PLACE:
                எனில் (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                இல்லையெனில் எனில் (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                இல்லையெனில் எனில் (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                இல்லையெனில்
                    xp->xp_context = EXPAND_NOTHING;
                நிறுத்து;
            நிலை SIGNCMD_UNPLACE:
            நிலை SIGNCMD_JUMP:
                எனில் (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                இல்லையெனில் எனில் (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                இல்லையெனில்
                    xp->xp_context = EXPAND_NOTHING;
                நிறுத்து;
            இயல்பு:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
நிலையான முழு
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = வெற்று;
    char_u *icon = வெற்று;
    char_u *linehl = வெற்று;
    char_u *text = வெற்று;
    char_u *texthl = வெற்று;
    char_u *culhl = வெற்று;
    char_u *numhl = வெற்று;
    முழு prio = -1;
    முழு retval = -1;

    எனில் (name_arg == வெற்று && dict == வெற்று)
        திரும்பு retval;

    எனில் (name_arg == வெற்று)
        name = dict_get_string(dict, "name", TRUE);
    இல்லையெனில்
        name = vim_strsave(name_arg);

    எனில் (name == வெற்று || name[0] == NUL)
        செல் cleanup;

    எனில் (dict != வெற்று)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_எண்_def(dict, "priority", -1);
    }

    எனில் (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    திரும்பு retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
நிலையான வெறுமை
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = வெற்று;
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
        முழு retval = -1;

        எனில் (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(வெற்று, li->li_tv.vval.v_dict);
        இல்லையெனில்
            emsg(_(e_dictionary_required));

        list_append_எண்(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
வெறுமை
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        திரும்பு;

    எனில் (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        எனில் (rettv_list_alloc(rettv) == FAIL)
            திரும்பு;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        திரும்பு;
    }

    // Define a single sign
    rettv->vval.v_எண் = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    எனில் (name == வெற்று)
        திரும்பு;

    எனில் (check_for_opt_dict_arg(argvars, 1) == FAIL)
        திரும்பு;

    rettv->vval.v_எண் = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : வெற்று);
}

/*
 * "sign_getdefined()" function
 */
வெறுமை
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    எனில் (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        திரும்பு;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        திரும்பு;

    char_u *name = வெற்று;
    எனில் (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
வெறுமை
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = வெற்று;
    linenr_T lnum = 0;
    முழு sign_id = 0;
    char_u *group = வெற்று;

    எனில் (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        திரும்பு;

    எனில் (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        எனில் (buf == வெற்று)
            திரும்பு;

        எனில் (argvars[1].v_type != VAR_UNKNOWN)
        {
            எனில் (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                திரும்பு;

            dictitem_T *di = வெற்று;
            dict_T *dict = argvars[1].vval.v_dict;

            எனில் ((di = dict_find(dict, (char_u *)"lnum", -1)) != வெற்று)
            {
                // get signs placed at this line
                முழு notanum = FALSE;
                tv_get_எண்_chk(&di->di_tv, &notanum);
                எனில் (notanum)
                    திரும்பு;

                lnum = tv_get_lnum(&di->di_tv);
            }

            எனில் ((di = dict_find(dict, (char_u *)"id", -1)) != வெற்று)
            {
                // get sign placed with this identifier
                முழு notanum = FALSE;
                sign_id = (முழு)tv_get_எண்_chk(&di->di_tv, &notanum);
                எனில் (notanum)
                    திரும்பு;
            }

            எனில் ((di = dict_find(dict, (char_u *)"group", -1)) != வெற்று)
            {
                group = tv_get_string_chk(&di->di_tv);
                எனில் (group == வெற்று)
                    திரும்பு;

                எனில் (*group == '\0') // empty string means global group
                    group = வெற்று;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
வெறுமை
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = வெற்று;
    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script() && (check_for_எண்_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        திரும்பு;

    முழு notanum = FALSE;
    // Sign identifier
    முழு sign_id = (முழு)tv_get_எண்_chk(&argvars[0], &notanum);
    எனில் (notanum)
        திரும்பு;

    எனில் (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        திரும்பு;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    எனில் (sign_group == வெற்று)
        திரும்பு;

    எனில் (sign_group[0] == '\0')
    {
        sign_group = வெற்று; // global sign group
    }
    இல்லையெனில்
    {
        sign_group = vim_strsave(sign_group);
        எனில் (sign_group == வெற்று)
            திரும்பு;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    எனில் (buf == வெற்று)
        செல் cleanup;

    rettv->vval.v_எண் = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
நிலையான முழு
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    முழு sign_id = 0;
    char_u *group = வெற்று;
    char_u *sign_name = வெற்று;
    buf_T *buf = வெற்று;
    dictitem_T *di = வெற்று;
    linenr_T lnum = 0;
    முழு prio = -1;
    முழு ret_sign_id = -1;

    // sign identifier
    எனில் (id_tv == வெற்று)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        எனில் (di != வெற்று)
            id_tv = &di->di_tv;
    }

    எனில் (id_tv == வெற்று)
    {
        sign_id = 0;
    }
    இல்லையெனில்
    {
        முழு notanum = FALSE;
        sign_id = tv_get_எண்_chk(id_tv, &notanum);

        எனில் (notanum)
            திரும்பு -1;

        எனில் (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            திரும்பு -1;
        }
    }

    // sign group
    எனில் (group_tv == வெற்று)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        எனில் (di != வெற்று)
            group_tv = &di->di_tv;
    }

    எனில் (group_tv == வெற்று)
    {
        group = வெற்று; // global group
    }
    இல்லையெனில்
    {
        group = tv_get_string_chk(group_tv);
        எனில் (group == வெற்று)
            செல் cleanup;

        எனில் (group[0] == '\0') // global sign group
        {
            group = வெற்று;
        }
        இல்லையெனில்
        {
            group = vim_strsave(group);
            எனில் (group == வெற்று)
                திரும்பு -1;
        }
    }

    // sign name
    எனில் (name_tv == வெற்று)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        எனில் (di != வெற்று)
            name_tv = &di->di_tv;
    }

    எனில் (name_tv == வெற்று)
        செல் cleanup;

    sign_name = tv_get_string_chk(name_tv);
    எனில் (sign_name == வெற்று)
        செல் cleanup;

    // buffer to place the sign
    எனில் (buf_tv == வெற்று)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        எனில் (di != வெற்று)
            buf_tv = &di->di_tv;
    }

    எனில் (buf_tv == வெற்று)
        செல் cleanup;

    buf = get_buf_arg(buf_tv);
    எனில் (buf == வெற்று)
        செல் cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    எனில் (di != வெற்று)
    {
        lnum = tv_get_lnum(&di->di_tv);
        எனில் (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            செல் cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    எனில் (di != வெற்று)
    {
        முழு notanum = FALSE;
        prio = (முழு)tv_get_எண்_chk(&di->di_tv, &notanum);
        எனில் (notanum)
            செல் cleanup;
    }

    எனில் (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    திரும்பு ret_sign_id;
}

/*
 * "sign_place()" function
 */
வெறுமை
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = வெற்று;
    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script() && (check_for_எண்_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        திரும்பு;

    எனில் (argvars[4].v_type != VAR_UNKNOWN)
    {
        எனில் (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            திரும்பு;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_எண் = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
வெறுமை
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) == FAIL)
        திரும்பு;

    எனில் (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        திரும்பு;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
        திரும்பு;

    // Process the List of sign attributes
    listitem_T *li = வெற்று;
    FOR_ALL_LIST_பொருட்கள்(argvars[0].vval.v_list, li)
    {
        முழு sign_id = -1;

        எனில் (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(வெற்று, வெற்று, வெற்று, வெற்று,
                                           li->li_tv.vval.v_dict);
        இல்லையெனில்
            emsg(_(e_dictionary_required));

        list_append_எண்(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
நிலையான வெறுமை
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = வெற்று;
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
        முழு retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        எனில் (name != வெற்று && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_எண்(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
வெறுமை
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    எனில் (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        திரும்பு;

    எனில் (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        எனில் (rettv_list_alloc(rettv) == FAIL)
            திரும்பு;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        திரும்பு;
    }

    rettv->vval.v_எண் = -1;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_எண் = 0;
    }
    இல்லையெனில்
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        எனில் (name == வெற்று)
            திரும்பு;

        எனில் (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_எண் = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
நிலையான முழு
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    முழு sign_id = 0;
    buf_T *buf = வெற்று;
    char_u *group = வெற்று;
    முழு retval = -1;

    // sign group
    group = (group_tv != வெற்று) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    எனில் (group != வெற்று)
    {
        எனில் (group[0] == '\0') // global sign group
        {
            group = வெற்று;
        }
        இல்லையெனில்
        {
            group = vim_strsave(group);
            எனில் (group == வெற்று)
                திரும்பு retval;
        }
    }

    எனில் (dict != வெற்று)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        எனில் (di != வெற்று)
        {
            buf = get_buf_arg(&di->di_tv);
            எனில் (buf == வெற்று)
                செல் cleanup;
        }

        எனில் (dict_has_சாவி(dict, "id"))
        {
            sign_id = dict_get_எண்(dict, "id");
            எனில் (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                செல் cleanup;
            }
        }
    }

    எனில் (buf == வெற்று)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            எனில் (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    இல்லையெனில் எனில் (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    திரும்பு retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    போது (sign != வெற்று && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    திரும்பு sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
முழு
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    எனில் (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        திரும்பு get_first_valid_sign(wp) != வெற்று && !wp->w_p_nu && !wp->w_p_rnu;

    எனில் (*wp->w_p_scl == 'n')
        திரும்பு FALSE;

    எனில் (*wp->w_p_scl == 'y')
        திரும்பு TRUE;

    திரும்பு (get_first_valid_sign(wp) != வெற்று
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
வெறுமை
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = வெற்று;
    rettv->vval.v_எண் = -1;

    எனில் ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_எண் = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
வெறுமை
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) == FAIL)
        திரும்பு;

    எனில் (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        திரும்பு;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
        திரும்பு;

    listitem_T *li = வெற்று;
    FOR_ALL_LIST_பொருட்கள்(argvars[0].vval.v_list, li)
    {
        முழு retval = -1;

        எனில் (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(வெற்று, li->li_tv.vval.v_dict);
        இல்லையெனில்
            emsg(_(e_dictionary_required));

        list_append_எண்(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
