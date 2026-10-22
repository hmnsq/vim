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

#जर defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
प्रकारव्याख्या संरचना sign sign_T;

संरचना sign
{
    sign_T *sn_next; // next sign in list
    पूर्णांक sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    शून्य *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    पूर्णांक sn_ओळ_hl; // highlight ID for line
    पूर्णांक sn_text_hl; // highlight ID for text
    पूर्णांक sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    पूर्णांक sn_अंक_hl; // highlight ID for line number
    पूर्णांक sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

स्थिर sign_T *first_sign = रिक्त;
स्थिर पूर्णांक next_sign_typenr = 1;

स्थिर शून्य sign_list_defined(sign_T *sp);
स्थिर शून्य sign_undefine(sign_T *sp, sign_T *sp_prev);

स्थिर अक्षर *cmds[] = { "define",
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
     साठी ((sp) = first_sign; (sp) != रिक्त; (sp) = (sp)->sn_next)

स्थिर hashtab_T sg_table; // sign group (signgroup_T) hashtable
स्थिर पूर्णांक next_sign_id = 1; // next sign id in the global group

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

    जर (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(दुरत्व(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        जर (group == रिक्त)
            परत रिक्त;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    अन्यथा
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    परत group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
स्थिर शून्य
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    जर (HASHITEM_EMPTY(hi))
        परत;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    जर (group->sg_refcount == 0)
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
स्थिर पूर्णांक
sign_in_group(sign_entry_T *sign, char_u *group)
{
    परत ((group != रिक्त && STRCMP(group, "*") == 0) ||
            (group == रिक्त && sign->se_group == रिक्त) ||
            (group != रिक्त && sign->se_group != रिक्त &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
स्थिर पूर्णांक
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    पूर्णांक for_popup = sign->se_group != रिक्त &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    परत WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
स्थिर पूर्णांक
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    पूर्णांक id = 1;
    signgroup_T *group = रिक्त;
    sign_entry_T *sign = रिक्त;

    जर (groupname != रिक्त)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        जर (HASHITEM_EMPTY(hi))
            परत id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    पूर्णांक found = FALSE;
    पर्यंत (!found)
    {
        जर (group == रिक्त)
            id = next_sign_id++; // global group
        अन्यथा
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            जर (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                थांबा;
            }
        }
    }

    परत id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
स्थिर शून्य
घाला_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            पूर्णांक id, // sign ID
            char_u *group, // sign group; NULL for global group
            पूर्णांक prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            पूर्णांक typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(आकार(sign_entry_T), FALSE, aid_घाला_sign);
    जर (newsign == रिक्त)
        परत;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    जर (group != रिक्त)
    {
        newsign->se_group = sign_group_ref(group);
        जर (newsign->se_group == रिक्त)
        {
            vim_free(newsign);
            परत;
        }
    }
    अन्यथा
    {
        newsign->se_group = रिक्त;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    जर (next != रिक्त)
        next->se_prev = newsign;

    जर (prev == रिक्त)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        जर (buf->b_signlist == रिक्त)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_ओळ_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        जर (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    अन्यथा
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
घाला_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         पूर्णांक id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         पूर्णांक prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         पूर्णांक typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    पर्यंत (prev != रिक्त && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == रिक्त) ? buf->b_signlist : prev->se_next;

    घाला_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
स्थिर sign_T *
find_sign_by_typenr(पूर्णांक typenr)
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
        जर (sp->sn_typenr == typenr)
            परत sp;
    परत रिक्त;
}

/*
 * Get the name of a sign by its typenr.
 */
स्थिर char_u *
sign_typenr2name(पूर्णांक typenr)
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
        जर (sp->sn_typenr == typenr)
            परत sp->sn_name;
    परत (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
स्थिर dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    जर (d == रिक्त)
        परत रिक्त;

    dict_add_संख्या(d, "id", sign->se_id);
    जर (sign->se_group == रिक्त)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    अन्यथा
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_संख्या(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_संख्या(d, "priority", sign->se_priority);

    परत d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
स्थिर शून्य
sign_sort_by_prio_on_ओळ(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    जर ((sign->se_prev == रिक्त || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == रिक्त || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        परत;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    पर्यंत (p->se_prev != रिक्त && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    जर (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        पर्यंत (p->se_next != रिक्त && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    जर (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    जर (sign->se_prev != रिक्त)
        sign->se_prev->se_next = sign->se_next;

    जर (sign->se_next != रिक्त)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = रिक्त;
    sign->se_next = रिक्त;

    // Re-insert 'sign' at the right place
    जर (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        जर (sign->se_prev != रिक्त)
            sign->se_prev->se_next = sign;

        जर (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    अन्यथा
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        जर (sign->se_next != रिक्त)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
स्थिर शून्य
buf_addsign(buf_T *buf, // buffer to store sign in
            पूर्णांक id, // sign ID
            char_u *groupname, // sign group
            पूर्णांक prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            पूर्णांक typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    sign_entry_T *prev = रिक्त; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    जर (buf->b_sign_finger != रिक्त && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    साठी (sign = (prev == रिक्त) ? buf->b_signlist : prev->se_next; sign != रिक्त;
         sign = sign->se_next)
    {
        जर (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ओळ(buf, sign);
            परत;
        }
        अन्यथा जर (lnum < sign->se_lnum)
        {
            घाला_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            परत;
        }
        prev = sign;
    }

    घाला_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
स्थिर linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     पूर्णांक markId, // sign ID
                     char_u *group, // sign group
                     पूर्णांक typenr, // typenr of sign we are adding
                     पूर्णांक prio) // sign priority
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        जर (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ओळ(buf, sign);
            परत sign->se_lnum;
        }
    }

    परत (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
पूर्णांक
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = रिक्त;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जर (sign->se_lnum > lnum)
            थांबा;

        जर (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            जर (sp == रिक्त)
                परत FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            जर (sattr->sat_text != रिक्त && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            जर (sp->sn_ओळ_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_ओळ_hl);

            जर (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            जर (sp->sn_अंक_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_अंक_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            जर (sign->se_next != रिक्त &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                जर (next_sp == रिक्त)
                    परत FALSE;

                जर (sattr->sat_icon == रिक्त && sattr->sat_text == रिक्त)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                जर (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                जर (sp->sn_ओळ_hl <= 0 && next_sp->sn_ओळ_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_ओळ_hl);

                जर (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                जर (sp->sn_अंक_hl <= 0 && next_sp->sn_अंक_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_अंक_hl);
            }
            परत TRUE;
        }
    }
    परत FALSE;
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
            पूर्णांक id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = रिक्त; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = रिक्त; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    साठी (sign_entry_T *sign = buf->b_signlist; sign != रिक्त; sign = next)
    {
        next = sign->se_next;

        जर ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            जर (next != रिक्त)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            जर (sign->se_group != रिक्त)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_ओळ_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            जर (group == रिक्त || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                थांबा;
        }
        अन्यथा
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    जर (buf->b_signlist == रिक्त)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ओळ_abv_curs();
    }

    परत lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
पूर्णांक
buf_findsign(buf_T *buf, // buffer to store sign in
             पूर्णांक id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        जर (sign->se_id == id && sign_in_group(sign, group))
            परत sign->se_lnum;

    परत 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
स्थिर sign_entry_T *
buf_getsign_at_ओळ(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जर (sign->se_lnum > lnum)
            थांबा;

        जर (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            परत sign;
    }

    परत रिक्त;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
पूर्णांक
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_ओळ(buf, lnum, groupname);
    जर (sign != रिक्त)
        परत sign->se_id;

    परत 0;
}

# जर defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
पूर्णांक
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    पूर्णांक typenr) // sign type number
{
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जर (sign->se_lnum > lnum)
            थांबा;

        जर (sign->se_lnum == lnum && sign->se_typenr == typenr)
            परत sign->se_id;
    }

    परत 0;
}

#  जर defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
पूर्णांक
buf_signcount(buf_T *buf, linenr_T lnum)
{
    पूर्णांक मोजणी = 0;
    sign_entry_T *sign = रिक्त; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        जर (sign->se_lnum > lnum)
            थांबा;

        जर (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != रिक्त)
            मोजणी++;
    }

    परत मोजणी;
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
    जर (buf->b_signlist != रिक्त && curwin != रिक्त)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ओळ_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = रिक्त;

    साठी (sign_entry_T *sign = buf->b_signlist; sign != रिक्त; sign = next)
    {
        next = sign->se_next;
        जर (sign_in_group(sign, group))
        {
            *lastp = next;

            जर (next != रिक्त)
                next->se_prev = sign->se_prev;

            जर (sign->se_group != रिक्त)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        अन्यथा
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
    अक्षर lbuf[संदेश_BUF_LEN];
    अक्षर group[संदेश_BUF_LEN];

    संदेश_puts_title(_("\n--- Signs ---"));
    संदेश_putchar('\n');

    buf_T *buf = (rbuf == रिक्त) ? firstbuf : rbuf;
    पर्यंत (buf != रिक्त && !got_int)
    {
        जर (buf->b_signlist != रिक्त)
        {
            vim_snprintf(lbuf, संदेश_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            संदेश_puts_attr(lbuf, HL_ATTR(HLF_D));
            संदेश_putchar('\n');
        }

        sign_entry_T *sign = रिक्त;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            जर (got_int)
                थांबा;

            जर (!sign_in_group(sign, sign_group))
                सुरूठेवा;

            जर (sign->se_group != रिक्त)
                vim_snprintf(group, संदेश_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            अन्यथा
                group[0] = '\0';

            vim_snprintf(lbuf, संदेश_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (दीर्घ)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            संदेश_puts(lbuf);
            संदेश_putchar('\n');
        }

        जर (rbuf != रिक्त)
            थांबा;

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
    दीर्घ        amount,
    दीर्घ        amount_after)
{
    curbuf->b_sign_finger = रिक्त; // line changes may reorder, drop the finger
    sign_entry_T *sign = रिक्त; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        जर (sign->se_lnum < line1)
            सुरूठेवा;

        linenr_T new_lnum = sign->se_lnum;

        जर (sign->se_lnum <= line2)
        {
            जर (amount != MAXLNUM)
                new_lnum += amount;
        }
        अन्यथा जर (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        जर (new_lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
स्थिर पूर्णांक
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    पूर्णांक idx = 0;
    अक्षर save = *end_cmd;
    *end_cmd = NUL;

    पर्यंत (cmds[idx] != रिक्त && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    परत idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
स्थिर sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    जर (sp_prev != रिक्त)
        *sp_prev = रिक्त;

    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जर (STRCMP(sp->sn_name, name) == 0)
            थांबा;

        जर (sp_prev != रिक्त)
            *sp_prev = sp;
    }

    परत sp;
}

/*
 * Allocate a new sign
 */
स्थिर sign_T *
alloc_new_sign(char_u *name)
{
    पूर्णांक start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(आकार(sign_T), aid_sign_define_by_name);
    जर (sp == रिक्त)
        परत रिक्त;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    पर्यंत (lp != रिक्त)
    {
        जर (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            जर (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            जर (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                परत रिक्त;
            }

            lp = first_sign; // start all over
            सुरूठेवा;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    जर (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    जर (sp->sn_name == रिक्त) // out of memory
    {
        vim_free(sp);
        परत रिक्त;
    }

    परत sp;
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
    जर (gui.in_use)
    {
        out_flush();
        जर (sp->sn_image != रिक्त)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
स्थिर पूर्णांक
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = रिक्त;
    char_u *endp = text + (पूर्णांक)STRLEN(text);
    पूर्णांक cells = 0;

    // Remove backslashes so that it is possible to use a space.
    साठी (s = text; s + 1 < endp; ++s)
    {
        जर (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    जर (has_mbyte)
    {
        साठी (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            जर (!vim_isprintc((*mb_ptr2char)(s)))
                थांबा;

            cells += (*mb_ptr2cells)(s);
        }
    }
    अन्यथा
    {
        साठी (s = text; s < endp; ++s)
        {
            जर (!vim_isprintc(*s))
                थांबा;
        }
        cells = (पूर्णांक)(s - text);
    }

    // Currently sign text must be one or two display cells
    जर (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        परत FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    पूर्णांक len = (पूर्णांक)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    जर (sp->sn_text != रिक्त && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    परत OK;
}

/*
 * Define a new sign or update an existing sign
 */
पूर्णांक
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    पूर्णांक prio)
{
    sign_T *sp_prev = रिक्त;
    sign_T *sp = sign_find(name, &sp_prev);
    जर (sp == रिक्त)
    {
        sp = alloc_new_sign(name);
        जर (sp == रिक्त)
            परत FAIL;

        // add the new sign to the list of signs
        जर (sp_prev == रिक्त)
            first_sign = sp;
        अन्यथा
            sp_prev->sn_next = sp;
    }
    अन्यथा
    {
        win_T *wp = रिक्त;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            जर (wp->w_buffer->b_signlist != रिक्त)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    जर (icon != रिक्त)
        sign_define_init_icon(sp, icon);

    जर (text != रिक्त && (sign_define_init_text(sp, text) == FAIL))
        परत FAIL;

    sp->sn_priority = prio;

    जर (linehl != रिक्त)
    {
        जर (*linehl == NUL)
            sp->sn_ओळ_hl = 0;
        अन्यथा
            sp->sn_ओळ_hl = syn_check_group(linehl, (पूर्णांक)STRLEN(linehl));
    }

    जर (texthl != रिक्त)
    {
        जर (*texthl == NUL)
            sp->sn_text_hl = 0;
        अन्यथा
            sp->sn_text_hl = syn_check_group(texthl, (पूर्णांक)STRLEN(texthl));
    }

    जर (culhl != रिक्त)
    {
        जर (*culhl == NUL)
            sp->sn_cul_hl = 0;
        अन्यथा
            sp->sn_cul_hl = syn_check_group(culhl, (पूर्णांक)STRLEN(culhl));
    }

    जर (numhl != रिक्त)
    {
        जर (*numhl == NUL)
            sp->sn_अंक_hl = 0;
        अन्यथा
            sp->sn_अंक_hl = syn_check_group(numhl, (पूर्णांक)STRLEN(numhl));
    }

    परत OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
पूर्णांक
sign_exists_by_name(char_u *name)
{
    परत sign_find(name, रिक्त) != रिक्त;
}

/*
 * Free the sign specified by 'name'.
 */
पूर्णांक
sign_undefine_by_name(char_u *name, पूर्णांक give_error)
{
    sign_T *sp_prev = रिक्त;
    sign_T *sp = sign_find(name, &sp_prev);
    जर (sp == रिक्त)
    {
        जर (give_error)
            semsg(_(e_unknown_sign_str), name);
        परत FAIL;
    }
    sign_undefine(sp, sp_prev);

    परत OK;
}

/*
 * List the signs matching 'name'
 */
स्थिर शून्य
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, रिक्त);
    जर (sp != रिक्त)
        sign_list_defined(sp);
    अन्यथा
        semsg(_(e_unknown_sign_str), name);
}

स्थिर शून्य
may_force_numberwidth_recompute(buf_T *buf, पूर्णांक unplace)
{
# जर defined(FEAT_LINEBREAK)
    tabpage_T *tp = रिक्त;
    win_T *wp = रिक्त;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        जर (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_ओळ_मोजणी = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
पूर्णांक
sign_place(पूर्णांक *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           पूर्णांक prio)
{
    // Check for reserved character '*' in group name
    जर (sign_group != रिक्त && (*sign_group == '*' || *sign_group == '\0'))
        परत FAIL;

    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जर (STRCMP(sp->sn_name, sign_name) == 0)
            थांबा;
    }

    जर (sp == रिक्त)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        परत FAIL;
    }

    जर (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    जर (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    जर (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    अन्यथा
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    जर (lnum > 0)
    {
        redraw_buf_ओळ_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    अन्यथा
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        परत FAIL;
    }

    परत OK;
}

/*
 * Unplace the specified sign
 */
स्थिर पूर्णांक
sign_unplace(पूर्णांक sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    जर (buf->b_signlist == रिक्त) // No signs in the buffer
        परत OK;

    जर (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    अन्यथा
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        जर (lnum == 0)
            परत FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    जर (buf->b_signlist == रिक्त)
        may_force_numberwidth_recompute(buf, TRUE);

    परत OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
स्थिर शून्य
sign_unplace_at_cursor(char_u *groupname)
{
    पूर्णांक id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    जर (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    अन्यथा
        emsg(_(e_missing_sign_संख्या));
}

/*
 * Jump to a sign.
 */
स्थिर linenr_T
sign_jump(पूर्णांक sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    जर (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        परत -1;
    }

    // goto a sign ...
    जर (buf_jump_open_win(buf) != रिक्त)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    अन्यथा
    { // ... not currently in a window
        जर (buf->b_fname == रिक्त)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            परत -1;
        }
        setpcmark();
        जर (do_ecmd(buf->b_fnum, रिक्त, रिक्त, रिक्त, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            परत -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    परत lnum;
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
    पूर्णांक prio = -1;
    पूर्णांक failed = FALSE;

    // set values for a defined sign.
    पर्यंत (TRUE)
    {
        arg = skipwhite(p);
        जर (*arg == NUL)
            थांबा;

        p = skiptowhite_esc(arg);
        जर (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        अन्यथा जर (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = म_से_प((अक्षर *)arg);
        }
        अन्यथा
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            थांबा;
        }
    }

    जर (!failed)
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
               पूर्णांक id,
               char_u *group,
               पूर्णांक prio)
{
    जर (id <= 0)
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
        जर (lnum >= 0 || sign_name != रिक्त || (group != रिक्त && *group == '\0'))
            emsg(_(e_invalid_argument));
        अन्यथा
            sign_list_placed(buf, group);
    }
    अन्यथा
    {
        // Place a new sign
        जर (sign_name == रिक्त || buf == रिक्त ||
            (group != रिक्त && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            परत;
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
                 पूर्णांक id,
                 char_u *group)
{
    जर (lnum >= 0 || sign_name != रिक्त || (group != रिक्त && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        परत;
    }

    जर (id == -2)
    {
        जर (buf != रिक्त)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        अन्यथा
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                जर (buf->b_signlist != रिक्त)
                    buf_delete_signs(buf, group);
            }
        }
    }
    अन्यथा
    {
        जर (buf != रिक्त)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        अन्यथा
        {
            जर (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            अन्यथा
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
              पूर्णांक id,
              char_u *group)
{
    जर (sign_name == रिक्त && group == रिक्त && id == -1)
    {
        emsg(_(e_argument_required));
        परत;
    }

    जर (buf == रिक्त || (group != रिक्त && *group == '\0') || lnum >= 0 ||
        sign_name != रिक्त)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        परत;
    }

    (शून्य)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
स्थिर पूर्णांक
parse_sign_cmd_args(पूर्णांक cmd,
                    char_u *arg,
                    char_u **sign_name,
                    पूर्णांक *signid,
                    char_u **group,
                    पूर्णांक *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = रिक्त;
    पूर्णांक lnum_arg = FALSE;

    // first arg could be placed sign id
    जर (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        जर (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        अन्यथा
        {
            arg = skipwhite(arg);
        }
    }

    पर्यंत (*arg != NUL)
    {
        जर (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = म_से_प((अक्षर *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        अन्यथा जर (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            जर (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                परत FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        अन्यथा जर (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            जर (*arg != NUL)
                *arg++ = NUL;

            पर्यंत (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        अन्यथा जर (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            जर (*arg != NUL)
                *arg++ = NUL;
        }
        अन्यथा जर (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = म_से_प((अक्षर *)arg);
            arg = skiptowhite(arg);
        }
        अन्यथा जर (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            थांबा;
        }
        अन्यथा जर (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((पूर्णांक)getdigits(&arg));

            जर (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            थांबा;
        }
        अन्यथा
        {
            emsg(_(e_invalid_argument));
            परत FAIL;
        }

        arg = skipwhite(arg);
    }

    जर (filename != रिक्त && *buf == रिक्त)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        परत FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    जर (filename == रिक्त &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    परत OK;
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
    पूर्णांक idx = sign_cmd_idx(arg, p);
    जर (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        परत;
    }
    arg = skipwhite(p);

    जर (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = रिक्त;
        पूर्णांक id = -1;
        char_u *group = रिक्त;
        पूर्णांक prio = -1;
        buf_T *buf = रिक्त;
        linenr_T lnum = -1;

        // Parse command line arguments
        जर (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            परत;

        जर (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        अन्यथा जर (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        अन्यथा जर (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        परत;
    }

    // Define, undefine or list signs.
    जर (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        साठी (sign_T *sp = first_sign; sp != रिक्त && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    अन्यथा जर (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    अन्यथा
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        जर (*p != NUL)
            *p++ = NUL;

        पर्यंत (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        जर (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        अन्यथा जर (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        अन्यथा
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

    जर (sp->sn_icon != रिक्त)
        dict_add_string(retdict, "icon", sp->sn_icon);

    जर (sp->sn_text != रिक्त)
        dict_add_string(retdict, "text", sp->sn_text);

    जर (sp->sn_priority > 0)
        dict_add_संख्या(retdict, "priority", sp->sn_priority);

    जर (sp->sn_ओळ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_ओळ_hl - 1, FALSE);
        जर (p == रिक्त)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अन्यथा
            dict_add_string(retdict, "linehl", p);
    }

    जर (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_text_hl - 1, FALSE);
        जर (p == रिक्त)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अन्यथा
            dict_add_string(retdict, "texthl", p);
    }

    जर (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_cul_hl - 1, FALSE);
        जर (p == रिक्त)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अन्यथा
            dict_add_string(retdict, "culhl", p);
    }

    जर (sp->sn_अंक_hl > 0)
    {
        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_अंक_hl - 1, FALSE);
        जर (p == रिक्त)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        अन्यथा
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

    जर (name != रिक्त)
    {
        sp = sign_find(name, रिक्त);
        जर (sp == रिक्त)
            परत;
    }

    साठी (; sp != रिक्त && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        जर (dict == रिक्त)
            परत;

        जर (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            परत;
        }

        sign_getinfo(sp, dict);

        जर (name != रिक्त) // handle only the specified sign
            थांबा;
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
        जर (d != रिक्त && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
स्थिर शून्य
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       पूर्णांक sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    जर (d == रिक्त)
        परत;

    जर (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        परत;
    }

    dict_add_संख्या(d, "bufnr", (दीर्घ)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    जर (l == रिक्त)
        परत;

    जर (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        परत;
    }

    sign_entry_T *sign = रिक्त;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        जर (!sign_in_group(sign, sign_group))
            सुरूठेवा;

        जर ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            जर (sdict != रिक्त && list_append_dict(l, sdict) == FAIL)
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
                पूर्णांक sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    जर (buf != रिक्त)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    अन्यथा
    {
        FOR_ALL_BUFFERS(buf)
        {
            जर (buf->b_signlist != रिक्त)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# जर defined(FEAT_SIGN_ICONS)
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
        जर (sp->sn_icon != रिक्त)
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
    अक्षर lbuf[संदेश_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    जर (sp->sn_icon != रिक्त)
    {
        संदेश_puts(" icon=");
        संदेश_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        जर (sp->sn_image == रिक्त)
            संदेश_puts(_(" (NOT FOUND)"));
# अन्यथा
        संदेश_puts(_(" (not supported)"));
# endif
    }

    जर (sp->sn_text != रिक्त)
    {
        संदेश_puts(" text=");
        संदेश_outtrans(sp->sn_text);
    }

    जर (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, संदेश_BUF_LEN, " priority=%d", sp->sn_priority);
        संदेश_puts(lbuf);
    }

    जर (sp->sn_ओळ_hl > 0)
    {
        संदेश_puts(" linehl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_ओळ_hl - 1, FALSE);
        जर (p == रिक्त)
            संदेश_puts("NONE");
        अन्यथा
            संदेश_puts((अक्षर *)p);
    }

    जर (sp->sn_text_hl > 0)
    {
        संदेश_puts(" texthl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_text_hl - 1, FALSE);
        जर (p == रिक्त)
            संदेश_puts("NONE");
        अन्यथा
            संदेश_puts((अक्षर *)p);
    }

    जर (sp->sn_cul_hl > 0)
    {
        संदेश_puts(" culhl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_cul_hl - 1, FALSE);
        जर (p == रिक्त)
            संदेश_puts("NONE");
        अन्यथा
            संदेश_puts((अक्षर *)p);
    }

    जर (sp->sn_अंक_hl > 0)
    {
        संदेश_puts(" numhl=");

        char_u *p = get_highlight_name_ext(रिक्त, sp->sn_अंक_hl - 1, FALSE);
        जर (p == रिक्त)
            संदेश_puts("NONE");
        अन्यथा
            संदेश_puts((अक्षर *)p);
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
    जर (sp->sn_image != रिक्त)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    जर (sp_prev == रिक्त)
        first_sign = sp->sn_next;
    अन्यथा
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# जर defined(FEAT_SIGN_ICONS)
शून्य *
sign_get_image(पूर्णांक typenr) // the attribute which may have a sign
{
    sign_T *sp = रिक्त;
    FOR_ALL_SIGNS(sp)
    {
        जर (sp->sn_typenr == typenr)
            परत sp->sn_image;
    }
    परत रिक्त;
}
# endif

/*
 * Undefine/free all signs.
 */
शून्य
free_signs(शून्य)
{
    पर्यंत (first_sign != रिक्त)
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
get_nth_sign_name(पूर्णांक idx)
{
    पूर्णांक current_idx = 0;
    sign_T *sp = रिक्त;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        जर (current_idx++ == idx)
            परत sp->sn_name;
    }
    परत रिक्त;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
स्थिर char_u *
get_nth_sign_group_name(पूर्णांक idx)
{
    पूर्णांक current_idx = 0;
    पूर्णांक todo = (पूर्णांक)sg_table.ht_used;
    hashitem_T *hi = रिक्त;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_वस्तुसंच(&sg_table, hi, todo)
    {
        जर (!HASHITEM_EMPTY(hi))
        {
            --todo;
            जर (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                परत group->sg_name;
            }
        }
    }
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    निवड (expand_what)
    {
        स्थिती EXP_SUBCMD:
            परत (char_u *)cmds[idx];
        स्थिती EXP_DEFINE:
        {
            अक्षर *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", रिक्त };
            परत (char_u *)define_arg[idx];
        }
        स्थिती EXP_PLACE:
        {
            अक्षर *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", रिक्त };
            परत (char_u *)place_arg[idx];
        }
        स्थिती EXP_LIST:
        {
            अक्षर *list_arg[] = { "group=", "file=", "buffer=", रिक्त };
            परत (char_u *)list_arg[idx];
        }
        स्थिती EXP_UNPLACE:
        {
            अक्षर *unplace_arg[] = { "group=", "file=", "buffer=", रिक्त };
            परत (char_u *)unplace_arg[idx];
        }
        स्थिती EXP_SIGN_NAMES:
            परत get_nth_sign_name(idx);
        स्थिती EXP_SIGN_GROUPS:
            परत get_nth_sign_group_name(idx);
        पूर्वनिर्धारित:
            परत रिक्त;
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
    पूर्णांक cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    जर (*end_subcmd == NUL)
        परत;

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
    करा
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    पर्यंत (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    जर (p == रिक्त)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        निवड (cmd_idx)
        {
            स्थिती SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                थांबा;
            स्थिती SIGNCMD_PLACE:
                // List placed signs
                जर (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                अन्यथा
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                थांबा;
            स्थिती SIGNCMD_LIST:
            स्थिती SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                थांबा;
            स्थिती SIGNCMD_JUMP:
            स्थिती SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                थांबा;
            पूर्वनिर्धारित:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    अन्यथा
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        निवड (cmd_idx)
        {
            स्थिती SIGNCMD_DEFINE:
                जर (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                अन्यथा जर (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                अन्यथा
                    xp->xp_context = EXPAND_NOTHING;
                थांबा;
            स्थिती SIGNCMD_PLACE:
                जर (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                अन्यथा जर (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                अन्यथा जर (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                अन्यथा
                    xp->xp_context = EXPAND_NOTHING;
                थांबा;
            स्थिती SIGNCMD_UNPLACE:
            स्थिती SIGNCMD_JUMP:
                जर (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                अन्यथा जर (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                अन्यथा
                    xp->xp_context = EXPAND_NOTHING;
                थांबा;
            पूर्वनिर्धारित:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
स्थिर पूर्णांक
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = रिक्त;
    char_u *icon = रिक्त;
    char_u *linehl = रिक्त;
    char_u *text = रिक्त;
    char_u *texthl = रिक्त;
    char_u *culhl = रिक्त;
    char_u *numhl = रिक्त;
    पूर्णांक prio = -1;
    पूर्णांक retval = -1;

    जर (name_arg == रिक्त && dict == रिक्त)
        परत retval;

    जर (name_arg == रिक्त)
        name = dict_get_string(dict, "name", TRUE);
    अन्यथा
        name = vim_strsave(name_arg);

    जर (name == रिक्त || name[0] == NUL)
        जा cleanup;

    जर (dict != रिक्त)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_संख्या_def(dict, "priority", -1);
    }

    जर (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    परत retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
स्थिर शून्य
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
        पूर्णांक retval = -1;

        जर (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(रिक्त, li->li_tv.vval.v_dict);
        अन्यथा
            emsg(_(e_dictionary_required));

        list_append_संख्या(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
शून्य
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        परत;

    जर (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        जर (rettv_list_alloc(rettv) == FAIL)
            परत;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        परत;
    }

    // Define a single sign
    rettv->vval.v_संख्या = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    जर (name == रिक्त)
        परत;

    जर (check_for_opt_dict_arg(argvars, 1) == FAIL)
        परत;

    rettv->vval.v_संख्या = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : रिक्त);
}

/*
 * "sign_getdefined()" function
 */
शून्य
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    जर (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        परत;

    जर (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        परत;

    char_u *name = रिक्त;
    जर (argvars[0].v_type != VAR_UNKNOWN)
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
    पूर्णांक sign_id = 0;
    char_u *group = रिक्त;

    जर (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        परत;

    जर (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        जर (buf == रिक्त)
            परत;

        जर (argvars[1].v_type != VAR_UNKNOWN)
        {
            जर (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                परत;

            dictitem_T *di = रिक्त;
            dict_T *dict = argvars[1].vval.v_dict;

            जर ((di = dict_find(dict, (char_u *)"lnum", -1)) != रिक्त)
            {
                // get signs placed at this line
                पूर्णांक notanum = FALSE;
                tv_get_संख्या_chk(&di->di_tv, &notanum);
                जर (notanum)
                    परत;

                lnum = tv_get_lnum(&di->di_tv);
            }

            जर ((di = dict_find(dict, (char_u *)"id", -1)) != रिक्त)
            {
                // get sign placed with this identifier
                पूर्णांक notanum = FALSE;
                sign_id = (पूर्णांक)tv_get_संख्या_chk(&di->di_tv, &notanum);
                जर (notanum)
                    परत;
            }

            जर ((di = dict_find(dict, (char_u *)"group", -1)) != रिक्त)
            {
                group = tv_get_string_chk(&di->di_tv);
                जर (group == रिक्त)
                    परत;

                जर (*group == '\0') // empty string means global group
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
    rettv->vval.v_संख्या = -1;

    जर (in_vim9script() && (check_for_संख्या_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        परत;

    पूर्णांक notanum = FALSE;
    // Sign identifier
    पूर्णांक sign_id = (पूर्णांक)tv_get_संख्या_chk(&argvars[0], &notanum);
    जर (notanum)
        परत;

    जर (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        परत;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    जर (sign_group == रिक्त)
        परत;

    जर (sign_group[0] == '\0')
    {
        sign_group = रिक्त; // global sign group
    }
    अन्यथा
    {
        sign_group = vim_strsave(sign_group);
        जर (sign_group == रिक्त)
            परत;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    जर (buf == रिक्त)
        जा cleanup;

    rettv->vval.v_संख्या = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
स्थिर पूर्णांक
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    पूर्णांक sign_id = 0;
    char_u *group = रिक्त;
    char_u *sign_name = रिक्त;
    buf_T *buf = रिक्त;
    dictitem_T *di = रिक्त;
    linenr_T lnum = 0;
    पूर्णांक prio = -1;
    पूर्णांक ret_sign_id = -1;

    // sign identifier
    जर (id_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        जर (di != रिक्त)
            id_tv = &di->di_tv;
    }

    जर (id_tv == रिक्त)
    {
        sign_id = 0;
    }
    अन्यथा
    {
        पूर्णांक notanum = FALSE;
        sign_id = tv_get_संख्या_chk(id_tv, &notanum);

        जर (notanum)
            परत -1;

        जर (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            परत -1;
        }
    }

    // sign group
    जर (group_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        जर (di != रिक्त)
            group_tv = &di->di_tv;
    }

    जर (group_tv == रिक्त)
    {
        group = रिक्त; // global group
    }
    अन्यथा
    {
        group = tv_get_string_chk(group_tv);
        जर (group == रिक्त)
            जा cleanup;

        जर (group[0] == '\0') // global sign group
        {
            group = रिक्त;
        }
        अन्यथा
        {
            group = vim_strsave(group);
            जर (group == रिक्त)
                परत -1;
        }
    }

    // sign name
    जर (name_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        जर (di != रिक्त)
            name_tv = &di->di_tv;
    }

    जर (name_tv == रिक्त)
        जा cleanup;

    sign_name = tv_get_string_chk(name_tv);
    जर (sign_name == रिक्त)
        जा cleanup;

    // buffer to place the sign
    जर (buf_tv == रिक्त)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        जर (di != रिक्त)
            buf_tv = &di->di_tv;
    }

    जर (buf_tv == रिक्त)
        जा cleanup;

    buf = get_buf_arg(buf_tv);
    जर (buf == रिक्त)
        जा cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    जर (di != रिक्त)
    {
        lnum = tv_get_lnum(&di->di_tv);
        जर (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            जा cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    जर (di != रिक्त)
    {
        पूर्णांक notanum = FALSE;
        prio = (पूर्णांक)tv_get_संख्या_chk(&di->di_tv, &notanum);
        जर (notanum)
            जा cleanup;
    }

    जर (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    परत ret_sign_id;
}

/*
 * "sign_place()" function
 */
शून्य
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = रिक्त;
    rettv->vval.v_संख्या = -1;

    जर (in_vim9script() && (check_for_संख्या_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        परत;

    जर (argvars[4].v_type != VAR_UNKNOWN)
    {
        जर (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            परत;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_संख्या = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
शून्य
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    जर (rettv_list_alloc(rettv) == FAIL)
        परत;

    जर (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        परत;

    जर (check_for_list_arg(argvars, 0) == FAIL)
        परत;

    // Process the List of sign attributes
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तुसंच(argvars[0].vval.v_list, li)
    {
        पूर्णांक sign_id = -1;

        जर (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(रिक्त, रिक्त, रिक्त, रिक्त,
                                           li->li_tv.vval.v_dict);
        अन्यथा
            emsg(_(e_dictionary_required));

        list_append_संख्या(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
स्थिर शून्य
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
        पूर्णांक retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        जर (name != रिक्त && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_संख्या(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
शून्य
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    जर (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        परत;

    जर (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        जर (rettv_list_alloc(rettv) == FAIL)
            परत;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        परत;
    }

    rettv->vval.v_संख्या = -1;

    जर (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_संख्या = 0;
    }
    अन्यथा
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        जर (name == रिक्त)
            परत;

        जर (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_संख्या = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
स्थिर पूर्णांक
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    पूर्णांक sign_id = 0;
    buf_T *buf = रिक्त;
    char_u *group = रिक्त;
    पूर्णांक retval = -1;

    // sign group
    group = (group_tv != रिक्त) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    जर (group != रिक्त)
    {
        जर (group[0] == '\0') // global sign group
        {
            group = रिक्त;
        }
        अन्यथा
        {
            group = vim_strsave(group);
            जर (group == रिक्त)
                परत retval;
        }
    }

    जर (dict != रिक्त)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        जर (di != रिक्त)
        {
            buf = get_buf_arg(&di->di_tv);
            जर (buf == रिक्त)
                जा cleanup;
        }

        जर (dict_has_किल्ली(dict, "id"))
        {
            sign_id = dict_get_संख्या(dict, "id");
            जर (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                जा cleanup;
            }
        }
    }

    जर (buf == रिक्त)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            जर (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    अन्यथा जर (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    परत retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    पर्यंत (sign != रिक्त && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    परत sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
पूर्णांक
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    जर (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        परत get_first_valid_sign(wp) != रिक्त && !wp->w_p_nu && !wp->w_p_rnu;

    जर (*wp->w_p_scl == 'n')
        परत FALSE;

    जर (*wp->w_p_scl == 'y')
        परत TRUE;

    परत (get_first_valid_sign(wp) != रिक्त
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
    rettv->vval.v_संख्या = -1;

    जर ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_संख्या = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
शून्य
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    जर (rettv_list_alloc(rettv) == FAIL)
        परत;

    जर (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        परत;

    जर (check_for_list_arg(argvars, 0) == FAIL)
        परत;

    listitem_T *li = रिक्त;
    FOR_ALL_LIST_वस्तुसंच(argvars[0].vval.v_list, li)
    {
        पूर्णांक retval = -1;

        जर (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(रिक्त, li->li_tv.vval.v_dict);
        अन्यथा
            emsg(_(e_dictionary_required));

        list_append_संख्या(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
