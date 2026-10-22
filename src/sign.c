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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
প্রকারনির্দেশ সংরচনা sign sign_T;

সংরচনা sign
{
    sign_T *sn_next; // next sign in list
    পূর্ণাঙ্ক sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    শূন্য *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    পূর্ণাঙ্ক sn_সাৰি_hl; // highlight ID for line
    পূর্ণাঙ্ক sn_text_hl; // highlight ID for text
    পূর্ণাঙ্ক sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    পূর্ণাঙ্ক sn_অঙ্ক_hl; // highlight ID for line number
    পূর্ণাঙ্ক sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

স্থির sign_T *first_sign = রিক্ত;
স্থির পূর্ণাঙ্ক next_sign_typenr = 1;

স্থির শূন্য sign_list_defined(sign_T *sp);
স্থির শূন্য sign_undefine(sign_T *sp, sign_T *sp_prev);

স্থির ৱর্ণ *cmds[] = { "define",
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
                        রিক্ত
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     বাবে ((sp) = first_sign; (sp) != রিক্ত; (sp) = (sp)->sn_next)

স্থির hashtab_T sg_table; // sign group (signgroup_T) hashtable
স্থির পূর্ণাঙ্ক next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
শূন্য
init_signs(শূন্য)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
স্থির signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = রিক্ত;

    যদি (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        যদি (group == রিক্ত)
            ঘূৰাই রিক্ত;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    অন্যথা
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    ঘূৰাই group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
স্থির শূন্য
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    যদি (HASHITEM_EMPTY(hi))
        ঘূৰাই;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    যদি (group->sg_refcount == 0)
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
স্থির পূর্ণাঙ্ক
sign_in_group(sign_entry_T *sign, char_u *group)
{
    ঘূৰাই ((group != রিক্ত && STRCMP(group, "*") == 0) ||
            (group == রিক্ত && sign->se_group == রিক্ত) ||
            (group != রিক্ত && sign->se_group != রিক্ত &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
স্থির পূর্ণাঙ্ক
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    পূর্ণাঙ্ক for_popup = sign->se_group != রিক্ত &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    ঘূৰাই WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
স্থির পূর্ণাঙ্ক
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    পূর্ণাঙ্ক id = 1;
    signgroup_T *group = রিক্ত;
    sign_entry_T *sign = রিক্ত;

    যদি (groupname != রিক্ত)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        যদি (HASHITEM_EMPTY(hi))
            ঘূৰাই id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    পূর্ণাঙ্ক found = FALSE;
    যেতিয়ালৈ (!found)
    {
        যদি (group == রিক্ত)
            id = next_sign_id++; // global group
        অন্যথা
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            যদি (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                ভঙ্গ;
            }
        }
    }

    ঘূৰাই id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
স্থির শূন্য
ঢোকাও_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            পূর্ণাঙ্ক id, // sign ID
            char_u *group, // sign group; NULL for global group
            পূর্ণাঙ্ক prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            পূর্ণাঙ্ক typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(পরিমাণ(sign_entry_T), FALSE, aid_ঢোকাও_sign);
    যদি (newsign == রিক্ত)
        ঘূৰাই;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    যদি (group != রিক্ত)
    {
        newsign->se_group = sign_group_ref(group);
        যদি (newsign->se_group == রিক্ত)
        {
            vim_free(newsign);
            ঘূৰাই;
        }
    }
    অন্যথা
    {
        newsign->se_group = রিক্ত;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    যদি (next != রিক্ত)
        next->se_prev = newsign;

    যদি (prev == রিক্ত)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        যদি (buf->b_signlist == রিক্ত)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_সাৰি_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        যদি (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    অন্যথা
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
স্থির শূন্য
ঢোকাও_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         পূর্ণাঙ্ক id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         পূর্ণাঙ্ক prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         পূর্ণাঙ্ক typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    যেতিয়ালৈ (prev != রিক্ত && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == রিক্ত) ? buf->b_signlist : prev->se_next;

    ঢোকাও_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
স্থির sign_T *
find_sign_by_typenr(পূর্ণাঙ্ক typenr)
{
    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
        যদি (sp->sn_typenr == typenr)
            ঘূৰাই sp;
    ঘূৰাই রিক্ত;
}

/*
 * Get the name of a sign by its typenr.
 */
স্থির char_u *
sign_typenr2name(পূর্ণাঙ্ক typenr)
{
    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
        যদি (sp->sn_typenr == typenr)
            ঘূৰাই sp->sn_name;
    ঘূৰাই (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
স্থির dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    যদি (d == রিক্ত)
        ঘূৰাই রিক্ত;

    dict_add_সংখ্যা(d, "id", sign->se_id);
    যদি (sign->se_group == রিক্ত)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    অন্যথা
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_সংখ্যা(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_সংখ্যা(d, "priority", sign->se_priority);

    ঘূৰাই d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
স্থির শূন্য
sign_sort_by_prio_on_সাৰি(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    যদি ((sign->se_prev == রিক্ত || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == রিক্ত || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        ঘূৰাই;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    যেতিয়ালৈ (p->se_prev != রিক্ত && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    যদি (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        যেতিয়ালৈ (p->se_next != রিক্ত && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    যদি (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    যদি (sign->se_prev != রিক্ত)
        sign->se_prev->se_next = sign->se_next;

    যদি (sign->se_next != রিক্ত)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = রিক্ত;
    sign->se_next = রিক্ত;

    // Re-insert 'sign' at the right place
    যদি (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        যদি (sign->se_prev != রিক্ত)
            sign->se_prev->se_next = sign;

        যদি (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    অন্যথা
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        যদি (sign->se_next != রিক্ত)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
স্থির শূন্য
buf_addsign(buf_T *buf, // buffer to store sign in
            পূর্ণাঙ্ক id, // sign ID
            char_u *groupname, // sign group
            পূর্ণাঙ্ক prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            পূর্ণাঙ্ক typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    sign_entry_T *prev = রিক্ত; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    যদি (buf->b_sign_finger != রিক্ত && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    বাবে (sign = (prev == রিক্ত) ? buf->b_signlist : prev->se_next; sign != রিক্ত;
         sign = sign->se_next)
    {
        যদি (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_সাৰি(buf, sign);
            ঘূৰাই;
        }
        অন্যথা যদি (lnum < sign->se_lnum)
        {
            ঢোকাও_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ঘূৰাই;
        }
        prev = sign;
    }

    ঢোকাও_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
স্থির linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     পূর্ণাঙ্ক markId, // sign ID
                     char_u *group, // sign group
                     পূর্ণাঙ্ক typenr, // typenr of sign we are adding
                     পূর্ণাঙ্ক prio) // sign priority
{
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        যদি (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_সাৰি(buf, sign);
            ঘূৰাই sign->se_lnum;
        }
    }

    ঘূৰাই (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
পূর্ণাঙ্ক
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = রিক্ত;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            ভঙ্গ;

        যদি (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            যদি (sp == রিক্ত)
                ঘূৰাই FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            যদি (sattr->sat_text != রিক্ত && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            যদি (sp->sn_সাৰি_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_সাৰি_hl);

            যদি (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            যদি (sp->sn_অঙ্ক_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_অঙ্ক_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            যদি (sign->se_next != রিক্ত &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                যদি (next_sp == রিক্ত)
                    ঘূৰাই FALSE;

                যদি (sattr->sat_icon == রিক্ত && sattr->sat_text == রিক্ত)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                যদি (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                যদি (sp->sn_সাৰি_hl <= 0 && next_sp->sn_সাৰি_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_সাৰি_hl);

                যদি (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                যদি (sp->sn_অঙ্ক_hl <= 0 && next_sp->sn_অঙ্ক_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_অঙ্ক_hl);
            }
            ঘূৰাই TRUE;
        }
    }
    ঘূৰাই FALSE;
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
            পূর্ণাঙ্ক id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = রিক্ত; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = রিক্ত; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    বাবে (sign_entry_T *sign = buf->b_signlist; sign != রিক্ত; sign = next)
    {
        next = sign->se_next;

        যদি ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            যদি (next != রিক্ত)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            যদি (sign->se_group != রিক্ত)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_সাৰি_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            যদি (group == রিক্ত || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                ভঙ্গ;
        }
        অন্যথা
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    যদি (buf->b_signlist == রিক্ত)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_সাৰি_abv_curs();
    }

    ঘূৰাই lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
পূর্ণাঙ্ক
buf_findsign(buf_T *buf, // buffer to store sign in
             পূর্ণাঙ্ক id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        যদি (sign->se_id == id && sign_in_group(sign, group))
            ঘূৰাই sign->se_lnum;

    ঘূৰাই 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
স্থির sign_entry_T *
buf_getsign_at_সাৰি(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            ভঙ্গ;

        যদি (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            ঘূৰাই sign;
    }

    ঘূৰাই রিক্ত;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
পূর্ণাঙ্ক
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_সাৰি(buf, lnum, groupname);
    যদি (sign != রিক্ত)
        ঘূৰাই sign->se_id;

    ঘূৰাই 0;
}

# যদি defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
পূর্ণাঙ্ক
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    পূর্ণাঙ্ক typenr) // sign type number
{
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            ভঙ্গ;

        যদি (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ঘূৰাই sign->se_id;
    }

    ঘূৰাই 0;
}

#  যদি defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
পূর্ণাঙ্ক
buf_signcount(buf_T *buf, linenr_T lnum)
{
    পূর্ণাঙ্ক গুনতি = 0;
    sign_entry_T *sign = রিক্ত; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            ভঙ্গ;

        যদি (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != রিক্ত)
            গুনতি++;
    }

    ঘূৰাই গুনতি;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
শূন্য
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = রিক্ত; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    যদি (buf->b_signlist != রিক্ত && curwin != রিক্ত)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_সাৰি_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = রিক্ত;

    বাবে (sign_entry_T *sign = buf->b_signlist; sign != রিক্ত; sign = next)
    {
        next = sign->se_next;
        যদি (sign_in_group(sign, group))
        {
            *lastp = next;

            যদি (next != রিক্ত)
                next->se_prev = sign->se_prev;

            যদি (sign->se_group != রিক্ত)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        অন্যথা
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
স্থির শূন্য
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    ৱর্ণ lbuf[বাৰ্তা_BUF_LEN];
    ৱর্ণ group[বাৰ্তা_BUF_LEN];

    বাৰ্তা_puts_title(_("\n--- Signs ---"));
    বাৰ্তা_putchar('\n');

    buf_T *buf = (rbuf == রিক্ত) ? firstbuf : rbuf;
    যেতিয়ালৈ (buf != রিক্ত && !got_int)
    {
        যদি (buf->b_signlist != রিক্ত)
        {
            vim_snprintf(lbuf, বাৰ্তা_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            বাৰ্তা_puts_attr(lbuf, HL_ATTR(HLF_D));
            বাৰ্তা_putchar('\n');
        }

        sign_entry_T *sign = রিক্ত;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            যদি (got_int)
                ভঙ্গ;

            যদি (!sign_in_group(sign, sign_group))
                অনুৱর্ত;

            যদি (sign->se_group != রিক্ত)
                vim_snprintf(group, বাৰ্তা_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            অন্যথা
                group[0] = '\0';

            vim_snprintf(lbuf, বাৰ্তা_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            বাৰ্তা_puts(lbuf);
            বাৰ্তা_putchar('\n');
        }

        যদি (rbuf != রিক্ত)
            ভঙ্গ;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
শূন্য
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = রিক্ত; // line changes may reorder, drop the finger
    sign_entry_T *sign = রিক্ত; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        যদি (sign->se_lnum < line1)
            অনুৱর্ত;

        linenr_T new_lnum = sign->se_lnum;

        যদি (sign->se_lnum <= line2)
        {
            যদি (amount != MAXLNUM)
                new_lnum += amount;
        }
        অন্যথা যদি (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        যদি (new_lnum <= curbuf->b_ml.ml_সাৰি_গুনতি)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
স্থির পূর্ণাঙ্ক
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    পূর্ণাঙ্ক idx = 0;
    ৱর্ণ save = *end_cmd;
    *end_cmd = NUL;

    যেতিয়ালৈ (cmds[idx] != রিক্ত && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    ঘূৰাই idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
স্থির sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    যদি (sp_prev != রিক্ত)
        *sp_prev = রিক্ত;

    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
    {
        যদি (STRCMP(sp->sn_name, name) == 0)
            ভঙ্গ;

        যদি (sp_prev != রিক্ত)
            *sp_prev = sp;
    }

    ঘূৰাই sp;
}

/*
 * Allocate a new sign
 */
স্থির sign_T *
alloc_new_sign(char_u *name)
{
    পূর্ণাঙ্ক start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(পরিমাণ(sign_T), aid_sign_define_by_name);
    যদি (sp == রিক্ত)
        ঘূৰাই রিক্ত;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    যেতিয়ালৈ (lp != রিক্ত)
    {
        যদি (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            যদি (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            যদি (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ঘূৰাই রিক্ত;
            }

            lp = first_sign; // start all over
            অনুৱর্ত;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    যদি (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    যদি (sp->sn_name == রিক্ত) // out of memory
    {
        vim_free(sp);
        ঘূৰাই রিক্ত;
    }

    ঘূৰাই sp;
}

/*
 * Initialize the icon information for a new sign
 */
স্থির শূন্য
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    যদি (gui.in_use)
    {
        out_flush();
        যদি (sp->sn_image != রিক্ত)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
স্থির পূর্ণাঙ্ক
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = রিক্ত;
    char_u *endp = text + (পূর্ণাঙ্ক)STRLEN(text);
    পূর্ণাঙ্ক cells = 0;

    // Remove backslashes so that it is possible to use a space.
    বাবে (s = text; s + 1 < endp; ++s)
    {
        যদি (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    যদি (has_mbyte)
    {
        বাবে (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            যদি (!vim_isprintc((*mb_ptr2char)(s)))
                ভঙ্গ;

            cells += (*mb_ptr2cells)(s);
        }
    }
    অন্যথা
    {
        বাবে (s = text; s < endp; ++s)
        {
            যদি (!vim_isprintc(*s))
                ভঙ্গ;
        }
        cells = (পূর্ণাঙ্ক)(s - text);
    }

    // Currently sign text must be one or two display cells
    যদি (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        ঘূৰাই FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    পূর্ণাঙ্ক len = (পূর্ণাঙ্ক)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    যদি (sp->sn_text != রিক্ত && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    ঘূৰাই OK;
}

/*
 * Define a new sign or update an existing sign
 */
পূর্ণাঙ্ক
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    পূর্ণাঙ্ক prio)
{
    sign_T *sp_prev = রিক্ত;
    sign_T *sp = sign_find(name, &sp_prev);
    যদি (sp == রিক্ত)
    {
        sp = alloc_new_sign(name);
        যদি (sp == রিক্ত)
            ঘূৰাই FAIL;

        // add the new sign to the list of signs
        যদি (sp_prev == রিক্ত)
            first_sign = sp;
        অন্যথা
            sp_prev->sn_next = sp;
    }
    অন্যথা
    {
        win_T *wp = রিক্ত;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            যদি (wp->w_buffer->b_signlist != রিক্ত)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    যদি (icon != রিক্ত)
        sign_define_init_icon(sp, icon);

    যদি (text != রিক্ত && (sign_define_init_text(sp, text) == FAIL))
        ঘূৰাই FAIL;

    sp->sn_priority = prio;

    যদি (linehl != রিক্ত)
    {
        যদি (*linehl == NUL)
            sp->sn_সাৰি_hl = 0;
        অন্যথা
            sp->sn_সাৰি_hl = syn_check_group(linehl, (পূর্ণাঙ্ক)STRLEN(linehl));
    }

    যদি (texthl != রিক্ত)
    {
        যদি (*texthl == NUL)
            sp->sn_text_hl = 0;
        অন্যথা
            sp->sn_text_hl = syn_check_group(texthl, (পূর্ণাঙ্ক)STRLEN(texthl));
    }

    যদি (culhl != রিক্ত)
    {
        যদি (*culhl == NUL)
            sp->sn_cul_hl = 0;
        অন্যথা
            sp->sn_cul_hl = syn_check_group(culhl, (পূর্ণাঙ্ক)STRLEN(culhl));
    }

    যদি (numhl != রিক্ত)
    {
        যদি (*numhl == NUL)
            sp->sn_অঙ্ক_hl = 0;
        অন্যথা
            sp->sn_অঙ্ক_hl = syn_check_group(numhl, (পূর্ণাঙ্ক)STRLEN(numhl));
    }

    ঘূৰাই OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
পূর্ণাঙ্ক
sign_exists_by_name(char_u *name)
{
    ঘূৰাই sign_find(name, রিক্ত) != রিক্ত;
}

/*
 * Free the sign specified by 'name'.
 */
পূর্ণাঙ্ক
sign_undefine_by_name(char_u *name, পূর্ণাঙ্ক give_error)
{
    sign_T *sp_prev = রিক্ত;
    sign_T *sp = sign_find(name, &sp_prev);
    যদি (sp == রিক্ত)
    {
        যদি (give_error)
            semsg(_(e_unknown_sign_str), name);
        ঘূৰাই FAIL;
    }
    sign_undefine(sp, sp_prev);

    ঘূৰাই OK;
}

/*
 * List the signs matching 'name'
 */
স্থির শূন্য
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, রিক্ত);
    যদি (sp != রিক্ত)
        sign_list_defined(sp);
    অন্যথা
        semsg(_(e_unknown_sign_str), name);
}

স্থির শূন্য
may_force_numberwidth_recompute(buf_T *buf, পূর্ণাঙ্ক unplace)
{
# যদি defined(FEAT_LINEBREAK)
    tabpage_T *tp = রিক্ত;
    win_T *wp = রিক্ত;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        যদি (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_সাৰি_গুনতি = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
পূর্ণাঙ্ক
sign_place(পূর্ণাঙ্ক *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           পূর্ণাঙ্ক prio)
{
    // Check for reserved character '*' in group name
    যদি (sign_group != রিক্ত && (*sign_group == '*' || *sign_group == '\0'))
        ঘূৰাই FAIL;

    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
    {
        যদি (STRCMP(sp->sn_name, sign_name) == 0)
            ভঙ্গ;
    }

    যদি (sp == রিক্ত)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ঘূৰাই FAIL;
    }

    যদি (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    যদি (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    যদি (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    অন্যথা
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    যদি (lnum > 0)
    {
        redraw_buf_সাৰি_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    অন্যথা
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

/*
 * Unplace the specified sign
 */
স্থির পূর্ণাঙ্ক
sign_unplace(পূর্ণাঙ্ক sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    যদি (buf->b_signlist == রিক্ত) // No signs in the buffer
        ঘূৰাই OK;

    যদি (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    অন্যথা
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        যদি (lnum == 0)
            ঘূৰাই FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    যদি (buf->b_signlist == রিক্ত)
        may_force_numberwidth_recompute(buf, TRUE);

    ঘূৰাই OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
স্থির শূন্য
sign_unplace_at_cursor(char_u *groupname)
{
    পূর্ণাঙ্ক id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    যদি (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    অন্যথা
        emsg(_(e_missing_sign_সংখ্যা));
}

/*
 * Jump to a sign.
 */
স্থির linenr_T
sign_jump(পূর্ণাঙ্ক sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    যদি (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ঘূৰাই -1;
    }

    // goto a sign ...
    যদি (buf_jump_open_win(buf) != রিক্ত)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    অন্যথা
    { // ... not currently in a window
        যদি (buf->b_fname == রিক্ত)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            ঘূৰাই -1;
        }
        setpcmark();
        যদি (do_ecmd(buf->b_fnum, রিক্ত, রিক্ত, রিক্ত, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ঘূৰাই -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ঘূৰাই lnum;
}

/*
 * ":sign define {name} ..." command
 */
স্থির শূন্য
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = রিক্ত;
    char_u *p = cmdline;
    char_u *icon = রিক্ত;
    char_u *text = রিক্ত;
    char_u *linehl = রিক্ত;
    char_u *texthl = রিক্ত;
    char_u *culhl = রিক্ত;
    char_u *numhl = রিক্ত;
    পূর্ণাঙ্ক prio = -1;
    পূর্ণাঙ্ক failed = FALSE;

    // set values for a defined sign.
    যেতিয়ালৈ (TRUE)
    {
        arg = skipwhite(p);
        যদি (*arg == NUL)
            ভঙ্গ;

        p = skiptowhite_esc(arg);
        যদি (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        অন্যথা যদি (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((ৱর্ণ *)arg);
        }
        অন্যথা
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            ভঙ্গ;
        }
    }

    যদি (!failed)
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
স্থির শূন্য
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               পূর্ণাঙ্ক id,
               char_u *group,
               পূর্ণাঙ্ক prio)
{
    যদি (id <= 0)
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
        যদি (lnum >= 0 || sign_name != রিক্ত || (group != রিক্ত && *group == '\0'))
            emsg(_(e_invalid_argument));
        অন্যথা
            sign_list_placed(buf, group);
    }
    অন্যথা
    {
        // Place a new sign
        যদি (sign_name == রিক্ত || buf == রিক্ত ||
            (group != রিক্ত && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            ঘূৰাই;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
স্থির শূন্য
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 পূর্ণাঙ্ক id,
                 char_u *group)
{
    যদি (lnum >= 0 || sign_name != রিক্ত || (group != রিক্ত && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        ঘূৰাই;
    }

    যদি (id == -2)
    {
        যদি (buf != রিক্ত)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        অন্যথা
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                যদি (buf->b_signlist != রিক্ত)
                    buf_delete_signs(buf, group);
            }
        }
    }
    অন্যথা
    {
        যদি (buf != রিক্ত)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        অন্যথা
        {
            যদি (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            অন্যথা
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
স্থির শূন্য
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              পূর্ণাঙ্ক id,
              char_u *group)
{
    যদি (sign_name == রিক্ত && group == রিক্ত && id == -1)
    {
        emsg(_(e_argument_required));
        ঘূৰাই;
    }

    যদি (buf == রিক্ত || (group != রিক্ত && *group == '\0') || lnum >= 0 ||
        sign_name != রিক্ত)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        ঘূৰাই;
    }

    (শূন্য)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
স্থির পূর্ণাঙ্ক
parse_sign_cmd_args(পূর্ণাঙ্ক cmd,
                    char_u *arg,
                    char_u **sign_name,
                    পূর্ণাঙ্ক *signid,
                    char_u **group,
                    পূর্ণাঙ্ক *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = রিক্ত;
    পূর্ণাঙ্ক lnum_arg = FALSE;

    // first arg could be placed sign id
    যদি (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        যদি (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        অন্যথা
        {
            arg = skipwhite(arg);
        }
    }

    যেতিয়ালৈ (*arg != NUL)
    {
        যদি (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((ৱর্ণ *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        অন্যথা যদি (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            যদি (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                ঘূৰাই FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        অন্যথা যদি (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            যদি (*arg != NUL)
                *arg++ = NUL;

            যেতিয়ালৈ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        অন্যথা যদি (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            যদি (*arg != NUL)
                *arg++ = NUL;
        }
        অন্যথা যদি (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((ৱর্ণ *)arg);
            arg = skiptowhite(arg);
        }
        অন্যথা যদি (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ভঙ্গ;
        }
        অন্যথা যদি (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((পূর্ণাঙ্ক)getdigits(&arg));

            যদি (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ভঙ্গ;
        }
        অন্যথা
        {
            emsg(_(e_invalid_argument));
            ঘূৰাই FAIL;
        }

        arg = skipwhite(arg);
    }

    যদি (filename != রিক্ত && *buf == রিক্ত)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        ঘূৰাই FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    যদি (filename == রিক্ত &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    ঘূৰাই OK;
}

/*
 * ":sign" command
 */
শূন্য
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    পূর্ণাঙ্ক idx = sign_cmd_idx(arg, p);
    যদি (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ঘূৰাই;
    }
    arg = skipwhite(p);

    যদি (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = রিক্ত;
        পূর্ণাঙ্ক id = -1;
        char_u *group = রিক্ত;
        পূর্ণাঙ্ক prio = -1;
        buf_T *buf = রিক্ত;
        linenr_T lnum = -1;

        // Parse command line arguments
        যদি (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            ঘূৰাই;

        যদি (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        অন্যথা যদি (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        অন্যথা যদি (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        ঘূৰাই;
    }

    // Define, undefine or list signs.
    যদি (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        বাবে (sign_T *sp = first_sign; sp != রিক্ত && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    অন্যথা যদি (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    অন্যথা
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        যদি (*p != NUL)
            *p++ = NUL;

        যেতিয়ালৈ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        যদি (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        অন্যথা যদি (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        অন্যথা
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
স্থির শূন্য
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    যদি (sp->sn_icon != রিক্ত)
        dict_add_string(retdict, "icon", sp->sn_icon);

    যদি (sp->sn_text != রিক্ত)
        dict_add_string(retdict, "text", sp->sn_text);

    যদি (sp->sn_priority > 0)
        dict_add_সংখ্যা(retdict, "priority", sp->sn_priority);

    যদি (sp->sn_সাৰি_hl > 0)
    {
        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_সাৰি_hl - 1, FALSE);
        যদি (p == রিক্ত)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "linehl", p);
    }

    যদি (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_text_hl - 1, FALSE);
        যদি (p == রিক্ত)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "texthl", p);
    }

    যদি (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_cul_hl - 1, FALSE);
        যদি (p == রিক্ত)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "culhl", p);
    }

    যদি (sp->sn_অঙ্ক_hl > 0)
    {
        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_অঙ্ক_hl - 1, FALSE);
        যদি (p == রিক্ত)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
স্থির শূন্য
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    যদি (name != রিক্ত)
    {
        sp = sign_find(name, রিক্ত);
        যদি (sp == রিক্ত)
            ঘূৰাই;
    }

    বাবে (; sp != রিক্ত && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        যদি (dict == রিক্ত)
            ঘূৰাই;

        যদি (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ঘূৰাই;
        }

        sign_getinfo(sp, dict);

        যদি (name != রিক্ত) // handle only the specified sign
            ভঙ্গ;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
শূন্য
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = রিক্ত;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        যদি (d != রিক্ত && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
স্থির শূন্য
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       পূর্ণাঙ্ক sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    যদি (d == রিক্ত)
        ঘূৰাই;

    যদি (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ঘূৰাই;
    }

    dict_add_সংখ্যা(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    যদি (l == রিক্ত)
        ঘূৰাই;

    যদি (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ঘূৰাই;
    }

    sign_entry_T *sign = রিক্ত;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        যদি (!sign_in_group(sign, sign_group))
            অনুৱর্ত;

        যদি ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            যদি (sdict != রিক্ত && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
স্থির শূন্য
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                পূর্ণাঙ্ক sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    যদি (buf != রিক্ত)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    অন্যথা
    {
        FOR_ALL_BUFFERS(buf)
        {
            যদি (buf->b_signlist != রিক্ত)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# যদি defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
শূন্য
sign_gui_started(শূন্য)
{
    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
    {
        যদি (sp->sn_icon != রিক্ত)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
স্থির শূন্য
sign_list_defined(sign_T *sp)
{
    ৱর্ণ lbuf[বাৰ্তা_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    যদি (sp->sn_icon != রিক্ত)
    {
        বাৰ্তা_puts(" icon=");
        বাৰ্তা_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        যদি (sp->sn_image == রিক্ত)
            বাৰ্তা_puts(_(" (NOT FOUND)"));
# অন্যথা
        বাৰ্তা_puts(_(" (not supported)"));
# endif
    }

    যদি (sp->sn_text != রিক্ত)
    {
        বাৰ্তা_puts(" text=");
        বাৰ্তা_outtrans(sp->sn_text);
    }

    যদি (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, বাৰ্তা_BUF_LEN, " priority=%d", sp->sn_priority);
        বাৰ্তা_puts(lbuf);
    }

    যদি (sp->sn_সাৰি_hl > 0)
    {
        বাৰ্তা_puts(" linehl=");

        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_সাৰি_hl - 1, FALSE);
        যদি (p == রিক্ত)
            বাৰ্তা_puts("NONE");
        অন্যথা
            বাৰ্তা_puts((ৱর্ণ *)p);
    }

    যদি (sp->sn_text_hl > 0)
    {
        বাৰ্তা_puts(" texthl=");

        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_text_hl - 1, FALSE);
        যদি (p == রিক্ত)
            বাৰ্তা_puts("NONE");
        অন্যথা
            বাৰ্তা_puts((ৱর্ণ *)p);
    }

    যদি (sp->sn_cul_hl > 0)
    {
        বাৰ্তা_puts(" culhl=");

        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_cul_hl - 1, FALSE);
        যদি (p == রিক্ত)
            বাৰ্তা_puts("NONE");
        অন্যথা
            বাৰ্তা_puts((ৱর্ণ *)p);
    }

    যদি (sp->sn_অঙ্ক_hl > 0)
    {
        বাৰ্তা_puts(" numhl=");

        char_u *p = get_highlight_name_ext(রিক্ত, sp->sn_অঙ্ক_hl - 1, FALSE);
        যদি (p == রিক্ত)
            বাৰ্তা_puts("NONE");
        অন্যথা
            বাৰ্তা_puts((ৱর্ণ *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
স্থির শূন্য
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    যদি (sp->sn_image != রিক্ত)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    যদি (sp_prev == রিক্ত)
        first_sign = sp->sn_next;
    অন্যথা
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# যদি defined(FEAT_SIGN_ICONS)
শূন্য *
sign_get_image(পূর্ণাঙ্ক typenr) // the attribute which may have a sign
{
    sign_T *sp = রিক্ত;
    FOR_ALL_SIGNS(sp)
    {
        যদি (sp->sn_typenr == typenr)
            ঘূৰাই sp->sn_image;
    }
    ঘূৰাই রিক্ত;
}
# endif

/*
 * Undefine/free all signs.
 */
শূন্য
free_signs(শূন্য)
{
    যেতিয়ালৈ (first_sign != রিক্ত)
        sign_undefine(first_sign, রিক্ত);
}

স্থির গণনা
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
স্থির char_u *
get_nth_sign_name(পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক current_idx = 0;
    sign_T *sp = রিক্ত;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        যদি (current_idx++ == idx)
            ঘূৰাই sp->sn_name;
    }
    ঘূৰাই রিক্ত;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
স্থির char_u *
get_nth_sign_group_name(পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক current_idx = 0;
    পূর্ণাঙ্ক todo = (পূর্ণাঙ্ক)sg_table.ht_used;
    hashitem_T *hi = রিক্ত;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_বস্তুসমূহ(&sg_table, hi, todo)
    {
        যদি (!HASHITEM_EMPTY(hi))
        {
            --todo;
            যদি (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                ঘূৰাই group->sg_name;
            }
        }
    }
    ঘূৰাই রিক্ত;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    ৱিকল্প (expand_what)
    {
        পক্ষ EXP_SUBCMD:
            ঘূৰাই (char_u *)cmds[idx];
        পক্ষ EXP_DEFINE:
        {
            ৱর্ণ *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", রিক্ত };
            ঘূৰাই (char_u *)define_arg[idx];
        }
        পক্ষ EXP_PLACE:
        {
            ৱর্ণ *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", রিক্ত };
            ঘূৰাই (char_u *)place_arg[idx];
        }
        পক্ষ EXP_LIST:
        {
            ৱর্ণ *list_arg[] = { "group=", "file=", "buffer=", রিক্ত };
            ঘূৰাই (char_u *)list_arg[idx];
        }
        পক্ষ EXP_UNPLACE:
        {
            ৱর্ণ *unplace_arg[] = { "group=", "file=", "buffer=", রিক্ত };
            ঘূৰাই (char_u *)unplace_arg[idx];
        }
        পক্ষ EXP_SIGN_NAMES:
            ঘূৰাই get_nth_sign_name(idx);
        পক্ষ EXP_SIGN_GROUPS:
            ঘূৰাই get_nth_sign_group_name(idx);
        স্ৱভাৱ:
            ঘূৰাই রিক্ত;
    }
}

/*
 * Handle command line completion for :sign command.
 */
শূন্য
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    পূর্ণাঙ্ক cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    যদি (*end_subcmd == NUL)
        ঘূৰাই;

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
    কৰা
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    যেতিয়ালৈ (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    যদি (p == রিক্ত)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        ৱিকল্প (cmd_idx)
        {
            পক্ষ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ভঙ্গ;
            পক্ষ SIGNCMD_PLACE:
                // List placed signs
                যদি (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                অন্যথা
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ভঙ্গ;
            পক্ষ SIGNCMD_LIST:
            পক্ষ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                ভঙ্গ;
            পক্ষ SIGNCMD_JUMP:
            পক্ষ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ভঙ্গ;
            স্ৱভাৱ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    অন্যথা
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        ৱিকল্প (cmd_idx)
        {
            পক্ষ SIGNCMD_DEFINE:
                যদি (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                অন্যথা যদি (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                ভঙ্গ;
            পক্ষ SIGNCMD_PLACE:
                যদি (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                অন্যথা যদি (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                অন্যথা যদি (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                ভঙ্গ;
            পক্ষ SIGNCMD_UNPLACE:
            পক্ষ SIGNCMD_JUMP:
                যদি (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                অন্যথা যদি (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                ভঙ্গ;
            স্ৱভাৱ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
স্থির পূর্ণাঙ্ক
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = রিক্ত;
    char_u *icon = রিক্ত;
    char_u *linehl = রিক্ত;
    char_u *text = রিক্ত;
    char_u *texthl = রিক্ত;
    char_u *culhl = রিক্ত;
    char_u *numhl = রিক্ত;
    পূর্ণাঙ্ক prio = -1;
    পূর্ণাঙ্ক retval = -1;

    যদি (name_arg == রিক্ত && dict == রিক্ত)
        ঘূৰাই retval;

    যদি (name_arg == রিক্ত)
        name = dict_get_string(dict, "name", TRUE);
    অন্যথা
        name = vim_strsave(name_arg);

    যদি (name == রিক্ত || name[0] == NUL)
        গচ্ছ cleanup;

    যদি (dict != রিক্ত)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_সংখ্যা_def(dict, "priority", -1);
    }

    যদি (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    ঘূৰাই retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
স্থির শূন্য
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = রিক্ত;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
        পূর্ণাঙ্ক retval = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(রিক্ত, li->li_tv.vval.v_dict);
        অন্যথা
            emsg(_(e_dictionary_required));

        list_append_সংখ্যা(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
শূন্য
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ঘূৰাই;

    যদি (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        যদি (rettv_list_alloc(rettv) == FAIL)
            ঘূৰাই;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ঘূৰাই;
    }

    // Define a single sign
    rettv->vval.v_সংখ্যা = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    যদি (name == রিক্ত)
        ঘূৰাই;

    যদি (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ঘূৰাই;

    rettv->vval.v_সংখ্যা = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : রিক্ত);
}

/*
 * "sign_getdefined()" function
 */
শূন্য
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    যদি (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ঘূৰাই;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    char_u *name = রিক্ত;
    যদি (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
শূন্য
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = রিক্ত;
    linenr_T lnum = 0;
    পূর্ণাঙ্ক sign_id = 0;
    char_u *group = রিক্ত;

    যদি (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ঘূৰাই;

    যদি (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ঘূৰাই;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        যদি (buf == রিক্ত)
            ঘূৰাই;

        যদি (argvars[1].v_type != VAR_UNKNOWN)
        {
            যদি (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ঘূৰাই;

            dictitem_T *di = রিক্ত;
            dict_T *dict = argvars[1].vval.v_dict;

            যদি ((di = dict_find(dict, (char_u *)"lnum", -1)) != রিক্ত)
            {
                // get signs placed at this line
                পূর্ণাঙ্ক notanum = FALSE;
                tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
                যদি (notanum)
                    ঘূৰাই;

                lnum = tv_get_lnum(&di->di_tv);
            }

            যদি ((di = dict_find(dict, (char_u *)"id", -1)) != রিক্ত)
            {
                // get sign placed with this identifier
                পূর্ণাঙ্ক notanum = FALSE;
                sign_id = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
                যদি (notanum)
                    ঘূৰাই;
            }

            যদি ((di = dict_find(dict, (char_u *)"group", -1)) != রিক্ত)
            {
                group = tv_get_string_chk(&di->di_tv);
                যদি (group == রিক্ত)
                    ঘূৰাই;

                যদি (*group == '\0') // empty string means global group
                    group = রিক্ত;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
শূন্য
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = রিক্ত;
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && (check_for_সংখ্যা_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        ঘূৰাই;

    পূর্ণাঙ্ক notanum = FALSE;
    // Sign identifier
    পূর্ণাঙ্ক sign_id = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], &notanum);
    যদি (notanum)
        ঘূৰাই;

    যদি (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        ঘূৰাই;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    যদি (sign_group == রিক্ত)
        ঘূৰাই;

    যদি (sign_group[0] == '\0')
    {
        sign_group = রিক্ত; // global sign group
    }
    অন্যথা
    {
        sign_group = vim_strsave(sign_group);
        যদি (sign_group == রিক্ত)
            ঘূৰাই;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    যদি (buf == রিক্ত)
        গচ্ছ cleanup;

    rettv->vval.v_সংখ্যা = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
স্থির পূর্ণাঙ্ক
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    পূর্ণাঙ্ক sign_id = 0;
    char_u *group = রিক্ত;
    char_u *sign_name = রিক্ত;
    buf_T *buf = রিক্ত;
    dictitem_T *di = রিক্ত;
    linenr_T lnum = 0;
    পূর্ণাঙ্ক prio = -1;
    পূর্ণাঙ্ক ret_sign_id = -1;

    // sign identifier
    যদি (id_tv == রিক্ত)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        যদি (di != রিক্ত)
            id_tv = &di->di_tv;
    }

    যদি (id_tv == রিক্ত)
    {
        sign_id = 0;
    }
    অন্যথা
    {
        পূর্ণাঙ্ক notanum = FALSE;
        sign_id = tv_get_সংখ্যা_chk(id_tv, &notanum);

        যদি (notanum)
            ঘূৰাই -1;

        যদি (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            ঘূৰাই -1;
        }
    }

    // sign group
    যদি (group_tv == রিক্ত)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        যদি (di != রিক্ত)
            group_tv = &di->di_tv;
    }

    যদি (group_tv == রিক্ত)
    {
        group = রিক্ত; // global group
    }
    অন্যথা
    {
        group = tv_get_string_chk(group_tv);
        যদি (group == রিক্ত)
            গচ্ছ cleanup;

        যদি (group[0] == '\0') // global sign group
        {
            group = রিক্ত;
        }
        অন্যথা
        {
            group = vim_strsave(group);
            যদি (group == রিক্ত)
                ঘূৰাই -1;
        }
    }

    // sign name
    যদি (name_tv == রিক্ত)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        যদি (di != রিক্ত)
            name_tv = &di->di_tv;
    }

    যদি (name_tv == রিক্ত)
        গচ্ছ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    যদি (sign_name == রিক্ত)
        গচ্ছ cleanup;

    // buffer to place the sign
    যদি (buf_tv == রিক্ত)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        যদি (di != রিক্ত)
            buf_tv = &di->di_tv;
    }

    যদি (buf_tv == রিক্ত)
        গচ্ছ cleanup;

    buf = get_buf_arg(buf_tv);
    যদি (buf == রিক্ত)
        গচ্ছ cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    যদি (di != রিক্ত)
    {
        lnum = tv_get_lnum(&di->di_tv);
        যদি (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            গচ্ছ cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    যদি (di != রিক্ত)
    {
        পূর্ণাঙ্ক notanum = FALSE;
        prio = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
        যদি (notanum)
            গচ্ছ cleanup;
    }

    যদি (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    ঘূৰাই ret_sign_id;
}

/*
 * "sign_place()" function
 */
শূন্য
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = রিক্ত;
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && (check_for_সংখ্যা_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ঘূৰাই;

    যদি (argvars[4].v_type != VAR_UNKNOWN)
    {
        যদি (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ঘূৰাই;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_সংখ্যা = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
শূন্য
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) == FAIL)
        ঘূৰাই;

    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    // Process the List of sign attributes
    listitem_T *li = রিক্ত;
    FOR_ALL_LIST_বস্তুসমূহ(argvars[0].vval.v_list, li)
    {
        পূর্ণাঙ্ক sign_id = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(রিক্ত, রিক্ত, রিক্ত, রিক্ত,
                                           li->li_tv.vval.v_dict);
        অন্যথা
            emsg(_(e_dictionary_required));

        list_append_সংখ্যা(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
স্থির শূন্য
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = রিক্ত;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
        পূর্ণাঙ্ক retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        যদি (name != রিক্ত && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_সংখ্যা(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
শূন্য
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    যদি (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    যদি (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        যদি (rettv_list_alloc(rettv) == FAIL)
            ঘূৰাই;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ঘূৰাই;
    }

    rettv->vval.v_সংখ্যা = -1;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        যদি (name == রিক্ত)
            ঘূৰাই;

        যদি (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_সংখ্যা = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
স্থির পূর্ণাঙ্ক
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    পূর্ণাঙ্ক sign_id = 0;
    buf_T *buf = রিক্ত;
    char_u *group = রিক্ত;
    পূর্ণাঙ্ক retval = -1;

    // sign group
    group = (group_tv != রিক্ত) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    যদি (group != রিক্ত)
    {
        যদি (group[0] == '\0') // global sign group
        {
            group = রিক্ত;
        }
        অন্যথা
        {
            group = vim_strsave(group);
            যদি (group == রিক্ত)
                ঘূৰাই retval;
        }
    }

    যদি (dict != রিক্ত)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        যদি (di != রিক্ত)
        {
            buf = get_buf_arg(&di->di_tv);
            যদি (buf == রিক্ত)
                গচ্ছ cleanup;
        }

        যদি (dict_has_চাবি(dict, "id"))
        {
            sign_id = dict_get_সংখ্যা(dict, "id");
            যদি (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                গচ্ছ cleanup;
            }
        }
    }

    যদি (buf == রিক্ত)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            যদি (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    অন্যথা যদি (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    ঘূৰাই retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    যেতিয়ালৈ (sign != রিক্ত && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    ঘূৰাই sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
পূর্ণাঙ্ক
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    যদি (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ঘূৰাই get_first_valid_sign(wp) != রিক্ত && !wp->w_p_nu && !wp->w_p_rnu;

    যদি (*wp->w_p_scl == 'n')
        ঘূৰাই FALSE;

    যদি (*wp->w_p_scl == 'y')
        ঘূৰাই TRUE;

    ঘূৰাই (get_first_valid_sign(wp) != রিক্ত
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
শূন্য
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = রিক্ত;
    rettv->vval.v_সংখ্যা = -1;

    যদি ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_সংখ্যা = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
শূন্য
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) == FAIL)
        ঘূৰাই;

    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
        ঘূৰাই;

    listitem_T *li = রিক্ত;
    FOR_ALL_LIST_বস্তুসমূহ(argvars[0].vval.v_list, li)
    {
        পূর্ণাঙ্ক retval = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(রিক্ত, li->li_tv.vval.v_dict);
        অন্যথা
            emsg(_(e_dictionary_required));

        list_append_সংখ্যা(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
