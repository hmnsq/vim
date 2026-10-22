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

#ถ้า defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
typedef sa_nracanaa sign sign_T;

sa_nracanaa sign
{
    sign_T *sn_next; // next sign in list
    จำนวนเต็ม sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    void *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    จำนวนเต็ม sn_line_hl; // highlight ID for line
    จำนวนเต็ม sn_text_hl; // highlight ID for text
    จำนวนเต็ม sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    จำนวนเต็ม sn_num_hl; // highlight ID for line number
    จำนวนเต็ม sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

static sign_T *first_sign = NULL;
static จำนวนเต็ม next_sign_typenr = 1;

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
     สำหรับ ((sp) = first_sign; (sp) != NULL; (sp) = (sp)->sn_next)

static hashtab_T sg_table; // sign group (signgroup_T) hashtable
static จำนวนเต็ม next_sign_id = 1; // next sign id in the global group

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

    ถ้า (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ถ้า (group == NULL)
            คืน NULL;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    มิฉะนั้น
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    คืน group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
static void
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    ถ้า (HASHITEM_EMPTY(hi))
        คืน;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    ถ้า (group->sg_refcount == 0)
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
static จำนวนเต็ม
sign_in_group(sign_entry_T *sign, char_u *group)
{
    คืน ((group != NULL && STRCMP(group, "*") == 0) ||
            (group == NULL && sign->se_group == NULL) ||
            (group != NULL && sign->se_group != NULL &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
static จำนวนเต็ม
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    จำนวนเต็ม for_popup = sign->se_group != NULL &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    คืน WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
static จำนวนเต็ม
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    จำนวนเต็ม id = 1;
    signgroup_T *group = NULL;
    sign_entry_T *sign = NULL;

    ถ้า (groupname != NULL)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        ถ้า (HASHITEM_EMPTY(hi))
            คืน id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    จำนวนเต็ม found = FALSE;
    ขณะ (!found)
    {
        ถ้า (group == NULL)
            id = next_sign_id++; // global group
        มิฉะนั้น
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ถ้า (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                bha_nga;
            }
        }
    }

    คืน id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
static void
insert_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            จำนวนเต็ม id, // sign ID
            char_u *group, // sign group; NULL for global group
            จำนวนเต็ม prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            จำนวนเต็ม typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(parimaa_na(sign_entry_T), FALSE, aid_insert_sign);
    ถ้า (newsign == NULL)
        คืน;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ถ้า (group != NULL)
    {
        newsign->se_group = sign_group_ref(group);
        ถ้า (newsign->se_group == NULL)
        {
            vim_free(newsign);
            คืน;
        }
    }
    มิฉะนั้น
    {
        newsign->se_group = NULL;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    ถ้า (next != NULL)
        next->se_prev = newsign;

    ถ้า (prev == NULL)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ถ้า (buf->b_signlist == NULL)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_line_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ถ้า (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    มิฉะนั้น
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
                         จำนวนเต็ม id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         จำนวนเต็ม prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         จำนวนเต็ม typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    ขณะ (prev != NULL && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == NULL) ? buf->b_signlist : prev->se_next;

    insert_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
static sign_T *
find_sign_by_typenr(จำนวนเต็ม typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        ถ้า (sp->sn_typenr == typenr)
            คืน sp;
    คืน NULL;
}

/*
 * Get the name of a sign by its typenr.
 */
static char_u *
sign_typenr2name(จำนวนเต็ม typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        ถ้า (sp->sn_typenr == typenr)
            คืน sp->sn_name;
    คืน (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
static dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ถ้า (d == NULL)
        คืน NULL;

    dict_add_number(d, "id", sign->se_id);
    ถ้า (sign->se_group == NULL)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    มิฉะนั้น
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_number(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_number(d, "priority", sign->se_priority);

    คืน d;
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
    ถ้า ((sign->se_prev == NULL || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == NULL || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        คืน;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    ขณะ (p->se_prev != NULL && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    ถ้า (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        ขณะ (p->se_next != NULL && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    ถ้า (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    ถ้า (sign->se_prev != NULL)
        sign->se_prev->se_next = sign->se_next;

    ถ้า (sign->se_next != NULL)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = NULL;
    sign->se_next = NULL;

    // Re-insert 'sign' at the right place
    ถ้า (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        ถ้า (sign->se_prev != NULL)
            sign->se_prev->se_next = sign;

        ถ้า (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    มิฉะนั้น
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        ถ้า (sign->se_next != NULL)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
static void
buf_addsign(buf_T *buf, // buffer to store sign in
            จำนวนเต็ม id, // sign ID
            char_u *groupname, // sign group
            จำนวนเต็ม prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            จำนวนเต็ม typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    sign_entry_T *prev = NULL; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ถ้า (buf->b_sign_finger != NULL && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    สำหรับ (sign = (prev == NULL) ? buf->b_signlist : prev->se_next; sign != NULL;
         sign = sign->se_next)
    {
        ถ้า (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            คืน;
        }
        มิฉะนั้น ถ้า (lnum < sign->se_lnum)
        {
            insert_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            คืน;
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
                     จำนวนเต็ม markId, // sign ID
                     char_u *group, // sign group
                     จำนวนเต็ม typenr, // typenr of sign we are adding
                     จำนวนเต็ม prio) // sign priority
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ถ้า (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            คืน sign->se_lnum;
        }
    }

    คืน (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
จำนวนเต็ม
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ถ้า (sign->se_lnum > lnum)
            bha_nga;

        ถ้า (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            ถ้า (sp == NULL)
                คืน FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            ถ้า (sattr->sat_text != NULL && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            ถ้า (sp->sn_line_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_line_hl);

            ถ้า (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ถ้า (sp->sn_num_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_num_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ถ้า (sign->se_next != NULL &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                ถ้า (next_sp == NULL)
                    คืน FALSE;

                ถ้า (sattr->sat_icon == NULL && sattr->sat_text == NULL)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                ถ้า (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                ถ้า (sp->sn_line_hl <= 0 && next_sp->sn_line_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_line_hl);

                ถ้า (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                ถ้า (sp->sn_num_hl <= 0 && next_sp->sn_num_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_num_hl);
            }
            คืน TRUE;
        }
    }
    คืน FALSE;
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
            จำนวนเต็ม id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = NULL; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    สำหรับ (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;

        ถ้า ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            ถ้า (next != NULL)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ถ้า (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_line_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ถ้า (group == NULL || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                bha_nga;
        }
        มิฉะนั้น
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ถ้า (buf->b_signlist == NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    คืน lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
จำนวนเต็ม
buf_findsign(buf_T *buf, // buffer to store sign in
             จำนวนเต็ม id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        ถ้า (sign->se_id == id && sign_in_group(sign, group))
            คืน sign->se_lnum;

    คืน 0;
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
        ถ้า (sign->se_lnum > lnum)
            bha_nga;

        ถ้า (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            คืน sign;
    }

    คืน NULL;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
จำนวนเต็ม
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_line(buf, lnum, groupname);
    ถ้า (sign != NULL)
        คืน sign->se_id;

    คืน 0;
}

# ถ้า defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
จำนวนเต็ม
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    จำนวนเต็ม typenr) // sign type number
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ถ้า (sign->se_lnum > lnum)
            bha_nga;

        ถ้า (sign->se_lnum == lnum && sign->se_typenr == typenr)
            คืน sign->se_id;
    }

    คืน 0;
}

#  ถ้า defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
จำนวนเต็ม
buf_signcount(buf_T *buf, linenr_T lnum)
{
    จำนวนเต็ม count = 0;
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ถ้า (sign->se_lnum > lnum)
            bha_nga;

        ถ้า (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != NULL)
            count++;
    }

    คืน count;
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
    ถ้า (buf->b_signlist != NULL && curwin != NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL;

    สำหรับ (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;
        ถ้า (sign_in_group(sign, group))
        {
            *lastp = next;

            ถ้า (next != NULL)
                next->se_prev = sign->se_prev;

            ถ้า (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        มิฉะนั้น
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
    ขณะ (buf != NULL && !got_int)
    {
        ถ้า (buf->b_signlist != NULL)
        {
            vim_snprintf(lbuf, MSG_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            msg_puts_attr(lbuf, HL_ATTR(HLF_D));
            msg_putchar('\n');
        }

        sign_entry_T *sign = NULL;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            ถ้า (got_int)
                bha_nga;

            ถ้า (!sign_in_group(sign, sign_group))
                continue;

            ถ้า (sign->se_group != NULL)
                vim_snprintf(group, MSG_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            มิฉะนั้น
                group[0] = '\0';

            vim_snprintf(lbuf, MSG_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            msg_puts(lbuf);
            msg_putchar('\n');
        }

        ถ้า (rbuf != NULL)
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
        ถ้า (sign->se_lnum < line1)
            continue;

        linenr_T new_lnum = sign->se_lnum;

        ถ้า (sign->se_lnum <= line2)
        {
            ถ้า (amount != MAXLNUM)
                new_lnum += amount;
        }
        มิฉะนั้น ถ้า (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ถ้า (new_lnum <= curbuf->b_ml.ml_line_count)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
static จำนวนเต็ม
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    จำนวนเต็ม idx = 0;
    var_na save = *end_cmd;
    *end_cmd = NUL;

    ขณะ (cmds[idx] != NULL && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    คืน idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
static sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    ถ้า (sp_prev != NULL)
        *sp_prev = NULL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ถ้า (STRCMP(sp->sn_name, name) == 0)
            bha_nga;

        ถ้า (sp_prev != NULL)
            *sp_prev = sp;
    }

    คืน sp;
}

/*
 * Allocate a new sign
 */
static sign_T *
alloc_new_sign(char_u *name)
{
    จำนวนเต็ม start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(parimaa_na(sign_T), aid_sign_define_by_name);
    ถ้า (sp == NULL)
        คืน NULL;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    ขณะ (lp != NULL)
    {
        ถ้า (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            ถ้า (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            ถ้า (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                คืน NULL;
            }

            lp = first_sign; // start all over
            continue;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    ถ้า (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ถ้า (sp->sn_name == NULL) // out of memory
    {
        vim_free(sp);
        คืน NULL;
    }

    คืน sp;
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
    ถ้า (gui.in_use)
    {
        out_flush();
        ถ้า (sp->sn_image != NULL)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
static จำนวนเต็ม
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = NULL;
    char_u *endp = text + (จำนวนเต็ม)STRLEN(text);
    จำนวนเต็ม cells = 0;

    // Remove backslashes so that it is possible to use a space.
    สำหรับ (s = text; s + 1 < endp; ++s)
    {
        ถ้า (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ถ้า (has_mbyte)
    {
        สำหรับ (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            ถ้า (!vim_isprintc((*mb_ptr2char)(s)))
                bha_nga;

            cells += (*mb_ptr2cells)(s);
        }
    }
    มิฉะนั้น
    {
        สำหรับ (s = text; s < endp; ++s)
        {
            ถ้า (!vim_isprintc(*s))
                bha_nga;
        }
        cells = (จำนวนเต็ม)(s - text);
    }

    // Currently sign text must be one or two display cells
    ถ้า (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        คืน FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    จำนวนเต็ม len = (จำนวนเต็ม)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    ถ้า (sp->sn_text != NULL && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    คืน OK;
}

/*
 * Define a new sign or update an existing sign
 */
จำนวนเต็ม
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    จำนวนเต็ม prio)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    ถ้า (sp == NULL)
    {
        sp = alloc_new_sign(name);
        ถ้า (sp == NULL)
            คืน FAIL;

        // add the new sign to the list of signs
        ถ้า (sp_prev == NULL)
            first_sign = sp;
        มิฉะนั้น
            sp_prev->sn_next = sp;
    }
    มิฉะนั้น
    {
        win_T *wp = NULL;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ถ้า (wp->w_buffer->b_signlist != NULL)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ถ้า (icon != NULL)
        sign_define_init_icon(sp, icon);

    ถ้า (text != NULL && (sign_define_init_text(sp, text) == FAIL))
        คืน FAIL;

    sp->sn_priority = prio;

    ถ้า (linehl != NULL)
    {
        ถ้า (*linehl == NUL)
            sp->sn_line_hl = 0;
        มิฉะนั้น
            sp->sn_line_hl = syn_check_group(linehl, (จำนวนเต็ม)STRLEN(linehl));
    }

    ถ้า (texthl != NULL)
    {
        ถ้า (*texthl == NUL)
            sp->sn_text_hl = 0;
        มิฉะนั้น
            sp->sn_text_hl = syn_check_group(texthl, (จำนวนเต็ม)STRLEN(texthl));
    }

    ถ้า (culhl != NULL)
    {
        ถ้า (*culhl == NUL)
            sp->sn_cul_hl = 0;
        มิฉะนั้น
            sp->sn_cul_hl = syn_check_group(culhl, (จำนวนเต็ม)STRLEN(culhl));
    }

    ถ้า (numhl != NULL)
    {
        ถ้า (*numhl == NUL)
            sp->sn_num_hl = 0;
        มิฉะนั้น
            sp->sn_num_hl = syn_check_group(numhl, (จำนวนเต็ม)STRLEN(numhl));
    }

    คืน OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
จำนวนเต็ม
sign_exists_by_name(char_u *name)
{
    คืน sign_find(name, NULL) != NULL;
}

/*
 * Free the sign specified by 'name'.
 */
จำนวนเต็ม
sign_undefine_by_name(char_u *name, จำนวนเต็ม give_error)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    ถ้า (sp == NULL)
    {
        ถ้า (give_error)
            semsg(_(e_unknown_sign_str), name);
        คืน FAIL;
    }
    sign_undefine(sp, sp_prev);

    คืน OK;
}

/*
 * List the signs matching 'name'
 */
static void
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, NULL);
    ถ้า (sp != NULL)
        sign_list_defined(sp);
    มิฉะนั้น
        semsg(_(e_unknown_sign_str), name);
}

static void
may_force_numberwidth_recompute(buf_T *buf, จำนวนเต็ม unplace)
{
# ถ้า defined(FEAT_LINEBREAK)
    tabpage_T *tp = NULL;
    win_T *wp = NULL;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ถ้า (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_line_count = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
จำนวนเต็ม
sign_place(จำนวนเต็ม *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           จำนวนเต็ม prio)
{
    // Check for reserved character '*' in group name
    ถ้า (sign_group != NULL && (*sign_group == '*' || *sign_group == '\0'))
        คืน FAIL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ถ้า (STRCMP(sp->sn_name, sign_name) == 0)
            bha_nga;
    }

    ถ้า (sp == NULL)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        คืน FAIL;
    }

    ถ้า (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    ถ้า (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    ถ้า (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    มิฉะนั้น
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    ถ้า (lnum > 0)
    {
        redraw_buf_line_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    มิฉะนั้น
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        คืน FAIL;
    }

    คืน OK;
}

/*
 * Unplace the specified sign
 */
static จำนวนเต็ม
sign_unplace(จำนวนเต็ม sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    ถ้า (buf->b_signlist == NULL) // No signs in the buffer
        คืน OK;

    ถ้า (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    มิฉะนั้น
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        ถ้า (lnum == 0)
            คืน FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ถ้า (buf->b_signlist == NULL)
        may_force_numberwidth_recompute(buf, TRUE);

    คืน OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
static void
sign_unplace_at_cursor(char_u *groupname)
{
    จำนวนเต็ม id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    ถ้า (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    มิฉะนั้น
        emsg(_(e_missing_sign_number));
}

/*
 * Jump to a sign.
 */
static linenr_T
sign_jump(จำนวนเต็ม sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    ถ้า (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        คืน -1;
    }

    // goto a sign ...
    ถ้า (buf_jump_open_win(buf) != NULL)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    มิฉะนั้น
    { // ... not currently in a window
        ถ้า (buf->b_fname == NULL)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            คืน -1;
        }
        setpcmark();
        ถ้า (do_ecmd(buf->b_fnum, NULL, NULL, NULL, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            คืน -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    คืน lnum;
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
    จำนวนเต็ม prio = -1;
    จำนวนเต็ม failed = FALSE;

    // set values for a defined sign.
    ขณะ (TRUE)
    {
        arg = skipwhite(p);
        ถ้า (*arg == NUL)
            bha_nga;

        p = skiptowhite_esc(arg);
        ถ้า (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((var_na *)arg);
        }
        มิฉะนั้น
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            bha_nga;
        }
    }

    ถ้า (!failed)
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
               จำนวนเต็ม id,
               char_u *group,
               จำนวนเต็ม prio)
{
    ถ้า (id <= 0)
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
        ถ้า (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
            emsg(_(e_invalid_argument));
        มิฉะนั้น
            sign_list_placed(buf, group);
    }
    มิฉะนั้น
    {
        // Place a new sign
        ถ้า (sign_name == NULL || buf == NULL ||
            (group != NULL && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            คืน;
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
                 จำนวนเต็ม id,
                 char_u *group)
{
    ถ้า (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        คืน;
    }

    ถ้า (id == -2)
    {
        ถ้า (buf != NULL)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        มิฉะนั้น
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ถ้า (buf->b_signlist != NULL)
                    buf_delete_signs(buf, group);
            }
        }
    }
    มิฉะนั้น
    {
        ถ้า (buf != NULL)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        มิฉะนั้น
        {
            ถ้า (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            มิฉะนั้น
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
              จำนวนเต็ม id,
              char_u *group)
{
    ถ้า (sign_name == NULL && group == NULL && id == -1)
    {
        emsg(_(e_argument_required));
        คืน;
    }

    ถ้า (buf == NULL || (group != NULL && *group == '\0') || lnum >= 0 ||
        sign_name != NULL)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        คืน;
    }

    (void)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
static จำนวนเต็ม
parse_sign_cmd_args(จำนวนเต็ม cmd,
                    char_u *arg,
                    char_u **sign_name,
                    จำนวนเต็ม *signid,
                    char_u **group,
                    จำนวนเต็ม *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = NULL;
    จำนวนเต็ม lnum_arg = FALSE;

    // first arg could be placed sign id
    ถ้า (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ถ้า (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        มิฉะนั้น
        {
            arg = skipwhite(arg);
        }
    }

    ขณะ (*arg != NUL)
    {
        ถ้า (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((var_na *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ถ้า (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                คืน FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ถ้า (*arg != NUL)
                *arg++ = NUL;

            ขณะ (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            ถ้า (*arg != NUL)
                *arg++ = NUL;
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((var_na *)arg);
            arg = skiptowhite(arg);
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            bha_nga;
        }
        มิฉะนั้น ถ้า (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((จำนวนเต็ม)getdigits(&arg));

            ถ้า (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            bha_nga;
        }
        มิฉะนั้น
        {
            emsg(_(e_invalid_argument));
            คืน FAIL;
        }

        arg = skipwhite(arg);
    }

    ถ้า (filename != NULL && *buf == NULL)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        คืน FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ถ้า (filename == NULL &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    คืน OK;
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
    จำนวนเต็ม idx = sign_cmd_idx(arg, p);
    ถ้า (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        คืน;
    }
    arg = skipwhite(p);

    ถ้า (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = NULL;
        จำนวนเต็ม id = -1;
        char_u *group = NULL;
        จำนวนเต็ม prio = -1;
        buf_T *buf = NULL;
        linenr_T lnum = -1;

        // Parse command line arguments
        ถ้า (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            คืน;

        ถ้า (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        มิฉะนั้น ถ้า (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        มิฉะนั้น ถ้า (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        คืน;
    }

    // Define, undefine or list signs.
    ถ้า (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        สำหรับ (sign_T *sp = first_sign; sp != NULL && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    มิฉะนั้น ถ้า (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    มิฉะนั้น
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ถ้า (*p != NUL)
            *p++ = NUL;

        ขณะ (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ถ้า (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        มิฉะนั้น ถ้า (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        มิฉะนั้น
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

    ถ้า (sp->sn_icon != NULL)
        dict_add_string(retdict, "icon", sp->sn_icon);

    ถ้า (sp->sn_text != NULL)
        dict_add_string(retdict, "text", sp->sn_text);

    ถ้า (sp->sn_priority > 0)
        dict_add_number(retdict, "priority", sp->sn_priority);

    ถ้า (sp->sn_line_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        ถ้า (p == NULL)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        มิฉะนั้น
            dict_add_string(retdict, "linehl", p);
    }

    ถ้า (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        ถ้า (p == NULL)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        มิฉะนั้น
            dict_add_string(retdict, "texthl", p);
    }

    ถ้า (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        ถ้า (p == NULL)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        มิฉะนั้น
            dict_add_string(retdict, "culhl", p);
    }

    ถ้า (sp->sn_num_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        ถ้า (p == NULL)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        มิฉะนั้น
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

    ถ้า (name != NULL)
    {
        sp = sign_find(name, NULL);
        ถ้า (sp == NULL)
            คืน;
    }

    สำหรับ (; sp != NULL && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ถ้า (dict == NULL)
            คืน;

        ถ้า (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            คืน;
        }

        sign_getinfo(sp, dict);

        ถ้า (name != NULL) // handle only the specified sign
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
        ถ้า (d != NULL && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
static void
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       จำนวนเต็ม sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ถ้า (d == NULL)
        คืน;

    ถ้า (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        คืน;
    }

    dict_add_number(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ถ้า (l == NULL)
        คืน;

    ถ้า (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        คืน;
    }

    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        ถ้า (!sign_in_group(sign, sign_group))
            continue;

        ถ้า ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ถ้า (sdict != NULL && list_append_dict(l, sdict) == FAIL)
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
                จำนวนเต็ม sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    ถ้า (buf != NULL)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    มิฉะนั้น
    {
        FOR_ALL_BUFFERS(buf)
        {
            ถ้า (buf->b_signlist != NULL)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# ถ้า defined(FEAT_SIGN_ICONS)
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
        ถ้า (sp->sn_icon != NULL)
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

    ถ้า (sp->sn_icon != NULL)
    {
        msg_puts(" icon=");
        msg_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ถ้า (sp->sn_image == NULL)
            msg_puts(_(" (NOT FOUND)"));
# มิฉะนั้น
        msg_puts(_(" (not supported)"));
# endif
    }

    ถ้า (sp->sn_text != NULL)
    {
        msg_puts(" text=");
        msg_outtrans(sp->sn_text);
    }

    ถ้า (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, MSG_BUF_LEN, " priority=%d", sp->sn_priority);
        msg_puts(lbuf);
    }

    ถ้า (sp->sn_line_hl > 0)
    {
        msg_puts(" linehl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        ถ้า (p == NULL)
            msg_puts("NONE");
        มิฉะนั้น
            msg_puts((var_na *)p);
    }

    ถ้า (sp->sn_text_hl > 0)
    {
        msg_puts(" texthl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        ถ้า (p == NULL)
            msg_puts("NONE");
        มิฉะนั้น
            msg_puts((var_na *)p);
    }

    ถ้า (sp->sn_cul_hl > 0)
    {
        msg_puts(" culhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        ถ้า (p == NULL)
            msg_puts("NONE");
        มิฉะนั้น
            msg_puts((var_na *)p);
    }

    ถ้า (sp->sn_num_hl > 0)
    {
        msg_puts(" numhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        ถ้า (p == NULL)
            msg_puts("NONE");
        มิฉะนั้น
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
    ถ้า (sp->sn_image != NULL)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    ถ้า (sp_prev == NULL)
        first_sign = sp->sn_next;
    มิฉะนั้น
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# ถ้า defined(FEAT_SIGN_ICONS)
void *
sign_get_image(จำนวนเต็ม typenr) // the attribute which may have a sign
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        ถ้า (sp->sn_typenr == typenr)
            คืน sp->sn_image;
    }
    คืน NULL;
}
# endif

/*
 * Undefine/free all signs.
 */
void
free_signs(void)
{
    ขณะ (first_sign != NULL)
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
get_nth_sign_name(จำนวนเต็ม idx)
{
    จำนวนเต็ม current_idx = 0;
    sign_T *sp = NULL;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ถ้า (current_idx++ == idx)
            คืน sp->sn_name;
    }
    คืน NULL;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
static char_u *
get_nth_sign_group_name(จำนวนเต็ม idx)
{
    จำนวนเต็ม current_idx = 0;
    จำนวนเต็ม todo = (จำนวนเต็ม)sg_table.ht_used;
    hashitem_T *hi = NULL;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ITEMS(&sg_table, hi, todo)
    {
        ถ้า (!HASHITEM_EMPTY(hi))
        {
            --todo;
            ถ้า (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                คืน group->sg_name;
            }
        }
    }
    คืน NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, จำนวนเต็ม idx)
{
    switch (expand_what)
    {
        pak_sa EXP_SUBCMD:
            คืน (char_u *)cmds[idx];
        pak_sa EXP_DEFINE:
        {
            var_na *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", NULL };
            คืน (char_u *)define_arg[idx];
        }
        pak_sa EXP_PLACE:
        {
            var_na *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", NULL };
            คืน (char_u *)place_arg[idx];
        }
        pak_sa EXP_LIST:
        {
            var_na *list_arg[] = { "group=", "file=", "buffer=", NULL };
            คืน (char_u *)list_arg[idx];
        }
        pak_sa EXP_UNPLACE:
        {
            var_na *unplace_arg[] = { "group=", "file=", "buffer=", NULL };
            คืน (char_u *)unplace_arg[idx];
        }
        pak_sa EXP_SIGN_NAMES:
            คืน get_nth_sign_name(idx);
        pak_sa EXP_SIGN_GROUPS:
            คืน get_nth_sign_group_name(idx);
        default:
            คืน NULL;
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
    จำนวนเต็ม cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ถ้า (*end_subcmd == NUL)
        คืน;

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
    ขณะ (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ถ้า (p == NULL)
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
                ถ้า (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                มิฉะนั้น
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
    มิฉะนั้น
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        switch (cmd_idx)
        {
            pak_sa SIGNCMD_DEFINE:
                ถ้า (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                มิฉะนั้น ถ้า (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                มิฉะนั้น
                    xp->xp_context = EXPAND_NOTHING;
                bha_nga;
            pak_sa SIGNCMD_PLACE:
                ถ้า (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                มิฉะนั้น ถ้า (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                มิฉะนั้น ถ้า (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                มิฉะนั้น
                    xp->xp_context = EXPAND_NOTHING;
                bha_nga;
            pak_sa SIGNCMD_UNPLACE:
            pak_sa SIGNCMD_JUMP:
                ถ้า (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                มิฉะนั้น ถ้า (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                มิฉะนั้น
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
static จำนวนเต็ม
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = NULL;
    char_u *icon = NULL;
    char_u *linehl = NULL;
    char_u *text = NULL;
    char_u *texthl = NULL;
    char_u *culhl = NULL;
    char_u *numhl = NULL;
    จำนวนเต็ม prio = -1;
    จำนวนเต็ม retval = -1;

    ถ้า (name_arg == NULL && dict == NULL)
        คืน retval;

    ถ้า (name_arg == NULL)
        name = dict_get_string(dict, "name", TRUE);
    มิฉะนั้น
        name = vim_strsave(name_arg);

    ถ้า (name == NULL || name[0] == NUL)
        goto cleanup;

    ถ้า (dict != NULL)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_number_def(dict, "priority", -1);
    }

    ถ้า (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    คืน retval;
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
        จำนวนเต็ม retval = -1;

        ถ้า (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(NULL, li->li_tv.vval.v_dict);
        มิฉะนั้น
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
    ถ้า (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        คืน;

    ถ้า (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ถ้า (rettv_list_alloc(rettv) == FAIL)
            คืน;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        คืน;
    }

    // Define a single sign
    rettv->vval.v_number = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ถ้า (name == NULL)
        คืน;

    ถ้า (check_for_opt_dict_arg(argvars, 1) == FAIL)
        คืน;

    rettv->vval.v_number = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : NULL);
}

/*
 * "sign_getdefined()" function
 */
void
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ถ้า (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        คืน;

    ถ้า (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        คืน;

    char_u *name = NULL;
    ถ้า (argvars[0].v_type != VAR_UNKNOWN)
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
    จำนวนเต็ม sign_id = 0;
    char_u *group = NULL;

    ถ้า (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        คืน;

    ถ้า (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        คืน;

    ถ้า (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ถ้า (buf == NULL)
            คืน;

        ถ้า (argvars[1].v_type != VAR_UNKNOWN)
        {
            ถ้า (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                คืน;

            dictitem_T *di = NULL;
            dict_T *dict = argvars[1].vval.v_dict;

            ถ้า ((di = dict_find(dict, (char_u *)"lnum", -1)) != NULL)
            {
                // get signs placed at this line
                จำนวนเต็ม notanum = FALSE;
                tv_get_number_chk(&di->di_tv, &notanum);
                ถ้า (notanum)
                    คืน;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ถ้า ((di = dict_find(dict, (char_u *)"id", -1)) != NULL)
            {
                // get sign placed with this identifier
                จำนวนเต็ม notanum = FALSE;
                sign_id = (จำนวนเต็ม)tv_get_number_chk(&di->di_tv, &notanum);
                ถ้า (notanum)
                    คืน;
            }

            ถ้า ((di = dict_find(dict, (char_u *)"group", -1)) != NULL)
            {
                group = tv_get_string_chk(&di->di_tv);
                ถ้า (group == NULL)
                    คืน;

                ถ้า (*group == '\0') // empty string means global group
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

    ถ้า (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        คืน;

    จำนวนเต็ม notanum = FALSE;
    // Sign identifier
    จำนวนเต็ม sign_id = (จำนวนเต็ม)tv_get_number_chk(&argvars[0], &notanum);
    ถ้า (notanum)
        คืน;

    ถ้า (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        คืน;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    ถ้า (sign_group == NULL)
        คืน;

    ถ้า (sign_group[0] == '\0')
    {
        sign_group = NULL; // global sign group
    }
    มิฉะนั้น
    {
        sign_group = vim_strsave(sign_group);
        ถ้า (sign_group == NULL)
            คืน;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ถ้า (buf == NULL)
        goto cleanup;

    rettv->vval.v_number = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
static จำนวนเต็ม
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    จำนวนเต็ม sign_id = 0;
    char_u *group = NULL;
    char_u *sign_name = NULL;
    buf_T *buf = NULL;
    dictitem_T *di = NULL;
    linenr_T lnum = 0;
    จำนวนเต็ม prio = -1;
    จำนวนเต็ม ret_sign_id = -1;

    // sign identifier
    ถ้า (id_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        ถ้า (di != NULL)
            id_tv = &di->di_tv;
    }

    ถ้า (id_tv == NULL)
    {
        sign_id = 0;
    }
    มิฉะนั้น
    {
        จำนวนเต็ม notanum = FALSE;
        sign_id = tv_get_number_chk(id_tv, &notanum);

        ถ้า (notanum)
            คืน -1;

        ถ้า (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            คืน -1;
        }
    }

    // sign group
    ถ้า (group_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        ถ้า (di != NULL)
            group_tv = &di->di_tv;
    }

    ถ้า (group_tv == NULL)
    {
        group = NULL; // global group
    }
    มิฉะนั้น
    {
        group = tv_get_string_chk(group_tv);
        ถ้า (group == NULL)
            goto cleanup;

        ถ้า (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        มิฉะนั้น
        {
            group = vim_strsave(group);
            ถ้า (group == NULL)
                คืน -1;
        }
    }

    // sign name
    ถ้า (name_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        ถ้า (di != NULL)
            name_tv = &di->di_tv;
    }

    ถ้า (name_tv == NULL)
        goto cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ถ้า (sign_name == NULL)
        goto cleanup;

    // buffer to place the sign
    ถ้า (buf_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        ถ้า (di != NULL)
            buf_tv = &di->di_tv;
    }

    ถ้า (buf_tv == NULL)
        goto cleanup;

    buf = get_buf_arg(buf_tv);
    ถ้า (buf == NULL)
        goto cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    ถ้า (di != NULL)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ถ้า (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            goto cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    ถ้า (di != NULL)
    {
        จำนวนเต็ม notanum = FALSE;
        prio = (จำนวนเต็ม)tv_get_number_chk(&di->di_tv, &notanum);
        ถ้า (notanum)
            goto cleanup;
    }

    ถ้า (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    คืน ret_sign_id;
}

/*
 * "sign_place()" function
 */
void
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = NULL;
    rettv->vval.v_number = -1;

    ถ้า (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        คืน;

    ถ้า (argvars[4].v_type != VAR_UNKNOWN)
    {
        ถ้า (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            คืน;
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
    ถ้า (rettv_list_alloc(rettv) == FAIL)
        คืน;

    ถ้า (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        คืน;

    ถ้า (check_for_list_arg(argvars, 0) == FAIL)
        คืน;

    // Process the List of sign attributes
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        จำนวนเต็ม sign_id = -1;

        ถ้า (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(NULL, NULL, NULL, NULL,
                                           li->li_tv.vval.v_dict);
        มิฉะนั้น
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
        จำนวนเต็ม retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ถ้า (name != NULL && (sign_undefine_by_name(name, TRUE) == OK))
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

    ถ้า (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        คืน;

    ถ้า (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ถ้า (rettv_list_alloc(rettv) == FAIL)
            คืน;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        คืน;
    }

    rettv->vval.v_number = -1;

    ถ้า (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_number = 0;
    }
    มิฉะนั้น
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ถ้า (name == NULL)
            คืน;

        ถ้า (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_number = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
static จำนวนเต็ม
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    จำนวนเต็ม sign_id = 0;
    buf_T *buf = NULL;
    char_u *group = NULL;
    จำนวนเต็ม retval = -1;

    // sign group
    group = (group_tv != NULL) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    ถ้า (group != NULL)
    {
        ถ้า (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        มิฉะนั้น
        {
            group = vim_strsave(group);
            ถ้า (group == NULL)
                คืน retval;
        }
    }

    ถ้า (dict != NULL)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        ถ้า (di != NULL)
        {
            buf = get_buf_arg(&di->di_tv);
            ถ้า (buf == NULL)
                goto cleanup;
        }

        ถ้า (dict_has_key(dict, "id"))
        {
            sign_id = dict_get_number(dict, "id");
            ถ้า (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                goto cleanup;
            }
        }
    }

    ถ้า (buf == NULL)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ถ้า (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    มิฉะนั้น ถ้า (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    คืน retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    ขณะ (sign != NULL && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    คืน sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
จำนวนเต็ม
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ถ้า (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        คืน get_first_valid_sign(wp) != NULL && !wp->w_p_nu && !wp->w_p_rnu;

    ถ้า (*wp->w_p_scl == 'n')
        คืน FALSE;

    ถ้า (*wp->w_p_scl == 'y')
        คืน TRUE;

    คืน (get_first_valid_sign(wp) != NULL
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

    ถ้า ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        คืน;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_number = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
void
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ถ้า (rettv_list_alloc(rettv) == FAIL)
        คืน;

    ถ้า (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        คืน;

    ถ้า (check_for_list_arg(argvars, 0) == FAIL)
        คืน;

    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        จำนวนเต็ม retval = -1;

        ถ้า (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(NULL, li->li_tv.vval.v_dict);
        มิฉะนั้น
            emsg(_(e_dictionary_required));

        list_append_number(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
