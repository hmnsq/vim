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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sign sign_T;

ସଂରଚନା sign
{
    sign_T *sn_next; // next sign in list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    ଶୂନ୍ୟ *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_ଧାଡ଼ି_hl; // highlight ID for line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_text_hl; // highlight ID for text
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_ଅଙ୍କ_hl; // highlight ID for line number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ସ୍ଥିର sign_T *first_sign = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_sign_typenr = 1;

ସ୍ଥିର ଶୂନ୍ୟ sign_list_defined(sign_T *sp);
ସ୍ଥିର ଶୂନ୍ୟ sign_undefine(sign_T *sp, sign_T *sp_prev);

ସ୍ଥିର ଅକ୍ଷର *cmds[] = { "define",
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
                        ଖାଲି
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     ପାଇଁ ((sp) = first_sign; (sp) != ଖାଲି; (sp) = (sp)->sn_next)

ସ୍ଥିର hashtab_T sg_table; // sign group (signgroup_T) hashtable
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
ଶୂନ୍ୟ
init_signs(ଶୂନ୍ୟ)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ସ୍ଥିର signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = ଖାଲି;

    ଯଦି (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ଯଦି (group == ଖାଲି)
            ଫେରନ୍ତୁ ଖାଲି;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    ଅନ୍ୟଥା
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    ଫେରନ୍ତୁ group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    ଯଦି (HASHITEM_EMPTY(hi))
        ଫେରନ୍ତୁ;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    ଯଦି (group->sg_refcount == 0)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_in_group(sign_entry_T *sign, char_u *group)
{
    ଫେରନ୍ତୁ ((group != ଖାଲି && STRCMP(group, "*") == 0) ||
            (group == ଖାଲି && sign->se_group == ଖାଲି) ||
            (group != ଖାଲି && sign->se_group != ଖାଲି &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା for_popup = sign->se_group != ଖାଲି &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    ଫେରନ୍ତୁ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = 1;
    signgroup_T *group = ଖାଲି;
    sign_entry_T *sign = ଖାଲି;

    ଯଦି (groupname != ଖାଲି)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        ଯଦି (HASHITEM_EMPTY(hi))
            ଫେରନ୍ତୁ id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = FALSE;
    ଯେତେବେଳେ (!found)
    {
        ଯଦି (group == ଖାଲି)
            id = next_sign_id++; // global group
        ଅନ୍ୟଥା
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ଯଦି (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                ଭାଙ୍ଗ;
            }
        }
    }

    ଫେରନ୍ତୁ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ସ୍ଥିର ଶୂନ୍ୟ
ଭର୍ତ୍ତି_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, // sign ID
            char_u *group, // sign group; NULL for global group
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(ଆକାର(sign_entry_T), FALSE, aid_ଭର୍ତ୍ତି_sign);
    ଯଦି (newsign == ଖାଲି)
        ଫେରନ୍ତୁ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ଯଦି (group != ଖାଲି)
    {
        newsign->se_group = sign_group_ref(group);
        ଯଦି (newsign->se_group == ଖାଲି)
        {
            vim_free(newsign);
            ଫେରନ୍ତୁ;
        }
    }
    ଅନ୍ୟଥା
    {
        newsign->se_group = ଖାଲି;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    ଯଦି (next != ଖାଲି)
        next->se_prev = newsign;

    ଯଦି (prev == ଖାଲି)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ଯଦି (buf->b_signlist == ଖାଲି)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_ଧାଡ଼ି_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ଯଦି (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    ଅନ୍ୟଥା
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
ସ୍ଥିର ଶୂନ୍ୟ
ଭର୍ତ୍ତି_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ଯେତେବେଳେ (prev != ଖାଲି && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == ଖାଲି) ? buf->b_signlist : prev->se_next;

    ଭର୍ତ୍ତି_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ସ୍ଥିର sign_T *
find_sign_by_typenr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr)
{
    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
        ଯଦି (sp->sn_typenr == typenr)
            ଫେରନ୍ତୁ sp;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the name of a sign by its typenr.
 */
ସ୍ଥିର char_u *
sign_typenr2name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr)
{
    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
        ଯଦି (sp->sn_typenr == typenr)
            ଫେରନ୍ତୁ sp->sn_name;
    ଫେରନ୍ତୁ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ସ୍ଥିର dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ଯଦି (d == ଖାଲି)
        ଫେରନ୍ତୁ ଖାଲି;

    dict_add_ସଂଖ୍ୟା(d, "id", sign->se_id);
    ଯଦି (sign->se_group == ଖାଲି)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    ଅନ୍ୟଥା
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_ସଂଖ୍ୟା(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_ସଂଖ୍ୟା(d, "priority", sign->se_priority);

    ଫେରନ୍ତୁ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_sort_by_prio_on_ଧାଡ଼ି(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ଯଦି ((sign->se_prev == ଖାଲି || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == ଖାଲି || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        ଫେରନ୍ତୁ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    ଯେତେବେଳେ (p->se_prev != ଖାଲି && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    ଯଦି (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        ଯେତେବେଳେ (p->se_next != ଖାଲି && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    ଯଦି (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    ଯଦି (sign->se_prev != ଖାଲି)
        sign->se_prev->se_next = sign->se_next;

    ଯଦି (sign->se_next != ଖାଲି)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = ଖାଲି;
    sign->se_next = ଖାଲି;

    // Re-insert 'sign' at the right place
    ଯଦି (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        ଯଦି (sign->se_prev != ଖାଲି)
            sign->se_prev->se_next = sign;

        ଯଦି (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ଅନ୍ୟଥା
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        ଯଦି (sign->se_next != ଖାଲି)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ସ୍ଥିର ଶୂନ୍ୟ
buf_addsign(buf_T *buf, // buffer to store sign in
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, // sign ID
            char_u *groupname, // sign group
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    sign_entry_T *prev = ଖାଲି; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ଯଦି (buf->b_sign_finger != ଖାଲି && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    ପାଇଁ (sign = (prev == ଖାଲି) ? buf->b_signlist : prev->se_next; sign != ଖାଲି;
         sign = sign->se_next)
    {
        ଯଦି (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ଧାଡ଼ି(buf, sign);
            ଫେରନ୍ତୁ;
        }
        ଅନ୍ୟଥା ଯଦି (lnum < sign->se_lnum)
        {
            ଭର୍ତ୍ତି_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ଫେରନ୍ତୁ;
        }
        prev = sign;
    }

    ଭର୍ତ୍ତି_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
ସ୍ଥିର linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା markId, // sign ID
                     char_u *group, // sign group
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr, // typenr of sign we are adding
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio) // sign priority
{
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ଯଦି (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_ଧାଡ଼ି(buf, sign);
            ଫେରନ୍ତୁ sign->se_lnum;
        }
    }

    ଫେରନ୍ତୁ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = ଖାଲି;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ଯଦି (sign->se_lnum > lnum)
            ଭାଙ୍ଗ;

        ଯଦି (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            ଯଦି (sp == ଖାଲି)
                ଫେରନ୍ତୁ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            ଯଦି (sattr->sat_text != ଖାଲି && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            ଯଦି (sp->sn_ଧାଡ଼ି_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_ଧାଡ଼ି_hl);

            ଯଦି (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ଯଦି (sp->sn_ଅଙ୍କ_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_ଅଙ୍କ_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ଯଦି (sign->se_next != ଖାଲି &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                ଯଦି (next_sp == ଖାଲି)
                    ଫେରନ୍ତୁ FALSE;

                ଯଦି (sattr->sat_icon == ଖାଲି && sattr->sat_text == ଖାଲି)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                ଯଦି (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                ଯଦି (sp->sn_ଧାଡ଼ି_hl <= 0 && next_sp->sn_ଧାଡ଼ି_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_ଧାଡ଼ି_hl);

                ଯଦି (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                ଯଦି (sp->sn_ଅଙ୍କ_hl <= 0 && next_sp->sn_ଅଙ୍କ_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_ଅଙ୍କ_hl);
            }
            ଫେରନ୍ତୁ TRUE;
        }
    }
    ଫେରନ୍ତୁ FALSE;
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
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = ଖାଲି; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ଖାଲି; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    ପାଇଁ (sign_entry_T *sign = buf->b_signlist; sign != ଖାଲି; sign = next)
    {
        next = sign->se_next;

        ଯଦି ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            ଯଦି (next != ଖାଲି)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ଯଦି (sign->se_group != ଖାଲି)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_ଧାଡ଼ି_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ଯଦି (group == ଖାଲି || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                ଭାଙ୍ଗ;
        }
        ଅନ୍ୟଥା
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ଯଦି (buf->b_signlist == ଖାଲି)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ଧାଡ଼ି_abv_curs();
    }

    ଫେରନ୍ତୁ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_findsign(buf_T *buf, // buffer to store sign in
             ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        ଯଦି (sign->se_id == id && sign_in_group(sign, group))
            ଫେରନ୍ତୁ sign->se_lnum;

    ଫେରନ୍ତୁ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ସ୍ଥିର sign_entry_T *
buf_getsign_at_ଧାଡ଼ି(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ଯଦି (sign->se_lnum > lnum)
            ଭାଙ୍ଗ;

        ଯଦି (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            ଫେରନ୍ତୁ sign;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_ଧାଡ଼ି(buf, lnum, groupname);
    ଯଦି (sign != ଖାଲି)
        ଫେରନ୍ତୁ sign->se_id;

    ଫେରନ୍ତୁ 0;
}

# ଯଦି defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr) // sign type number
{
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ଯଦି (sign->se_lnum > lnum)
            ଭାଙ୍ଗ;

        ଯଦି (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ଫେରନ୍ତୁ sign->se_id;
    }

    ଫେରନ୍ତୁ 0;
}

#  ଯଦି defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_signcount(buf_T *buf, linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
    sign_entry_T *sign = ଖାଲି; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ଯଦି (sign->se_lnum > lnum)
            ଭାଙ୍ଗ;

        ଯଦି (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ଖାଲି)
            ଗଣନା_2++;
    }

    ଫେରନ୍ତୁ ଗଣନା_2;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
ଶୂନ୍ୟ
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = ଖାଲି; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ଯଦି (buf->b_signlist != ଖାଲି && curwin != ଖାଲି)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_ଧାଡ଼ି_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = ଖାଲି;

    ପାଇଁ (sign_entry_T *sign = buf->b_signlist; sign != ଖାଲି; sign = next)
    {
        next = sign->se_next;
        ଯଦି (sign_in_group(sign, group))
        {
            *lastp = next;

            ଯଦି (next != ଖାଲି)
                next->se_prev = sign->se_prev;

            ଯଦି (sign->se_group != ଖାଲି)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        ଅନ୍ୟଥା
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    ଅକ୍ଷର lbuf[ସନ୍ଦେଶ_BUF_LEN];
    ଅକ୍ଷର group[ସନ୍ଦେଶ_BUF_LEN];

    ସନ୍ଦେଶ_puts_title(_("\n--- Signs ---"));
    ସନ୍ଦେଶ_putchar('\n');

    buf_T *buf = (rbuf == ଖାଲି) ? firstbuf : rbuf;
    ଯେତେବେଳେ (buf != ଖାଲି && !got_int)
    {
        ଯଦି (buf->b_signlist != ଖାଲି)
        {
            vim_snprintf(lbuf, ସନ୍ଦେଶ_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            ସନ୍ଦେଶ_puts_attr(lbuf, HL_ATTR(HLF_D));
            ସନ୍ଦେଶ_putchar('\n');
        }

        sign_entry_T *sign = ଖାଲି;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ଯଦି (got_int)
                ଭାଙ୍ଗ;

            ଯଦି (!sign_in_group(sign, sign_group))
                ଜାରି;

            ଯଦି (sign->se_group != ଖାଲି)
                vim_snprintf(group, ସନ୍ଦେଶ_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            ଅନ୍ୟଥା
                group[0] = '\0';

            vim_snprintf(lbuf, ସନ୍ଦେଶ_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            ସନ୍ଦେଶ_puts(lbuf);
            ସନ୍ଦେଶ_putchar('\n');
        }

        ଯଦି (rbuf != ଖାଲି)
            ଭାଙ୍ଗ;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
ଶୂନ୍ୟ
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = ଖାଲି; // line changes may reorder, drop the finger
    sign_entry_T *sign = ଖାଲି; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ଯଦି (sign->se_lnum < line1)
            ଜାରି;

        linenr_T new_lnum = sign->se_lnum;

        ଯଦି (sign->se_lnum <= line2)
        {
            ଯଦି (amount != MAXLNUM)
                new_lnum += amount;
        }
        ଅନ୍ୟଥା ଯଦି (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ଯଦି (new_lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0;
    ଅକ୍ଷର save = *end_cmd;
    *end_cmd = NUL;

    ଯେତେବେଳେ (cmds[idx] != ଖାଲି && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    ଫେରନ୍ତୁ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ସ୍ଥିର sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    ଯଦି (sp_prev != ଖାଲି)
        *sp_prev = ଖାଲି;

    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
    {
        ଯଦି (STRCMP(sp->sn_name, name) == 0)
            ଭାଙ୍ଗ;

        ଯଦି (sp_prev != ଖାଲି)
            *sp_prev = sp;
    }

    ଫେରନ୍ତୁ sp;
}

/*
 * Allocate a new sign
 */
ସ୍ଥିର sign_T *
alloc_new_sign(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(ଆକାର(sign_T), aid_sign_define_by_name);
    ଯଦି (sp == ଖାଲି)
        ଫେରନ୍ତୁ ଖାଲି;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    ଯେତେବେଳେ (lp != ଖାଲି)
    {
        ଯଦି (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            ଯଦି (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            ଯଦି (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ଫେରନ୍ତୁ ଖାଲି;
            }

            lp = first_sign; // start all over
            ଜାରି;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    ଯଦି (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ଯଦି (sp->sn_name == ଖାଲି) // out of memory
    {
        vim_free(sp);
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ sp;
}

/*
 * Initialize the icon information for a new sign
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ଯଦି (gui.in_use)
    {
        out_flush();
        ଯଦି (sp->sn_image != ଖାଲି)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = ଖାଲି;
    char_u *endp = text + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(text);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells = 0;

    // Remove backslashes so that it is possible to use a space.
    ପାଇଁ (s = text; s + 1 < endp; ++s)
    {
        ଯଦି (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ଯଦି (has_mbyte)
    {
        ପାଇଁ (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            ଯଦି (!vim_isprintc((*mb_ptr2char)(s)))
                ଭାଙ୍ଗ;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ଅନ୍ୟଥା
    {
        ପାଇଁ (s = text; s < endp; ++s)
        {
            ଯଦି (!vim_isprintc(*s))
                ଭାଙ୍ଗ;
        }
        cells = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - text);
    }

    // Currently sign text must be one or two display cells
    ଯଦି (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        ଫେରନ୍ତୁ FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    ଯଦି (sp->sn_text != ଖାଲି && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    ଫେରନ୍ତୁ OK;
}

/*
 * Define a new sign or update an existing sign
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio)
{
    sign_T *sp_prev = ଖାଲି;
    sign_T *sp = sign_find(name, &sp_prev);
    ଯଦି (sp == ଖାଲି)
    {
        sp = alloc_new_sign(name);
        ଯଦି (sp == ଖାଲି)
            ଫେରନ୍ତୁ FAIL;

        // add the new sign to the list of signs
        ଯଦି (sp_prev == ଖାଲି)
            first_sign = sp;
        ଅନ୍ୟଥା
            sp_prev->sn_next = sp;
    }
    ଅନ୍ୟଥା
    {
        win_T *wp = ଖାଲି;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ଯଦି (wp->w_buffer->b_signlist != ଖାଲି)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ଯଦି (icon != ଖାଲି)
        sign_define_init_icon(sp, icon);

    ଯଦି (text != ଖାଲି && (sign_define_init_text(sp, text) == FAIL))
        ଫେରନ୍ତୁ FAIL;

    sp->sn_priority = prio;

    ଯଦି (linehl != ଖାଲି)
    {
        ଯଦି (*linehl == NUL)
            sp->sn_ଧାଡ଼ି_hl = 0;
        ଅନ୍ୟଥା
            sp->sn_ଧାଡ଼ି_hl = syn_check_group(linehl, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(linehl));
    }

    ଯଦି (texthl != ଖାଲି)
    {
        ଯଦି (*texthl == NUL)
            sp->sn_text_hl = 0;
        ଅନ୍ୟଥା
            sp->sn_text_hl = syn_check_group(texthl, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(texthl));
    }

    ଯଦି (culhl != ଖାଲି)
    {
        ଯଦି (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ଅନ୍ୟଥା
            sp->sn_cul_hl = syn_check_group(culhl, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(culhl));
    }

    ଯଦି (numhl != ଖାଲି)
    {
        ଯଦି (*numhl == NUL)
            sp->sn_ଅଙ୍କ_hl = 0;
        ଅନ୍ୟଥା
            sp->sn_ଅଙ୍କ_hl = syn_check_group(numhl, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(numhl));
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_exists_by_name(char_u *name)
{
    ଫେରନ୍ତୁ sign_find(name, ଖାଲି) != ଖାଲି;
}

/*
 * Free the sign specified by 'name'.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_undefine_by_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା give_error)
{
    sign_T *sp_prev = ଖାଲି;
    sign_T *sp = sign_find(name, &sp_prev);
    ଯଦି (sp == ଖାଲି)
    {
        ଯଦି (give_error)
            semsg(_(e_unknown_sign_str), name);
        ଫେରନ୍ତୁ FAIL;
    }
    sign_undefine(sp, sp_prev);

    ଫେରନ୍ତୁ OK;
}

/*
 * List the signs matching 'name'
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, ଖାଲି);
    ଯଦି (sp != ଖାଲି)
        sign_list_defined(sp);
    ଅନ୍ୟଥା
        semsg(_(e_unknown_sign_str), name);
}

ସ୍ଥିର ଶୂନ୍ୟ
may_force_numberwidth_recompute(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା unplace)
{
# ଯଦି defined(FEAT_LINEBREAK)
    tabpage_T *tp = ଖାଲି;
    win_T *wp = ଖାଲି;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ଯଦି (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_ଧାଡ଼ି_ଗଣନା = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_place(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio)
{
    // Check for reserved character '*' in group name
    ଯଦି (sign_group != ଖାଲି && (*sign_group == '*' || *sign_group == '\0'))
        ଫେରନ୍ତୁ FAIL;

    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
    {
        ଯଦି (STRCMP(sp->sn_name, sign_name) == 0)
            ଭାଙ୍ଗ;
    }

    ଯଦି (sp == ଖାଲି)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    ଯଦି (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    ଯଦି (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    ଅନ୍ୟଥା
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    ଯଦି (lnum > 0)
    {
        redraw_buf_ଧାଡ଼ି_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ଅନ୍ୟଥା
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Unplace the specified sign
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_unplace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    ଯଦି (buf->b_signlist == ଖାଲି) // No signs in the buffer
        ଫେରନ୍ତୁ OK;

    ଯଦି (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    ଅନ୍ୟଥା
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        ଯଦି (lnum == 0)
            ଫେରନ୍ତୁ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ଯଦି (buf->b_signlist == ଖାଲି)
        may_force_numberwidth_recompute(buf, TRUE);

    ଫେରନ୍ତୁ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_unplace_at_cursor(char_u *groupname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    ଯଦି (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    ଅନ୍ୟଥା
        emsg(_(e_missing_sign_ସଂଖ୍ୟା));
}

/*
 * Jump to a sign.
 */
ସ୍ଥିର linenr_T
sign_jump(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    ଯଦି (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ଫେରନ୍ତୁ -1;
    }

    // goto a sign ...
    ଯଦି (buf_jump_open_win(buf) != ଖାଲି)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ଅନ୍ୟଥା
    { // ... not currently in a window
        ଯଦି (buf->b_fname == ଖାଲି)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            ଫେରନ୍ତୁ -1;
        }
        setpcmark();
        ଯଦି (do_ecmd(buf->b_fnum, ଖାଲି, ଖାଲି, ଖାଲି, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ଫେରନ୍ତୁ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ଫେରନ୍ତୁ lnum;
}

/*
 * ":sign define {name} ..." command
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ଖାଲି;
    char_u *p = cmdline;
    char_u *icon = ଖାଲି;
    char_u *text = ଖାଲି;
    char_u *linehl = ଖାଲି;
    char_u *texthl = ଖାଲି;
    char_u *culhl = ଖାଲି;
    char_u *numhl = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା failed = FALSE;

    // set values for a defined sign.
    ଯେତେବେଳେ (TRUE)
    {
        arg = skipwhite(p);
        ଯଦି (*arg == NUL)
            ଭାଙ୍ଗ;

        p = skiptowhite_esc(arg);
        ଯଦି (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((ଅକ୍ଷର *)arg);
        }
        ଅନ୍ୟଥା
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            ଭାଙ୍ଗ;
        }
    }

    ଯଦି (!failed)
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
ସ୍ଥିର ଶୂନ୍ୟ
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               ପୂର୍ଣ୍ଣସଂଖ୍ୟା id,
               char_u *group,
               ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio)
{
    ଯଦି (id <= 0)
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
        ଯଦି (lnum >= 0 || sign_name != ଖାଲି || (group != ଖାଲି && *group == '\0'))
            emsg(_(e_invalid_argument));
        ଅନ୍ୟଥା
            sign_list_placed(buf, group);
    }
    ଅନ୍ୟଥା
    {
        // Place a new sign
        ଯଦି (sign_name == ଖାଲି || buf == ଖାଲି ||
            (group != ଖାଲି && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            ଫେରନ୍ତୁ;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା id,
                 char_u *group)
{
    ଯଦି (lnum >= 0 || sign_name != ଖାଲି || (group != ଖାଲି && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        ଫେରନ୍ତୁ;
    }

    ଯଦି (id == -2)
    {
        ଯଦି (buf != ଖାଲି)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        ଅନ୍ୟଥା
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ଯଦି (buf->b_signlist != ଖାଲି)
                    buf_delete_signs(buf, group);
            }
        }
    }
    ଅନ୍ୟଥା
    {
        ଯଦି (buf != ଖାଲି)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        ଅନ୍ୟଥା
        {
            ଯଦି (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            ଅନ୍ୟଥା
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
ସ୍ଥିର ଶୂନ୍ୟ
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              ପୂର୍ଣ୍ଣସଂଖ୍ୟା id,
              char_u *group)
{
    ଯଦି (sign_name == ଖାଲି && group == ଖାଲି && id == -1)
    {
        emsg(_(e_argument_required));
        ଫେରନ୍ତୁ;
    }

    ଯଦି (buf == ଖାଲି || (group != ଖାଲି && *group == '\0') || lnum >= 0 ||
        sign_name != ଖାଲି)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        ଫେରନ୍ତୁ;
    }

    (ଶୂନ୍ୟ)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_sign_cmd_args(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd,
                    char_u *arg,
                    char_u **sign_name,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *signid,
                    char_u **group,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum_arg = FALSE;

    // first arg could be placed sign id
    ଯଦି (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ଯଦି (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ଅନ୍ୟଥା
        {
            arg = skipwhite(arg);
        }
    }

    ଯେତେବେଳେ (*arg != NUL)
    {
        ଯଦି (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((ଅକ୍ଷର *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ଯଦି (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                ଫେରନ୍ତୁ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ଯଦି (*arg != NUL)
                *arg++ = NUL;

            ଯେତେବେଳେ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            ଯଦି (*arg != NUL)
                *arg++ = NUL;
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((ଅକ୍ଷର *)arg);
            arg = skiptowhite(arg);
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ଭାଙ୍ଗ;
        }
        ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getdigits(&arg));

            ଯଦି (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ଭାଙ୍ଗ;
        }
        ଅନ୍ୟଥା
        {
            emsg(_(e_invalid_argument));
            ଫେରନ୍ତୁ FAIL;
        }

        arg = skipwhite(arg);
    }

    ଯଦି (filename != ଖାଲି && *buf == ଖାଲି)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        ଫେରନ୍ତୁ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ଯଦି (filename == ଖାଲି &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    ଫେରନ୍ତୁ OK;
}

/*
 * ":sign" command
 */
ଶୂନ୍ୟ
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = sign_cmd_idx(arg, p);
    ଯଦି (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ଫେରନ୍ତୁ;
    }
    arg = skipwhite(p);

    ଯଦି (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = -1;
        char_u *group = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = -1;
        buf_T *buf = ଖାଲି;
        linenr_T lnum = -1;

        // Parse command line arguments
        ଯଦି (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            ଫେରନ୍ତୁ;

        ଯଦି (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        ଅନ୍ୟଥା ଯଦି (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        ଅନ୍ୟଥା ଯଦି (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        ଫେରନ୍ତୁ;
    }

    // Define, undefine or list signs.
    ଯଦି (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        ପାଇଁ (sign_T *sp = first_sign; sp != ଖାଲି && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    ଅନ୍ୟଥା ଯଦି (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ଅନ୍ୟଥା
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ଯଦି (*p != NUL)
            *p++ = NUL;

        ଯେତେବେଳେ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ଯଦି (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ଅନ୍ୟଥା ଯଦି (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ଅନ୍ୟଥା
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    ଯଦି (sp->sn_icon != ଖାଲି)
        dict_add_string(retdict, "icon", sp->sn_icon);

    ଯଦି (sp->sn_text != ଖାଲି)
        dict_add_string(retdict, "text", sp->sn_text);

    ଯଦି (sp->sn_priority > 0)
        dict_add_ସଂଖ୍ୟା(retdict, "priority", sp->sn_priority);

    ଯଦି (sp->sn_ଧାଡ଼ି_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_ଧାଡ଼ି_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ଅନ୍ୟଥା
            dict_add_string(retdict, "linehl", p);
    }

    ଯଦି (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_text_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ଅନ୍ୟଥା
            dict_add_string(retdict, "texthl", p);
    }

    ଯଦି (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_cul_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ଅନ୍ୟଥା
            dict_add_string(retdict, "culhl", p);
    }

    ଯଦି (sp->sn_ଅଙ୍କ_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_ଅଙ୍କ_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ଅନ୍ୟଥା
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    ଯଦି (name != ଖାଲି)
    {
        sp = sign_find(name, ଖାଲି);
        ଯଦି (sp == ଖାଲି)
            ଫେରନ୍ତୁ;
    }

    ପାଇଁ (; sp != ଖାଲି && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ଯଦି (dict == ଖାଲି)
            ଫେରନ୍ତୁ;

        ଯଦି (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ଫେରନ୍ତୁ;
        }

        sign_getinfo(sp, dict);

        ଯଦି (name != ଖାଲି) // handle only the specified sign
            ଭାଙ୍ଗ;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
ଶୂନ୍ୟ
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = ଖାଲି;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ଯଦି (d != ଖାଲି && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ଯଦି (d == ଖାଲି)
        ଫେରନ୍ତୁ;

    ଯଦି (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ଫେରନ୍ତୁ;
    }

    dict_add_ସଂଖ୍ୟା(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ଯଦି (l == ଖାଲି)
        ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ଫେରନ୍ତୁ;
    }

    sign_entry_T *sign = ଖାଲି;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ଯଦି (!sign_in_group(sign, sign_group))
            ଜାରି;

        ଯଦି ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ଯଦି (sdict != ଖାଲି && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    ଯଦି (buf != ଖାଲି)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    ଅନ୍ୟଥା
    {
        FOR_ALL_BUFFERS(buf)
        {
            ଯଦି (buf->b_signlist != ଖାଲି)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# ଯଦି defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
ଶୂନ୍ୟ
sign_gui_started(ଶୂନ୍ୟ)
{
    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
    {
        ଯଦି (sp->sn_icon != ଖାଲି)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_list_defined(sign_T *sp)
{
    ଅକ୍ଷର lbuf[ସନ୍ଦେଶ_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ଯଦି (sp->sn_icon != ଖାଲି)
    {
        ସନ୍ଦେଶ_puts(" icon=");
        ସନ୍ଦେଶ_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ଯଦି (sp->sn_image == ଖାଲି)
            ସନ୍ଦେଶ_puts(_(" (NOT FOUND)"));
# ଅନ୍ୟଥା
        ସନ୍ଦେଶ_puts(_(" (not supported)"));
# endif
    }

    ଯଦି (sp->sn_text != ଖାଲି)
    {
        ସନ୍ଦେଶ_puts(" text=");
        ସନ୍ଦେଶ_outtrans(sp->sn_text);
    }

    ଯଦି (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, ସନ୍ଦେଶ_BUF_LEN, " priority=%d", sp->sn_priority);
        ସନ୍ଦେଶ_puts(lbuf);
    }

    ଯଦି (sp->sn_ଧାଡ଼ି_hl > 0)
    {
        ସନ୍ଦେଶ_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_ଧାଡ଼ି_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            ସନ୍ଦେଶ_puts("NONE");
        ଅନ୍ୟଥା
            ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
    }

    ଯଦି (sp->sn_text_hl > 0)
    {
        ସନ୍ଦେଶ_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_text_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            ସନ୍ଦେଶ_puts("NONE");
        ଅନ୍ୟଥା
            ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
    }

    ଯଦି (sp->sn_cul_hl > 0)
    {
        ସନ୍ଦେଶ_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_cul_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            ସନ୍ଦେଶ_puts("NONE");
        ଅନ୍ୟଥା
            ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
    }

    ଯଦି (sp->sn_ଅଙ୍କ_hl > 0)
    {
        ସନ୍ଦେଶ_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ଖାଲି, sp->sn_ଅଙ୍କ_hl - 1, FALSE);
        ଯଦି (p == ଖାଲି)
            ସନ୍ଦେଶ_puts("NONE");
        ଅନ୍ୟଥା
            ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ଯଦି (sp->sn_image != ଖାଲି)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    ଯଦି (sp_prev == ଖାଲି)
        first_sign = sp->sn_next;
    ଅନ୍ୟଥା
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# ଯଦି defined(FEAT_SIGN_ICONS)
ଶୂନ୍ୟ *
sign_get_image(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr) // the attribute which may have a sign
{
    sign_T *sp = ଖାଲି;
    FOR_ALL_SIGNS(sp)
    {
        ଯଦି (sp->sn_typenr == typenr)
            ଫେରନ୍ତୁ sp->sn_image;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}
# endif

/*
 * Undefine/free all signs.
 */
ଶୂନ୍ୟ
free_signs(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (first_sign != ଖାଲି)
        sign_undefine(first_sign, ଖାଲି);
}

ସ୍ଥିର ଗଣନା
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
ସ୍ଥିର char_u *
get_nth_sign_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_idx = 0;
    sign_T *sp = ଖାଲି;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ଯଦି (current_idx++ == idx)
            ଫେରନ୍ତୁ sp->sn_name;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ସ୍ଥିର char_u *
get_nth_sign_group_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sg_table.ht_used;
    hashitem_T *hi = ଖାଲି;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&sg_table, hi, todo)
    {
        ଯଦି (!HASHITEM_EMPTY(hi))
        {
            --todo;
            ଯଦି (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                ଫେରନ୍ତୁ group->sg_name;
            }
        }
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ବାଛ (expand_what)
    {
        ସ୍ଥିତି EXP_SUBCMD:
            ଫେରନ୍ତୁ (char_u *)cmds[idx];
        ସ୍ଥିତି EXP_DEFINE:
        {
            ଅକ୍ଷର *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ଖାଲି };
            ଫେରନ୍ତୁ (char_u *)define_arg[idx];
        }
        ସ୍ଥିତି EXP_PLACE:
        {
            ଅକ୍ଷର *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ଖାଲି };
            ଫେରନ୍ତୁ (char_u *)place_arg[idx];
        }
        ସ୍ଥିତି EXP_LIST:
        {
            ଅକ୍ଷର *list_arg[] = { "group=", "file=", "buffer=", ଖାଲି };
            ଫେରନ୍ତୁ (char_u *)list_arg[idx];
        }
        ସ୍ଥିତି EXP_UNPLACE:
        {
            ଅକ୍ଷର *unplace_arg[] = { "group=", "file=", "buffer=", ଖାଲି };
            ଫେରନ୍ତୁ (char_u *)unplace_arg[idx];
        }
        ସ୍ଥିତି EXP_SIGN_NAMES:
            ଫେରନ୍ତୁ get_nth_sign_name(idx);
        ସ୍ଥିତି EXP_SIGN_GROUPS:
            ଫେରନ୍ତୁ get_nth_sign_group_name(idx);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Handle command line completion for :sign command.
 */
ଶୂନ୍ୟ
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ଯଦି (*end_subcmd == NUL)
        ଫେରନ୍ତୁ;

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
    କର
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    ଯେତେବେଳେ (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ଯଦି (p == ଖାଲି)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        ବାଛ (cmd_idx)
        {
            ସ୍ଥିତି SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି SIGNCMD_PLACE:
                // List placed signs
                ଯଦି (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                ଅନ୍ୟଥା
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି SIGNCMD_LIST:
            ସ୍ଥିତି SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି SIGNCMD_JUMP:
            ସ୍ଥିତି SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ଅନ୍ୟଥା
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        ବାଛ (cmd_idx)
        {
            ସ୍ଥିତି SIGNCMD_DEFINE:
                ଯଦି (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ଅନ୍ୟଥା ଯଦି (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ଅନ୍ୟଥା
                    xp->xp_context = EXPAND_NOTHING;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି SIGNCMD_PLACE:
                ଯଦି (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                ଅନ୍ୟଥା ଯଦି (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ଅନ୍ୟଥା ଯଦି (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ଅନ୍ୟଥା
                    xp->xp_context = EXPAND_NOTHING;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି SIGNCMD_UNPLACE:
            ସ୍ଥିତି SIGNCMD_JUMP:
                ଯଦି (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ଅନ୍ୟଥା ଯଦି (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ଅନ୍ୟଥା
                    xp->xp_context = EXPAND_NOTHING;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ଖାଲି;
    char_u *icon = ଖାଲି;
    char_u *linehl = ଖାଲି;
    char_u *text = ଖାଲି;
    char_u *texthl = ଖାଲି;
    char_u *culhl = ଖାଲି;
    char_u *numhl = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;

    ଯଦି (name_arg == ଖାଲି && dict == ଖାଲି)
        ଫେରନ୍ତୁ retval;

    ଯଦି (name_arg == ଖାଲି)
        name = dict_get_string(dict, "name", TRUE);
    ଅନ୍ୟଥା
        name = vim_strsave(name_arg);

    ଯଦି (name == ଖାଲି || name[0] == NUL)
        ଯାଅ cleanup;

    ଯଦି (dict != ଖାଲି)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_ସଂଖ୍ୟା_def(dict, "priority", -1);
    }

    ଯଦି (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    ଫେରନ୍ତୁ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ଖାଲି;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;

        ଯଦି (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ଖାଲି, li->li_tv.vval.v_dict);
        ଅନ୍ୟଥା
            emsg(_(e_dictionary_required));

        list_append_ସଂଖ୍ୟା(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
ଶୂନ୍ୟ
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ଯଦି (rettv_list_alloc(rettv) == FAIL)
            ଫେରନ୍ତୁ;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ଫେରନ୍ତୁ;
    }

    // Define a single sign
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ଯଦି (name == ଖାଲି)
        ଫେରନ୍ତୁ;

    ଯଦି (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ଖାଲି);
}

/*
 * "sign_getdefined()" function
 */
ଶୂନ୍ୟ
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    char_u *name = ଖାଲି;
    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
ଶୂନ୍ୟ
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ଖାଲି;
    linenr_T lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id = 0;
    char_u *group = ଖାଲି;

    ଯଦି (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ଯଦି (buf == ଖାଲି)
            ଫେରନ୍ତୁ;

        ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
        {
            ଯଦି (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ଫେରନ୍ତୁ;

            dictitem_T *di = ଖାଲି;
            dict_T *dict = argvars[1].vval.v_dict;

            ଯଦି ((di = dict_find(dict, (char_u *)"lnum", -1)) != ଖାଲି)
            {
                // get signs placed at this line
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା notanum = FALSE;
                tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &notanum);
                ଯଦି (notanum)
                    ଫେରନ୍ତୁ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ଯଦି ((di = dict_find(dict, (char_u *)"id", -1)) != ଖାଲି)
            {
                // get sign placed with this identifier
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା notanum = FALSE;
                sign_id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &notanum);
                ଯଦି (notanum)
                    ଫେରନ୍ତୁ;
            }

            ଯଦି ((di = dict_find(dict, (char_u *)"group", -1)) != ଖାଲି)
            {
                group = tv_get_string_chk(&di->di_tv);
                ଯଦି (group == ଖାଲି)
                    ଫେରନ୍ତୁ;

                ଯଦି (*group == '\0') // empty string means global group
                    group = ଖାଲି;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
ଶୂନ୍ୟ
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = ଖାଲି;
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script() && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା notanum = FALSE;
    // Sign identifier
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &notanum);
    ଯଦି (notanum)
        ଫେରନ୍ତୁ;

    ଯଦି (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        ଫେରନ୍ତୁ;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    ଯଦି (sign_group == ଖାଲି)
        ଫେରନ୍ତୁ;

    ଯଦି (sign_group[0] == '\0')
    {
        sign_group = ଖାଲି; // global sign group
    }
    ଅନ୍ୟଥା
    {
        sign_group = vim_strsave(sign_group);
        ଯଦି (sign_group == ଖାଲି)
            ଫେରନ୍ତୁ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ଯଦି (buf == ଖାଲି)
        ଯାଅ cleanup;

    rettv->vval.v_ସଂଖ୍ୟା = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id = 0;
    char_u *group = ଖାଲି;
    char_u *sign_name = ଖାଲି;
    buf_T *buf = ଖାଲି;
    dictitem_T *di = ଖାଲି;
    linenr_T lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_sign_id = -1;

    // sign identifier
    ଯଦି (id_tv == ଖାଲି)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        ଯଦି (di != ଖାଲି)
            id_tv = &di->di_tv;
    }

    ଯଦି (id_tv == ଖାଲି)
    {
        sign_id = 0;
    }
    ଅନ୍ୟଥା
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା notanum = FALSE;
        sign_id = tv_get_ସଂଖ୍ୟା_chk(id_tv, &notanum);

        ଯଦି (notanum)
            ଫେରନ୍ତୁ -1;

        ଯଦି (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            ଫେରନ୍ତୁ -1;
        }
    }

    // sign group
    ଯଦି (group_tv == ଖାଲି)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        ଯଦି (di != ଖାଲି)
            group_tv = &di->di_tv;
    }

    ଯଦି (group_tv == ଖାଲି)
    {
        group = ଖାଲି; // global group
    }
    ଅନ୍ୟଥା
    {
        group = tv_get_string_chk(group_tv);
        ଯଦି (group == ଖାଲି)
            ଯାଅ cleanup;

        ଯଦି (group[0] == '\0') // global sign group
        {
            group = ଖାଲି;
        }
        ଅନ୍ୟଥା
        {
            group = vim_strsave(group);
            ଯଦି (group == ଖାଲି)
                ଫେରନ୍ତୁ -1;
        }
    }

    // sign name
    ଯଦି (name_tv == ଖାଲି)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        ଯଦି (di != ଖାଲି)
            name_tv = &di->di_tv;
    }

    ଯଦି (name_tv == ଖାଲି)
        ଯାଅ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ଯଦି (sign_name == ଖାଲି)
        ଯାଅ cleanup;

    // buffer to place the sign
    ଯଦି (buf_tv == ଖାଲି)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        ଯଦି (di != ଖାଲି)
            buf_tv = &di->di_tv;
    }

    ଯଦି (buf_tv == ଖାଲି)
        ଯାଅ cleanup;

    buf = get_buf_arg(buf_tv);
    ଯଦି (buf == ଖାଲି)
        ଯାଅ cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    ଯଦି (di != ଖାଲି)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ଯଦି (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            ଯାଅ cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    ଯଦି (di != ଖାଲି)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା notanum = FALSE;
        prio = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&di->di_tv, &notanum);
        ଯଦି (notanum)
            ଯାଅ cleanup;
    }

    ଯଦି (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    ଫେରନ୍ତୁ ret_sign_id;
}

/*
 * "sign_place()" function
 */
ଶୂନ୍ୟ
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ଖାଲି;
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script() && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ଫେରନ୍ତୁ;

    ଯଦି (argvars[4].v_type != VAR_UNKNOWN)
    {
        ଯଦି (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ଫେରନ୍ତୁ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_ସଂଖ୍ୟା = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
ଶୂନ୍ୟ
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    // Process the List of sign attributes
    listitem_T *li = ଖାଲି;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvars[0].vval.v_list, li)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id = -1;

        ଯଦି (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି,
                                           li->li_tv.vval.v_dict);
        ଅନ୍ୟଥା
            emsg(_(e_dictionary_required));

        list_append_ସଂଖ୍ୟା(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ସ୍ଥିର ଶୂନ୍ୟ
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = ଖାଲି;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ଯଦି (name != ଖାଲି && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_ସଂଖ୍ୟା(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
ଶୂନ୍ୟ
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    ଯଦି (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ଯଦି (rettv_list_alloc(rettv) == FAIL)
            ଫେରନ୍ତୁ;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ଫେରନ୍ତୁ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ଯଦି (name == ଖାଲି)
            ଫେରନ୍ତୁ;

        ଯଦି (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_id = 0;
    buf_T *buf = ଖାଲି;
    char_u *group = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;

    // sign group
    group = (group_tv != ଖାଲି) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    ଯଦି (group != ଖାଲି)
    {
        ଯଦି (group[0] == '\0') // global sign group
        {
            group = ଖାଲି;
        }
        ଅନ୍ୟଥା
        {
            group = vim_strsave(group);
            ଯଦି (group == ଖାଲି)
                ଫେରନ୍ତୁ retval;
        }
    }

    ଯଦି (dict != ଖାଲି)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        ଯଦି (di != ଖାଲି)
        {
            buf = get_buf_arg(&di->di_tv);
            ଯଦି (buf == ଖାଲି)
                ଯାଅ cleanup;
        }

        ଯଦି (dict_has_ଚାବି(dict, "id"))
        {
            sign_id = dict_get_ସଂଖ୍ୟା(dict, "id");
            ଯଦି (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                ଯାଅ cleanup;
            }
        }
    }

    ଯଦି (buf == ଖାଲି)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ଯଦି (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    ଅନ୍ୟଥା ଯଦି (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    ଫେରନ୍ତୁ retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    ଯେତେବେଳେ (sign != ଖାଲି && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    ଫେରନ୍ତୁ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ଯଦି (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ଫେରନ୍ତୁ get_first_valid_sign(wp) != ଖାଲି && !wp->w_p_nu && !wp->w_p_rnu;

    ଯଦି (*wp->w_p_scl == 'n')
        ଫେରନ୍ତୁ FALSE;

    ଯଦି (*wp->w_p_scl == 'y')
        ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ (get_first_valid_sign(wp) != ଖାଲି
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
ଶୂନ୍ୟ
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ଖାଲି;
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_ସଂଖ୍ୟା = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
ଶୂନ୍ୟ
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
        ଫେରନ୍ତୁ;

    listitem_T *li = ଖାଲି;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvars[0].vval.v_list, li)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = -1;

        ଯଦି (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ଖାଲି, li->li_tv.vval.v_dict);
        ଅନ୍ୟଥା
            emsg(_(e_dictionary_required));

        list_append_ସଂଖ୍ୟା(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
