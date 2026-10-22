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

#include "vim.h"

#ပေႃး defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
typedef sa_nracanaa sign sign_T;

sa_nracanaa sign
{
    sign_T *sn_next; // next sign in list
    puur_naa_nka sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    void *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    puur_naa_nka sn_line_hl; // highlight ID for line
    puur_naa_nka sn_text_hl; // highlight ID for text
    puur_naa_nka sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    puur_naa_nka sn_num_hl; // highlight ID for line number
    puur_naa_nka sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

static sign_T *first_sign = NULL;
static puur_naa_nka next_sign_typenr = 1;

static void sign_list_defined(sign_T *sp);
static void sign_undefine(sign_T *sp, sign_T *sp_prev);

static var_na *cmds[] = { "define",
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
                        NULL
# define SIGNCMD_LAST 6
};

# define FOR_ALL_SIGNS(sp) \
     for ((sp) = first_sign; (sp) != NULL; (sp) = (sp)->sn_next)

static hashtab_T sg_table; // sign group (signgroup_T) hashtable
static puur_naa_nka next_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
void
init_signs(void)
{
    hash_init(&sg_table); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
static signgroup_T *
sign_group_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_table, groupname, hash);
    signgroup_T *group = NULL;

    ပေႃး (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ပေႃး (group == NULL)
            ပွၵ်ႈ NULL;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    ပေႃးဢမ်ႇ
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    ပွၵ်ႈ group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
static void
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    ပေႃး (HASHITEM_EMPTY(hi))
        ပွၵ်ႈ;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    ပေႃး (group->sg_refcount == 0)
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
static puur_naa_nka
sign_in_group(sign_entry_T *sign, char_u *group)
{
    ပွၵ်ႈ ((group != NULL && STRCMP(group, "*") == 0) ||
            (group == NULL && sign->se_group == NULL) ||
            (group != NULL && sign->se_group != NULL &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
static puur_naa_nka
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    puur_naa_nka for_popup = sign->se_group != NULL &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    ပွၵ်ႈ WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
static puur_naa_nka
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    puur_naa_nka id = 1;
    signgroup_T *group = NULL;
    sign_entry_T *sign = NULL;

    ပေႃး (groupname != NULL)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        ပေႃး (HASHITEM_EMPTY(hi))
            ပွၵ်ႈ id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    puur_naa_nka found = FALSE;
    ၶၢဝ်း (!found)
    {
        ပေႃး (group == NULL)
            id = next_sign_id++; // global group
        ပေႃးဢမ်ႇ
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ပေႃး (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                bha_nga;
            }
        }
    }

    ပွၵ်ႈ id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
static void
insert_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            puur_naa_nka id, // sign ID
            char_u *group, // sign group; NULL for global group
            puur_naa_nka prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            puur_naa_nka typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(parimaa_na(sign_entry_T), FALSE, aid_insert_sign);
    ပေႃး (newsign == NULL)
        ပွၵ်ႈ;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ပေႃး (group != NULL)
    {
        newsign->se_group = sign_group_ref(group);
        ပေႃး (newsign->se_group == NULL)
        {
            vim_free(newsign);
            ပွၵ်ႈ;
        }
    }
    ပေႃးဢမ်ႇ
    {
        newsign->se_group = NULL;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    ပေႃး (next != NULL)
        next->se_prev = newsign;

    ပေႃး (prev == NULL)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ပေႃး (buf->b_signlist == NULL)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_line_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ပေႃး (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    ပေႃးဢမ်ႇ
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
static void
insert_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_entry_T *prev, // previous sign entry
                         puur_naa_nka id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         puur_naa_nka prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         puur_naa_nka typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ၶၢဝ်း (prev != NULL && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == NULL) ? buf->b_signlist : prev->se_next;

    insert_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
static sign_T *
find_sign_by_typenr(puur_naa_nka typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        ပေႃး (sp->sn_typenr == typenr)
            ပွၵ်ႈ sp;
    ပွၵ်ႈ NULL;
}

/*
 * Get the name of a sign by its typenr.
 */
static char_u *
sign_typenr2name(puur_naa_nka typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        ပေႃး (sp->sn_typenr == typenr)
            ပွၵ်ႈ sp->sn_name;
    ပွၵ်ႈ (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
static dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ပေႃး (d == NULL)
        ပွၵ်ႈ NULL;

    dict_add_number(d, "id", sign->se_id);
    ပေႃး (sign->se_group == NULL)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    ပေႃးဢမ်ႇ
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_number(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_number(d, "priority", sign->se_priority);

    ပွၵ်ႈ d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
static void
sign_sort_by_prio_on_line(buf_T *buf, sign_entry_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ပေႃး ((sign->se_prev == NULL || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == NULL || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        ပွၵ်ႈ;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    ၶၢဝ်း (p->se_prev != NULL && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    ပေႃး (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        ၶၢဝ်း (p->se_next != NULL && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    ပေႃး (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    ပေႃး (sign->se_prev != NULL)
        sign->se_prev->se_next = sign->se_next;

    ပေႃး (sign->se_next != NULL)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = NULL;
    sign->se_next = NULL;

    // Re-insert 'sign' at the right place
    ပေႃး (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        ပေႃး (sign->se_prev != NULL)
            sign->se_prev->se_next = sign;

        ပေႃး (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ပေႃးဢမ်ႇ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        ပေႃး (sign->se_next != NULL)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
static void
buf_addsign(buf_T *buf, // buffer to store sign in
            puur_naa_nka id, // sign ID
            char_u *groupname, // sign group
            puur_naa_nka prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            puur_naa_nka typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    sign_entry_T *prev = NULL; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ပေႃး (buf->b_sign_finger != NULL && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    for (sign = (prev == NULL) ? buf->b_signlist : prev->se_next; sign != NULL;
         sign = sign->se_next)
    {
        ပေႃး (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            ပွၵ်ႈ;
        }
        ပေႃးဢမ်ႇ ပေႃး (lnum < sign->se_lnum)
        {
            insert_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            ပွၵ်ႈ;
        }
        prev = sign;
    }

    insert_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
static linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     puur_naa_nka markId, // sign ID
                     char_u *group, // sign group
                     puur_naa_nka typenr, // typenr of sign we are adding
                     puur_naa_nka prio) // sign priority
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ပေႃး (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            ပွၵ်ႈ sign->se_lnum;
        }
    }

    ပွၵ်ႈ (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
puur_naa_nka
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ပေႃး (sign->se_lnum > lnum)
            bha_nga;

        ပေႃး (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            ပေႃး (sp == NULL)
                ပွၵ်ႈ FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            ပေႃး (sattr->sat_text != NULL && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            ပေႃး (sp->sn_line_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_line_hl);

            ပေႃး (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ပေႃး (sp->sn_num_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_num_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ပေႃး (sign->se_next != NULL &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                ပေႃး (next_sp == NULL)
                    ပွၵ်ႈ FALSE;

                ပေႃး (sattr->sat_icon == NULL && sattr->sat_text == NULL)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                ပေႃး (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                ပေႃး (sp->sn_line_hl <= 0 && next_sp->sn_line_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_line_hl);

                ပေႃး (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                ပေႃး (sp->sn_num_hl <= 0 && next_sp->sn_num_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_num_hl);
            }
            ပွၵ်ႈ TRUE;
        }
    }
    ပွၵ်ႈ FALSE;
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
            puur_naa_nka id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = NULL; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    for (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;

        ပေႃး ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            ပေႃး (next != NULL)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ပေႃး (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_line_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ပေႃး (group == NULL || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                bha_nga;
        }
        ပေႃးဢမ်ႇ
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ပေႃး (buf->b_signlist == NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    ပွၵ်ႈ lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
puur_naa_nka
buf_findsign(buf_T *buf, // buffer to store sign in
             puur_naa_nka id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        ပေႃး (sign->se_id == id && sign_in_group(sign, group))
            ပွၵ်ႈ sign->se_lnum;

    ပွၵ်ႈ 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
static sign_entry_T *
buf_getsign_at_line(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ပေႃး (sign->se_lnum > lnum)
            bha_nga;

        ပေႃး (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            ပွၵ်ႈ sign;
    }

    ပွၵ်ႈ NULL;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
puur_naa_nka
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_line(buf, lnum, groupname);
    ပေႃး (sign != NULL)
        ပွၵ်ႈ sign->se_id;

    ပွၵ်ႈ 0;
}

# ပေႃး defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
puur_naa_nka
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    puur_naa_nka typenr) // sign type number
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ပေႃး (sign->se_lnum > lnum)
            bha_nga;

        ပေႃး (sign->se_lnum == lnum && sign->se_typenr == typenr)
            ပွၵ်ႈ sign->se_id;
    }

    ပွၵ်ႈ 0;
}

#  ပေႃး defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
puur_naa_nka
buf_signcount(buf_T *buf, linenr_T lnum)
{
    puur_naa_nka count = 0;
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ပေႃး (sign->se_lnum > lnum)
            bha_nga;

        ပေႃး (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != NULL)
            count++;
    }

    ပွၵ်ႈ count;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
void
buf_delete_signs(buf_T *buf, char_u *group)
{
    buf->b_sign_finger = NULL; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ပေႃး (buf->b_signlist != NULL && curwin != NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL;

    for (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;
        ပေႃး (sign_in_group(sign, group))
        {
            *lastp = next;

            ပေႃး (next != NULL)
                next->se_prev = sign->se_prev;

            ပေႃး (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        ပေႃးဢမ်ႇ
        {
            lastp = &sign->se_next;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
static void
sign_list_placed(buf_T *rbuf, char_u *sign_group)
{
    var_na lbuf[MSG_BUF_LEN];
    var_na group[MSG_BUF_LEN];

    msg_puts_title(_("\n--- Signs ---"));
    msg_putchar('\n');

    buf_T *buf = (rbuf == NULL) ? firstbuf : rbuf;
    ၶၢဝ်း (buf != NULL && !got_int)
    {
        ပေႃး (buf->b_signlist != NULL)
        {
            vim_snprintf(lbuf, MSG_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            msg_puts_attr(lbuf, HL_ATTR(HLF_D));
            msg_putchar('\n');
        }

        sign_entry_T *sign = NULL;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ပေႃး (got_int)
                bha_nga;

            ပေႃး (!sign_in_group(sign, sign_group))
                continue;

            ပေႃး (sign->se_group != NULL)
                vim_snprintf(group, MSG_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            ပေႃးဢမ်ႇ
                group[0] = '\0';

            vim_snprintf(lbuf, MSG_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            msg_puts(lbuf);
            msg_putchar('\n');
        }

        ပေႃး (rbuf != NULL)
            bha_nga;

        buf = buf->b_next;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
void
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    long        amount,
    long        amount_after)
{
    curbuf->b_sign_finger = NULL; // line changes may reorder, drop the finger
    sign_entry_T *sign = NULL; // a sign in a b_signlist
    FOR_ALL_SIGNS_IN_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ပေႃး (sign->se_lnum < line1)
            continue;

        linenr_T new_lnum = sign->se_lnum;

        ပေႃး (sign->se_lnum <= line2)
        {
            ပေႃး (amount != MAXLNUM)
                new_lnum += amount;
        }
        ပေႃးဢမ်ႇ ပေႃး (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ပေႃး (new_lnum <= curbuf->b_ml.ml_line_count)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
static puur_naa_nka
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    puur_naa_nka idx = 0;
    var_na save = *end_cmd;
    *end_cmd = NUL;

    ၶၢဝ်း (cmds[idx] != NULL && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    ပွၵ်ႈ idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
static sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    ပေႃး (sp_prev != NULL)
        *sp_prev = NULL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ပေႃး (STRCMP(sp->sn_name, name) == 0)
            bha_nga;

        ပေႃး (sp_prev != NULL)
            *sp_prev = sp;
    }

    ပွၵ်ႈ sp;
}

/*
 * Allocate a new sign
 */
static sign_T *
alloc_new_sign(char_u *name)
{
    puur_naa_nka start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(parimaa_na(sign_T), aid_sign_define_by_name);
    ပေႃး (sp == NULL)
        ပွၵ်ႈ NULL;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    ၶၢဝ်း (lp != NULL)
    {
        ပေႃး (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            ပေႃး (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            ပေႃး (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                ပွၵ်ႈ NULL;
            }

            lp = first_sign; // start all over
            continue;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    ပေႃး (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ပေႃး (sp->sn_name == NULL) // out of memory
    {
        vim_free(sp);
        ပွၵ်ႈ NULL;
    }

    ပွၵ်ႈ sp;
}

/*
 * Initialize the icon information for a new sign
 */
static void
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ပေႃး (gui.in_use)
    {
        out_flush();
        ပေႃး (sp->sn_image != NULL)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
static puur_naa_nka
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = NULL;
    char_u *endp = text + (puur_naa_nka)STRLEN(text);
    puur_naa_nka cells = 0;

    // Remove backslashes so that it is possible to use a space.
    for (s = text; s + 1 < endp; ++s)
    {
        ပေႃး (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ပေႃး (has_mbyte)
    {
        for (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            ပေႃး (!vim_isprintc((*mb_ptr2char)(s)))
                bha_nga;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ပေႃးဢမ်ႇ
    {
        for (s = text; s < endp; ++s)
        {
            ပေႃး (!vim_isprintc(*s))
                bha_nga;
        }
        cells = (puur_naa_nka)(s - text);
    }

    // Currently sign text must be one or two display cells
    ပေႃး (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        ပွၵ်ႈ FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    puur_naa_nka len = (puur_naa_nka)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    ပေႃး (sp->sn_text != NULL && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    ပွၵ်ႈ OK;
}

/*
 * Define a new sign or update an existing sign
 */
puur_naa_nka
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    puur_naa_nka prio)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    ပေႃး (sp == NULL)
    {
        sp = alloc_new_sign(name);
        ပေႃး (sp == NULL)
            ပွၵ်ႈ FAIL;

        // add the new sign to the list of signs
        ပေႃး (sp_prev == NULL)
            first_sign = sp;
        ပေႃးဢမ်ႇ
            sp_prev->sn_next = sp;
    }
    ပေႃးဢမ်ႇ
    {
        win_T *wp = NULL;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ပေႃး (wp->w_buffer->b_signlist != NULL)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ပေႃး (icon != NULL)
        sign_define_init_icon(sp, icon);

    ပေႃး (text != NULL && (sign_define_init_text(sp, text) == FAIL))
        ပွၵ်ႈ FAIL;

    sp->sn_priority = prio;

    ပေႃး (linehl != NULL)
    {
        ပေႃး (*linehl == NUL)
            sp->sn_line_hl = 0;
        ပေႃးဢမ်ႇ
            sp->sn_line_hl = syn_check_group(linehl, (puur_naa_nka)STRLEN(linehl));
    }

    ပေႃး (texthl != NULL)
    {
        ပေႃး (*texthl == NUL)
            sp->sn_text_hl = 0;
        ပေႃးဢမ်ႇ
            sp->sn_text_hl = syn_check_group(texthl, (puur_naa_nka)STRLEN(texthl));
    }

    ပေႃး (culhl != NULL)
    {
        ပေႃး (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ပေႃးဢမ်ႇ
            sp->sn_cul_hl = syn_check_group(culhl, (puur_naa_nka)STRLEN(culhl));
    }

    ပေႃး (numhl != NULL)
    {
        ပေႃး (*numhl == NUL)
            sp->sn_num_hl = 0;
        ပေႃးဢမ်ႇ
            sp->sn_num_hl = syn_check_group(numhl, (puur_naa_nka)STRLEN(numhl));
    }

    ပွၵ်ႈ OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
puur_naa_nka
sign_exists_by_name(char_u *name)
{
    ပွၵ်ႈ sign_find(name, NULL) != NULL;
}

/*
 * Free the sign specified by 'name'.
 */
puur_naa_nka
sign_undefine_by_name(char_u *name, puur_naa_nka give_error)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    ပေႃး (sp == NULL)
    {
        ပေႃး (give_error)
            semsg(_(e_unknown_sign_str), name);
        ပွၵ်ႈ FAIL;
    }
    sign_undefine(sp, sp_prev);

    ပွၵ်ႈ OK;
}

/*
 * List the signs matching 'name'
 */
static void
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, NULL);
    ပေႃး (sp != NULL)
        sign_list_defined(sp);
    ပေႃးဢမ်ႇ
        semsg(_(e_unknown_sign_str), name);
}

static void
may_force_numberwidth_recompute(buf_T *buf, puur_naa_nka unplace)
{
# ပေႃး defined(FEAT_LINEBREAK)
    tabpage_T *tp = NULL;
    win_T *wp = NULL;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ပေႃး (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_line_count = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
puur_naa_nka
sign_place(puur_naa_nka *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           puur_naa_nka prio)
{
    // Check for reserved character '*' in group name
    ပေႃး (sign_group != NULL && (*sign_group == '*' || *sign_group == '\0'))
        ပွၵ်ႈ FAIL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ပေႃး (STRCMP(sp->sn_name, sign_name) == 0)
            bha_nga;
    }

    ပေႃး (sp == NULL)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        ပွၵ်ႈ FAIL;
    }

    ပေႃး (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    ပေႃး (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    ပေႃး (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    ပေႃးဢမ်ႇ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    ပေႃး (lnum > 0)
    {
        redraw_buf_line_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ပေႃးဢမ်ႇ
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        ပွၵ်ႈ FAIL;
    }

    ပွၵ်ႈ OK;
}

/*
 * Unplace the specified sign
 */
static puur_naa_nka
sign_unplace(puur_naa_nka sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    ပေႃး (buf->b_signlist == NULL) // No signs in the buffer
        ပွၵ်ႈ OK;

    ပေႃး (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    ပေႃးဢမ်ႇ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        ပေႃး (lnum == 0)
            ပွၵ်ႈ FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ပေႃး (buf->b_signlist == NULL)
        may_force_numberwidth_recompute(buf, TRUE);

    ပွၵ်ႈ OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
static void
sign_unplace_at_cursor(char_u *groupname)
{
    puur_naa_nka id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    ပေႃး (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    ပေႃးဢမ်ႇ
        emsg(_(e_missing_sign_number));
}

/*
 * Jump to a sign.
 */
static linenr_T
sign_jump(puur_naa_nka sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    ပေႃး (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        ပွၵ်ႈ -1;
    }

    // goto a sign ...
    ပေႃး (buf_jump_open_win(buf) != NULL)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ပေႃးဢမ်ႇ
    { // ... not currently in a window
        ပေႃး (buf->b_fname == NULL)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            ပွၵ်ႈ -1;
        }
        setpcmark();
        ပေႃး (do_ecmd(buf->b_fnum, NULL, NULL, NULL, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            ပွၵ်ႈ -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    ပွၵ်ႈ lnum;
}

/*
 * ":sign define {name} ..." command
 */
static void
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = NULL;
    char_u *p = cmdline;
    char_u *icon = NULL;
    char_u *text = NULL;
    char_u *linehl = NULL;
    char_u *texthl = NULL;
    char_u *culhl = NULL;
    char_u *numhl = NULL;
    puur_naa_nka prio = -1;
    puur_naa_nka failed = FALSE;

    // set values for a defined sign.
    ၶၢဝ်း (TRUE)
    {
        arg = skipwhite(p);
        ပေႃး (*arg == NUL)
            bha_nga;

        p = skiptowhite_esc(arg);
        ပေႃး (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((var_na *)arg);
        }
        ပေႃးဢမ်ႇ
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            bha_nga;
        }
    }

    ပေႃး (!failed)
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
static void
sign_place_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               puur_naa_nka id,
               char_u *group,
               puur_naa_nka prio)
{
    ပေႃး (id <= 0)
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
        ပေႃး (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
            emsg(_(e_invalid_argument));
        ပေႃးဢမ်ႇ
            sign_list_placed(buf, group);
    }
    ပေႃးဢမ်ႇ
    {
        // Place a new sign
        ပေႃး (sign_name == NULL || buf == NULL ||
            (group != NULL && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            ပွၵ်ႈ;
        }

        sign_place(&id, group, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
static void
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 puur_naa_nka id,
                 char_u *group)
{
    ပေႃး (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        ပွၵ်ႈ;
    }

    ပေႃး (id == -2)
    {
        ပေႃး (buf != NULL)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        ပေႃးဢမ်ႇ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ပေႃး (buf->b_signlist != NULL)
                    buf_delete_signs(buf, group);
            }
        }
    }
    ပေႃးဢမ်ႇ
    {
        ပေႃး (buf != NULL)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        ပေႃးဢမ်ႇ
        {
            ပေႃး (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            ပေႃးဢမ်ႇ
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
static void
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              puur_naa_nka id,
              char_u *group)
{
    ပေႃး (sign_name == NULL && group == NULL && id == -1)
    {
        emsg(_(e_argument_required));
        ပွၵ်ႈ;
    }

    ပေႃး (buf == NULL || (group != NULL && *group == '\0') || lnum >= 0 ||
        sign_name != NULL)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        ပွၵ်ႈ;
    }

    (void)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
static puur_naa_nka
parse_sign_cmd_args(puur_naa_nka cmd,
                    char_u *arg,
                    char_u **sign_name,
                    puur_naa_nka *signid,
                    char_u **group,
                    puur_naa_nka *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = NULL;
    puur_naa_nka lnum_arg = FALSE;

    // first arg could be placed sign id
    ပေႃး (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ပေႃး (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ပေႃးဢမ်ႇ
        {
            arg = skipwhite(arg);
        }
    }

    ၶၢဝ်း (*arg != NUL)
    {
        ပေႃး (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((var_na *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ပေႃး (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                ပွၵ်ႈ FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ပေႃး (*arg != NUL)
                *arg++ = NUL;

            ၶၢဝ်း (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            ပေႃး (*arg != NUL)
                *arg++ = NUL;
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((var_na *)arg);
            arg = skiptowhite(arg);
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            bha_nga;
        }
        ပေႃးဢမ်ႇ ပေႃး (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((puur_naa_nka)getdigits(&arg));

            ပေႃး (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            bha_nga;
        }
        ပေႃးဢမ်ႇ
        {
            emsg(_(e_invalid_argument));
            ပွၵ်ႈ FAIL;
        }

        arg = skipwhite(arg);
    }

    ပေႃး (filename != NULL && *buf == NULL)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        ပွၵ်ႈ FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ပေႃး (filename == NULL &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    ပွၵ်ႈ OK;
}

/*
 * ":sign" command
 */
void
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    puur_naa_nka idx = sign_cmd_idx(arg, p);
    ပေႃး (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        ပွၵ်ႈ;
    }
    arg = skipwhite(p);

    ပေႃး (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = NULL;
        puur_naa_nka id = -1;
        char_u *group = NULL;
        puur_naa_nka prio = -1;
        buf_T *buf = NULL;
        linenr_T lnum = -1;

        // Parse command line arguments
        ပေႃး (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            ပွၵ်ႈ;

        ပေႃး (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        ပေႃးဢမ်ႇ ပေႃး (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        ပေႃးဢမ်ႇ ပေႃး (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        ပွၵ်ႈ;
    }

    // Define, undefine or list signs.
    ပေႃး (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        for (sign_T *sp = first_sign; sp != NULL && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    ပေႃးဢမ်ႇ ပေႃး (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ပေႃးဢမ်ႇ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ပေႃး (*p != NUL)
            *p++ = NUL;

        ၶၢဝ်း (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ပေႃး (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ပေႃးဢမ်ႇ ပေႃး (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ပေႃးဢမ်ႇ
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
static void
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_add_string(retdict, "name", sp->sn_name);

    ပေႃး (sp->sn_icon != NULL)
        dict_add_string(retdict, "icon", sp->sn_icon);

    ပေႃး (sp->sn_text != NULL)
        dict_add_string(retdict, "text", sp->sn_text);

    ပေႃး (sp->sn_priority > 0)
        dict_add_number(retdict, "priority", sp->sn_priority);

    ပေႃး (sp->sn_line_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        ပေႃး (p == NULL)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ပေႃးဢမ်ႇ
            dict_add_string(retdict, "linehl", p);
    }

    ပေႃး (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        ပေႃး (p == NULL)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ပေႃးဢမ်ႇ
            dict_add_string(retdict, "texthl", p);
    }

    ပေႃး (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        ပေႃး (p == NULL)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ပေႃးဢမ်ႇ
            dict_add_string(retdict, "culhl", p);
    }

    ပေႃး (sp->sn_num_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        ပေႃး (p == NULL)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ပေႃးဢမ်ႇ
            dict_add_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
static void
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = first_sign;

    ပေႃး (name != NULL)
    {
        sp = sign_find(name, NULL);
        ပေႃး (sp == NULL)
            ပွၵ်ႈ;
    }

    for (; sp != NULL && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ပေႃး (dict == NULL)
            ပွၵ်ႈ;

        ပေႃး (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            ပွၵ်ႈ;
        }

        sign_getinfo(sp, dict);

        ပေႃး (name != NULL) // handle only the specified sign
            bha_nga;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
void
get_buffer_signs(buf_T *buf, list_T *l)
{
    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ပေႃး (d != NULL && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
static void
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       puur_naa_nka sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ပေႃး (d == NULL)
        ပွၵ်ႈ;

    ပေႃး (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        ပွၵ်ႈ;
    }

    dict_add_number(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ပေႃး (l == NULL)
        ပွၵ်ႈ;

    ပေႃး (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        ပွၵ်ႈ;
    }

    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ပေႃး (!sign_in_group(sign, sign_group))
            continue;

        ပေႃး ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ပေႃး (sdict != NULL && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
static void
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                puur_naa_nka sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    ပေႃး (buf != NULL)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    ပေႃးဢမ်ႇ
    {
        FOR_ALL_BUFFERS(buf)
        {
            ပေႃး (buf->b_signlist != NULL)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# ပေႃး defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
void
sign_gui_started(void)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ပေႃး (sp->sn_icon != NULL)
            sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
static void
sign_list_defined(sign_T *sp)
{
    var_na lbuf[MSG_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ပေႃး (sp->sn_icon != NULL)
    {
        msg_puts(" icon=");
        msg_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ပေႃး (sp->sn_image == NULL)
            msg_puts(_(" (NOT FOUND)"));
# ပေႃးဢမ်ႇ
        msg_puts(_(" (not supported)"));
# endif
    }

    ပေႃး (sp->sn_text != NULL)
    {
        msg_puts(" text=");
        msg_outtrans(sp->sn_text);
    }

    ပေႃး (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, MSG_BUF_LEN, " priority=%d", sp->sn_priority);
        msg_puts(lbuf);
    }

    ပေႃး (sp->sn_line_hl > 0)
    {
        msg_puts(" linehl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        ပေႃး (p == NULL)
            msg_puts("NONE");
        ပေႃးဢမ်ႇ
            msg_puts((var_na *)p);
    }

    ပေႃး (sp->sn_text_hl > 0)
    {
        msg_puts(" texthl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        ပေႃး (p == NULL)
            msg_puts("NONE");
        ပေႃးဢမ်ႇ
            msg_puts((var_na *)p);
    }

    ပေႃး (sp->sn_cul_hl > 0)
    {
        msg_puts(" culhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        ပေႃး (p == NULL)
            msg_puts("NONE");
        ပေႃးဢမ်ႇ
            msg_puts((var_na *)p);
    }

    ပေႃး (sp->sn_num_hl > 0)
    {
        msg_puts(" numhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        ပေႃး (p == NULL)
            msg_puts("NONE");
        ပေႃးဢမ်ႇ
            msg_puts((var_na *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
static void
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ပေႃး (sp->sn_image != NULL)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    ပေႃး (sp_prev == NULL)
        first_sign = sp->sn_next;
    ပေႃးဢမ်ႇ
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# ပေႃး defined(FEAT_SIGN_ICONS)
void *
sign_get_image(puur_naa_nka typenr) // the attribute which may have a sign
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ပေႃး (sp->sn_typenr == typenr)
            ပွၵ်ႈ sp->sn_image;
    }
    ပွၵ်ႈ NULL;
}
# endif

/*
 * Undefine/free all signs.
 */
void
free_signs(void)
{
    ၶၢဝ်း (first_sign != NULL)
        sign_undefine(first_sign, NULL);
}

static ga_nanaa
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
static char_u *
get_nth_sign_name(puur_naa_nka idx)
{
    puur_naa_nka current_idx = 0;
    sign_T *sp = NULL;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ပေႃး (current_idx++ == idx)
            ပွၵ်ႈ sp->sn_name;
    }
    ပွၵ်ႈ NULL;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
static char_u *
get_nth_sign_group_name(puur_naa_nka idx)
{
    puur_naa_nka current_idx = 0;
    puur_naa_nka todo = (puur_naa_nka)sg_table.ht_used;
    hashitem_T *hi = NULL;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ITEMS(&sg_table, hi, todo)
    {
        ပေႃး (!HASHITEM_EMPTY(hi))
        {
            --todo;
            ပေႃး (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                ပွၵ်ႈ group->sg_name;
            }
        }
    }
    ပွၵ်ႈ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    switch (expand_what)
    {
        pak_sa EXP_SUBCMD:
            ပွၵ်ႈ (char_u *)cmds[idx];
        pak_sa EXP_DEFINE:
        {
            var_na *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", NULL };
            ပွၵ်ႈ (char_u *)define_arg[idx];
        }
        pak_sa EXP_PLACE:
        {
            var_na *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", NULL };
            ပွၵ်ႈ (char_u *)place_arg[idx];
        }
        pak_sa EXP_LIST:
        {
            var_na *list_arg[] = { "group=", "file=", "buffer=", NULL };
            ပွၵ်ႈ (char_u *)list_arg[idx];
        }
        pak_sa EXP_UNPLACE:
        {
            var_na *unplace_arg[] = { "group=", "file=", "buffer=", NULL };
            ပွၵ်ႈ (char_u *)unplace_arg[idx];
        }
        pak_sa EXP_SIGN_NAMES:
            ပွၵ်ႈ get_nth_sign_name(idx);
        pak_sa EXP_SIGN_GROUPS:
            ပွၵ်ႈ get_nth_sign_group_name(idx);
        default:
            ပွၵ်ႈ NULL;
    }
}

/*
 * Handle command line completion for :sign command.
 */
void
set_context_in_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *end_subcmd;
    char_u *last;
    puur_naa_nka cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ပေႃး (*end_subcmd == NUL)
        ပွၵ်ႈ;

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
    do
    {
        p = skipwhite(p);
        last = p;
        p = skiptowhite(p);
    }
    ၶၢဝ်း (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ပေႃး (p == NULL)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        switch (cmd_idx)
        {
            pak_sa SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                bha_nga;
            pak_sa SIGNCMD_PLACE:
                // List placed signs
                ပေႃး (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                ပေႃးဢမ်ႇ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                bha_nga;
            pak_sa SIGNCMD_LIST:
            pak_sa SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                bha_nga;
            pak_sa SIGNCMD_JUMP:
            pak_sa SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                bha_nga;
            default:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ပေႃးဢမ်ႇ
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        switch (cmd_idx)
        {
            pak_sa SIGNCMD_DEFINE:
                ပေႃး (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ပေႃးဢမ်ႇ ပေႃး (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ပေႃးဢမ်ႇ
                    xp->xp_context = EXPAND_NOTHING;
                bha_nga;
            pak_sa SIGNCMD_PLACE:
                ပေႃး (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                ပေႃးဢမ်ႇ ပေႃး (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ပေႃးဢမ်ႇ ပေႃး (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ပေႃးဢမ်ႇ
                    xp->xp_context = EXPAND_NOTHING;
                bha_nga;
            pak_sa SIGNCMD_UNPLACE:
            pak_sa SIGNCMD_JUMP:
                ပေႃး (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ပေႃးဢမ်ႇ ပေႃး (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ပေႃးဢမ်ႇ
                    xp->xp_context = EXPAND_NOTHING;
                bha_nga;
            default:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
static puur_naa_nka
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = NULL;
    char_u *icon = NULL;
    char_u *linehl = NULL;
    char_u *text = NULL;
    char_u *texthl = NULL;
    char_u *culhl = NULL;
    char_u *numhl = NULL;
    puur_naa_nka prio = -1;
    puur_naa_nka retval = -1;

    ပေႃး (name_arg == NULL && dict == NULL)
        ပွၵ်ႈ retval;

    ပေႃး (name_arg == NULL)
        name = dict_get_string(dict, "name", TRUE);
    ပေႃးဢမ်ႇ
        name = vim_strsave(name_arg);

    ပေႃး (name == NULL || name[0] == NUL)
        goto cleanup;

    ပေႃး (dict != NULL)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_number_def(dict, "priority", -1);
    }

    ပေႃး (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    ပွၵ်ႈ retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
static void
sign_define_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(l, li)
    {
        puur_naa_nka retval = -1;

        ပေႃး (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(NULL, li->li_tv.vval.v_dict);
        ပေႃးဢမ်ႇ
            emsg(_(e_dictionary_required));

        list_append_number(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
void
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    ပေႃး (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        ပွၵ်ႈ;

    ပေႃး (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ပေႃး (rettv_list_alloc(rettv) == FAIL)
            ပွၵ်ႈ;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ပွၵ်ႈ;
    }

    // Define a single sign
    rettv->vval.v_number = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ပေႃး (name == NULL)
        ပွၵ်ႈ;

    ပေႃး (check_for_opt_dict_arg(argvars, 1) == FAIL)
        ပွၵ်ႈ;

    rettv->vval.v_number = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : NULL);
}

/*
 * "sign_getdefined()" function
 */
void
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ပေႃး (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    char_u *name = NULL;
    ပေႃး (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
void
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = NULL;
    linenr_T lnum = 0;
    puur_naa_nka sign_id = 0;
    char_u *group = NULL;

    ပေႃး (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        ပွၵ်ႈ;

    ပေႃး (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ပေႃး (buf == NULL)
            ပွၵ်ႈ;

        ပေႃး (argvars[1].v_type != VAR_UNKNOWN)
        {
            ပေႃး (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                ပွၵ်ႈ;

            dictitem_T *di = NULL;
            dict_T *dict = argvars[1].vval.v_dict;

            ပေႃး ((di = dict_find(dict, (char_u *)"lnum", -1)) != NULL)
            {
                // get signs placed at this line
                puur_naa_nka notanum = FALSE;
                tv_get_number_chk(&di->di_tv, &notanum);
                ပေႃး (notanum)
                    ပွၵ်ႈ;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ပေႃး ((di = dict_find(dict, (char_u *)"id", -1)) != NULL)
            {
                // get sign placed with this identifier
                puur_naa_nka notanum = FALSE;
                sign_id = (puur_naa_nka)tv_get_number_chk(&di->di_tv, &notanum);
                ပေႃး (notanum)
                    ပွၵ်ႈ;
            }

            ပေႃး ((di = dict_find(dict, (char_u *)"group", -1)) != NULL)
            {
                group = tv_get_string_chk(&di->di_tv);
                ပေႃး (group == NULL)
                    ပွၵ်ႈ;

                ပေႃး (*group == '\0') // empty string means global group
                    group = NULL;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, group, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
void
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_group = NULL;
    rettv->vval.v_number = -1;

    ပေႃး (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        ပွၵ်ႈ;

    puur_naa_nka notanum = FALSE;
    // Sign identifier
    puur_naa_nka sign_id = (puur_naa_nka)tv_get_number_chk(&argvars[0], &notanum);
    ပေႃး (notanum)
        ပွၵ်ႈ;

    ပေႃး (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        ပွၵ်ႈ;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    ပေႃး (sign_group == NULL)
        ပွၵ်ႈ;

    ပေႃး (sign_group[0] == '\0')
    {
        sign_group = NULL; // global sign group
    }
    ပေႃးဢမ်ႇ
    {
        sign_group = vim_strsave(sign_group);
        ပေႃး (sign_group == NULL)
            ပွၵ်ႈ;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ပေႃး (buf == NULL)
        goto cleanup;

    rettv->vval.v_number = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
static puur_naa_nka
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    puur_naa_nka sign_id = 0;
    char_u *group = NULL;
    char_u *sign_name = NULL;
    buf_T *buf = NULL;
    dictitem_T *di = NULL;
    linenr_T lnum = 0;
    puur_naa_nka prio = -1;
    puur_naa_nka ret_sign_id = -1;

    // sign identifier
    ပေႃး (id_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        ပေႃး (di != NULL)
            id_tv = &di->di_tv;
    }

    ပေႃး (id_tv == NULL)
    {
        sign_id = 0;
    }
    ပေႃးဢမ်ႇ
    {
        puur_naa_nka notanum = FALSE;
        sign_id = tv_get_number_chk(id_tv, &notanum);

        ပေႃး (notanum)
            ပွၵ်ႈ -1;

        ပေႃး (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            ပွၵ်ႈ -1;
        }
    }

    // sign group
    ပေႃး (group_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        ပေႃး (di != NULL)
            group_tv = &di->di_tv;
    }

    ပေႃး (group_tv == NULL)
    {
        group = NULL; // global group
    }
    ပေႃးဢမ်ႇ
    {
        group = tv_get_string_chk(group_tv);
        ပေႃး (group == NULL)
            goto cleanup;

        ပေႃး (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        ပေႃးဢမ်ႇ
        {
            group = vim_strsave(group);
            ပေႃး (group == NULL)
                ပွၵ်ႈ -1;
        }
    }

    // sign name
    ပေႃး (name_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        ပေႃး (di != NULL)
            name_tv = &di->di_tv;
    }

    ပေႃး (name_tv == NULL)
        goto cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ပေႃး (sign_name == NULL)
        goto cleanup;

    // buffer to place the sign
    ပေႃး (buf_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        ပေႃး (di != NULL)
            buf_tv = &di->di_tv;
    }

    ပေႃး (buf_tv == NULL)
        goto cleanup;

    buf = get_buf_arg(buf_tv);
    ပေႃး (buf == NULL)
        goto cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    ပေႃး (di != NULL)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ပေႃး (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            goto cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    ပေႃး (di != NULL)
    {
        puur_naa_nka notanum = FALSE;
        prio = (puur_naa_nka)tv_get_number_chk(&di->di_tv, &notanum);
        ပေႃး (notanum)
            goto cleanup;
    }

    ပေႃး (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    ပွၵ်ႈ ret_sign_id;
}

/*
 * "sign_place()" function
 */
void
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = NULL;
    rettv->vval.v_number = -1;

    ပေႃး (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        ပွၵ်ႈ;

    ပေႃး (argvars[4].v_type != VAR_UNKNOWN)
    {
        ပေႃး (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            ပွၵ်ႈ;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_number = sign_place_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
void
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ပေႃး (rettv_list_alloc(rettv) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (check_for_list_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    // Process the List of sign attributes
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        puur_naa_nka sign_id = -1;

        ပေႃး (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(NULL, NULL, NULL, NULL,
                                           li->li_tv.vval.v_dict);
        ပေႃးဢမ်ႇ
            emsg(_(e_dictionary_required));

        list_append_number(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
static void
sign_undefine_multiple(list_T *l, list_T *retlist)
{
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(l, li)
    {
        puur_naa_nka retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ပေႃး (name != NULL && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_number(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
void
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    ပေႃး (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ပေႃး (rettv_list_alloc(rettv) == FAIL)
            ပွၵ်ႈ;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        ပွၵ်ႈ;
    }

    rettv->vval.v_number = -1;

    ပေႃး (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_number = 0;
    }
    ပေႃးဢမ်ႇ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ပေႃး (name == NULL)
            ပွၵ်ႈ;

        ပေႃး (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_number = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
static puur_naa_nka
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    puur_naa_nka sign_id = 0;
    buf_T *buf = NULL;
    char_u *group = NULL;
    puur_naa_nka retval = -1;

    // sign group
    group = (group_tv != NULL) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    ပေႃး (group != NULL)
    {
        ပေႃး (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        ပေႃးဢမ်ႇ
        {
            group = vim_strsave(group);
            ပေႃး (group == NULL)
                ပွၵ်ႈ retval;
        }
    }

    ပေႃး (dict != NULL)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        ပေႃး (di != NULL)
        {
            buf = get_buf_arg(&di->di_tv);
            ပေႃး (buf == NULL)
                goto cleanup;
        }

        ပေႃး (dict_has_key(dict, "id"))
        {
            sign_id = dict_get_number(dict, "id");
            ပေႃး (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                goto cleanup;
            }
        }
    }

    ပေႃး (buf == NULL)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ပေႃး (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    ပေႃးဢမ်ႇ ပေႃး (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    ပွၵ်ႈ retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    ၶၢဝ်း (sign != NULL && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    ပွၵ်ႈ sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
puur_naa_nka
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ပေႃး (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        ပွၵ်ႈ get_first_valid_sign(wp) != NULL && !wp->w_p_nu && !wp->w_p_rnu;

    ပေႃး (*wp->w_p_scl == 'n')
        ပွၵ်ႈ FALSE;

    ပေႃး (*wp->w_p_scl == 'y')
        ပွၵ်ႈ TRUE;

    ပွၵ်ႈ (get_first_valid_sign(wp) != NULL
# ifdef FEAT_NETBEANS_INTG
            || wp->w_buffer->b_has_sign_column
# endif
    );
}

/*
 * "sign_unplace()" function
 */
void
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = NULL;
    rettv->vval.v_number = -1;

    ပေႃး ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        ပွၵ်ႈ;

    ပေႃး (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_number = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
void
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ပေႃး (rettv_list_alloc(rettv) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    ပေႃး (check_for_list_arg(argvars, 0) == FAIL)
        ပွၵ်ႈ;

    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        puur_naa_nka retval = -1;

        ပေႃး (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(NULL, li->li_tv.vval.v_dict);
        ပေႃးဢမ်ႇ
            emsg(_(e_dictionary_required));

        list_append_number(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
