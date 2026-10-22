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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
પ્રકારવ્યાખ્યા સંરચના sign sign_T;

સંરચના sign
{
    sign_T *sn_next; // next sign in list
    પૂર્ણાંક sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    શૂન્ય *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    પૂર્ણાંક sn_લીટી_hl; // highlight ID for line
    પૂર્ણાંક sn_text_hl; // highlight ID for text
    પૂર્ણાંક sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    પૂર્ણાંક sn_અંક_hl; // highlight ID for line number
    પૂર્ણાંક sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

સ્થિર sign_T *first_sign = ખાલી;
સ્થિર પૂર્ણાંક next_sign_typenr = 1;

સ્થિર શૂન્ય sign_list_defined(sign_T *sp);
સ્થિર શૂન્ય sign_undefine(sign_T *sp, sign_T *sp_prev);

સ્થિર અક્ષર *cmds[] = { "define",
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
                        ખાલી
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     માટે ((sp) = first_sign; (sp) != ખાલી; (sp) = (sp)->sn_next)

સ્થિર hashtab_T sg_table; // sign group (signgroup_T) hashtable
સ્થિર પૂર્ણાંક next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
શૂન્ય
init_signs(શૂન્ય)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
સ્થિર signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = ખાલી;

    જો (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        જો (group == ખાલી)
            પાછા ખાલી;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    નહીંતર
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    પાછા group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
સ્થિર શૂન્ય
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    જો (HASHITEM_EMPTY(hi))
        પાછા;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    જો (group->sg_refcount == 0)
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
સ્થિર પૂર્ણાંક
sign_in_group(sign_entry_T *sign, char_u *group)
{
    પાછા ((group != ખાલી && STRCMP(group, "*") == 0) ||
            (group == ખાલી && sign->se_group == ખાલી) ||
            (group != ખાલી && sign->se_group != ખાલી &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
સ્થિર પૂર્ણાંક
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    પૂર્ણાંક for_popup = sign->se_group != ખાલી &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    પાછા WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
સ્થિર પૂર્ણાંક
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    પૂર્ણાંક id = 1;
    signgroup_T *group = ખાલી;
    sign_entry_T *sign = ખાલી;

    જો (groupname != ખાલી)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        જો (HASHITEM_EMPTY(hi))
            પાછા id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    પૂર્ણાંક found = FALSE;
    જ્યારે (!found)
    {
        જો (group == ખાલી)
            id = next_sign_id++; // global group
        નહીંતર
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            જો (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                તોડો;
            }
        }
    }

    પાછા id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
સ્થિર શૂન્ય
ઉમેરો_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            પૂર્ણાંક id, // sign ID
            char_u *group, // sign group; NULL for global group
            પૂર્ણાંક prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            પૂર્ણાંક typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(કદ(sign_entry_T), FALSE, aid_ઉમેરો_sign);
    જો (newsign == ખાલી)
        પાછા;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    જો (group != ખાલી)
    {
        newsign->se_group = sign_group_ref(group);
        જો (newsign->se_group == ખાલી)
        {
            vim_free(newsign);
            પાછા;
        }
    }
    નહીંતર
    {
        newsign->se_group = ખાલી;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    જો (next != ખાલી)
        next->se_prev = newsign;

    જો (prev == ખાલી)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        જો (buf->b_signlist == ખાલી)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_લીટી_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        જો (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    નહીંતર
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
સ્થિર શૂન્ય
ઉમેરો_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         પૂર્ણાંક id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         પૂર્ણાંક prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         પૂર્ણાંક typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    જ્યારે (prev != ખાલી && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == ખાલી) ? buf->b_signlist : prev->se_next;

    ઉમેરો_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
સ્થિર sign_T *
find_sign_by_typenr(પૂર્ણાંક typenr)
{
    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
        જો (sp->sn_typenr == typenr)
            પાછા sp;
    પાછા ખાલી;
}

/*
 * Get the name of a sign by its typenr.
 */
સ્થિર char_u *
sign_typenr2name(પૂર્ણાંક typenr)
{
    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
        જો (sp->sn_typenr == typenr)
            પાછા sp->sn_name;
    પાછા (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
સ્થિર dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    જો (d == ખાલી)
        પાછા ખાલી;

    dict_add_સંખ્યા(d, "id", sign->se_id);
    જો (sign->se_group == ખાલી)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    નહીંતર
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_સંખ્યા(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_સંખ્યા(d, "priority", sign->se_priority);

    પાછા d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
સ્થિર શૂન્ય
sign_sort_by_prio_on_લીટી(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    જો ((sign->se_prev == ખાલી || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == ખાલી || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        પાછા;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    જ્યારે (p->se_prev != ખાલી && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    જો (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        જ્યારે (p->se_next != ખાલી && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    જો (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    જો (sign->se_prev != ખાલી)
        sign->se_prev->se_next = sign->se_next;

    જો (sign->se_next != ખાલી)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = ખાલી;
    sign->se_next = ખાલી;

    // Re-insert 'sign' at the right place
    જો (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        જો (sign->se_prev != ખાલી)
            sign->se_prev->se_next = sign;

        જો (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    નહીંતર
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        જો (sign->se_next != ખાલી)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
સ્થિર શૂન્ય
buf_addsign(buf_T *buf, // buffer to store sign in
            પૂર્ણાંક id, // sign ID
            char_u *groupname, // sign group
            પૂર્ણાંક prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            પૂર્ણાંક typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    sign_entry_T *prev = ખાલી; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    જો (buf->b_sign_finger != ખાલી && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    માટે (sign = (prev == ખાલી) ? buf->b_signlist : prev->se_next; sign != ખાલી;
         sign = sign->se_next)
    {
        જો (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_લીટી(buf, sign);
            પાછા;
        }
        નહીંતર જો (lnum < sign->se_lnum)
        {
            ઉમેરો_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            પાછા;
        }
        prev = sign;
    }

    ઉમેરો_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
સ્થિર linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     પૂર્ણાંક markId, // sign ID
                     char_u *group, // sign group
                     પૂર્ણાંક typenr, // typenr of sign we are adding
                     પૂર્ણાંક prio) // sign priority
{
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        જો (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_લીટી(buf, sign);
            પાછા sign->se_lnum;
        }
    }

    પાછા (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
પૂર્ણાંક
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = ખાલી;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        જો (sign->se_lnum > lnum)
            તોડો;

        જો (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            જો (sp == ખાલી)
                પાછા FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            જો (sattr->sat_text != ખાલી && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            જો (sp->sn_લીટી_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_લીટી_hl);

            જો (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            જો (sp->sn_અંક_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_અંક_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            જો (sign->se_next != ખાલી &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                જો (next_sp == ખાલી)
                    પાછા FALSE;

                જો (sattr->sat_icon == ખાલી && sattr->sat_text == ખાલી)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                જો (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                જો (sp->sn_લીટી_hl <= 0 && next_sp->sn_લીટી_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_લીટી_hl);

                જો (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                જો (sp->sn_અંક_hl <= 0 && next_sp->sn_અંક_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_અંક_hl);
            }
            પાછા TRUE;
        }
    }
    પાછા FALSE;
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
            પૂર્ણાંક id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = ખાલી; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ખાલી; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    માટે (sign_entry_T *sign = buf->b_signlist; sign != ખાલી; sign = next)
    {
        next = sign->se_next;

        જો ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            જો (next != ખાલી)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            જો (sign->se_group != ખાલી)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_લીટી_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            જો (group == ખાલી || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                તોડો;
        }
        નહીંતર
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    જો (buf->b_signlist == ખાલી)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_લીટી_abv_curs();
    }

    પાછા lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
પૂર્ણાંક
buf_findsign(buf_T *buf, // buffer to store sign in
             પૂર્ણાંક id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        જો (sign->se_id == id && sign_in_group(sign, group))
            પાછા sign->se_lnum;

    પાછા 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
સ્થિર sign_entry_T *
buf_getsign_at_લીટી(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        જો (sign->se_lnum > lnum)
            તોડો;

        જો (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            પાછા sign;
    }

    પાછા ખાલી;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
પૂર્ણાંક
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_લીટી(buf, lnum, groupname);
    જો (sign != ખાલી)
        પાછા sign->se_id;

    પાછા 0;
}

# જો defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
પૂર્ણાંક
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    પૂર્ણાંક typenr) // sign type number
{
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        જો (sign->se_lnum > lnum)
            તોડો;

        જો (sign->se_lnum == lnum && sign->se_typenr == typenr)
            પાછા sign->se_id;
    }

    પાછા 0;
}

#  જો defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
પૂર્ણાંક
buf_signcount(buf_T *buf, linenr_T lnum)
{
    પૂર્ણાંક ગણતરી = 0;
    sign_entry_T *sign = ખાલી; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        જો (sign->se_lnum > lnum)
            તોડો;

        જો (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ખાલી)
            ગણતરી++;
    }

    પાછા ગણતરી;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
શૂન્ય
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = ખાલી; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    જો (buf->b_signlist != ખાલી && curwin != ખાલી)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_લીટી_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ખાલી;

    માટે (sign_entry_T *sign = buf->b_signlist; sign != ખાલી; sign = next)
    {
        next = sign->se_next;
        જો (sign_in_group(sign, group))
        {
            *lastp = next;

            જો (next != ખાલી)
                next->se_prev = sign->se_prev;

            જો (sign->se_group != ખાલી)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        નહીંતર
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
સ્થિર શૂન્ય
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    અક્ષર lbuf[સંદેશ_BUF_LEN];
    અક્ષર group[સંદેશ_BUF_LEN];

    સંદેશ_puts_title(_("\n--- Signs ---"));
    સંદેશ_putchar('\n');

    buf_T *buf = (rbuf == ખાલી) ? firstbuf : rbuf;
    જ્યારે (buf != ખાલી && !got_int)
    {
        જો (buf->b_signlist != ખાલી)
        {
            vim_snprintf(lbuf, સંદેશ_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            સંદેશ_puts_attr(lbuf, HL_ATTR(HLF_D));
            સંદેશ_putchar('\n');
        }

        sign_entry_T *sign = ખાલી;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            જો (got_int)
                તોડો;

            જો (!sign_in_group(sign, sign_group))
                ચાલુ;

            જો (sign->se_group != ખાલી)
                vim_snprintf(group, સંદેશ_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            નહીંતર
                group[0] = '\0';

            vim_snprintf(lbuf, સંદેશ_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            સંદેશ_puts(lbuf);
            સંદેશ_putchar('\n');
        }

        જો (rbuf != ખાલી)
            તોડો;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
શૂન્ય
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = ખાલી; // line changes may reorder, drop the finger
    sign_entry_T *sign = ખાલી; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        જો (sign->se_lnum < line1)
            ચાલુ;

        linenr_T new_lnum = sign->se_lnum;

        જો (sign->se_lnum <= line2)
        {
            જો (amount != MAXLNUM)
                new_lnum += amount;
        }
        નહીંતર જો (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        જો (new_lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
સ્થિર પૂર્ણાંક
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    પૂર્ણાંક idx = 0;
    અક્ષર save = *end_cmd;
    *end_cmd = NUL;

    જ્યારે (cmds[idx] != ખાલી && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    પાછા idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
સ્થિર sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    જો (sp_prev != ખાલી)
        *sp_prev = ખાલી;

    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
    {
        જો (STRCMP(sp->sn_name, name) == 0)
            તોડો;

        જો (sp_prev != ખાલી)
            *sp_prev = sp;
    }

    પાછા sp;
}

/*
 * Allocate a new sign
 */
સ્થિર sign_T *
alloc_new_sign(char_u *name)
{
    પૂર્ણાંક start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(કદ(sign_T), aid_sign_define_by_name);
    જો (sp == ખાલી)
        પાછા ખાલી;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    જ્યારે (lp != ખાલી)
    {
        જો (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            જો (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            જો (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                પાછા ખાલી;
            }

            lp = first_sign; // start all over
            ચાલુ;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    જો (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    જો (sp->sn_name == ખાલી) // out of memory
    {
        vim_free(sp);
        પાછા ખાલી;
    }

    પાછા sp;
}

/*
 * Initialize the icon information for a new sign
 */
સ્થિર શૂન્ય
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    જો (gui.in_use)
    {
        out_flush();
        જો (sp->sn_image != ખાલી)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
સ્થિર પૂર્ણાંક
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = ખાલી;
    char_u *endp = text + (પૂર્ણાંક)STRLEN(text);
    પૂર્ણાંક cells = 0;

    // Remove backslashes so that it is possible to use a space.
    માટે (s = text; s + 1 < endp; ++s)
    {
        જો (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    જો (has_mbyte)
    {
        માટે (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            જો (!vim_isprintc((*mb_ptr2char)(s)))
                તોડો;

            cells += (*mb_ptr2cells)(s);
        }
    }
    નહીંતર
    {
        માટે (s = text; s < endp; ++s)
        {
            જો (!vim_isprintc(*s))
                તોડો;
        }
        cells = (પૂર્ણાંક)(s - text);
    }

    // Currently sign text must be one or two display cells
    જો (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        પાછા FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    પૂર્ણાંક len = (પૂર્ણાંક)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    જો (sp->sn_text != ખાલી && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    પાછા OK;
}

/*
 * Define a new sign or update an existing sign
 */
પૂર્ણાંક
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    પૂર્ણાંક prio)
{
    sign_T *sp_prev = ખાલી;
    sign_T *sp = sign_find(name, &sp_prev);
    જો (sp == ખાલી)
    {
        sp = alloc_new_sign(name);
        જો (sp == ખાલી)
            પાછા FAIL;

        // add the new sign to the list of signs
        જો (sp_prev == ખાલી)
            first_sign = sp;
        નહીંતર
            sp_prev->sn_next = sp;
    }
    નહીંતર
    {
        win_T *wp = ખાલી;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            જો (wp->w_buffer->b_signlist != ખાલી)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    જો (icon != ખાલી)
        sign_define_init_icon(sp, icon);

    જો (text != ખાલી && (sign_define_init_text(sp, text) == FAIL))
        પાછા FAIL;

    sp->sn_priority = prio;

    જો (linehl != ખાલી)
    {
        જો (*linehl == NUL)
            sp->sn_લીટી_hl = 0;
        નહીંતર
            sp->sn_લીટી_hl = syn_check_group(linehl, (પૂર્ણાંક)STRLEN(linehl));
    }

    જો (texthl != ખાલી)
    {
        જો (*texthl == NUL)
            sp->sn_text_hl = 0;
        નહીંતર
            sp->sn_text_hl = syn_check_group(texthl, (પૂર્ણાંક)STRLEN(texthl));
    }

    જો (culhl != ખાલી)
    {
        જો (*culhl == NUL)
            sp->sn_cul_hl = 0;
        નહીંતર
            sp->sn_cul_hl = syn_check_group(culhl, (પૂર્ણાંક)STRLEN(culhl));
    }

    જો (numhl != ખાલી)
    {
        જો (*numhl == NUL)
            sp->sn_અંક_hl = 0;
        નહીંતર
            sp->sn_અંક_hl = syn_check_group(numhl, (પૂર્ણાંક)STRLEN(numhl));
    }

    પાછા OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
પૂર્ણાંક
sign_exists_by_name(char_u *name)
{
    પાછા sign_find(name, ખાલી) != ખાલી;
}

/*
 * Free the sign specified by 'name'.
 */
પૂર્ણાંક
sign_undefine_by_name(char_u *name, પૂર્ણાંક give_error)
{
    sign_T *sp_prev = ખાલી;
    sign_T *sp = sign_find(name, &sp_prev);
    જો (sp == ખાલી)
    {
        જો (give_error)
            semsg(_(e_unknown_sign_str), name);
        પાછા FAIL;
    }
    sign_undefine(sp, sp_prev);

    પાછા OK;
}

/*
 * List the signs matching 'name'
 */
સ્થિર શૂન્ય
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, ખાલી);
    જો (sp != ખાલી)
        sign_list_defined(sp);
    નહીંતર
        semsg(_(e_unknown_sign_str), name);
}

સ્થિર શૂન્ય
may_force_numberwidth_recompute(buf_T *buf, પૂર્ણાંક unplace)
{
# જો defined(FEAT_LINEBREAK)
    tabpage_T *tp = ખાલી;
    win_T *wp = ખાલી;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        જો (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_લીટી_ગણતરી = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
પૂર્ણાંક
sign_place(પૂર્ણાંક *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           પૂર્ણાંક prio)
{
    // Check for reserved character '*' in group name
    જો (sign_group != ખાલી && (*sign_group == '*' || *sign_group == '\0'))
        પાછા FAIL;

    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
    {
        જો (STRCMP(sp->sn_name, sign_name) == 0)
            તોડો;
    }

    જો (sp == ખાલી)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        પાછા FAIL;
    }

    જો (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    જો (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    જો (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    નહીંતર
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    જો (lnum > 0)
    {
        redraw_buf_લીટી_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    નહીંતર
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Unplace the specified sign
 */
સ્થિર પૂર્ણાંક
sign_unplace(પૂર્ણાંક sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    જો (buf->b_signlist == ખાલી) // No signs in the buffer
        પાછા OK;

    જો (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    નહીંતર
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        જો (lnum == 0)
            પાછા FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    જો (buf->b_signlist == ખાલી)
        may_force_numberwidth_recompute(buf, TRUE);

    પાછા OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
સ્થિર શૂન્ય
sign_unplace_at_cursor(char_u *groupname)
{
    પૂર્ણાંક id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    જો (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    નહીંતર
        emsg(_(e_missing_sign_સંખ્યા));
}

/*
 * Jump to a sign.
 */
સ્થિર linenr_T
sign_jump(પૂર્ણાંક sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    જો (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        પાછા -1;
    }

    // goto a sign ...
    જો (buf_jump_open_win(buf) != ખાલી)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    નહીંતર
    { // ... not currently in a window
        જો (buf->b_fname == ખાલી)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            પાછા -1;
        }
        setpcmark();
        જો (do_ecmd(buf->b_fnum, ખાલી, ખાલી, ખાલી, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            પાછા -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    પાછા lnum;
}

/*
 * ":sign define {name} ..." command
 */
સ્થિર શૂન્ય
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ખાલી;
    char_u *p = cmdline;
    char_u *icon = ખાલી;
    char_u *text = ખાલી;
    char_u *linehl = ખાલી;
    char_u *texthl = ખાલી;
    char_u *culhl = ખાલી;
    char_u *numhl = ખાલી;
    પૂર્ણાંક prio = -1;
    પૂર્ણાંક failed = FALSE;

    // set values for a defined sign.
    જ્યારે (TRUE)
    {
        arg = skipwhite(p);
        જો (*arg == NUL)
            તોડો;

        p = skiptowhite_esc(arg);
        જો (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        નહીંતર જો (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((અક્ષર *)arg);
        }
        નહીંતર
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            તોડો;
        }
    }

    જો (!failed)
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
સ્થિર શૂન્ય
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               પૂર્ણાંક id,
               char_u *group,
               પૂર્ણાંક prio)
{
    જો (id <= 0)
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
        જો (lnum >= 0 || sign_name != ખાલી || (group != ખાલી && *group == '\0'))
            emsg(_(e_invalid_argument));
        નહીંતર
            sign_list_placed(buf, group);
    }
    નહીંતર
    {
        // Place a new sign
        જો (sign_name == ખાલી || buf == ખાલી ||
            (group != ખાલી && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            પાછા;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
સ્થિર શૂન્ય
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 પૂર્ણાંક id,
                 char_u *group)
{
    જો (lnum >= 0 || sign_name != ખાલી || (group != ખાલી && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        પાછા;
    }

    જો (id == -2)
    {
        જો (buf != ખાલી)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        નહીંતર
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                જો (buf->b_signlist != ખાલી)
                    buf_delete_signs(buf, group);
            }
        }
    }
    નહીંતર
    {
        જો (buf != ખાલી)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        નહીંતર
        {
            જો (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            નહીંતર
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
સ્થિર શૂન્ય
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              પૂર્ણાંક id,
              char_u *group)
{
    જો (sign_name == ખાલી && group == ખાલી && id == -1)
    {
        emsg(_(e_argument_required));
        પાછા;
    }

    જો (buf == ખાલી || (group != ખાલી && *group == '\0') || lnum >= 0 ||
        sign_name != ખાલી)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        પાછા;
    }

    (શૂન્ય)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
સ્થિર પૂર્ણાંક
parse_sign_cmd_args(પૂર્ણાંક cmd,
                    char_u *arg,
                    char_u **sign_name,
                    પૂર્ણાંક *signid,
                    char_u **group,
                    પૂર્ણાંક *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ખાલી;
    પૂર્ણાંક lnum_arg = FALSE;

    // first arg could be placed sign id
    જો (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        જો (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        નહીંતર
        {
            arg = skipwhite(arg);
        }
    }

    જ્યારે (*arg != NUL)
    {
        જો (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((અક્ષર *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        નહીંતર જો (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            જો (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                પાછા FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        નહીંતર જો (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            જો (*arg != NUL)
                *arg++ = NUL;

            જ્યારે (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        નહીંતર જો (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            જો (*arg != NUL)
                *arg++ = NUL;
        }
        નહીંતર જો (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((અક્ષર *)arg);
            arg = skiptowhite(arg);
        }
        નહીંતર જો (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            તોડો;
        }
        નહીંતર જો (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((પૂર્ણાંક)getdigits(&arg));

            જો (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            તોડો;
        }
        નહીંતર
        {
            emsg(_(e_invalid_argument));
            પાછા FAIL;
        }

        arg = skipwhite(arg);
    }

    જો (filename != ખાલી && *buf == ખાલી)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        પાછા FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    જો (filename == ખાલી &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    પાછા OK;
}

/*
 * ":sign" command
 */
શૂન્ય
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    પૂર્ણાંક idx = sign_cmd_idx(arg, p);
    જો (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        પાછા;
    }
    arg = skipwhite(p);

    જો (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ખાલી;
        પૂર્ણાંક id = -1;
        char_u *group = ખાલી;
        પૂર્ણાંક prio = -1;
        buf_T *buf = ખાલી;
        linenr_T lnum = -1;

        // Parse command line arguments
        જો (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            પાછા;

        જો (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        નહીંતર જો (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        નહીંતર જો (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        પાછા;
    }

    // Define, undefine or list signs.
    જો (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        માટે (sign_T *sp = first_sign; sp != ખાલી && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    નહીંતર જો (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    નહીંતર
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        જો (*p != NUL)
            *p++ = NUL;

        જ્યારે (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        જો (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        નહીંતર જો (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        નહીંતર
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
સ્થિર શૂન્ય
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    જો (sp->sn_icon != ખાલી)
        dict_add_string(retdict, "icon", sp->sn_icon);

    જો (sp->sn_text != ખાલી)
        dict_add_string(retdict, "text", sp->sn_text);

    જો (sp->sn_priority > 0)
        dict_add_સંખ્યા(retdict, "priority", sp->sn_priority);

    જો (sp->sn_લીટી_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_લીટી_hl - 1, FALSE);
        જો (p == ખાલી)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        નહીંતર
            dict_add_string(retdict, "linehl", p);
    }

    જો (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_text_hl - 1, FALSE);
        જો (p == ખાલી)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        નહીંતર
            dict_add_string(retdict, "texthl", p);
    }

    જો (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_cul_hl - 1, FALSE);
        જો (p == ખાલી)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        નહીંતર
            dict_add_string(retdict, "culhl", p);
    }

    જો (sp->sn_અંક_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_અંક_hl - 1, FALSE);
        જો (p == ખાલી)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        નહીંતર
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
સ્થિર શૂન્ય
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    જો (name != ખાલી)
    {
        sp = sign_find(name, ખાલી);
        જો (sp == ખાલી)
            પાછા;
    }

    માટે (; sp != ખાલી && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        જો (dict == ખાલી)
            પાછા;

        જો (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            પાછા;
        }

        sign_getinfo(sp, dict);

        જો (name != ખાલી) // handle only the specified sign
            તોડો;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
શૂન્ય
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = ખાલી;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        જો (d != ખાલી && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
સ્થિર શૂન્ય
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       પૂર્ણાંક sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    જો (d == ખાલી)
        પાછા;

    જો (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        પાછા;
    }

    dict_add_સંખ્યા(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    જો (l == ખાલી)
        પાછા;

    જો (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        પાછા;
    }

    sign_entry_T *sign = ખાલી;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        જો (!sign_in_group(sign, sign_group))
            ચાલુ;

        જો ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            જો (sdict != ખાલી && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
સ્થિર શૂન્ય
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                પૂર્ણાંક sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    જો (buf != ખાલી)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    નહીંતર
    {
        FOR_ALL_BUFFERS(buf)
        {
            જો (buf->b_signlist != ખાલી)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# જો defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
શૂન્ય
sign_gui_started(શૂન્ય)
{
    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
    {
        જો (sp->sn_icon != ખાલી)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
સ્થિર શૂન્ય
sign_list_defined(sign_T *sp)
{
    અક્ષર lbuf[સંદેશ_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    જો (sp->sn_icon != ખાલી)
    {
        સંદેશ_puts(" icon=");
        સંદેશ_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        જો (sp->sn_image == ખાલી)
            સંદેશ_puts(_(" (NOT FOUND)"));
# નહીંતર
        સંદેશ_puts(_(" (not supported)"));
# endif
    }

    જો (sp->sn_text != ખાલી)
    {
        સંદેશ_puts(" text=");
        સંદેશ_outtrans(sp->sn_text);
    }

    જો (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, સંદેશ_BUF_LEN, " priority=%d", sp->sn_priority);
        સંદેશ_puts(lbuf);
    }

    જો (sp->sn_લીટી_hl > 0)
    {
        સંદેશ_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_લીટી_hl - 1, FALSE);
        જો (p == ખાલી)
            સંદેશ_puts("NONE");
        નહીંતર
            સંદેશ_puts((અક્ષર *)p);
    }

    જો (sp->sn_text_hl > 0)
    {
        સંદેશ_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_text_hl - 1, FALSE);
        જો (p == ખાલી)
            સંદેશ_puts("NONE");
        નહીંતર
            સંદેશ_puts((અક્ષર *)p);
    }

    જો (sp->sn_cul_hl > 0)
    {
        સંદેશ_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_cul_hl - 1, FALSE);
        જો (p == ખાલી)
            સંદેશ_puts("NONE");
        નહીંતર
            સંદેશ_puts((અક્ષર *)p);
    }

    જો (sp->sn_અંક_hl > 0)
    {
        સંદેશ_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ખાલી, sp->sn_અંક_hl - 1, FALSE);
        જો (p == ખાલી)
            સંદેશ_puts("NONE");
        નહીંતર
            સંદેશ_puts((અક્ષર *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
સ્થિર શૂન્ય
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    જો (sp->sn_image != ખાલી)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    જો (sp_prev == ખાલી)
        first_sign = sp->sn_next;
    નહીંતર
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# જો defined(FEAT_SIGN_ICONS)
શૂન્ય *
sign_get_image(પૂર્ણાંક typenr) // the attribute which may have a sign
{
    sign_T *sp = ખાલી;
    FOR_ALL_SIGNS(sp)
    {
        જો (sp->sn_typenr == typenr)
            પાછા sp->sn_image;
    }
    પાછા ખાલી;
}
# endif

/*
 * Undefine/free all signs.
 */
શૂન્ય
free_signs(શૂન્ય)
{
    જ્યારે (first_sign != ખાલી)
        sign_undefine(first_sign, ખાલી);
}

સ્થિર ગણના
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
સ્થિર char_u *
get_nth_sign_name(પૂર્ણાંક idx)
{
    પૂર્ણાંક current_idx = 0;
    sign_T *sp = ખાલી;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        જો (current_idx++ == idx)
            પાછા sp->sn_name;
    }
    પાછા ખાલી;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
સ્થિર char_u *
get_nth_sign_group_name(પૂર્ણાંક idx)
{
    પૂર્ણાંક current_idx = 0;
    પૂર્ણાંક todo = (પૂર્ણાંક)sg_table.ht_used;
    hashitem_T *hi = ખાલી;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_વસ્તુઓ(&sg_table, hi, todo)
    {
        જો (!HASHITEM_EMPTY(hi))
        {
            --todo;
            જો (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                પાછા group->sg_name;
            }
        }
    }
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પસંદગી (expand_what)
    {
        સ્થિતિ EXP_SUBCMD:
            પાછા (char_u *)cmds[idx];
        સ્થિતિ EXP_DEFINE:
        {
            અક્ષર *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ખાલી };
            પાછા (char_u *)define_arg[idx];
        }
        સ્થિતિ EXP_PLACE:
        {
            અક્ષર *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ખાલી };
            પાછા (char_u *)place_arg[idx];
        }
        સ્થિતિ EXP_LIST:
        {
            અક્ષર *list_arg[] = { "group=", "file=", "buffer=", ખાલી };
            પાછા (char_u *)list_arg[idx];
        }
        સ્થિતિ EXP_UNPLACE:
        {
            અક્ષર *unplace_arg[] = { "group=", "file=", "buffer=", ખાલી };
            પાછા (char_u *)unplace_arg[idx];
        }
        સ્થિતિ EXP_SIGN_NAMES:
            પાછા get_nth_sign_name(idx);
        સ્થિતિ EXP_SIGN_GROUPS:
            પાછા get_nth_sign_group_name(idx);
        મૂળભૂત:
            પાછા ખાલી;
    }
}

/*
 * Handle command line completion for :sign command.
 */
શૂન્ય
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    પૂર્ણાંક cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    જો (*end_subcmd == NUL)
        પાછા;

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
    કરો
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    જ્યારે (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    જો (p == ખાલી)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        પસંદગી (cmd_idx)
        {
            સ્થિતિ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                તોડો;
            સ્થિતિ SIGNCMD_PLACE:
                // List placed signs
                જો (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                નહીંતર
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                તોડો;
            સ્થિતિ SIGNCMD_LIST:
            સ્થિતિ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                તોડો;
            સ્થિતિ SIGNCMD_JUMP:
            સ્થિતિ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                તોડો;
            મૂળભૂત:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    નહીંતર
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        પસંદગી (cmd_idx)
        {
            સ્થિતિ SIGNCMD_DEFINE:
                જો (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                નહીંતર જો (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                નહીંતર
                    xp->xp_context = EXPAND_NOTHING;
                તોડો;
            સ્થિતિ SIGNCMD_PLACE:
                જો (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                નહીંતર જો (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                નહીંતર જો (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                નહીંતર
                    xp->xp_context = EXPAND_NOTHING;
                તોડો;
            સ્થિતિ SIGNCMD_UNPLACE:
            સ્થિતિ SIGNCMD_JUMP:
                જો (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                નહીંતર જો (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                નહીંતર
                    xp->xp_context = EXPAND_NOTHING;
                તોડો;
            મૂળભૂત:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
સ્થિર પૂર્ણાંક
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ખાલી;
    char_u *icon = ખાલી;
    char_u *linehl = ખાલી;
    char_u *text = ખાલી;
    char_u *texthl = ખાલી;
    char_u *culhl = ખાલી;
    char_u *numhl = ખાલી;
    પૂર્ણાંક prio = -1;
    પૂર્ણાંક retval = -1;

    જો (name_arg == ખાલી && dict == ખાલી)
        પાછા retval;

    જો (name_arg == ખાલી)
        name = dict_get_string(dict, "name", TRUE);
    નહીંતર
        name = vim_strsave(name_arg);

    જો (name == ખાલી || name[0] == NUL)
        જાઓ cleanup;

    જો (dict != ખાલી)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_સંખ્યા_def(dict, "priority", -1);
    }

    જો (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    પાછા retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
સ્થિર શૂન્ય
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ખાલી;
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
        પૂર્ણાંક retval = -1;

        જો (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ખાલી, li->li_tv.vval.v_dict);
        નહીંતર
            emsg(_(e_dictionary_required));

        list_append_સંખ્યા(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
શૂન્ય
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        પાછા;

    જો (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        જો (rettv_list_alloc(rettv) == FAIL)
            પાછા;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        પાછા;
    }

    // Define a single sign
    rettv->vval.v_સંખ્યા = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    જો (name == ખાલી)
        પાછા;

    જો (check_for_opt_dict_arg(argvars, 1) == FAIL)
        પાછા;

    rettv->vval.v_સંખ્યા = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ખાલી);
}

/*
 * "sign_getdefined()" function
 */
શૂન્ય
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    જો (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        પાછા;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        પાછા;

    char_u *name = ખાલી;
    જો (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
શૂન્ય
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ખાલી;
    linenr_T lnum = 0;
    પૂર્ણાંક sign_id = 0;
    char_u *group = ખાલી;

    જો (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        પાછા;

    જો (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        જો (buf == ખાલી)
            પાછા;

        જો (argvars[1].v_type != VAR_UNKNOWN)
        {
            જો (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                પાછા;

            dictitem_T *di = ખાલી;
            dict_T *dict = argvars[1].vval.v_dict;

            જો ((di = dict_find(dict, (char_u *)"lnum", -1)) != ખાલી)
            {
                // get signs placed at this line
                પૂર્ણાંક notanum = FALSE;
                tv_get_સંખ્યા_chk(&di->di_tv, &notanum);
                જો (notanum)
                    પાછા;

                lnum = tv_get_lnum(&di->di_tv);
            }

            જો ((di = dict_find(dict, (char_u *)"id", -1)) != ખાલી)
            {
                // get sign placed with this identifier
                પૂર્ણાંક notanum = FALSE;
                sign_id = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&di->di_tv, &notanum);
                જો (notanum)
                    પાછા;
            }

            જો ((di = dict_find(dict, (char_u *)"group", -1)) != ખાલી)
            {
                group = tv_get_string_chk(&di->di_tv);
                જો (group == ખાલી)
                    પાછા;

                જો (*group == '\0') // empty string means global group
                    group = ખાલી;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
શૂન્ય
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = ખાલી;
    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script() && (check_for_સંખ્યા_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        પાછા;

    પૂર્ણાંક notanum = FALSE;
    // Sign identifier
    પૂર્ણાંક sign_id = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], &notanum);
    જો (notanum)
        પાછા;

    જો (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        પાછા;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    જો (sign_group == ખાલી)
        પાછા;

    જો (sign_group[0] == '\0')
    {
        sign_group = ખાલી; // global sign group
    }
    નહીંતર
    {
        sign_group = vim_strsave(sign_group);
        જો (sign_group == ખાલી)
            પાછા;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    જો (buf == ખાલી)
        જાઓ cleanup;

    rettv->vval.v_સંખ્યા = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
સ્થિર પૂર્ણાંક
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    પૂર્ણાંક sign_id = 0;
    char_u *group = ખાલી;
    char_u *sign_name = ખાલી;
    buf_T *buf = ખાલી;
    dictitem_T *di = ખાલી;
    linenr_T lnum = 0;
    પૂર્ણાંક prio = -1;
    પૂર્ણાંક ret_sign_id = -1;

    // sign identifier
    જો (id_tv == ખાલી)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        જો (di != ખાલી)
            id_tv = &di->di_tv;
    }

    જો (id_tv == ખાલી)
    {
        sign_id = 0;
    }
    નહીંતર
    {
        પૂર્ણાંક notanum = FALSE;
        sign_id = tv_get_સંખ્યા_chk(id_tv, &notanum);

        જો (notanum)
            પાછા -1;

        જો (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            પાછા -1;
        }
    }

    // sign group
    જો (group_tv == ખાલી)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        જો (di != ખાલી)
            group_tv = &di->di_tv;
    }

    જો (group_tv == ખાલી)
    {
        group = ખાલી; // global group
    }
    નહીંતર
    {
        group = tv_get_string_chk(group_tv);
        જો (group == ખાલી)
            જાઓ cleanup;

        જો (group[0] == '\0') // global sign group
        {
            group = ખાલી;
        }
        નહીંતર
        {
            group = vim_strsave(group);
            જો (group == ખાલી)
                પાછા -1;
        }
    }

    // sign name
    જો (name_tv == ખાલી)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        જો (di != ખાલી)
            name_tv = &di->di_tv;
    }

    જો (name_tv == ખાલી)
        જાઓ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    જો (sign_name == ખાલી)
        જાઓ cleanup;

    // buffer to place the sign
    જો (buf_tv == ખાલી)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        જો (di != ખાલી)
            buf_tv = &di->di_tv;
    }

    જો (buf_tv == ખાલી)
        જાઓ cleanup;

    buf = get_buf_arg(buf_tv);
    જો (buf == ખાલી)
        જાઓ cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    જો (di != ખાલી)
    {
        lnum = tv_get_lnum(&di->di_tv);
        જો (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            જાઓ cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    જો (di != ખાલી)
    {
        પૂર્ણાંક notanum = FALSE;
        prio = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&di->di_tv, &notanum);
        જો (notanum)
            જાઓ cleanup;
    }

    જો (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    પાછા ret_sign_id;
}

/*
 * "sign_place()" function
 */
શૂન્ય
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ખાલી;
    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script() && (check_for_સંખ્યા_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        પાછા;

    જો (argvars[4].v_type != VAR_UNKNOWN)
    {
        જો (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            પાછા;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_સંખ્યા = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
શૂન્ય
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) == FAIL)
        પાછા;

    જો (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        પાછા;

    જો (check_for_list_arg(argvars, 0) == FAIL)
        પાછા;

    // Process the List of sign attributes
    listitem_T *li = ખાલી;
    FOR_ALL_LIST_વસ્તુઓ(argvars[0].vval.v_list, li)
    {
        પૂર્ણાંક sign_id = -1;

        જો (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(ખાલી, ખાલી, ખાલી, ખાલી,
                                           li->li_tv.vval.v_dict);
        નહીંતર
            emsg(_(e_dictionary_required));

        list_append_સંખ્યા(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
સ્થિર શૂન્ય
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ખાલી;
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
        પૂર્ણાંક retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        જો (name != ખાલી && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_સંખ્યા(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
શૂન્ય
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    જો (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        પાછા;

    જો (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        જો (rettv_list_alloc(rettv) == FAIL)
            પાછા;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        પાછા;
    }

    rettv->vval.v_સંખ્યા = -1;

    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        જો (name == ખાલી)
            પાછા;

        જો (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_સંખ્યા = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
સ્થિર પૂર્ણાંક
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    પૂર્ણાંક sign_id = 0;
    buf_T *buf = ખાલી;
    char_u *group = ખાલી;
    પૂર્ણાંક retval = -1;

    // sign group
    group = (group_tv != ખાલી) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    જો (group != ખાલી)
    {
        જો (group[0] == '\0') // global sign group
        {
            group = ખાલી;
        }
        નહીંતર
        {
            group = vim_strsave(group);
            જો (group == ખાલી)
                પાછા retval;
        }
    }

    જો (dict != ખાલી)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        જો (di != ખાલી)
        {
            buf = get_buf_arg(&di->di_tv);
            જો (buf == ખાલી)
                જાઓ cleanup;
        }

        જો (dict_has_ચાવી(dict, "id"))
        {
            sign_id = dict_get_સંખ્યા(dict, "id");
            જો (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                જાઓ cleanup;
            }
        }
    }

    જો (buf == ખાલી)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            જો (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    નહીંતર જો (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    પાછા retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    જ્યારે (sign != ખાલી && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    પાછા sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
પૂર્ણાંક
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    જો (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        પાછા get_first_valid_sign(wp) != ખાલી && !wp->w_p_nu && !wp->w_p_rnu;

    જો (*wp->w_p_scl == 'n')
        પાછા FALSE;

    જો (*wp->w_p_scl == 'y')
        પાછા TRUE;

    પાછા (get_first_valid_sign(wp) != ખાલી
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
શૂન્ય
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ખાલી;
    rettv->vval.v_સંખ્યા = -1;

    જો ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_સંખ્યા = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
શૂન્ય
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) == FAIL)
        પાછા;

    જો (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        પાછા;

    જો (check_for_list_arg(argvars, 0) == FAIL)
        પાછા;

    listitem_T *li = ખાલી;
    FOR_ALL_LIST_વસ્તુઓ(argvars[0].vval.v_list, li)
    {
        પૂર્ણાંક retval = -1;

        જો (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ખાલી, li->li_tv.vval.v_dict);
        નહીંતર
            emsg(_(e_dictionary_required));

        list_append_સંખ્યા(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
