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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
প্রকারলেখ কাঠামো sign sign_T;

কাঠামো sign
{
    sign_T *sn_পরবর্তী; // next sign in list
    পূর্ণসংখ্যা sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    শূন্য *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    পূর্ণসংখ্যা sn_সারি_hl; // highlight ID for line
    পূর্ণসংখ্যা sn_text_hl; // highlight ID for text
    পূর্ণসংখ্যা sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    পূর্ণসংখ্যা sn_অঙ্ক_hl; // highlight ID for line number
    পূর্ণসংখ্যা sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

স্থির sign_T *first_sign = ফাঁকা;
স্থির পূর্ণসংখ্যা পরবর্তী_sign_typenr = 1;

স্থির শূন্য sign_list_defined(sign_T *sp);
স্থির শূন্য sign_undefine(sign_T *sp, sign_T *sp_পূর্ব);

স্থির অক্ষর *cmds[] = { "define",
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
                        ফাঁকা
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     জন্য ((sp) = first_sign; (sp) != ফাঁকা; (sp) = (sp)->sn_পরবর্তী)

স্থির hashtab_T sg_table; // sign group (signgroup_T) hashtable
স্থির পূর্ণসংখ্যা পরবর্তী_sign_id = 1; // next sign id in the global group

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
    signgroup_T *group = ফাঁকা;

    যদি (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        যদি (group == ফাঁকা)
            ফেরত ফাঁকা;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_পরবর্তী_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    অন্যথা
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    ফেরত group;
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
        ফেরত;

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
স্থির পূর্ণসংখ্যা
sign_in_group(sign_entry_T *sign, char_u *group)
{
    ফেরত ((group != ফাঁকা && STRCMP(group, "*") == 0) ||
            (group == ফাঁকা && sign->se_group == ফাঁকা) ||
            (group != ফাঁকা && sign->se_group != ফাঁকা &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
স্থির পূর্ণসংখ্যা
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    পূর্ণসংখ্যা for_popup = sign->se_group != ফাঁকা &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    ফেরত WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
স্থির পূর্ণসংখ্যা
sign_group_get_পরবর্তী_signid(buf_T *buf, char_u *groupname)
{
    পূর্ণসংখ্যা id = 1;
    signgroup_T *group = ফাঁকা;
    sign_entry_T *sign = ফাঁকা;

    যদি (groupname != ফাঁকা)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        যদি (HASHITEM_EMPTY(hi))
            ফেরত id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    পূর্ণসংখ্যা found = FALSE;
    যখন (!found)
    {
        যদি (group == ফাঁকা)
            id = পরবর্তী_sign_id++; // global group
        অন্যথা
            id = group->sg_পরবর্তী_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            যদি (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                বিরতি;
            }
        }
    }

    ফেরত id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
স্থির শূন্য
ঢোকাও_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *পূর্ব, // previous sign entry
            sign_entry_T *পরবর্তী, // next sign entry
            পূর্ণসংখ্যা id, // sign ID
            char_u *group, // sign group; NULL for global group
            পূর্ণসংখ্যা prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            পূর্ণসংখ্যা typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(আকার(sign_entry_T), FALSE, aid_ঢোকাও_sign);
    যদি (newsign == ফাঁকা)
        ফেরত;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    যদি (group != ফাঁকা)
    {
        newsign->se_group = sign_group_ref(group);
        যদি (newsign->se_group == ফাঁকা)
        {
            vim_free(newsign);
            ফেরত;
        }
    }
    অন্যথা
    {
        newsign->se_group = ফাঁকা;
    }

    newsign->se_priority = prio;
    newsign->se_পরবর্তী = পরবর্তী;
    newsign->se_পূর্ব = পূর্ব;
    যদি (পরবর্তী != ফাঁকা)
        পরবর্তী->se_পূর্ব = newsign;

    যদি (পূর্ব == ফাঁকা)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        যদি (buf->b_signlist == ফাঁকা)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_সারি_abv_curs();
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
        পূর্ব->se_পরবর্তী = newsign;
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
                         sign_entry_T *পূর্ব, // previous sign entry
                         পূর্ণসংখ্যা id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         পূর্ণসংখ্যা prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         পূর্ণসংখ্যা typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    যখন (পূর্ব != ফাঁকা && পূর্ব->se_lnum == lnum && পূর্ব->se_priority <= prio)
        পূর্ব = পূর্ব->se_পূর্ব;

    sign_entry_T *sign = (পূর্ব == ফাঁকা) ? buf->b_signlist : পূর্ব->se_পরবর্তী;

    ঢোকাও_sign(buf, পূর্ব, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
স্থির sign_T *
find_sign_by_typenr(পূর্ণসংখ্যা typenr)
{
    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
        যদি (sp->sn_typenr == typenr)
            ফেরত sp;
    ফেরত ফাঁকা;
}

/*
 * Get the name of a sign by its typenr.
 */
স্থির char_u *
sign_typenr2name(পূর্ণসংখ্যা typenr)
{
    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
        যদি (sp->sn_typenr == typenr)
            ফেরত sp->sn_name;
    ফেরত (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
স্থির dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    যদি (d == ফাঁকা)
        ফেরত ফাঁকা;

    dict_add_সংখ্যা(d, "id", sign->se_id);
    যদি (sign->se_group == ফাঁকা)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    অন্যথা
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_সংখ্যা(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_সংখ্যা(d, "priority", sign->se_priority);

    ফেরত d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
স্থির শূন্য
sign_sort_by_prio_on_সারি(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    যদি ((sign->se_পূর্ব == ফাঁকা || sign->se_পূর্ব->se_lnum != sign->se_lnum ||
         sign->se_পূর্ব->se_priority > sign->se_priority) &&
        (sign->se_পরবর্তী == ফাঁকা || sign->se_পরবর্তী->se_lnum != sign->se_lnum ||
         sign->se_পরবর্তী->se_priority < sign->se_priority))
        ফেরত;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    যখন (p->se_পূর্ব != ফাঁকা && p->se_পূর্ব->se_lnum == sign->se_lnum &&
           p->se_পূর্ব->se_priority <= sign->se_priority)
        p = p->se_পূর্ব;

    যদি (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_পরবর্তী;
        যখন (p->se_পরবর্তী != ফাঁকা && p->se_পরবর্তী->se_lnum == sign->se_lnum &&
               p->se_পরবর্তী->se_priority > sign->se_priority)
            p = p->se_পরবর্তী;
    }

    // Remove 'sign' from the list
    যদি (buf->b_signlist == sign)
        buf->b_signlist = sign->se_পরবর্তী;

    যদি (sign->se_পূর্ব != ফাঁকা)
        sign->se_পূর্ব->se_পরবর্তী = sign->se_পরবর্তী;

    যদি (sign->se_পরবর্তী != ফাঁকা)
        sign->se_পরবর্তী->se_পূর্ব = sign->se_পূর্ব;

    sign->se_পূর্ব = ফাঁকা;
    sign->se_পরবর্তী = ফাঁকা;

    // Re-insert 'sign' at the right place
    যদি (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_পূর্ব = p->se_পূর্ব;
        sign->se_পরবর্তী = p;
        p->se_পূর্ব = sign;
        যদি (sign->se_পূর্ব != ফাঁকা)
            sign->se_পূর্ব->se_পরবর্তী = sign;

        যদি (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    অন্যথা
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_পূর্ব = p;
        sign->se_পরবর্তী = p->se_পরবর্তী;
        p->se_পরবর্তী = sign;
        যদি (sign->se_পরবর্তী != ফাঁকা)
            sign->se_পরবর্তী->se_পূর্ব = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
স্থির শূন্য
buf_addsign(buf_T *buf, // buffer to store sign in
            পূর্ণসংখ্যা id, // sign ID
            char_u *groupname, // sign group
            পূর্ণসংখ্যা prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            পূর্ণসংখ্যা typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    sign_entry_T *পূর্ব = ফাঁকা; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    যদি (buf->b_sign_finger != ফাঁকা && buf->b_sign_finger->se_lnum < lnum)
        পূর্ব = buf->b_sign_finger;

    জন্য (sign = (পূর্ব == ফাঁকা) ? buf->b_signlist : পূর্ব->se_পরবর্তী; sign != ফাঁকা;
         sign = sign->se_পরবর্তী)
    {
        যদি (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_সারি(buf, sign);
            ফেরত;
        }
        অন্যথা যদি (lnum < sign->se_lnum)
        {
            ঢোকাও_sign_by_lnum_prio(buf, পূর্ব, id, groupname, prio, lnum,
                                     typenr);
            ফেরত;
        }
        পূর্ব = sign;
    }

    ঢোকাও_sign_by_lnum_prio(buf, পূর্ব, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
স্থির linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     পূর্ণসংখ্যা markId, // sign ID
                     char_u *group, // sign group
                     পূর্ণসংখ্যা typenr, // typenr of sign we are adding
                     পূর্ণসংখ্যা prio) // sign priority
{
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        যদি (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_সারি(buf, sign);
            ফেরত sign->se_lnum;
        }
    }

    ফেরত (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
পূর্ণসংখ্যা
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_বাফার;
    sign_entry_T *sign = ফাঁকা;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            বিরতি;

        যদি (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            যদি (sp == ফাঁকা)
                ফেরত FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            যদি (sattr->sat_text != ফাঁকা && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            যদি (sp->sn_সারি_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_সারি_hl);

            যদি (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            যদি (sp->sn_অঙ্ক_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_অঙ্ক_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            যদি (sign->se_পরবর্তী != ফাঁকা &&
                sign->se_পরবর্তী->se_priority == sign->se_priority &&
                sign->se_পরবর্তী->se_lnum == sign->se_lnum)
            {
                sign_T *পরবর্তী_sp = find_sign_by_typenr(sign->se_পরবর্তী->se_typenr);
                যদি (পরবর্তী_sp == ফাঁকা)
                    ফেরত FALSE;

                যদি (sattr->sat_icon == ফাঁকা && sattr->sat_text == ফাঁকা)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = পরবর্তী_sp->sn_image;
# endif
                    sattr->sat_text = পরবর্তী_sp->sn_text;
                }

                যদি (sp->sn_text_hl <= 0 && পরবর্তী_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(পরবর্তী_sp->sn_text_hl);

                যদি (sp->sn_সারি_hl <= 0 && পরবর্তী_sp->sn_সারি_hl > 0)
                    sattr->sat_linehl = syn_id2attr(পরবর্তী_sp->sn_সারি_hl);

                যদি (sp->sn_cul_hl <= 0 && পরবর্তী_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(পরবর্তী_sp->sn_cul_hl);

                যদি (sp->sn_অঙ্ক_hl <= 0 && পরবর্তী_sp->sn_অঙ্ক_hl > 0)
                    sattr->sat_numhl = syn_id2attr(পরবর্তী_sp->sn_অঙ্ক_hl);
            }
            ফেরত TRUE;
        }
    }
    ফেরত FALSE;
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
            পূর্ণসংখ্যা id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = ফাঁকা; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *পরবর্তী = ফাঁকা; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    জন্য (sign_entry_T *sign = buf->b_signlist; sign != ফাঁকা; sign = পরবর্তী)
    {
        পরবর্তী = sign->se_পরবর্তী;

        যদি ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = পরবর্তী;
            যদি (পরবর্তী != ফাঁকা)
                পরবর্তী->se_পূর্ব = sign->se_পূর্ব;

            lnum = sign->se_lnum;

            যদি (sign->se_group != ফাঁকা)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_সারি_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            যদি (group == ফাঁকা || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                বিরতি;
        }
        অন্যথা
        {
            lastp = &sign->se_পরবর্তী;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    যদি (buf->b_signlist == ফাঁকা)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_সারি_abv_curs();
    }

    ফেরত lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
পূর্ণসংখ্যা
buf_findsign(buf_T *buf, // buffer to store sign in
             পূর্ণসংখ্যা id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        যদি (sign->se_id == id && sign_in_group(sign, group))
            ফেরত sign->se_lnum;

    ফেরত 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
স্থির sign_entry_T *
buf_getsign_at_সারি(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            বিরতি;

        যদি (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            ফেরত sign;
    }

    ফেরত ফাঁকা;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
পূর্ণসংখ্যা
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_সারি(buf, lnum, groupname);
    যদি (sign != ফাঁকা)
        ফেরত sign->se_id;

    ফেরত 0;
}

# যদি defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
পূর্ণসংখ্যা
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    পূর্ণসংখ্যা typenr) // sign type number
{
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            বিরতি;

        যদি (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ফেরত sign->se_id;
    }

    ফেরত 0;
}

#  যদি defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
পূর্ণসংখ্যা
buf_signcount(buf_T *buf, linenr_T lnum)
{
    পূর্ণসংখ্যা গুনতি = 0;
    sign_entry_T *sign = ফাঁকা; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        যদি (sign->se_lnum > lnum)
            বিরতি;

        যদি (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ফাঁকা)
            গুনতি++;
    }

    ফেরত গুনতি;
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
    buf->b_sign_finger = ফাঁকা; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    যদি (buf->b_signlist != ফাঁকা && curwin != ফাঁকা)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_সারি_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *পরবর্তী = ফাঁকা;

    জন্য (sign_entry_T *sign = buf->b_signlist; sign != ফাঁকা; sign = পরবর্তী)
    {
        পরবর্তী = sign->se_পরবর্তী;
        যদি (sign_in_group(sign, group))
        {
            *lastp = পরবর্তী;

            যদি (পরবর্তী != ফাঁকা)
                পরবর্তী->se_পূর্ব = sign->se_পূর্ব;

            যদি (sign->se_group != ফাঁকা)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        অন্যথা
        {
            lastp = &sign->se_পরবর্তী;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
স্থির শূন্য
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    অক্ষর lbuf[বার্তা_BUF_LEN];
    অক্ষর group[বার্তা_BUF_LEN];

    বার্তা_puts_title(_("\n--- Signs ---"));
    বার্তা_putchar('\n');

    buf_T *buf = (rbuf == ফাঁকা) ? firstbuf : rbuf;
    যখন (buf != ফাঁকা && !got_int)
    {
        যদি (buf->b_signlist != ফাঁকা)
        {
            vim_snprintf(lbuf, বার্তা_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            বার্তা_puts_attr(lbuf, HL_ATTR(HLF_D));
            বার্তা_putchar('\n');
        }

        sign_entry_T *sign = ফাঁকা;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            যদি (got_int)
                বিরতি;

            যদি (!sign_in_group(sign, sign_group))
                চালু;

            যদি (sign->se_group != ফাঁকা)
                vim_snprintf(group, বার্তা_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            অন্যথা
                group[0] = '\0';

            vim_snprintf(lbuf, বার্তা_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            বার্তা_puts(lbuf);
            বার্তা_putchar('\n');
        }

        যদি (rbuf != ফাঁকা)
            বিরতি;

        buf = buf->b_পরবর্তী;
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
    curbuf->b_sign_finger = ফাঁকা; // line changes may reorder, drop the finger
    sign_entry_T *sign = ফাঁকা; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        যদি (sign->se_lnum < line1)
            চালু;

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
        যদি (new_lnum <= curbuf->b_ml.ml_সারি_গুনতি)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
স্থির পূর্ণসংখ্যা
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    পূর্ণসংখ্যা idx = 0;
    অক্ষর save = *end_cmd;
    *end_cmd = NUL;

    যখন (cmds[idx] != ফাঁকা && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    ফেরত idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
স্থির sign_T *
sign_find(char_u *name, sign_T **sp_পূর্ব)
{
    যদি (sp_পূর্ব != ফাঁকা)
        *sp_পূর্ব = ফাঁকা;

    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
    {
        যদি (STRCMP(sp->sn_name, name) == 0)
            বিরতি;

        যদি (sp_পূর্ব != ফাঁকা)
            *sp_পূর্ব = sp;
    }

    ফেরত sp;
}

/*
 * Allocate a new sign
 */
স্থির sign_T *
alloc_new_sign(char_u *name)
{
    পূর্ণসংখ্যা start = পরবর্তী_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(আকার(sign_T), aid_sign_define_by_name);
    যদি (sp == ফাঁকা)
        ফেরত ফাঁকা;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    যখন (lp != ফাঁকা)
    {
        যদি (lp->sn_typenr == পরবর্তী_sign_typenr)
        {
            ++পরবর্তী_sign_typenr;

            যদি (পরবর্তী_sign_typenr == MAX_TYPENR)
                পরবর্তী_sign_typenr = 1;

            যদি (পরবর্তী_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ফেরত ফাঁকা;
            }

            lp = first_sign; // start all over
            চালু;
        }
        lp = lp->sn_পরবর্তী;
    }

    sp->sn_typenr = পরবর্তী_sign_typenr;

    যদি (++পরবর্তী_sign_typenr == MAX_TYPENR)
        পরবর্তী_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    যদি (sp->sn_name == ফাঁকা) // out of memory
    {
        vim_free(sp);
        ফেরত ফাঁকা;
    }

    ফেরত sp;
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
        যদি (sp->sn_image != ফাঁকা)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
স্থির পূর্ণসংখ্যা
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = ফাঁকা;
    char_u *endp = text + (পূর্ণসংখ্যা)STRLEN(text);
    পূর্ণসংখ্যা cells = 0;

    // Remove backslashes so that it is possible to use a space.
    জন্য (s = text; s + 1 < endp; ++s)
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
        জন্য (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            যদি (!vim_isprintc((*mb_ptr2char)(s)))
                বিরতি;

            cells += (*mb_ptr2cells)(s);
        }
    }
    অন্যথা
    {
        জন্য (s = text; s < endp; ++s)
        {
            যদি (!vim_isprintc(*s))
                বিরতি;
        }
        cells = (পূর্ণসংখ্যা)(s - text);
    }

    // Currently sign text must be one or two display cells
    যদি (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        ফেরত FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    যদি (sp->sn_text != ফাঁকা && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    ফেরত OK;
}

/*
 * Define a new sign or update an existing sign
 */
পূর্ণসংখ্যা
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    পূর্ণসংখ্যা prio)
{
    sign_T *sp_পূর্ব = ফাঁকা;
    sign_T *sp = sign_find(name, &sp_পূর্ব);
    যদি (sp == ফাঁকা)
    {
        sp = alloc_new_sign(name);
        যদি (sp == ফাঁকা)
            ফেরত FAIL;

        // add the new sign to the list of signs
        যদি (sp_পূর্ব == ফাঁকা)
            first_sign = sp;
        অন্যথা
            sp_পূর্ব->sn_পরবর্তী = sp;
    }
    অন্যথা
    {
        win_T *wp = ফাঁকা;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            যদি (wp->w_বাফার->b_signlist != ফাঁকা)
                redraw_buf_later(wp->w_বাফার, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    যদি (icon != ফাঁকা)
        sign_define_init_icon(sp, icon);

    যদি (text != ফাঁকা && (sign_define_init_text(sp, text) == FAIL))
        ফেরত FAIL;

    sp->sn_priority = prio;

    যদি (linehl != ফাঁকা)
    {
        যদি (*linehl == NUL)
            sp->sn_সারি_hl = 0;
        অন্যথা
            sp->sn_সারি_hl = syn_check_group(linehl, (পূর্ণসংখ্যা)STRLEN(linehl));
    }

    যদি (texthl != ফাঁকা)
    {
        যদি (*texthl == NUL)
            sp->sn_text_hl = 0;
        অন্যথা
            sp->sn_text_hl = syn_check_group(texthl, (পূর্ণসংখ্যা)STRLEN(texthl));
    }

    যদি (culhl != ফাঁকা)
    {
        যদি (*culhl == NUL)
            sp->sn_cul_hl = 0;
        অন্যথা
            sp->sn_cul_hl = syn_check_group(culhl, (পূর্ণসংখ্যা)STRLEN(culhl));
    }

    যদি (numhl != ফাঁকা)
    {
        যদি (*numhl == NUL)
            sp->sn_অঙ্ক_hl = 0;
        অন্যথা
            sp->sn_অঙ্ক_hl = syn_check_group(numhl, (পূর্ণসংখ্যা)STRLEN(numhl));
    }

    ফেরত OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
পূর্ণসংখ্যা
sign_exists_by_name(char_u *name)
{
    ফেরত sign_find(name, ফাঁকা) != ফাঁকা;
}

/*
 * Free the sign specified by 'name'.
 */
পূর্ণসংখ্যা
sign_undefine_by_name(char_u *name, পূর্ণসংখ্যা give_error)
{
    sign_T *sp_পূর্ব = ফাঁকা;
    sign_T *sp = sign_find(name, &sp_পূর্ব);
    যদি (sp == ফাঁকা)
    {
        যদি (give_error)
            semsg(_(e_unknown_sign_str), name);
        ফেরত FAIL;
    }
    sign_undefine(sp, sp_পূর্ব);

    ফেরত OK;
}

/*
 * List the signs matching 'name'
 */
স্থির শূন্য
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, ফাঁকা);
    যদি (sp != ফাঁকা)
        sign_list_defined(sp);
    অন্যথা
        semsg(_(e_unknown_sign_str), name);
}

স্থির শূন্য
may_force_numberwidth_recompute(buf_T *buf, পূর্ণসংখ্যা unplace)
{
# যদি defined(FEAT_LINEBREAK)
    tabpage_T *tp = ফাঁকা;
    win_T *wp = ফাঁকা;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        যদি (wp->w_বাফার == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_সারি_গুনতি = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
পূর্ণসংখ্যা
sign_place(পূর্ণসংখ্যা *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           পূর্ণসংখ্যা prio)
{
    // Check for reserved character '*' in group name
    যদি (sign_group != ফাঁকা && (*sign_group == '*' || *sign_group == '\0'))
        ফেরত FAIL;

    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
    {
        যদি (STRCMP(sp->sn_name, sign_name) == 0)
            বিরতি;
    }

    যদি (sp == ফাঁকা)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ফেরত FAIL;
    }

    যদি (*sign_id == 0)
        *sign_id = sign_group_get_পরবর্তী_signid(buf, sign_group);

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
        redraw_buf_সারি_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    অন্যথা
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Unplace the specified sign
 */
স্থির পূর্ণসংখ্যা
sign_unplace(পূর্ণসংখ্যা sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    যদি (buf->b_signlist == ফাঁকা) // No signs in the buffer
        ফেরত OK;

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
            ফেরত FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    যদি (buf->b_signlist == ফাঁকা)
        may_force_numberwidth_recompute(buf, TRUE);

    ফেরত OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
স্থির শূন্য
sign_unplace_at_cursor(char_u *groupname)
{
    পূর্ণসংখ্যা id =
        buf_findsign_id(curwin->w_বাফার, curwin->w_cursor.lnum, groupname);

    যদি (id > 0)
        sign_unplace(id, groupname, curwin->w_বাফার, curwin->w_cursor.lnum);
    অন্যথা
        emsg(_(e_missing_sign_সংখ্যা));
}

/*
 * Jump to a sign.
 */
স্থির linenr_T
sign_jump(পূর্ণসংখ্যা sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    যদি (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ফেরত -1;
    }

    // goto a sign ...
    যদি (buf_jump_open_win(buf) != ফাঁকা)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    অন্যথা
    { // ... not currently in a window
        যদি (buf->b_fname == ফাঁকা)
        {
            emsg(_(e_cannot_jump_to_বাফার_that_does_not_have_name));
            ফেরত -1;
        }
        setpcmark();
        যদি (do_ecmd(buf->b_fnum, ফাঁকা, ফাঁকা, ফাঁকা, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ফেরত -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ফেরত lnum;
}

/*
 * ":sign define {name} ..." command
 */
স্থির শূন্য
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ফাঁকা;
    char_u *p = cmdline;
    char_u *icon = ফাঁকা;
    char_u *text = ফাঁকা;
    char_u *linehl = ফাঁকা;
    char_u *texthl = ফাঁকা;
    char_u *culhl = ফাঁকা;
    char_u *numhl = ফাঁকা;
    পূর্ণসংখ্যা prio = -1;
    পূর্ণসংখ্যা failed = FALSE;

    // set values for a defined sign.
    যখন (TRUE)
    {
        arg = skipwhite(p);
        যদি (*arg == NUL)
            বিরতি;

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
            prio = atoi((অক্ষর *)arg);
        }
        অন্যথা
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            বিরতি;
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
               পূর্ণসংখ্যা id,
               char_u *group,
               পূর্ণসংখ্যা prio)
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
        যদি (lnum >= 0 || sign_name != ফাঁকা || (group != ফাঁকা && *group == '\0'))
            emsg(_(e_invalid_argument));
        অন্যথা
            sign_list_placed(buf, group);
    }
    অন্যথা
    {
        // Place a new sign
        যদি (sign_name == ফাঁকা || buf == ফাঁকা ||
            (group != ফাঁকা && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            ফেরত;
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
                 পূর্ণসংখ্যা id,
                 char_u *group)
{
    যদি (lnum >= 0 || sign_name != ফাঁকা || (group != ফাঁকা && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        ফেরত;
    }

    যদি (id == -2)
    {
        যদি (buf != ফাঁকা)
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
                যদি (buf->b_signlist != ফাঁকা)
                    buf_delete_signs(buf, group);
            }
        }
    }
    অন্যথা
    {
        যদি (buf != ফাঁকা)
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
              পূর্ণসংখ্যা id,
              char_u *group)
{
    যদি (sign_name == ফাঁকা && group == ফাঁকা && id == -1)
    {
        emsg(_(e_argument_required));
        ফেরত;
    }

    যদি (buf == ফাঁকা || (group != ফাঁকা && *group == '\0') || lnum >= 0 ||
        sign_name != ফাঁকা)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        ফেরত;
    }

    (শূন্য)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
স্থির পূর্ণসংখ্যা
parse_sign_cmd_args(পূর্ণসংখ্যা cmd,
                    char_u *arg,
                    char_u **sign_name,
                    পূর্ণসংখ্যা *signid,
                    char_u **group,
                    পূর্ণসংখ্যা *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ফাঁকা;
    পূর্ণসংখ্যা lnum_arg = FALSE;

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

    যখন (*arg != NUL)
    {
        যদি (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((অক্ষর *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        অন্যথা যদি (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            যদি (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                ফেরত FAIL;
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

            যখন (name[0] == '0' && name[1] != NUL)
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
            *prio = atoi((অক্ষর *)arg);
            arg = skiptowhite(arg);
        }
        অন্যথা যদি (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            বিরতি;
        }
        অন্যথা যদি (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((পূর্ণসংখ্যা)getdigits(&arg));

            যদি (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            বিরতি;
        }
        অন্যথা
        {
            emsg(_(e_invalid_argument));
            ফেরত FAIL;
        }

        arg = skipwhite(arg);
    }

    যদি (filename != ফাঁকা && *buf == ফাঁকা)
    {
        semsg(_(e_invalid_বাফার_name_str), filename);
        ফেরত FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    যদি (filename == ফাঁকা &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_বাফার;

    ফেরত OK;
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
    পূর্ণসংখ্যা idx = sign_cmd_idx(arg, p);
    যদি (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ফেরত;
    }
    arg = skipwhite(p);

    যদি (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ফাঁকা;
        পূর্ণসংখ্যা id = -1;
        char_u *group = ফাঁকা;
        পূর্ণসংখ্যা prio = -1;
        buf_T *buf = ফাঁকা;
        linenr_T lnum = -1;

        // Parse command line arguments
        যদি (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            ফেরত;

        যদি (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        অন্যথা যদি (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        অন্যথা যদি (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        ফেরত;
    }

    // Define, undefine or list signs.
    যদি (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        জন্য (sign_T *sp = first_sign; sp != ফাঁকা && !got_int; sp = sp->sn_পরবর্তী)
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

        যখন (arg[0] == '0' && arg[1] != NUL)
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

    যদি (sp->sn_icon != ফাঁকা)
        dict_add_string(retdict, "icon", sp->sn_icon);

    যদি (sp->sn_text != ফাঁকা)
        dict_add_string(retdict, "text", sp->sn_text);

    যদি (sp->sn_priority > 0)
        dict_add_সংখ্যা(retdict, "priority", sp->sn_priority);

    যদি (sp->sn_সারি_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_সারি_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "linehl", p);
    }

    যদি (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_text_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "texthl", p);
    }

    যদি (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_cul_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        অন্যথা
            dict_add_string(retdict, "culhl", p);
    }

    যদি (sp->sn_অঙ্ক_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_অঙ্ক_hl - 1, FALSE);
        যদি (p == ফাঁকা)
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

    যদি (name != ফাঁকা)
    {
        sp = sign_find(name, ফাঁকা);
        যদি (sp == ফাঁকা)
            ফেরত;
    }

    জন্য (; sp != ফাঁকা && !got_int; sp = sp->sn_পরবর্তী)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        যদি (dict == ফাঁকা)
            ফেরত;

        যদি (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ফেরত;
        }

        sign_getinfo(sp, dict);

        যদি (name != ফাঁকা) // handle only the specified sign
            বিরতি;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
শূন্য
get_বাফার_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = ফাঁকা;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        যদি (d != ফাঁকা && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
স্থির শূন্য
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       পূর্ণসংখ্যা sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    যদি (d == ফাঁকা)
        ফেরত;

    যদি (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ফেরত;
    }

    dict_add_সংখ্যা(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    যদি (l == ফাঁকা)
        ফেরত;

    যদি (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ফেরত;
    }

    sign_entry_T *sign = ফাঁকা;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        যদি (!sign_in_group(sign, sign_group))
            চালু;

        যদি ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            যদি (sdict != ফাঁকা && list_append_dict(l, sdict) == FAIL)
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
                পূর্ণসংখ্যা sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    যদি (buf != ফাঁকা)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    অন্যথা
    {
        FOR_ALL_BUFFERS(buf)
        {
            যদি (buf->b_signlist != ফাঁকা)
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
    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
    {
        যদি (sp->sn_icon != ফাঁকা)
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
    অক্ষর lbuf[বার্তা_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    যদি (sp->sn_icon != ফাঁকা)
    {
        বার্তা_puts(" icon=");
        বার্তা_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        যদি (sp->sn_image == ফাঁকা)
            বার্তা_puts(_(" (NOT FOUND)"));
# অন্যথা
        বার্তা_puts(_(" (not supported)"));
# endif
    }

    যদি (sp->sn_text != ফাঁকা)
    {
        বার্তা_puts(" text=");
        বার্তা_outtrans(sp->sn_text);
    }

    যদি (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, বার্তা_BUF_LEN, " priority=%d", sp->sn_priority);
        বার্তা_puts(lbuf);
    }

    যদি (sp->sn_সারি_hl > 0)
    {
        বার্তা_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_সারি_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            বার্তা_puts("NONE");
        অন্যথা
            বার্তা_puts((অক্ষর *)p);
    }

    যদি (sp->sn_text_hl > 0)
    {
        বার্তা_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_text_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            বার্তা_puts("NONE");
        অন্যথা
            বার্তা_puts((অক্ষর *)p);
    }

    যদি (sp->sn_cul_hl > 0)
    {
        বার্তা_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_cul_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            বার্তা_puts("NONE");
        অন্যথা
            বার্তা_puts((অক্ষর *)p);
    }

    যদি (sp->sn_অঙ্ক_hl > 0)
    {
        বার্তা_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ফাঁকা, sp->sn_অঙ্ক_hl - 1, FALSE);
        যদি (p == ফাঁকা)
            বার্তা_puts("NONE");
        অন্যথা
            বার্তা_puts((অক্ষর *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
স্থির শূন্য
sign_undefine(sign_T *sp, sign_T *sp_পূর্ব)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    যদি (sp->sn_image != ফাঁকা)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    যদি (sp_পূর্ব == ফাঁকা)
        first_sign = sp->sn_পরবর্তী;
    অন্যথা
        sp_পূর্ব->sn_পরবর্তী = sp->sn_পরবর্তী;

    vim_free(sp);
}

# যদি defined(FEAT_SIGN_ICONS)
শূন্য *
sign_get_image(পূর্ণসংখ্যা typenr) // the attribute which may have a sign
{
    sign_T *sp = ফাঁকা;
    FOR_ALL_SIGNS(sp)
    {
        যদি (sp->sn_typenr == typenr)
            ফেরত sp->sn_image;
    }
    ফেরত ফাঁকা;
}
# endif

/*
 * Undefine/free all signs.
 */
শূন্য
free_signs(শূন্য)
{
    যখন (first_sign != ফাঁকা)
        sign_undefine(first_sign, ফাঁকা);
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
get_nth_sign_name(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা current_idx = 0;
    sign_T *sp = ফাঁকা;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        যদি (current_idx++ == idx)
            ফেরত sp->sn_name;
    }
    ফেরত ফাঁকা;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
স্থির char_u *
get_nth_sign_group_name(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা current_idx = 0;
    পূর্ণসংখ্যা todo = (পূর্ণসংখ্যা)sg_table.ht_used;
    hashitem_T *hi = ফাঁকা;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_বস্তুসমূহ(&sg_table, hi, todo)
    {
        যদি (!HASHITEM_EMPTY(hi))
        {
            --todo;
            যদি (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                ফেরত group->sg_name;
            }
        }
    }
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    নির্বাচন (expand_what)
    {
        ক্ষেত্র EXP_SUBCMD:
            ফেরত (char_u *)cmds[idx];
        ক্ষেত্র EXP_DEFINE:
        {
            অক্ষর *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ফাঁকা };
            ফেরত (char_u *)define_arg[idx];
        }
        ক্ষেত্র EXP_PLACE:
        {
            অক্ষর *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ফাঁকা };
            ফেরত (char_u *)place_arg[idx];
        }
        ক্ষেত্র EXP_LIST:
        {
            অক্ষর *list_arg[] = { "group=", "file=", "buffer=", ফাঁকা };
            ফেরত (char_u *)list_arg[idx];
        }
        ক্ষেত্র EXP_UNPLACE:
        {
            অক্ষর *unplace_arg[] = { "group=", "file=", "buffer=", ফাঁকা };
            ফেরত (char_u *)unplace_arg[idx];
        }
        ক্ষেত্র EXP_SIGN_NAMES:
            ফেরত get_nth_sign_name(idx);
        ক্ষেত্র EXP_SIGN_GROUPS:
            ফেরত get_nth_sign_group_name(idx);
        পূর্বনির্ধারিত:
            ফেরত ফাঁকা;
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
    পূর্ণসংখ্যা cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    যদি (*end_subcmd == NUL)
        ফেরত;

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
    করো
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    যখন (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    যদি (p == ফাঁকা)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        নির্বাচন (cmd_idx)
        {
            ক্ষেত্র SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                বিরতি;
            ক্ষেত্র SIGNCMD_PLACE:
                // List placed signs
                যদি (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                অন্যথা
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                বিরতি;
            ক্ষেত্র SIGNCMD_LIST:
            ক্ষেত্র SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                বিরতি;
            ক্ষেত্র SIGNCMD_JUMP:
            ক্ষেত্র SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                বিরতি;
            পূর্বনির্ধারিত:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    অন্যথা
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        নির্বাচন (cmd_idx)
        {
            ক্ষেত্র SIGNCMD_DEFINE:
                যদি (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                অন্যথা যদি (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                বিরতি;
            ক্ষেত্র SIGNCMD_PLACE:
                যদি (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                অন্যথা যদি (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                অন্যথা যদি (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                বিরতি;
            ক্ষেত্র SIGNCMD_UNPLACE:
            ক্ষেত্র SIGNCMD_JUMP:
                যদি (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                অন্যথা যদি (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                অন্যথা
                    xp->xp_context = EXPAND_NOTHING;
                বিরতি;
            পূর্বনির্ধারিত:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
স্থির পূর্ণসংখ্যা
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ফাঁকা;
    char_u *icon = ফাঁকা;
    char_u *linehl = ফাঁকা;
    char_u *text = ফাঁকা;
    char_u *texthl = ফাঁকা;
    char_u *culhl = ফাঁকা;
    char_u *numhl = ফাঁকা;
    পূর্ণসংখ্যা prio = -1;
    পূর্ণসংখ্যা retval = -1;

    যদি (name_arg == ফাঁকা && dict == ফাঁকা)
        ফেরত retval;

    যদি (name_arg == ফাঁকা)
        name = dict_get_string(dict, "name", TRUE);
    অন্যথা
        name = vim_strsave(name_arg);

    যদি (name == ফাঁকা || name[0] == NUL)
        যাও cleanup;

    যদি (dict != ফাঁকা)
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

    ফেরত retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
স্থির শূন্য
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ফাঁকা;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
        পূর্ণসংখ্যা retval = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ফাঁকা, li->li_tv.vval.v_dict);
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
        ফেরত;

    যদি (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        যদি (rettv_list_alloc(rettv) == FAIL)
            ফেরত;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ফেরত;
    }

    // Define a single sign
    rettv->vval.v_সংখ্যা = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    যদি (name == ফাঁকা)
        ফেরত;

    যদি (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ফেরত;

    rettv->vval.v_সংখ্যা = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ফাঁকা);
}

/*
 * "sign_getdefined()" function
 */
শূন্য
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    যদি (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ফেরত;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ফেরত;

    char_u *name = ফাঁকা;
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
    buf_T *buf = ফাঁকা;
    linenr_T lnum = 0;
    পূর্ণসংখ্যা sign_id = 0;
    char_u *group = ফাঁকা;

    যদি (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ফেরত;

    যদি (in_vim9script() && (check_for_opt_বাফার_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        যদি (buf == ফাঁকা)
            ফেরত;

        যদি (argvars[1].v_type != VAR_UNKNOWN)
        {
            যদি (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ফেরত;

            dictitem_T *di = ফাঁকা;
            dict_T *dict = argvars[1].vval.v_dict;

            যদি ((di = dict_find(dict, (char_u *)"lnum", -1)) != ফাঁকা)
            {
                // get signs placed at this line
                পূর্ণসংখ্যা notanum = FALSE;
                tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
                যদি (notanum)
                    ফেরত;

                lnum = tv_get_lnum(&di->di_tv);
            }

            যদি ((di = dict_find(dict, (char_u *)"id", -1)) != ফাঁকা)
            {
                // get sign placed with this identifier
                পূর্ণসংখ্যা notanum = FALSE;
                sign_id = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
                যদি (notanum)
                    ফেরত;
            }

            যদি ((di = dict_find(dict, (char_u *)"group", -1)) != ফাঁকা)
            {
                group = tv_get_string_chk(&di->di_tv);
                যদি (group == ফাঁকা)
                    ফেরত;

                যদি (*group == '\0') // empty string means global group
                    group = ফাঁকা;
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
    char_u *sign_group = ফাঁকা;
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && (check_for_সংখ্যা_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_বাফার_arg(argvars, 2) == FAIL))
        ফেরত;

    পূর্ণসংখ্যা notanum = FALSE;
    // Sign identifier
    পূর্ণসংখ্যা sign_id = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], &notanum);
    যদি (notanum)
        ফেরত;

    যদি (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        ফেরত;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    যদি (sign_group == ফাঁকা)
        ফেরত;

    যদি (sign_group[0] == '\0')
    {
        sign_group = ফাঁকা; // global sign group
    }
    অন্যথা
    {
        sign_group = vim_strsave(sign_group);
        যদি (sign_group == ফাঁকা)
            ফেরত;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    যদি (buf == ফাঁকা)
        যাও cleanup;

    rettv->vval.v_সংখ্যা = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
স্থির পূর্ণসংখ্যা
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    পূর্ণসংখ্যা sign_id = 0;
    char_u *group = ফাঁকা;
    char_u *sign_name = ফাঁকা;
    buf_T *buf = ফাঁকা;
    dictitem_T *di = ফাঁকা;
    linenr_T lnum = 0;
    পূর্ণসংখ্যা prio = -1;
    পূর্ণসংখ্যা ret_sign_id = -1;

    // sign identifier
    যদি (id_tv == ফাঁকা)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        যদি (di != ফাঁকা)
            id_tv = &di->di_tv;
    }

    যদি (id_tv == ফাঁকা)
    {
        sign_id = 0;
    }
    অন্যথা
    {
        পূর্ণসংখ্যা notanum = FALSE;
        sign_id = tv_get_সংখ্যা_chk(id_tv, &notanum);

        যদি (notanum)
            ফেরত -1;

        যদি (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            ফেরত -1;
        }
    }

    // sign group
    যদি (group_tv == ফাঁকা)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        যদি (di != ফাঁকা)
            group_tv = &di->di_tv;
    }

    যদি (group_tv == ফাঁকা)
    {
        group = ফাঁকা; // global group
    }
    অন্যথা
    {
        group = tv_get_string_chk(group_tv);
        যদি (group == ফাঁকা)
            যাও cleanup;

        যদি (group[0] == '\0') // global sign group
        {
            group = ফাঁকা;
        }
        অন্যথা
        {
            group = vim_strsave(group);
            যদি (group == ফাঁকা)
                ফেরত -1;
        }
    }

    // sign name
    যদি (name_tv == ফাঁকা)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        যদি (di != ফাঁকা)
            name_tv = &di->di_tv;
    }

    যদি (name_tv == ফাঁকা)
        যাও cleanup;

    sign_name = tv_get_string_chk(name_tv);
    যদি (sign_name == ফাঁকা)
        যাও cleanup;

    // buffer to place the sign
    যদি (buf_tv == ফাঁকা)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        যদি (di != ফাঁকা)
            buf_tv = &di->di_tv;
    }

    যদি (buf_tv == ফাঁকা)
        যাও cleanup;

    buf = get_buf_arg(buf_tv);
    যদি (buf == ফাঁকা)
        যাও cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    যদি (di != ফাঁকা)
    {
        lnum = tv_get_lnum(&di->di_tv);
        যদি (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            যাও cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    যদি (di != ফাঁকা)
    {
        পূর্ণসংখ্যা notanum = FALSE;
        prio = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&di->di_tv, &notanum);
        যদি (notanum)
            যাও cleanup;
    }

    যদি (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    ফেরত ret_sign_id;
}

/*
 * "sign_place()" function
 */
শূন্য
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ফাঁকা;
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && (check_for_সংখ্যা_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_বাফার_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ফেরত;

    যদি (argvars[4].v_type != VAR_UNKNOWN)
    {
        যদি (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ফেরত;
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
        ফেরত;

    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ফেরত;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
        ফেরত;

    // Process the List of sign attributes
    listitem_T *li = ফাঁকা;
    FOR_ALL_LIST_বস্তুসমূহ(argvars[0].vval.v_list, li)
    {
        পূর্ণসংখ্যা sign_id = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা,
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
    listitem_T *li = ফাঁকা;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
        পূর্ণসংখ্যা retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        যদি (name != ফাঁকা && (sign_undefine_by_name(name, TRUE) == OK))
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
        ফেরত;

    যদি (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        যদি (rettv_list_alloc(rettv) == FAIL)
            ফেরত;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ফেরত;
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
        যদি (name == ফাঁকা)
            ফেরত;

        যদি (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_সংখ্যা = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
স্থির পূর্ণসংখ্যা
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    পূর্ণসংখ্যা sign_id = 0;
    buf_T *buf = ফাঁকা;
    char_u *group = ফাঁকা;
    পূর্ণসংখ্যা retval = -1;

    // sign group
    group = (group_tv != ফাঁকা) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    যদি (group != ফাঁকা)
    {
        যদি (group[0] == '\0') // global sign group
        {
            group = ফাঁকা;
        }
        অন্যথা
        {
            group = vim_strsave(group);
            যদি (group == ফাঁকা)
                ফেরত retval;
        }
    }

    যদি (dict != ফাঁকা)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        যদি (di != ফাঁকা)
        {
            buf = get_buf_arg(&di->di_tv);
            যদি (buf == ফাঁকা)
                যাও cleanup;
        }

        যদি (dict_has_চাবি(dict, "id"))
        {
            sign_id = dict_get_সংখ্যা(dict, "id");
            যদি (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                যাও cleanup;
            }
        }
    }

    যদি (buf == ফাঁকা)
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

    ফেরত retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_বাফার->b_signlist;

# ifdef FEAT_PROP_POPUP
    যখন (sign != ফাঁকা && !sign_group_for_window(sign, wp))
        sign = sign->se_পরবর্তী;
# endif
    ফেরত sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
পূর্ণসংখ্যা
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    যদি (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ফেরত get_first_valid_sign(wp) != ফাঁকা && !wp->w_p_nu && !wp->w_p_rnu;

    যদি (*wp->w_p_scl == 'n')
        ফেরত FALSE;

    যদি (*wp->w_p_scl == 'y')
        ফেরত TRUE;

    ফেরত (get_first_valid_sign(wp) != ফাঁকা
# ifdef FEAT_NETBEANS_INTG
            || wp->w_বাফার->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
শূন্য
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ফাঁকা;
    rettv->vval.v_সংখ্যা = -1;

    যদি ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ফেরত;

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
        ফেরত;

    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ফেরত;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
        ফেরত;

    listitem_T *li = ফাঁকা;
    FOR_ALL_LIST_বস্তুসমূহ(argvars[0].vval.v_list, li)
    {
        পূর্ণসংখ্যা retval = -1;

        যদি (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ফাঁকা, li->li_tv.vval.v_dict);
        অন্যথা
            emsg(_(e_dictionary_required));

        list_append_সংখ্যা(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
