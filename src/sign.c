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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
തരനിർവചനം ഘടന sign sign_T;

ഘടന sign
{
    sign_T *sn_next; // next sign in list
    പൂർണ്ണസംഖ്യ sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    ശൂന്യം *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    പൂർണ്ണസംഖ്യ sn_വരി_hl; // highlight ID for line
    പൂർണ്ണസംഖ്യ sn_text_hl; // highlight ID for text
    പൂർണ്ണസംഖ്യ sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    പൂർണ്ണസംഖ്യ sn_അക്കം_hl; // highlight ID for line number
    പൂർണ്ണസംഖ്യ sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

സ്ഥിതം sign_T *first_sign = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ next_sign_typenr = 1;

സ്ഥിതം ശൂന്യം sign_list_defined(sign_T *sp);
സ്ഥിതം ശൂന്യം sign_undefine(sign_T *sp, sign_T *sp_prev);

സ്ഥിതം അക്ഷരം *cmds[] = { "define",
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
                        ശൂന്യ
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     വേണ്ടി ((sp) = first_sign; (sp) != ശൂന്യ; (sp) = (sp)->sn_next)

സ്ഥിതം hashtab_T sg_table; // sign group (signgroup_T) hashtable
സ്ഥിതം പൂർണ്ണസംഖ്യ next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
ശൂന്യം
init_signs(ശൂന്യം)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
സ്ഥിതം signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = ശൂന്യ;

    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        എങ്കിൽ (group == ശൂന്യ)
            തിരികെ ശൂന്യ;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    അല്ലെങ്കിൽ
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    തിരികെ group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
സ്ഥിതം ശൂന്യം
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
        തിരികെ;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    എങ്കിൽ (group->sg_refcount == 0)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_in_group(sign_entry_T *sign, char_u *group)
{
    തിരികെ ((group != ശൂന്യ && STRCMP(group, "*") == 0) ||
            (group == ശൂന്യ && sign->se_group == ശൂന്യ) ||
            (group != ശൂന്യ && sign->se_group != ശൂന്യ &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    പൂർണ്ണസംഖ്യ for_popup = sign->se_group != ശൂന്യ &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    തിരികെ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    പൂർണ്ണസംഖ്യ id = 1;
    signgroup_T *group = ശൂന്യ;
    sign_entry_T *sign = ശൂന്യ;

    എങ്കിൽ (groupname != ശൂന്യ)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        എങ്കിൽ (HASHITEM_EMPTY(hi))
            തിരികെ id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    പൂർണ്ണസംഖ്യ found = FALSE;
    ആകുമ്പോൾ (!found)
    {
        എങ്കിൽ (group == ശൂന്യ)
            id = next_sign_id++; // global group
        അല്ലെങ്കിൽ
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            എങ്കിൽ (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                നിർത്തുക;
            }
        }
    }

    തിരികെ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
സ്ഥിതം ശൂന്യം
തിരുകുക_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            പൂർണ്ണസംഖ്യ id, // sign ID
            char_u *group, // sign group; NULL for global group
            പൂർണ്ണസംഖ്യ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            പൂർണ്ണസംഖ്യ typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(വലുപ്പം(sign_entry_T), FALSE, aid_തിരുകുക_sign);
    എങ്കിൽ (newsign == ശൂന്യ)
        തിരികെ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    എങ്കിൽ (group != ശൂന്യ)
    {
        newsign->se_group = sign_group_ref(group);
        എങ്കിൽ (newsign->se_group == ശൂന്യ)
        {
            vim_free(newsign);
            തിരികെ;
        }
    }
    അല്ലെങ്കിൽ
    {
        newsign->se_group = ശൂന്യ;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    എങ്കിൽ (next != ശൂന്യ)
        next->se_prev = newsign;

    എങ്കിൽ (prev == ശൂന്യ)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        എങ്കിൽ (buf->b_signlist == ശൂന്യ)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_വരി_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        എങ്കിൽ (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    അല്ലെങ്കിൽ
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
സ്ഥിതം ശൂന്യം
തിരുകുക_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         പൂർണ്ണസംഖ്യ id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         പൂർണ്ണസംഖ്യ prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         പൂർണ്ണസംഖ്യ typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ആകുമ്പോൾ (prev != ശൂന്യ && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == ശൂന്യ) ? buf->b_signlist : prev->se_next;

    തിരുകുക_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
സ്ഥിതം sign_T *
find_sign_by_typenr(പൂർണ്ണസംഖ്യ typenr)
{
    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
        എങ്കിൽ (sp->sn_typenr == typenr)
            തിരികെ sp;
    തിരികെ ശൂന്യ;
}

/*
 * Get the name of a sign by its typenr.
 */
സ്ഥിതം char_u *
sign_typenr2name(പൂർണ്ണസംഖ്യ typenr)
{
    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
        എങ്കിൽ (sp->sn_typenr == typenr)
            തിരികെ sp->sn_name;
    തിരികെ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
സ്ഥിതം dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    എങ്കിൽ (d == ശൂന്യ)
        തിരികെ ശൂന്യ;

    dict_add_സംഖ്യ(d, "id", sign->se_id);
    എങ്കിൽ (sign->se_group == ശൂന്യ)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    അല്ലെങ്കിൽ
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_സംഖ്യ(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_സംഖ്യ(d, "priority", sign->se_priority);

    തിരികെ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
സ്ഥിതം ശൂന്യം
sign_sort_by_prio_on_വരി(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    എങ്കിൽ ((sign->se_prev == ശൂന്യ || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == ശൂന്യ || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        തിരികെ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    ആകുമ്പോൾ (p->se_prev != ശൂന്യ && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    എങ്കിൽ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        ആകുമ്പോൾ (p->se_next != ശൂന്യ && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    എങ്കിൽ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    എങ്കിൽ (sign->se_prev != ശൂന്യ)
        sign->se_prev->se_next = sign->se_next;

    എങ്കിൽ (sign->se_next != ശൂന്യ)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = ശൂന്യ;
    sign->se_next = ശൂന്യ;

    // Re-insert 'sign' at the right place
    എങ്കിൽ (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        എങ്കിൽ (sign->se_prev != ശൂന്യ)
            sign->se_prev->se_next = sign;

        എങ്കിൽ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    അല്ലെങ്കിൽ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        എങ്കിൽ (sign->se_next != ശൂന്യ)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
സ്ഥിതം ശൂന്യം
buf_addsign(buf_T *buf, // buffer to store sign in
            പൂർണ്ണസംഖ്യ id, // sign ID
            char_u *groupname, // sign group
            പൂർണ്ണസംഖ്യ prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            പൂർണ്ണസംഖ്യ typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    sign_entry_T *prev = ശൂന്യ; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    എങ്കിൽ (buf->b_sign_finger != ശൂന്യ && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    വേണ്ടി (sign = (prev == ശൂന്യ) ? buf->b_signlist : prev->se_next; sign != ശൂന്യ;
         sign = sign->se_next)
    {
        എങ്കിൽ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_വരി(buf, sign);
            തിരികെ;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (lnum < sign->se_lnum)
        {
            തിരുകുക_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            തിരികെ;
        }
        prev = sign;
    }

    തിരുകുക_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
സ്ഥിതം linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     പൂർണ്ണസംഖ്യ markId, // sign ID
                     char_u *group, // sign group
                     പൂർണ്ണസംഖ്യ typenr, // typenr of sign we are adding
                     പൂർണ്ണസംഖ്യ prio) // sign priority
{
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        എങ്കിൽ (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_വരി(buf, sign);
            തിരികെ sign->se_lnum;
        }
    }

    തിരികെ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
പൂർണ്ണസംഖ്യ
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = ശൂന്യ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        എങ്കിൽ (sign->se_lnum > lnum)
            നിർത്തുക;

        എങ്കിൽ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            എങ്കിൽ (sp == ശൂന്യ)
                തിരികെ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            എങ്കിൽ (sattr->sat_text != ശൂന്യ && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            എങ്കിൽ (sp->sn_വരി_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_വരി_hl);

            എങ്കിൽ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            എങ്കിൽ (sp->sn_അക്കം_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_അക്കം_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            എങ്കിൽ (sign->se_next != ശൂന്യ &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                എങ്കിൽ (next_sp == ശൂന്യ)
                    തിരികെ FALSE;

                എങ്കിൽ (sattr->sat_icon == ശൂന്യ && sattr->sat_text == ശൂന്യ)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                എങ്കിൽ (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                എങ്കിൽ (sp->sn_വരി_hl <= 0 && next_sp->sn_വരി_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_വരി_hl);

                എങ്കിൽ (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                എങ്കിൽ (sp->sn_അക്കം_hl <= 0 && next_sp->sn_അക്കം_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_അക്കം_hl);
            }
            തിരികെ TRUE;
        }
    }
    തിരികെ FALSE;
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
            പൂർണ്ണസംഖ്യ id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = ശൂന്യ; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ശൂന്യ; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    വേണ്ടി (sign_entry_T *sign = buf->b_signlist; sign != ശൂന്യ; sign = next)
    {
        next = sign->se_next;

        എങ്കിൽ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            എങ്കിൽ (next != ശൂന്യ)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            എങ്കിൽ (sign->se_group != ശൂന്യ)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_വരി_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            എങ്കിൽ (group == ശൂന്യ || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                നിർത്തുക;
        }
        അല്ലെങ്കിൽ
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    എങ്കിൽ (buf->b_signlist == ശൂന്യ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_വരി_abv_curs();
    }

    തിരികെ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
പൂർണ്ണസംഖ്യ
buf_findsign(buf_T *buf, // buffer to store sign in
             പൂർണ്ണസംഖ്യ id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        എങ്കിൽ (sign->se_id == id && sign_in_group(sign, group))
            തിരികെ sign->se_lnum;

    തിരികെ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
സ്ഥിതം sign_entry_T *
buf_getsign_at_വരി(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        എങ്കിൽ (sign->se_lnum > lnum)
            നിർത്തുക;

        എങ്കിൽ (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            തിരികെ sign;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
പൂർണ്ണസംഖ്യ
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_വരി(buf, lnum, groupname);
    എങ്കിൽ (sign != ശൂന്യ)
        തിരികെ sign->se_id;

    തിരികെ 0;
}

# എങ്കിൽ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
പൂർണ്ണസംഖ്യ
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    പൂർണ്ണസംഖ്യ typenr) // sign type number
{
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        എങ്കിൽ (sign->se_lnum > lnum)
            നിർത്തുക;

        എങ്കിൽ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            തിരികെ sign->se_id;
    }

    തിരികെ 0;
}

#  എങ്കിൽ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
പൂർണ്ണസംഖ്യ
buf_signcount(buf_T *buf, linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ എണ്ണം = 0;
    sign_entry_T *sign = ശൂന്യ; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        എങ്കിൽ (sign->se_lnum > lnum)
            നിർത്തുക;

        എങ്കിൽ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ശൂന്യ)
            എണ്ണം++;
    }

    തിരികെ എണ്ണം;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
ശൂന്യം
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = ശൂന്യ; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    എങ്കിൽ (buf->b_signlist != ശൂന്യ && curwin != ശൂന്യ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_വരി_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ശൂന്യ;

    വേണ്ടി (sign_entry_T *sign = buf->b_signlist; sign != ശൂന്യ; sign = next)
    {
        next = sign->se_next;
        എങ്കിൽ (sign_in_group(sign, group))
        {
            *lastp = next;

            എങ്കിൽ (next != ശൂന്യ)
                next->se_prev = sign->se_prev;

            എങ്കിൽ (sign->se_group != ശൂന്യ)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        അല്ലെങ്കിൽ
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
സ്ഥിതം ശൂന്യം
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    അക്ഷരം lbuf[സന്ദേശം_BUF_LEN];
    അക്ഷരം group[സന്ദേശം_BUF_LEN];

    സന്ദേശം_puts_title(_("\n--- Signs ---"));
    സന്ദേശം_putchar('\n');

    buf_T *buf = (rbuf == ശൂന്യ) ? firstbuf : rbuf;
    ആകുമ്പോൾ (buf != ശൂന്യ && !got_int)
    {
        എങ്കിൽ (buf->b_signlist != ശൂന്യ)
        {
            vim_snprintf(lbuf, സന്ദേശം_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            സന്ദേശം_puts_attr(lbuf, HL_ATTR(HLF_D));
            സന്ദേശം_putchar('\n');
        }

        sign_entry_T *sign = ശൂന്യ;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            എങ്കിൽ (got_int)
                നിർത്തുക;

            എങ്കിൽ (!sign_in_group(sign, sign_group))
                തുടരുക;

            എങ്കിൽ (sign->se_group != ശൂന്യ)
                vim_snprintf(group, സന്ദേശം_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            അല്ലെങ്കിൽ
                group[0] = '\0';

            vim_snprintf(lbuf, സന്ദേശം_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            സന്ദേശം_puts(lbuf);
            സന്ദേശം_putchar('\n');
        }

        എങ്കിൽ (rbuf != ശൂന്യ)
            നിർത്തുക;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
ശൂന്യം
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = ശൂന്യ; // line changes may reorder, drop the finger
    sign_entry_T *sign = ശൂന്യ; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        എങ്കിൽ (sign->se_lnum < line1)
            തുടരുക;

        linenr_T new_lnum = sign->se_lnum;

        എങ്കിൽ (sign->se_lnum <= line2)
        {
            എങ്കിൽ (amount != MAXLNUM)
                new_lnum += amount;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        എങ്കിൽ (new_lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    പൂർണ്ണസംഖ്യ idx = 0;
    അക്ഷരം save = *end_cmd;
    *end_cmd = NUL;

    ആകുമ്പോൾ (cmds[idx] != ശൂന്യ && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    തിരികെ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
സ്ഥിതം sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    എങ്കിൽ (sp_prev != ശൂന്യ)
        *sp_prev = ശൂന്യ;

    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
    {
        എങ്കിൽ (STRCMP(sp->sn_name, name) == 0)
            നിർത്തുക;

        എങ്കിൽ (sp_prev != ശൂന്യ)
            *sp_prev = sp;
    }

    തിരികെ sp;
}

/*
 * Allocate a new sign
 */
സ്ഥിതം sign_T *
alloc_new_sign(char_u *name)
{
    പൂർണ്ണസംഖ്യ start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(വലുപ്പം(sign_T), aid_sign_define_by_name);
    എങ്കിൽ (sp == ശൂന്യ)
        തിരികെ ശൂന്യ;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    ആകുമ്പോൾ (lp != ശൂന്യ)
    {
        എങ്കിൽ (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            എങ്കിൽ (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            എങ്കിൽ (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                തിരികെ ശൂന്യ;
            }

            lp = first_sign; // start all over
            തുടരുക;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    എങ്കിൽ (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    എങ്കിൽ (sp->sn_name == ശൂന്യ) // out of memory
    {
        vim_free(sp);
        തിരികെ ശൂന്യ;
    }

    തിരികെ sp;
}

/*
 * Initialize the icon information for a new sign
 */
സ്ഥിതം ശൂന്യം
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    എങ്കിൽ (gui.in_use)
    {
        out_flush();
        എങ്കിൽ (sp->sn_image != ശൂന്യ)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = ശൂന്യ;
    char_u *endp = text + (പൂർണ്ണസംഖ്യ)STRLEN(text);
    പൂർണ്ണസംഖ്യ cells = 0;

    // Remove backslashes so that it is possible to use a space.
    വേണ്ടി (s = text; s + 1 < endp; ++s)
    {
        എങ്കിൽ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    എങ്കിൽ (has_mbyte)
    {
        വേണ്ടി (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            എങ്കിൽ (!vim_isprintc((*mb_ptr2char)(s)))
                നിർത്തുക;

            cells += (*mb_ptr2cells)(s);
        }
    }
    അല്ലെങ്കിൽ
    {
        വേണ്ടി (s = text; s < endp; ++s)
        {
            എങ്കിൽ (!vim_isprintc(*s))
                നിർത്തുക;
        }
        cells = (പൂർണ്ണസംഖ്യ)(s - text);
    }

    // Currently sign text must be one or two display cells
    എങ്കിൽ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        തിരികെ FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    എങ്കിൽ (sp->sn_text != ശൂന്യ && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    തിരികെ OK;
}

/*
 * Define a new sign or update an existing sign
 */
പൂർണ്ണസംഖ്യ
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    പൂർണ്ണസംഖ്യ prio)
{
    sign_T *sp_prev = ശൂന്യ;
    sign_T *sp = sign_find(name, &sp_prev);
    എങ്കിൽ (sp == ശൂന്യ)
    {
        sp = alloc_new_sign(name);
        എങ്കിൽ (sp == ശൂന്യ)
            തിരികെ FAIL;

        // add the new sign to the list of signs
        എങ്കിൽ (sp_prev == ശൂന്യ)
            first_sign = sp;
        അല്ലെങ്കിൽ
            sp_prev->sn_next = sp;
    }
    അല്ലെങ്കിൽ
    {
        win_T *wp = ശൂന്യ;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            എങ്കിൽ (wp->w_buffer->b_signlist != ശൂന്യ)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    എങ്കിൽ (icon != ശൂന്യ)
        sign_define_init_icon(sp, icon);

    എങ്കിൽ (text != ശൂന്യ && (sign_define_init_text(sp, text) == FAIL))
        തിരികെ FAIL;

    sp->sn_priority = prio;

    എങ്കിൽ (linehl != ശൂന്യ)
    {
        എങ്കിൽ (*linehl == NUL)
            sp->sn_വരി_hl = 0;
        അല്ലെങ്കിൽ
            sp->sn_വരി_hl = syn_check_group(linehl, (പൂർണ്ണസംഖ്യ)STRLEN(linehl));
    }

    എങ്കിൽ (texthl != ശൂന്യ)
    {
        എങ്കിൽ (*texthl == NUL)
            sp->sn_text_hl = 0;
        അല്ലെങ്കിൽ
            sp->sn_text_hl = syn_check_group(texthl, (പൂർണ്ണസംഖ്യ)STRLEN(texthl));
    }

    എങ്കിൽ (culhl != ശൂന്യ)
    {
        എങ്കിൽ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        അല്ലെങ്കിൽ
            sp->sn_cul_hl = syn_check_group(culhl, (പൂർണ്ണസംഖ്യ)STRLEN(culhl));
    }

    എങ്കിൽ (numhl != ശൂന്യ)
    {
        എങ്കിൽ (*numhl == NUL)
            sp->sn_അക്കം_hl = 0;
        അല്ലെങ്കിൽ
            sp->sn_അക്കം_hl = syn_check_group(numhl, (പൂർണ്ണസംഖ്യ)STRLEN(numhl));
    }

    തിരികെ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
പൂർണ്ണസംഖ്യ
sign_exists_by_name(char_u *name)
{
    തിരികെ sign_find(name, ശൂന്യ) != ശൂന്യ;
}

/*
 * Free the sign specified by 'name'.
 */
പൂർണ്ണസംഖ്യ
sign_undefine_by_name(char_u *name, പൂർണ്ണസംഖ്യ give_error)
{
    sign_T *sp_prev = ശൂന്യ;
    sign_T *sp = sign_find(name, &sp_prev);
    എങ്കിൽ (sp == ശൂന്യ)
    {
        എങ്കിൽ (give_error)
            semsg(_(e_unknown_sign_str), name);
        തിരികെ FAIL;
    }
    sign_undefine(sp, sp_prev);

    തിരികെ OK;
}

/*
 * List the signs matching 'name'
 */
സ്ഥിതം ശൂന്യം
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, ശൂന്യ);
    എങ്കിൽ (sp != ശൂന്യ)
        sign_list_defined(sp);
    അല്ലെങ്കിൽ
        semsg(_(e_unknown_sign_str), name);
}

സ്ഥിതം ശൂന്യം
may_force_numberwidth_recompute(buf_T *buf, പൂർണ്ണസംഖ്യ unplace)
{
# എങ്കിൽ defined(FEAT_LINEBREAK)
    tabpage_T *tp = ശൂന്യ;
    win_T *wp = ശൂന്യ;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        എങ്കിൽ (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_വരി_എണ്ണം = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
പൂർണ്ണസംഖ്യ
sign_place(പൂർണ്ണസംഖ്യ *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           പൂർണ്ണസംഖ്യ prio)
{
    // Check for reserved character '*' in group name
    എങ്കിൽ (sign_group != ശൂന്യ && (*sign_group == '*' || *sign_group == '\0'))
        തിരികെ FAIL;

    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
    {
        എങ്കിൽ (STRCMP(sp->sn_name, sign_name) == 0)
            നിർത്തുക;
    }

    എങ്കിൽ (sp == ശൂന്യ)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        തിരികെ FAIL;
    }

    എങ്കിൽ (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    എങ്കിൽ (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    എങ്കിൽ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    അല്ലെങ്കിൽ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    എങ്കിൽ (lnum > 0)
    {
        redraw_buf_വരി_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    അല്ലെങ്കിൽ
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Unplace the specified sign
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_unplace(പൂർണ്ണസംഖ്യ sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    എങ്കിൽ (buf->b_signlist == ശൂന്യ) // No signs in the buffer
        തിരികെ OK;

    എങ്കിൽ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    അല്ലെങ്കിൽ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        എങ്കിൽ (lnum == 0)
            തിരികെ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    എങ്കിൽ (buf->b_signlist == ശൂന്യ)
        may_force_numberwidth_recompute(buf, TRUE);

    തിരികെ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
സ്ഥിതം ശൂന്യം
sign_unplace_at_cursor(char_u *groupname)
{
    പൂർണ്ണസംഖ്യ id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    എങ്കിൽ (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    അല്ലെങ്കിൽ
        emsg(_(e_missing_sign_സംഖ്യ));
}

/*
 * Jump to a sign.
 */
സ്ഥിതം linenr_T
sign_jump(പൂർണ്ണസംഖ്യ sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    എങ്കിൽ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        തിരികെ -1;
    }

    // goto a sign ...
    എങ്കിൽ (buf_jump_open_win(buf) != ശൂന്യ)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    അല്ലെങ്കിൽ
    { // ... not currently in a window
        എങ്കിൽ (buf->b_fname == ശൂന്യ)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            തിരികെ -1;
        }
        setpcmark();
        എങ്കിൽ (do_ecmd(buf->b_fnum, ശൂന്യ, ശൂന്യ, ശൂന്യ, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            തിരികെ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    തിരികെ lnum;
}

/*
 * ":sign define {name} ..." command
 */
സ്ഥിതം ശൂന്യം
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ശൂന്യ;
    char_u *p = cmdline;
    char_u *icon = ശൂന്യ;
    char_u *text = ശൂന്യ;
    char_u *linehl = ശൂന്യ;
    char_u *texthl = ശൂന്യ;
    char_u *culhl = ശൂന്യ;
    char_u *numhl = ശൂന്യ;
    പൂർണ്ണസംഖ്യ prio = -1;
    പൂർണ്ണസംഖ്യ failed = FALSE;

    // set values for a defined sign.
    ആകുമ്പോൾ (TRUE)
    {
        arg = skipwhite(p);
        എങ്കിൽ (*arg == NUL)
            നിർത്തുക;

        p = skiptowhite_esc(arg);
        എങ്കിൽ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((അക്ഷരം *)arg);
        }
        അല്ലെങ്കിൽ
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            നിർത്തുക;
        }
    }

    എങ്കിൽ (!failed)
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
സ്ഥിതം ശൂന്യം
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               പൂർണ്ണസംഖ്യ id,
               char_u *group,
               പൂർണ്ണസംഖ്യ prio)
{
    എങ്കിൽ (id <= 0)
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
        എങ്കിൽ (lnum >= 0 || sign_name != ശൂന്യ || (group != ശൂന്യ && *group == '\0'))
            emsg(_(e_invalid_argument));
        അല്ലെങ്കിൽ
            sign_list_placed(buf, group);
    }
    അല്ലെങ്കിൽ
    {
        // Place a new sign
        എങ്കിൽ (sign_name == ശൂന്യ || buf == ശൂന്യ ||
            (group != ശൂന്യ && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            തിരികെ;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
സ്ഥിതം ശൂന്യം
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 പൂർണ്ണസംഖ്യ id,
                 char_u *group)
{
    എങ്കിൽ (lnum >= 0 || sign_name != ശൂന്യ || (group != ശൂന്യ && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        തിരികെ;
    }

    എങ്കിൽ (id == -2)
    {
        എങ്കിൽ (buf != ശൂന്യ)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        അല്ലെങ്കിൽ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                എങ്കിൽ (buf->b_signlist != ശൂന്യ)
                    buf_delete_signs(buf, group);
            }
        }
    }
    അല്ലെങ്കിൽ
    {
        എങ്കിൽ (buf != ശൂന്യ)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        അല്ലെങ്കിൽ
        {
            എങ്കിൽ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            അല്ലെങ്കിൽ
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
സ്ഥിതം ശൂന്യം
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              പൂർണ്ണസംഖ്യ id,
              char_u *group)
{
    എങ്കിൽ (sign_name == ശൂന്യ && group == ശൂന്യ && id == -1)
    {
        emsg(_(e_argument_required));
        തിരികെ;
    }

    എങ്കിൽ (buf == ശൂന്യ || (group != ശൂന്യ && *group == '\0') || lnum >= 0 ||
        sign_name != ശൂന്യ)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        തിരികെ;
    }

    (ശൂന്യം)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_sign_cmd_args(പൂർണ്ണസംഖ്യ cmd,
                    char_u *arg,
                    char_u **sign_name,
                    പൂർണ്ണസംഖ്യ *signid,
                    char_u **group,
                    പൂർണ്ണസംഖ്യ *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ശൂന്യ;
    പൂർണ്ണസംഖ്യ lnum_arg = FALSE;

    // first arg could be placed sign id
    എങ്കിൽ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        എങ്കിൽ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        അല്ലെങ്കിൽ
        {
            arg = skipwhite(arg);
        }
    }

    ആകുമ്പോൾ (*arg != NUL)
    {
        എങ്കിൽ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((അക്ഷരം *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            എങ്കിൽ (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                തിരികെ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            എങ്കിൽ (*arg != NUL)
                *arg++ = NUL;

            ആകുമ്പോൾ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            എങ്കിൽ (*arg != NUL)
                *arg++ = NUL;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((അക്ഷരം *)arg);
            arg = skiptowhite(arg);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            നിർത്തുക;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((പൂർണ്ണസംഖ്യ)getdigits(&arg));

            എങ്കിൽ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            നിർത്തുക;
        }
        അല്ലെങ്കിൽ
        {
            emsg(_(e_invalid_argument));
            തിരികെ FAIL;
        }

        arg = skipwhite(arg);
    }

    എങ്കിൽ (filename != ശൂന്യ && *buf == ശൂന്യ)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        തിരികെ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    എങ്കിൽ (filename == ശൂന്യ &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    തിരികെ OK;
}

/*
 * ":sign" command
 */
ശൂന്യം
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    പൂർണ്ണസംഖ്യ idx = sign_cmd_idx(arg, p);
    എങ്കിൽ (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        തിരികെ;
    }
    arg = skipwhite(p);

    എങ്കിൽ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ശൂന്യ;
        പൂർണ്ണസംഖ്യ id = -1;
        char_u *group = ശൂന്യ;
        പൂർണ്ണസംഖ്യ prio = -1;
        buf_T *buf = ശൂന്യ;
        linenr_T lnum = -1;

        // Parse command line arguments
        എങ്കിൽ (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            തിരികെ;

        എങ്കിൽ (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        അല്ലെങ്കിൽ എങ്കിൽ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        അല്ലെങ്കിൽ എങ്കിൽ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        തിരികെ;
    }

    // Define, undefine or list signs.
    എങ്കിൽ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        വേണ്ടി (sign_T *sp = first_sign; sp != ശൂന്യ && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    അല്ലെങ്കിൽ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        എങ്കിൽ (*p != NUL)
            *p++ = NUL;

        ആകുമ്പോൾ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        എങ്കിൽ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        അല്ലെങ്കിൽ എങ്കിൽ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        അല്ലെങ്കിൽ
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
സ്ഥിതം ശൂന്യം
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    എങ്കിൽ (sp->sn_icon != ശൂന്യ)
        dict_add_string(retdict, "icon", sp->sn_icon);

    എങ്കിൽ (sp->sn_text != ശൂന്യ)
        dict_add_string(retdict, "text", sp->sn_text);

    എങ്കിൽ (sp->sn_priority > 0)
        dict_add_സംഖ്യ(retdict, "priority", sp->sn_priority);

    എങ്കിൽ (sp->sn_വരി_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_വരി_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        അല്ലെങ്കിൽ
            dict_add_string(retdict, "linehl", p);
    }

    എങ്കിൽ (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_text_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        അല്ലെങ്കിൽ
            dict_add_string(retdict, "texthl", p);
    }

    എങ്കിൽ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_cul_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        അല്ലെങ്കിൽ
            dict_add_string(retdict, "culhl", p);
    }

    എങ്കിൽ (sp->sn_അക്കം_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_അക്കം_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        അല്ലെങ്കിൽ
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
സ്ഥിതം ശൂന്യം
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    എങ്കിൽ (name != ശൂന്യ)
    {
        sp = sign_find(name, ശൂന്യ);
        എങ്കിൽ (sp == ശൂന്യ)
            തിരികെ;
    }

    വേണ്ടി (; sp != ശൂന്യ && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        എങ്കിൽ (dict == ശൂന്യ)
            തിരികെ;

        എങ്കിൽ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            തിരികെ;
        }

        sign_getinfo(sp, dict);

        എങ്കിൽ (name != ശൂന്യ) // handle only the specified sign
            നിർത്തുക;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
ശൂന്യം
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = ശൂന്യ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        എങ്കിൽ (d != ശൂന്യ && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
സ്ഥിതം ശൂന്യം
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       പൂർണ്ണസംഖ്യ sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    എങ്കിൽ (d == ശൂന്യ)
        തിരികെ;

    എങ്കിൽ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        തിരികെ;
    }

    dict_add_സംഖ്യ(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    എങ്കിൽ (l == ശൂന്യ)
        തിരികെ;

    എങ്കിൽ (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        തിരികെ;
    }

    sign_entry_T *sign = ശൂന്യ;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        എങ്കിൽ (!sign_in_group(sign, sign_group))
            തുടരുക;

        എങ്കിൽ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            എങ്കിൽ (sdict != ശൂന്യ && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
സ്ഥിതം ശൂന്യം
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                പൂർണ്ണസംഖ്യ sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    എങ്കിൽ (buf != ശൂന്യ)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    അല്ലെങ്കിൽ
    {
        FOR_ALL_BUFFERS(buf)
        {
            എങ്കിൽ (buf->b_signlist != ശൂന്യ)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# എങ്കിൽ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
ശൂന്യം
sign_gui_started(ശൂന്യം)
{
    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
    {
        എങ്കിൽ (sp->sn_icon != ശൂന്യ)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
സ്ഥിതം ശൂന്യം
sign_list_defined(sign_T *sp)
{
    അക്ഷരം lbuf[സന്ദേശം_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    എങ്കിൽ (sp->sn_icon != ശൂന്യ)
    {
        സന്ദേശം_puts(" icon=");
        സന്ദേശം_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        എങ്കിൽ (sp->sn_image == ശൂന്യ)
            സന്ദേശം_puts(_(" (NOT FOUND)"));
# അല്ലെങ്കിൽ
        സന്ദേശം_puts(_(" (not supported)"));
# endif
    }

    എങ്കിൽ (sp->sn_text != ശൂന്യ)
    {
        സന്ദേശം_puts(" text=");
        സന്ദേശം_outtrans(sp->sn_text);
    }

    എങ്കിൽ (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, സന്ദേശം_BUF_LEN, " priority=%d", sp->sn_priority);
        സന്ദേശം_puts(lbuf);
    }

    എങ്കിൽ (sp->sn_വരി_hl > 0)
    {
        സന്ദേശം_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_വരി_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            സന്ദേശം_puts("NONE");
        അല്ലെങ്കിൽ
            സന്ദേശം_puts((അക്ഷരം *)p);
    }

    എങ്കിൽ (sp->sn_text_hl > 0)
    {
        സന്ദേശം_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_text_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            സന്ദേശം_puts("NONE");
        അല്ലെങ്കിൽ
            സന്ദേശം_puts((അക്ഷരം *)p);
    }

    എങ്കിൽ (sp->sn_cul_hl > 0)
    {
        സന്ദേശം_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_cul_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            സന്ദേശം_puts("NONE");
        അല്ലെങ്കിൽ
            സന്ദേശം_puts((അക്ഷരം *)p);
    }

    എങ്കിൽ (sp->sn_അക്കം_hl > 0)
    {
        സന്ദേശം_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ശൂന്യ, sp->sn_അക്കം_hl - 1, FALSE);
        എങ്കിൽ (p == ശൂന്യ)
            സന്ദേശം_puts("NONE");
        അല്ലെങ്കിൽ
            സന്ദേശം_puts((അക്ഷരം *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
സ്ഥിതം ശൂന്യം
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    എങ്കിൽ (sp->sn_image != ശൂന്യ)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    എങ്കിൽ (sp_prev == ശൂന്യ)
        first_sign = sp->sn_next;
    അല്ലെങ്കിൽ
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# എങ്കിൽ defined(FEAT_SIGN_ICONS)
ശൂന്യം *
sign_get_image(പൂർണ്ണസംഖ്യ typenr) // the attribute which may have a sign
{
    sign_T *sp = ശൂന്യ;
    FOR_ALL_SIGNS(sp)
    {
        എങ്കിൽ (sp->sn_typenr == typenr)
            തിരികെ sp->sn_image;
    }
    തിരികെ ശൂന്യ;
}
# endif

/*
 * Undefine/free all signs.
 */
ശൂന്യം
free_signs(ശൂന്യം)
{
    ആകുമ്പോൾ (first_sign != ശൂന്യ)
        sign_undefine(first_sign, ശൂന്യ);
}

സ്ഥിതം എണ്ണൽ
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
സ്ഥിതം char_u *
get_nth_sign_name(പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ current_idx = 0;
    sign_T *sp = ശൂന്യ;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        എങ്കിൽ (current_idx++ == idx)
            തിരികെ sp->sn_name;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
സ്ഥിതം char_u *
get_nth_sign_group_name(പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ current_idx = 0;
    പൂർണ്ണസംഖ്യ todo = (പൂർണ്ണസംഖ്യ)sg_table.ht_used;
    hashitem_T *hi = ശൂന്യ;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&sg_table, hi, todo)
    {
        എങ്കിൽ (!HASHITEM_EMPTY(hi))
        {
            --todo;
            എങ്കിൽ (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                തിരികെ group->sg_name;
            }
        }
    }
    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    തിരഞ്ഞെടുപ്പ് (expand_what)
    {
        അവസ്ഥ EXP_SUBCMD:
            തിരികെ (char_u *)cmds[idx];
        അവസ്ഥ EXP_DEFINE:
        {
            അക്ഷരം *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ശൂന്യ };
            തിരികെ (char_u *)define_arg[idx];
        }
        അവസ്ഥ EXP_PLACE:
        {
            അക്ഷരം *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ശൂന്യ };
            തിരികെ (char_u *)place_arg[idx];
        }
        അവസ്ഥ EXP_LIST:
        {
            അക്ഷരം *list_arg[] = { "group=", "file=", "buffer=", ശൂന്യ };
            തിരികെ (char_u *)list_arg[idx];
        }
        അവസ്ഥ EXP_UNPLACE:
        {
            അക്ഷരം *unplace_arg[] = { "group=", "file=", "buffer=", ശൂന്യ };
            തിരികെ (char_u *)unplace_arg[idx];
        }
        അവസ്ഥ EXP_SIGN_NAMES:
            തിരികെ get_nth_sign_name(idx);
        അവസ്ഥ EXP_SIGN_GROUPS:
            തിരികെ get_nth_sign_group_name(idx);
        സ്ഥിരസ്ഥിതി:
            തിരികെ ശൂന്യ;
    }
}

/*
 * Handle command line completion for :sign command.
 */
ശൂന്യം
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    പൂർണ്ണസംഖ്യ cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    എങ്കിൽ (*end_subcmd == NUL)
        തിരികെ;

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
    ചെയ്യുക
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    ആകുമ്പോൾ (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    എങ്കിൽ (p == ശൂന്യ)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        തിരഞ്ഞെടുപ്പ് (cmd_idx)
        {
            അവസ്ഥ SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                നിർത്തുക;
            അവസ്ഥ SIGNCMD_PLACE:
                // List placed signs
                എങ്കിൽ (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                അല്ലെങ്കിൽ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                നിർത്തുക;
            അവസ്ഥ SIGNCMD_LIST:
            അവസ്ഥ SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                നിർത്തുക;
            അവസ്ഥ SIGNCMD_JUMP:
            അവസ്ഥ SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    അല്ലെങ്കിൽ
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        തിരഞ്ഞെടുപ്പ് (cmd_idx)
        {
            അവസ്ഥ SIGNCMD_DEFINE:
                എങ്കിൽ (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                അല്ലെങ്കിൽ
                    xp->xp_context = EXPAND_NOTHING;
                നിർത്തുക;
            അവസ്ഥ SIGNCMD_PLACE:
                എങ്കിൽ (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                അല്ലെങ്കിൽ
                    xp->xp_context = EXPAND_NOTHING;
                നിർത്തുക;
            അവസ്ഥ SIGNCMD_UNPLACE:
            അവസ്ഥ SIGNCMD_JUMP:
                എങ്കിൽ (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                അല്ലെങ്കിൽ
                    xp->xp_context = EXPAND_NOTHING;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ശൂന്യ;
    char_u *icon = ശൂന്യ;
    char_u *linehl = ശൂന്യ;
    char_u *text = ശൂന്യ;
    char_u *texthl = ശൂന്യ;
    char_u *culhl = ശൂന്യ;
    char_u *numhl = ശൂന്യ;
    പൂർണ്ണസംഖ്യ prio = -1;
    പൂർണ്ണസംഖ്യ retval = -1;

    എങ്കിൽ (name_arg == ശൂന്യ && dict == ശൂന്യ)
        തിരികെ retval;

    എങ്കിൽ (name_arg == ശൂന്യ)
        name = dict_get_string(dict, "name", TRUE);
    അല്ലെങ്കിൽ
        name = vim_strsave(name_arg);

    എങ്കിൽ (name == ശൂന്യ || name[0] == NUL)
        പോകുക cleanup;

    എങ്കിൽ (dict != ശൂന്യ)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_സംഖ്യ_def(dict, "priority", -1);
    }

    എങ്കിൽ (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    തിരികെ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
സ്ഥിതം ശൂന്യം
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ശൂന്യ;
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
        പൂർണ്ണസംഖ്യ retval = -1;

        എങ്കിൽ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ശൂന്യ, li->li_tv.vval.v_dict);
        അല്ലെങ്കിൽ
            emsg(_(e_dictionary_required));

        list_append_സംഖ്യ(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
ശൂന്യം
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
            തിരികെ;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        തിരികെ;
    }

    // Define a single sign
    rettv->vval.v_സംഖ്യ = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (name == ശൂന്യ)
        തിരികെ;

    എങ്കിൽ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        തിരികെ;

    rettv->vval.v_സംഖ്യ = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ശൂന്യ);
}

/*
 * "sign_getdefined()" function
 */
ശൂന്യം
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        തിരികെ;

    char_u *name = ശൂന്യ;
    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
ശൂന്യം
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ശൂന്യ;
    linenr_T lnum = 0;
    പൂർണ്ണസംഖ്യ sign_id = 0;
    char_u *group = ശൂന്യ;

    എങ്കിൽ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        തിരികെ;

    എങ്കിൽ (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        എങ്കിൽ (buf == ശൂന്യ)
            തിരികെ;

        എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
        {
            എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                തിരികെ;

            dictitem_T *di = ശൂന്യ;
            dict_T *dict = argvars[1].vval.v_dict;

            എങ്കിൽ ((di = dict_find(dict, (char_u *)"lnum", -1)) != ശൂന്യ)
            {
                // get signs placed at this line
                പൂർണ്ണസംഖ്യ notanum = FALSE;
                tv_get_സംഖ്യ_chk(&di->di_tv, &notanum);
                എങ്കിൽ (notanum)
                    തിരികെ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            എങ്കിൽ ((di = dict_find(dict, (char_u *)"id", -1)) != ശൂന്യ)
            {
                // get sign placed with this identifier
                പൂർണ്ണസംഖ്യ notanum = FALSE;
                sign_id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&di->di_tv, &notanum);
                എങ്കിൽ (notanum)
                    തിരികെ;
            }

            എങ്കിൽ ((di = dict_find(dict, (char_u *)"group", -1)) != ശൂന്യ)
            {
                group = tv_get_string_chk(&di->di_tv);
                എങ്കിൽ (group == ശൂന്യ)
                    തിരികെ;

                എങ്കിൽ (*group == '\0') // empty string means global group
                    group = ശൂന്യ;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
ശൂന്യം
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = ശൂന്യ;
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script() && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        തിരികെ;

    പൂർണ്ണസംഖ്യ notanum = FALSE;
    // Sign identifier
    പൂർണ്ണസംഖ്യ sign_id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], &notanum);
    എങ്കിൽ (notanum)
        തിരികെ;

    എങ്കിൽ (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        തിരികെ;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (sign_group == ശൂന്യ)
        തിരികെ;

    എങ്കിൽ (sign_group[0] == '\0')
    {
        sign_group = ശൂന്യ; // global sign group
    }
    അല്ലെങ്കിൽ
    {
        sign_group = vim_strsave(sign_group);
        എങ്കിൽ (sign_group == ശൂന്യ)
            തിരികെ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    എങ്കിൽ (buf == ശൂന്യ)
        പോകുക cleanup;

    rettv->vval.v_സംഖ്യ = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    പൂർണ്ണസംഖ്യ sign_id = 0;
    char_u *group = ശൂന്യ;
    char_u *sign_name = ശൂന്യ;
    buf_T *buf = ശൂന്യ;
    dictitem_T *di = ശൂന്യ;
    linenr_T lnum = 0;
    പൂർണ്ണസംഖ്യ prio = -1;
    പൂർണ്ണസംഖ്യ ret_sign_id = -1;

    // sign identifier
    എങ്കിൽ (id_tv == ശൂന്യ)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        എങ്കിൽ (di != ശൂന്യ)
            id_tv = &di->di_tv;
    }

    എങ്കിൽ (id_tv == ശൂന്യ)
    {
        sign_id = 0;
    }
    അല്ലെങ്കിൽ
    {
        പൂർണ്ണസംഖ്യ notanum = FALSE;
        sign_id = tv_get_സംഖ്യ_chk(id_tv, &notanum);

        എങ്കിൽ (notanum)
            തിരികെ -1;

        എങ്കിൽ (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            തിരികെ -1;
        }
    }

    // sign group
    എങ്കിൽ (group_tv == ശൂന്യ)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        എങ്കിൽ (di != ശൂന്യ)
            group_tv = &di->di_tv;
    }

    എങ്കിൽ (group_tv == ശൂന്യ)
    {
        group = ശൂന്യ; // global group
    }
    അല്ലെങ്കിൽ
    {
        group = tv_get_string_chk(group_tv);
        എങ്കിൽ (group == ശൂന്യ)
            പോകുക cleanup;

        എങ്കിൽ (group[0] == '\0') // global sign group
        {
            group = ശൂന്യ;
        }
        അല്ലെങ്കിൽ
        {
            group = vim_strsave(group);
            എങ്കിൽ (group == ശൂന്യ)
                തിരികെ -1;
        }
    }

    // sign name
    എങ്കിൽ (name_tv == ശൂന്യ)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        എങ്കിൽ (di != ശൂന്യ)
            name_tv = &di->di_tv;
    }

    എങ്കിൽ (name_tv == ശൂന്യ)
        പോകുക cleanup;

    sign_name = tv_get_string_chk(name_tv);
    എങ്കിൽ (sign_name == ശൂന്യ)
        പോകുക cleanup;

    // buffer to place the sign
    എങ്കിൽ (buf_tv == ശൂന്യ)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        എങ്കിൽ (di != ശൂന്യ)
            buf_tv = &di->di_tv;
    }

    എങ്കിൽ (buf_tv == ശൂന്യ)
        പോകുക cleanup;

    buf = get_buf_arg(buf_tv);
    എങ്കിൽ (buf == ശൂന്യ)
        പോകുക cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
        lnum = tv_get_lnum(&di->di_tv);
        എങ്കിൽ (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            പോകുക cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
        പൂർണ്ണസംഖ്യ notanum = FALSE;
        prio = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&di->di_tv, &notanum);
        എങ്കിൽ (notanum)
            പോകുക cleanup;
    }

    എങ്കിൽ (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    തിരികെ ret_sign_id;
}

/*
 * "sign_place()" function
 */
ശൂന്യം
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ശൂന്യ;
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script() && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        തിരികെ;

    എങ്കിൽ (argvars[4].v_type != VAR_UNKNOWN)
    {
        എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            തിരികെ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_സംഖ്യ = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
ശൂന്യം
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
        തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        തിരികെ;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
        തിരികെ;

    // Process the List of sign attributes
    listitem_T *li = ശൂന്യ;
    FOR_ALL_LIST_ഇനങ്ങൾ(argvars[0].vval.v_list, li)
    {
        പൂർണ്ണസംഖ്യ sign_id = -1;

        എങ്കിൽ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ,
                                           li->li_tv.vval.v_dict);
        അല്ലെങ്കിൽ
            emsg(_(e_dictionary_required));

        list_append_സംഖ്യ(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
സ്ഥിതം ശൂന്യം
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ശൂന്യ;
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
        പൂർണ്ണസംഖ്യ retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        എങ്കിൽ (name != ശൂന്യ && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_സംഖ്യ(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
ശൂന്യം
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    എങ്കിൽ (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
            തിരികെ;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        തിരികെ;
    }

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        എങ്കിൽ (name == ശൂന്യ)
            തിരികെ;

        എങ്കിൽ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_സംഖ്യ = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    പൂർണ്ണസംഖ്യ sign_id = 0;
    buf_T *buf = ശൂന്യ;
    char_u *group = ശൂന്യ;
    പൂർണ്ണസംഖ്യ retval = -1;

    // sign group
    group = (group_tv != ശൂന്യ) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    എങ്കിൽ (group != ശൂന്യ)
    {
        എങ്കിൽ (group[0] == '\0') // global sign group
        {
            group = ശൂന്യ;
        }
        അല്ലെങ്കിൽ
        {
            group = vim_strsave(group);
            എങ്കിൽ (group == ശൂന്യ)
                തിരികെ retval;
        }
    }

    എങ്കിൽ (dict != ശൂന്യ)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        എങ്കിൽ (di != ശൂന്യ)
        {
            buf = get_buf_arg(&di->di_tv);
            എങ്കിൽ (buf == ശൂന്യ)
                പോകുക cleanup;
        }

        എങ്കിൽ (dict_has_താക്കോൽ(dict, "id"))
        {
            sign_id = dict_get_സംഖ്യ(dict, "id");
            എങ്കിൽ (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                പോകുക cleanup;
            }
        }
    }

    എങ്കിൽ (buf == ശൂന്യ)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            എങ്കിൽ (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    തിരികെ retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    ആകുമ്പോൾ (sign != ശൂന്യ && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    തിരികെ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
പൂർണ്ണസംഖ്യ
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    എങ്കിൽ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        തിരികെ get_first_valid_sign(wp) != ശൂന്യ && !wp->w_p_nu && !wp->w_p_rnu;

    എങ്കിൽ (*wp->w_p_scl == 'n')
        തിരികെ FALSE;

    എങ്കിൽ (*wp->w_p_scl == 'y')
        തിരികെ TRUE;

    തിരികെ (get_first_valid_sign(wp) != ശൂന്യ
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
ശൂന്യം
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ശൂന്യ;
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_സംഖ്യ = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
ശൂന്യം
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
        തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        തിരികെ;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
        തിരികെ;

    listitem_T *li = ശൂന്യ;
    FOR_ALL_LIST_ഇനങ്ങൾ(argvars[0].vval.v_list, li)
    {
        പൂർണ്ണസംഖ്യ retval = -1;

        എങ്കിൽ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ശൂന്യ, li->li_tv.vval.v_dict);
        അല്ലെങ്കിൽ
            emsg(_(e_dictionary_required));

        list_append_സംഖ്യ(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
