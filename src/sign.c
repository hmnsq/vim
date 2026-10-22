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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
प्रकारनिर्देश संरचना sign sign_T;

संरचना sign
{
    sign_T *sn_next; // next sign in list
    पूर्णाङ्क sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    शून्य *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    पूर्णाङ्क sn_पङ्क्तिः_hl; // highlight ID for line
    पूर्णाङ्क sn_text_hl; // highlight ID for text
    पूर्णाङ्क sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    पूर्णाङ्क sn_अङ्कः_hl; // highlight ID for line number
    पूर्णाङ्क sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

स्थिर sign_T *first_sign = रिक्त;
स्थिर पूर्णाङ्क next_sign_typenr = 1;

स्थिर शून्य sign_list_defined(sign_T *sp);
स्थिर शून्य sign_undefine(sign_T *sp, sign_T *sp_prev);

स्थिर वर्ण *cmds[] = { "define",
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
                        रिक्त
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     प्रति ((sp) = first_sign; (sp) != रिक्त; (sp) = (sp)->sn_next)

स्थिर hashtab_T sg_table; // sign group (signgroup_T) hashtable
स्थिर पूर्णाङ्क next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
शून्य
init_signs(शून्य)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
स्थिर signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = रिक्त;

    जइ (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        जइ (group == रिक्त)
            प्रत्यागम रिक्त;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    अण्णहा
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    प्रत्यागम group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
स्थिर शून्य
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    जइ (HASHITEM_EMPTY(hi))
        प्रत्यागम;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    जइ (group->sg_refcount == 0)
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
स्थिर पूर्णाङ्क
sign_in_group(sign_entry_T *sign, char_u *group)
{
    प्रत्यागम ((group != रिक्त && STRCMP(group, "*") == 0) ||
            (group == रिक्त && sign->se_group == रिक्त) ||
            (group != रिक्त && sign->se_group != रिक्त &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
स्थिर पूर्णाङ्क
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    पूर्णाङ्क for_popup = sign->se_group != रिक्त &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    प्रत्यागम WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
स्थिर पूर्णाङ्क
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    पूर्णाङ्क id = 1;
    signgroup_T *group = रिक्त;
    sign_entry_T *sign = रिक्त;

    जइ (groupname != रिक्त)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        जइ (HASHITEM_EMPTY(hi))
            प्रत्यागम id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    पूर्णाङ्क found = FALSE;
    जाव (!found)
    {
        जइ (group == रिक्त)
            id = next_sign_id++; // global group
        अण्णहा
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            जइ (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                भङ्ग;
            }
        }
    }

    प्रत्यागम id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
स्थिर शून्य
निवेशय_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            पूर्णाङ्क id, // sign ID
            char_u *group, // sign group; NULL for global group
            पूर्णाङ्क prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            पूर्णाङ्क typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(परिमाण(sign_entry_T), FALSE, aid_निवेशय_sign);
    जइ (newsign == रिक्त)
        प्रत्यागम;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    जइ (group != रिक्त)
    {
        newsign->se_group = sign_group_ref(group);
        जइ (newsign->se_group == रिक्त)
        {
            vim_free(newsign);
            प्रत्यागम;
        }
    }
    अण्णहा
    {
        newsign->se_group = रिक्त;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    जइ (next != रिक्त)
        next->se_prev = newsign;

    जइ (prev == रिक्त)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        जइ (buf->b_signlist == रिक्त)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_पङ्क्तिः_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        जइ (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    अण्णहा
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
स्थिर शून्य
निवेशय_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         पूर्णाङ्क id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         पूर्णाङ्क prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         पूर्णाङ्क typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    जाव (prev != रिक्त && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == रिक्त) ? buf->b_signlist : prev->se_next;

    निवेशय_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
स्थिर sign_T *
find_sign_by_typenr(पूर्णाङ्क typenr)
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
        जइ (sp->sn_typenr == typenr)
            प्रत्यागम sp;
    प्रत्यागम रिक्त;
}

/*
 * Get the name of a sign by its typenr.
 */
स्थिर char_u *
sign_typenr2name(पूर्णाङ्क typenr)
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
        जइ (sp->sn_typenr == typenr)
            प्रत्यागम sp->sn_name;
    प्रत्यागम (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
स्थिर dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    जइ (d == रिक्त)
        प्रत्यागम रिक्त;

    dict_add_सङ्ख्या(d, "id", sign->se_id);
    जइ (sign->se_group == रिक्त)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    अण्णहा
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_सङ्ख्या(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_सङ्ख्या(d, "priority", sign->se_priority);

    प्रत्यागम d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
स्थिर शून्य
sign_sort_by_prio_on_पङ्क्तिः(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    जइ ((sign->se_prev == रिक्त || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == रिक्त || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        प्रत्यागम;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    जाव (p->se_prev != रिक्त && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    जइ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        जाव (p->se_next != रिक्त && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    जइ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    जइ (sign->se_prev != रिक्त)
        sign->se_prev->se_next = sign->se_next;

    जइ (sign->se_next != रिक्त)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = रिक्त;
    sign->se_next = रिक्त;

    // Re-insert 'sign' at the right place
    जइ (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        जइ (sign->se_prev != रिक्त)
            sign->se_prev->se_next = sign;

        जइ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    अण्णहा
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        जइ (sign->se_next != रिक्त)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
स्थिर शून्य
buf_addsign(buf_T *buf, // buffer to store sign in
            पूर्णाङ्क id, // sign ID
            char_u *groupname, // sign group
            पूर्णाङ्क prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            पूर्णाङ्क typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    sign_entry_T *prev = रिक्त; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    जइ (buf->b_sign_finger != रिक्त && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    प्रति (sign = (prev == रिक्त) ? buf->b_signlist : prev->se_next; sign != रिक्त;
         sign = sign->se_next)
    {
        जइ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_पङ्क्तिः(buf, sign);
            प्रत्यागम;
        }
        अण्णहा जइ (lnum < sign->se_lnum)
        {
            निवेशय_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            प्रत्यागम;
        }
        prev = sign;
    }

    निवेशय_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
स्थिर linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     पूर्णाङ्क markId, // sign ID
                     char_u *group, // sign group
                     पूर्णाङ्क typenr, // typenr of sign we are adding
                     पूर्णाङ्क prio) // sign priority
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        जइ (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_पङ्क्तिः(buf, sign);
            प्रत्यागम sign->se_lnum;
        }
    }

    प्रत्यागम (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
पूर्णाङ्क
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = रिक्त;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जइ (sign->se_lnum > lnum)
            भङ्ग;

        जइ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            जइ (sp == रिक्त)
                प्रत्यागम FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            जइ (sattr->sat_text != रिक्त && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            जइ (sp->sn_पङ्क्तिः_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_पङ्क्तिः_hl);

            जइ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            जइ (sp->sn_अङ्कः_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_अङ्कः_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            जइ (sign->se_next != रिक्त &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                जइ (next_sp == रिक्त)
                    प्रत्यागम FALSE;

                जइ (sattr->sat_icon == रिक्त && sattr->sat_text == रिक्त)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                जइ (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                जइ (sp->sn_पङ्क्तिः_hl <= 0 && next_sp->sn_पङ्क्तिः_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_पङ्क्तिः_hl);

                जइ (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                जइ (sp->sn_अङ्कः_hl <= 0 && next_sp->sn_अङ्कः_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_अङ्कः_hl);
            }
            प्रत्यागम TRUE;
        }
    }
    प्रत्यागम FALSE;
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
            पूर्णाङ्क id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = रिक्त; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = रिक्त; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    प्रति (sign_entry_T *sign = buf->b_signlist; sign != रिक्त; sign = next)
    {
        next = sign->se_next;

        जइ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            जइ (next != रिक्त)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            जइ (sign->se_group != रिक्त)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_पङ्क्तिः_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            जइ (group == रिक्त || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                भङ्ग;
        }
        अण्णहा
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    जइ (buf->b_signlist == रिक्त)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_पङ्क्तिः_abv_curs();
    }

    प्रत्यागम lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
पूर्णाङ्क
buf_findsign(buf_T *buf, // buffer to store sign in
             पूर्णाङ्क id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        जइ (sign->se_id == id && sign_in_group(sign, group))
            प्रत्यागम sign->se_lnum;

    प्रत्यागम 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
स्थिर sign_entry_T *
buf_getsign_at_पङ्क्तिः(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जइ (sign->se_lnum > lnum)
            भङ्ग;

        जइ (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            प्रत्यागम sign;
    }

    प्रत्यागम रिक्त;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
पूर्णाङ्क
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_पङ्क्तिः(buf, lnum, groupname);
    जइ (sign != रिक्त)
        प्रत्यागम sign->se_id;

    प्रत्यागम 0;
}

# जइ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
पूर्णाङ्क
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    पूर्णाङ्क typenr) // sign type number
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जइ (sign->se_lnum > lnum)
            भङ्ग;

        जइ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            प्रत्यागम sign->se_id;
    }

    प्रत्यागम 0;
}

#  जइ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
पूर्णाङ्क
buf_signcount(buf_T *buf, linenr_T lnum)
{
    पूर्णाङ्क गणनम् = 0;
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जइ (sign->se_lnum > lnum)
            भङ्ग;

        जइ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != रिक्त)
            गणनम्++;
    }

    प्रत्यागम गणनम्;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
शून्य
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = रिक्त; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    जइ (buf->b_signlist != रिक्त && curwin != रिक्त)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_पङ्क्तिः_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = रिक्त;

    प्रति (sign_entry_T *sign = buf->b_signlist; sign != रिक्त; sign = next)
    {
        next = sign->se_next;
        जइ (sign_in_group(sign, group))
        {
            *lastp = next;

            जइ (next != रिक्त)
                next->se_prev = sign->se_prev;

            जइ (sign->se_group != रिक्त)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        अण्णहा
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
स्थिर शून्य
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    वर्ण lbuf[सन्देशः_BUF_LEN];
    वर्ण group[सन्देशः_BUF_LEN];

    सन्देशः_puts_title(_("\n--- Signs ---"));
    सन्देशः_putchar('\n');

    buf_T *buf = (rbuf == रिक्त) ? firstbuf : rbuf;
    जाव (buf != रिक्त && !got_int)
    {
        जइ (buf->b_signlist != रिक्त)
        {
            vim_snprintf(lbuf, सन्देशः_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            सन्देशः_puts_attr(lbuf, HL_ATTR(HLF_D));
            सन्देशः_putchar('\n');
        }

        sign_entry_T *sign = रिक्त;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            जइ (got_int)
                भङ्ग;

            जइ (!sign_in_group(sign, sign_group))
                अनुवर्त;

            जइ (sign->se_group != रिक्त)
                vim_snprintf(group, सन्देशः_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            अण्णहा
                group[0] = '\0';

            vim_snprintf(lbuf, सन्देशः_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            सन्देशः_puts(lbuf);
            सन्देशः_putchar('\n');
        }

        जइ (rbuf != रिक्त)
            भङ्ग;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
शून्य
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = रिक्त; // line changes may reorder, drop the finger
    sign_entry_T *sign = रिक्त; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        जइ (sign->se_lnum < line1)
            अनुवर्त;

        linenr_T new_lnum = sign->se_lnum;

        जइ (sign->se_lnum <= line2)
        {
            जइ (amount != MAXLNUM)
                new_lnum += amount;
        }
        अण्णहा जइ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        जइ (new_lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
स्थिर पूर्णाङ्क
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    पूर्णाङ्क idx = 0;
    वर्ण save = *end_cmd;
    *end_cmd = NUL;

    जाव (cmds[idx] != रिक्त && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    प्रत्यागम idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
स्थिर sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    जइ (sp_prev != रिक्त)
        *sp_prev = रिक्त;

    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जइ (STRCMP(sp->sn_name, name) == 0)
            भङ्ग;

        जइ (sp_prev != रिक्त)
            *sp_prev = sp;
    }

    प्रत्यागम sp;
}

/*
 * Allocate a new sign
 */
स्थिर sign_T *
alloc_new_sign(char_u *name)
{
    पूर्णाङ्क start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(परिमाण(sign_T), aid_sign_define_by_name);
    जइ (sp == रिक्त)
        प्रत्यागम रिक्त;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    जाव (lp != रिक्त)
    {
        जइ (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            जइ (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            जइ (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                प्रत्यागम रिक्त;
            }

            lp = first_sign; // start all over
            अनुवर्त;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    जइ (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    जइ (sp->sn_name == रिक्त) // out of memory
    {
        vim_free(sp);
        प्रत्यागम रिक्त;
    }

    प्रत्यागम sp;
}

/*
 * Initialize the icon information for a new sign
 */
स्थिर शून्य
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    जइ (gui.in_use)
    {
        out_flush();
        जइ (sp->sn_image != रिक्त)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
स्थिर पूर्णाङ्क
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = रिक्त;
    char_u *endp = text + (पूर्णाङ्क)STRLEN(text);
    पूर्णाङ्क cells = 0;

    // Remove backslashes so that it is possible to use a space.
    प्रति (s = text; s + 1 < endp; ++s)
    {
        जइ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    जइ (has_mbyte)
    {
        प्रति (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            जइ (!vim_isprintc((*mb_ptr2char)(s)))
                भङ्ग;

            cells += (*mb_ptr2cells)(s);
        }
    }
    अण्णहा
    {
        प्रति (s = text; s < endp; ++s)
        {
            जइ (!vim_isprintc(*s))
                भङ्ग;
        }
        cells = (पूर्णाङ्क)(s - text);
    }

    // Currently sign text must be one or two display cells
    जइ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        प्रत्यागम FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    पूर्णाङ्क len = (पूर्णाङ्क)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    जइ (sp->sn_text != रिक्त && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    प्रत्यागम OK;
}

/*
 * Define a new sign or update an existing sign
 */
पूर्णाङ्क
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    पूर्णाङ्क prio)
{
    sign_T *sp_prev = रिक्त;
    sign_T *sp = sign_find(name, &sp_prev);
    जइ (sp == रिक्त)
    {
        sp = alloc_new_sign(name);
        जइ (sp == रिक्त)
            प्रत्यागम FAIL;

        // add the new sign to the list of signs
        जइ (sp_prev == रिक्त)
            first_sign = sp;
        अण्णहा
            sp_prev->sn_next = sp;
    }
    अण्णहा
    {
        win_T *wp = रिक्त;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            जइ (wp->w_buffer->b_signlist != रिक्त)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    जइ (icon != रिक्त)
        sign_define_init_icon(sp, icon);

    जइ (text != रिक्त && (sign_define_init_text(sp, text) == FAIL))
        प्रत्यागम FAIL;

    sp->sn_priority = prio;

    जइ (linehl != रिक्त)
    {
        जइ (*linehl == NUL)
            sp->sn_पङ्क्तिः_hl = 0;
        अण्णहा
            sp->sn_पङ्क्तिः_hl = syn_check_group(linehl, (पूर्णाङ्क)STRLEN(linehl));
    }

    जइ (texthl != रिक्त)
    {
        जइ (*texthl == NUL)
            sp->sn_text_hl = 0;
        अण्णहा
            sp->sn_text_hl = syn_check_group(texthl, (पूर्णाङ्क)STRLEN(texthl));
    }

    जइ (culhl != रिक्त)
    {
        जइ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        अण्णहा
            sp->sn_cul_hl = syn_check_group(culhl, (पूर्णाङ्क)STRLEN(culhl));
    }

    जइ (numhl != रिक्त)
    {
        जइ (*numhl == NUL)
            sp->sn_अङ्कः_hl = 0;
        अण्णहा
            sp->sn_अङ्कः_hl = syn_check_group(numhl, (पूर्णाङ्क)STRLEN(numhl));
    }

    प्रत्यागम OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
पूर्णाङ्क
sign_exists_by_name(char_u *name)
{
    प्रत्यागम sign_find(name, रिक्त) != रिक्त;
}

/*
 * Free the sign specified by 'name'.
 */
पूर्णाङ्क
sign_undefine_by_name(char_u *name, पूर्णाङ्क give_error)
{
    sign_T *sp_prev = रिक्त;
    sign_T *sp = sign_find(name, &sp_prev);
    जइ (sp == रिक्त)
    {
        जइ (give_error)
            semsg(_(e_unknown_sign_str), name);
        प्रत्यागम FAIL;
    }
    sign_undefine(sp, sp_prev);

    प्रत्यागम OK;
}

/*
 * List the signs matching 'name'
 */
स्थिर शून्य
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, रिक्त);
    जइ (sp != रिक्त)
        sign_list_defined(sp);
    अण्णहा
        semsg(_(e_unknown_sign_str), name);
}

स्थिर शून्य
may_force_numberwidth_recompute(buf_T *buf, पूर्णाङ्क unplace)
{
# जइ defined(FEAT_LINEBREAK)
    tabpage_T *tp = रिक्त;
    win_T *wp = रिक्त;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        जइ (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_पङ्क्तिः_गणनम् = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
पूर्णाङ्क
sign_place(पूर्णाङ्क *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           पूर्णाङ्क prio)
{
    // Check for reserved character '*' in group name
    जइ (sign_group != रिक्त && (*sign_group == '*' || *sign_group == '\0'))
        प्रत्यागम FAIL;

    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जइ (STRCMP(sp->sn_name, sign_name) == 0)
            भङ्ग;
    }

    जइ (sp == रिक्त)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        प्रत्यागम FAIL;
    }

    जइ (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    जइ (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    जइ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    अण्णहा
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    जइ (lnum > 0)
    {
        redraw_buf_पङ्क्तिः_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    अण्णहा
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Unplace the specified sign
 */
स्थिर पूर्णाङ्क
sign_unplace(पूर्णाङ्क sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    जइ (buf->b_signlist == रिक्त) // No signs in the buffer
        प्रत्यागम OK;

    जइ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    अण्णहा
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        जइ (lnum == 0)
            प्रत्यागम FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    जइ (buf->b_signlist == रिक्त)
        may_force_numberwidth_recompute(buf, TRUE);

    प्रत्यागम OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
स्थिर शून्य
sign_unplace_at_cursor(char_u *groupname)
{
    पूर्णाङ्क id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    जइ (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    अण्णहा
        emsg(_(e_missing_sign_सङ्ख्या));
}

/*
 * Jump to a sign.
 */
स्थिर linenr_T
sign_jump(पूर्णाङ्क sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    जइ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        प्रत्यागम -1;
    }

    // goto a sign ...
    जइ (buf_jump_open_win(buf) != रिक्त)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    अण्णहा
    { // ... not currently in a window
        जइ (buf->b_fname == रिक्त)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            प्रत्यागम -1;
        }
        setpcmark();
        जइ (do_ecmd(buf->b_fnum, रिक्त, रिक्त, रिक्त, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            प्रत्यागम -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    प्रत्यागम lnum;
}

/*
 * ":sign define {name} ..." command
 */
स्थिर शून्य
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = रिक्त;
    char_u *p = cmdline;
    char_u *icon = रिक्त;
    char_u *text = रिक्त;
    char_u *linehl = रिक्त;
    char_u *texthl = रिक्त;
    char_u *culhl = रिक्त;
    char_u *numhl = रिक्त;
    पूर्णाङ्क prio = -1;
    पूर्णाङ्क failed = FALSE;

    // set values for a defined sign.
    जाव (TRUE)
    {
        arg = skipwhite(p);
        जइ (*arg == NUL)
            भङ्ग;

        p = skiptowhite_esc(arg);
        जइ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        अण्णहा जइ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((वर्ण *)arg);
        }
        अण्णहा
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            भङ्ग;
        }
    }

    जइ (!failed)
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
स्थिर शून्य
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               पूर्णाङ्क id,
               char_u *group,
               पूर्णाङ्क prio)
{
    जइ (id <= 0)
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
        जइ (lnum >= 0 || sign_name != रिक्त || (group != रिक्त && *group == '\0'))
            emsg(_(e_invalid_argument));
        अण्णहा
            sign_list_placed(buf, group);
    }
    अण्णहा
    {
        // Place a new sign
        जइ (sign_name == रिक्त || buf == रिक्त ||
            (group != रिक्त && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            प्रत्यागम;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
स्थिर शून्य
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 पूर्णाङ्क id,
                 char_u *group)
{
    जइ (lnum >= 0 || sign_name != रिक्त || (group != रिक्त && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        प्रत्यागम;
    }

    जइ (id == -2)
    {
        जइ (buf != रिक्त)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        अण्णहा
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                जइ (buf->b_signlist != रिक्त)
                    buf_delete_signs(buf, group);
            }
        }
    }
    अण्णहा
    {
        जइ (buf != रिक्त)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        अण्णहा
        {
            जइ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            अण्णहा
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
स्थिर शून्य
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              पूर्णाङ्क id,
              char_u *group)
{
    जइ (sign_name == रिक्त && group == रिक्त && id == -1)
    {
        emsg(_(e_argument_required));
        प्रत्यागम;
    }

    जइ (buf == रिक्त || (group != रिक्त && *group == '\0') || lnum >= 0 ||
        sign_name != रिक्त)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        प्रत्यागम;
    }

    (शून्य)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
स्थिर पूर्णाङ्क
parse_sign_cmd_args(पूर्णाङ्क cmd,
                    char_u *arg,
                    char_u **sign_name,
                    पूर्णाङ्क *signid,
                    char_u **group,
                    पूर्णाङ्क *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = रिक्त;
    पूर्णाङ्क lnum_arg = FALSE;

    // first arg could be placed sign id
    जइ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        जइ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        अण्णहा
        {
            arg = skipwhite(arg);
        }
    }

    जाव (*arg != NUL)
    {
        जइ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((वर्ण *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        अण्णहा जइ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            जइ (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                प्रत्यागम FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        अण्णहा जइ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            जइ (*arg != NUL)
                *arg++ = NUL;

            जाव (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        अण्णहा जइ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            जइ (*arg != NUL)
                *arg++ = NUL;
        }
        अण्णहा जइ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((वर्ण *)arg);
            arg = skiptowhite(arg);
        }
        अण्णहा जइ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            भङ्ग;
        }
        अण्णहा जइ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((पूर्णाङ्क)getdigits(&arg));

            जइ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            भङ्ग;
        }
        अण्णहा
        {
            emsg(_(e_invalid_argument));
            प्रत्यागम FAIL;
        }

        arg = skipwhite(arg);
    }

    जइ (filename != रिक्त && *buf == रिक्त)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        प्रत्यागम FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    जइ (filename == रिक्त &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    प्रत्यागम OK;
}

/*
 * ":sign" command
 */
शून्य
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    पूर्णाङ्क idx = sign_cmd_idx(arg, p);
    जइ (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        प्रत्यागम;
    }
    arg = skipwhite(p);

    जइ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = रिक्त;
        पूर्णाङ्क id = -1;
        char_u *group = रिक्त;
        पूर्णाङ्क prio = -1;
        buf_T *buf = रिक्त;
        linenr_T lnum = -1;

        // Parse command line arguments
        जइ (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            प्रत्यागम;

        जइ (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        अण्णहा जइ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        अण्णहा जइ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        प्रत्यागम;
    }

    // Define, undefine or list signs.
    जइ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        प्रति (sign_T *sp = first_sign; sp != रिक्त && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    अण्णहा जइ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    अण्णहा
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        जइ (*p != NUL)
            *p++ = NUL;

        जाव (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        जइ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        अण्णहा जइ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        अण्णहा
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
स्थिर शून्य
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    जइ (sp->sn_icon != रिक्त)
        dict_add_string(retdict, "icon", sp->sn_icon);

    जइ (sp->sn_text != रिक्त)
        dict_add_string(retdict, "text", sp->sn_text);

    जइ (sp->sn_priority > 0)
        dict_add_सङ्ख्या(retdict, "priority", sp->sn_priority);

    जइ (sp->sn_पङ्क्तिः_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_पङ्क्तिः_hl - 1, FALSE);
        जइ (p == रिक्त)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अण्णहा
            dict_add_string(retdict, "linehl", p);
    }

    जइ (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_text_hl - 1, FALSE);
        जइ (p == रिक्त)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अण्णहा
            dict_add_string(retdict, "texthl", p);
    }

    जइ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_cul_hl - 1, FALSE);
        जइ (p == रिक्त)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अण्णहा
            dict_add_string(retdict, "culhl", p);
    }

    जइ (sp->sn_अङ्कः_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_अङ्कः_hl - 1, FALSE);
        जइ (p == रिक्त)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अण्णहा
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
स्थिर शून्य
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    जइ (name != रिक्त)
    {
        sp = sign_find(name, रिक्त);
        जइ (sp == रिक्त)
            प्रत्यागम;
    }

    प्रति (; sp != रिक्त && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        जइ (dict == रिक्त)
            प्रत्यागम;

        जइ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            प्रत्यागम;
        }

        sign_getinfo(sp, dict);

        जइ (name != रिक्त) // handle only the specified sign
            भङ्ग;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
शून्य
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = रिक्त;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        जइ (d != रिक्त && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
स्थिर शून्य
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       पूर्णाङ्क sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    जइ (d == रिक्त)
        प्रत्यागम;

    जइ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        प्रत्यागम;
    }

    dict_add_सङ्ख्या(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    जइ (l == रिक्त)
        प्रत्यागम;

    जइ (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        प्रत्यागम;
    }

    sign_entry_T *sign = रिक्त;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        जइ (!sign_in_group(sign, sign_group))
            अनुवर्त;

        जइ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            जइ (sdict != रिक्त && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
स्थिर शून्य
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                पूर्णाङ्क sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    जइ (buf != रिक्त)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    अण्णहा
    {
        FOR_ALL_BUFFERS(buf)
        {
            जइ (buf->b_signlist != रिक्त)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# जइ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
शून्य
sign_gui_started(शून्य)
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जइ (sp->sn_icon != रिक्त)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
स्थिर शून्य
sign_list_defined(sign_T *sp)
{
    वर्ण lbuf[सन्देशः_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    जइ (sp->sn_icon != रिक्त)
    {
        सन्देशः_puts(" icon=");
        सन्देशः_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        जइ (sp->sn_image == रिक्त)
            सन्देशः_puts(_(" (NOT FOUND)"));
# अण्णहा
        सन्देशः_puts(_(" (not supported)"));
# endif
    }

    जइ (sp->sn_text != रिक्त)
    {
        सन्देशः_puts(" text=");
        सन्देशः_outtrans(sp->sn_text);
    }

    जइ (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, सन्देशः_BUF_LEN, " priority=%d", sp->sn_priority);
        सन्देशः_puts(lbuf);
    }

    जइ (sp->sn_पङ्क्तिः_hl > 0)
    {
        सन्देशः_puts(" linehl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_पङ्क्तिः_hl - 1, FALSE);
        जइ (p == रिक्त)
            सन्देशः_puts("NONE");
        अण्णहा
            सन्देशः_puts((वर्ण *)p);
    }

    जइ (sp->sn_text_hl > 0)
    {
        सन्देशः_puts(" texthl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_text_hl - 1, FALSE);
        जइ (p == रिक्त)
            सन्देशः_puts("NONE");
        अण्णहा
            सन्देशः_puts((वर्ण *)p);
    }

    जइ (sp->sn_cul_hl > 0)
    {
        सन्देशः_puts(" culhl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_cul_hl - 1, FALSE);
        जइ (p == रिक्त)
            सन्देशः_puts("NONE");
        अण्णहा
            सन्देशः_puts((वर्ण *)p);
    }

    जइ (sp->sn_अङ्कः_hl > 0)
    {
        सन्देशः_puts(" numhl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_अङ्कः_hl - 1, FALSE);
        जइ (p == रिक्त)
            सन्देशः_puts("NONE");
        अण्णहा
            सन्देशः_puts((वर्ण *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
स्थिर शून्य
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    जइ (sp->sn_image != रिक्त)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    जइ (sp_prev == रिक्त)
        first_sign = sp->sn_next;
    अण्णहा
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# जइ defined(FEAT_SIGN_ICONS)
शून्य *
sign_get_image(पूर्णाङ्क typenr) // the attribute which may have a sign
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जइ (sp->sn_typenr == typenr)
            प्रत्यागम sp->sn_image;
    }
    प्रत्यागम रिक्त;
}
# endif

/*
 * Undefine/free all signs.
 */
शून्य
free_signs(शून्य)
{
    जाव (first_sign != रिक्त)
        sign_undefine(first_sign, रिक्त);
}

स्थिर गणना
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
स्थिर char_u *
get_nth_sign_name(पूर्णाङ्क idx)
{
    पूर्णाङ्क current_idx = 0;
    sign_T *sp = रिक्त;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        जइ (current_idx++ == idx)
            प्रत्यागम sp->sn_name;
    }
    प्रत्यागम रिक्त;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
स्थिर char_u *
get_nth_sign_group_name(पूर्णाङ्क idx)
{
    पूर्णाङ्क current_idx = 0;
    पूर्णाङ्क todo = (पूर्णाङ्क)sg_table.ht_used;
    hashitem_T *hi = रिक्त;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_वस्तूनि(&sg_table, hi, todo)
    {
        जइ (!HASHITEM_EMPTY(hi))
        {
            --todo;
            जइ (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                प्रत्यागम group->sg_name;
            }
        }
    }
    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    विकल्प (expand_what)
    {
        पक्ष EXP_SUBCMD:
            प्रत्यागम (char_u *)cmds[idx];
        पक्ष EXP_DEFINE:
        {
            वर्ण *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", रिक्त };
            प्रत्यागम (char_u *)define_arg[idx];
        }
        पक्ष EXP_PLACE:
        {
            वर्ण *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", रिक्त };
            प्रत्यागम (char_u *)place_arg[idx];
        }
        पक्ष EXP_LIST:
        {
            वर्ण *list_arg[] = { "group=", "file=", "buffer=", रिक्त };
            प्रत्यागम (char_u *)list_arg[idx];
        }
        पक्ष EXP_UNPLACE:
        {
            वर्ण *unplace_arg[] = { "group=", "file=", "buffer=", रिक्त };
            प्रत्यागम (char_u *)unplace_arg[idx];
        }
        पक्ष EXP_SIGN_NAMES:
            प्रत्यागम get_nth_sign_name(idx);
        पक्ष EXP_SIGN_GROUPS:
            प्रत्यागम get_nth_sign_group_name(idx);
        स्वभाव:
            प्रत्यागम रिक्त;
    }
}

/*
 * Handle command line completion for :sign command.
 */
शून्य
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    पूर्णाङ्क cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    जइ (*end_subcmd == NUL)
        प्रत्यागम;

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
    करेहि
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    जाव (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    जइ (p == रिक्त)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        विकल्प (cmd_idx)
        {
            पक्ष SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                भङ्ग;
            पक्ष SIGNCMD_PLACE:
                // List placed signs
                जइ (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                अण्णहा
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                भङ्ग;
            पक्ष SIGNCMD_LIST:
            पक्ष SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                भङ्ग;
            पक्ष SIGNCMD_JUMP:
            पक्ष SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                भङ्ग;
            स्वभाव:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    अण्णहा
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        विकल्प (cmd_idx)
        {
            पक्ष SIGNCMD_DEFINE:
                जइ (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                अण्णहा जइ (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                अण्णहा
                    xp->xp_context = EXPAND_NOTHING;
                भङ्ग;
            पक्ष SIGNCMD_PLACE:
                जइ (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                अण्णहा जइ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                अण्णहा जइ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                अण्णहा
                    xp->xp_context = EXPAND_NOTHING;
                भङ्ग;
            पक्ष SIGNCMD_UNPLACE:
            पक्ष SIGNCMD_JUMP:
                जइ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                अण्णहा जइ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                अण्णहा
                    xp->xp_context = EXPAND_NOTHING;
                भङ्ग;
            स्वभाव:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
स्थिर पूर्णाङ्क
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = रिक्त;
    char_u *icon = रिक्त;
    char_u *linehl = रिक्त;
    char_u *text = रिक्त;
    char_u *texthl = रिक्त;
    char_u *culhl = रिक्त;
    char_u *numhl = रिक्त;
    पूर्णाङ्क prio = -1;
    पूर्णाङ्क retval = -1;

    जइ (name_arg == रिक्त && dict == रिक्त)
        प्रत्यागम retval;

    जइ (name_arg == रिक्त)
        name = dict_get_string(dict, "name", TRUE);
    अण्णहा
        name = vim_strsave(name_arg);

    जइ (name == रिक्त || name[0] == NUL)
        गच्छ cleanup;

    जइ (dict != रिक्त)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_सङ्ख्या_def(dict, "priority", -1);
    }

    जइ (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    प्रत्यागम retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
स्थिर शून्य
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तूनि(l, li)
    {
        पूर्णाङ्क retval = -1;

        जइ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(रिक्त, li->li_tv.vval.v_dict);
        अण्णहा
            emsg(_(e_dictionary_required));

        list_append_सङ्ख्या(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
शून्य
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        प्रत्यागम;

    जइ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        जइ (rettv_list_alloc(rettv) == FAIL)
            प्रत्यागम;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        प्रत्यागम;
    }

    // Define a single sign
    rettv->vval.v_सङ्ख्या = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    जइ (name == रिक्त)
        प्रत्यागम;

    जइ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        प्रत्यागम;

    rettv->vval.v_सङ्ख्या = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : रिक्त);
}

/*
 * "sign_getdefined()" function
 */
शून्य
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    जइ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        प्रत्यागम;

    जइ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    char_u *name = रिक्त;
    जइ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
शून्य
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = रिक्त;
    linenr_T lnum = 0;
    पूर्णाङ्क sign_id = 0;
    char_u *group = रिक्त;

    जइ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        प्रत्यागम;

    जइ (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        प्रत्यागम;

    जइ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        जइ (buf == रिक्त)
            प्रत्यागम;

        जइ (argvars[1].v_type != VAR_UNKNOWN)
        {
            जइ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                प्रत्यागम;

            dictitem_T *di = रिक्त;
            dict_T *dict = argvars[1].vval.v_dict;

            जइ ((di = dict_find(dict, (char_u *)"lnum", -1)) != रिक्त)
            {
                // get signs placed at this line
                पूर्णाङ्क notanum = FALSE;
                tv_get_सङ्ख्या_chk(&di->di_tv, &notanum);
                जइ (notanum)
                    प्रत्यागम;

                lnum = tv_get_lnum(&di->di_tv);
            }

            जइ ((di = dict_find(dict, (char_u *)"id", -1)) != रिक्त)
            {
                // get sign placed with this identifier
                पूर्णाङ्क notanum = FALSE;
                sign_id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&di->di_tv, &notanum);
                जइ (notanum)
                    प्रत्यागम;
            }

            जइ ((di = dict_find(dict, (char_u *)"group", -1)) != रिक्त)
            {
                group = tv_get_string_chk(&di->di_tv);
                जइ (group == रिक्त)
                    प्रत्यागम;

                जइ (*group == '\0') // empty string means global group
                    group = रिक्त;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
शून्य
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = रिक्त;
    rettv->vval.v_सङ्ख्या = -1;

    जइ (in_vim9script() && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        प्रत्यागम;

    पूर्णाङ्क notanum = FALSE;
    // Sign identifier
    पूर्णाङ्क sign_id = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], &notanum);
    जइ (notanum)
        प्रत्यागम;

    जइ (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        प्रत्यागम;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    जइ (sign_group == रिक्त)
        प्रत्यागम;

    जइ (sign_group[0] == '\0')
    {
        sign_group = रिक्त; // global sign group
    }
    अण्णहा
    {
        sign_group = vim_strsave(sign_group);
        जइ (sign_group == रिक्त)
            प्रत्यागम;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    जइ (buf == रिक्त)
        गच्छ cleanup;

    rettv->vval.v_सङ्ख्या = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
स्थिर पूर्णाङ्क
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    पूर्णाङ्क sign_id = 0;
    char_u *group = रिक्त;
    char_u *sign_name = रिक्त;
    buf_T *buf = रिक्त;
    dictitem_T *di = रिक्त;
    linenr_T lnum = 0;
    पूर्णाङ्क prio = -1;
    पूर्णाङ्क ret_sign_id = -1;

    // sign identifier
    जइ (id_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        जइ (di != रिक्त)
            id_tv = &di->di_tv;
    }

    जइ (id_tv == रिक्त)
    {
        sign_id = 0;
    }
    अण्णहा
    {
        पूर्णाङ्क notanum = FALSE;
        sign_id = tv_get_सङ्ख्या_chk(id_tv, &notanum);

        जइ (notanum)
            प्रत्यागम -1;

        जइ (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            प्रत्यागम -1;
        }
    }

    // sign group
    जइ (group_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        जइ (di != रिक्त)
            group_tv = &di->di_tv;
    }

    जइ (group_tv == रिक्त)
    {
        group = रिक्त; // global group
    }
    अण्णहा
    {
        group = tv_get_string_chk(group_tv);
        जइ (group == रिक्त)
            गच्छ cleanup;

        जइ (group[0] == '\0') // global sign group
        {
            group = रिक्त;
        }
        अण्णहा
        {
            group = vim_strsave(group);
            जइ (group == रिक्त)
                प्रत्यागम -1;
        }
    }

    // sign name
    जइ (name_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        जइ (di != रिक्त)
            name_tv = &di->di_tv;
    }

    जइ (name_tv == रिक्त)
        गच्छ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    जइ (sign_name == रिक्त)
        गच्छ cleanup;

    // buffer to place the sign
    जइ (buf_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        जइ (di != रिक्त)
            buf_tv = &di->di_tv;
    }

    जइ (buf_tv == रिक्त)
        गच्छ cleanup;

    buf = get_buf_arg(buf_tv);
    जइ (buf == रिक्त)
        गच्छ cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    जइ (di != रिक्त)
    {
        lnum = tv_get_lnum(&di->di_tv);
        जइ (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            गच्छ cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    जइ (di != रिक्त)
    {
        पूर्णाङ्क notanum = FALSE;
        prio = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&di->di_tv, &notanum);
        जइ (notanum)
            गच्छ cleanup;
    }

    जइ (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    प्रत्यागम ret_sign_id;
}

/*
 * "sign_place()" function
 */
शून्य
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = रिक्त;
    rettv->vval.v_सङ्ख्या = -1;

    जइ (in_vim9script() && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        प्रत्यागम;

    जइ (argvars[4].v_type != VAR_UNKNOWN)
    {
        जइ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            प्रत्यागम;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_सङ्ख्या = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
शून्य
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    जइ (rettv_list_alloc(rettv) == FAIL)
        प्रत्यागम;

    जइ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    जइ (check_for_list_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    // Process the List of sign attributes
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तूनि(argvars[0].vval.v_list, li)
    {
        पूर्णाङ्क sign_id = -1;

        जइ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(रिक्त, रिक्त, रिक्त, रिक्त,
                                           li->li_tv.vval.v_dict);
        अण्णहा
            emsg(_(e_dictionary_required));

        list_append_सङ्ख्या(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
स्थिर शून्य
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तूनि(l, li)
    {
        पूर्णाङ्क retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        जइ (name != रिक्त && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_सङ्ख्या(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
शून्य
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    जइ (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    जइ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        जइ (rettv_list_alloc(rettv) == FAIL)
            प्रत्यागम;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        प्रत्यागम;
    }

    rettv->vval.v_सङ्ख्या = -1;

    जइ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_सङ्ख्या = 0;
    }
    अण्णहा
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        जइ (name == रिक्त)
            प्रत्यागम;

        जइ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_सङ्ख्या = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
स्थिर पूर्णाङ्क
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    पूर्णाङ्क sign_id = 0;
    buf_T *buf = रिक्त;
    char_u *group = रिक्त;
    पूर्णाङ्क retval = -1;

    // sign group
    group = (group_tv != रिक्त) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    जइ (group != रिक्त)
    {
        जइ (group[0] == '\0') // global sign group
        {
            group = रिक्त;
        }
        अण्णहा
        {
            group = vim_strsave(group);
            जइ (group == रिक्त)
                प्रत्यागम retval;
        }
    }

    जइ (dict != रिक्त)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        जइ (di != रिक्त)
        {
            buf = get_buf_arg(&di->di_tv);
            जइ (buf == रिक्त)
                गच्छ cleanup;
        }

        जइ (dict_has_कुञ्चिका(dict, "id"))
        {
            sign_id = dict_get_सङ्ख्या(dict, "id");
            जइ (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                गच्छ cleanup;
            }
        }
    }

    जइ (buf == रिक्त)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            जइ (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    अण्णहा जइ (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    प्रत्यागम retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    जाव (sign != रिक्त && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    प्रत्यागम sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
पूर्णाङ्क
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    जइ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        प्रत्यागम get_first_valid_sign(wp) != रिक्त && !wp->w_p_nu && !wp->w_p_rnu;

    जइ (*wp->w_p_scl == 'n')
        प्रत्यागम FALSE;

    जइ (*wp->w_p_scl == 'y')
        प्रत्यागम TRUE;

    प्रत्यागम (get_first_valid_sign(wp) != रिक्त
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
शून्य
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = रिक्त;
    rettv->vval.v_सङ्ख्या = -1;

    जइ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        प्रत्यागम;

    जइ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_सङ्ख्या = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
शून्य
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    जइ (rettv_list_alloc(rettv) == FAIL)
        प्रत्यागम;

    जइ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    जइ (check_for_list_arg(argvars, 0) == FAIL)
        प्रत्यागम;

    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तूनि(argvars[0].vval.v_list, li)
    {
        पूर्णाङ्क retval = -1;

        जइ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(रिक्त, li->li_tv.vval.v_dict);
        अण्णहा
            emsg(_(e_dictionary_required));

        list_append_सङ्ख्या(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
