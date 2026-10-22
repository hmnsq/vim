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

#nếu defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
typedef struct sign sign_T;

struct sign
{
    sign_T *sn_next; // next sign in list
    số_nguyên sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    void *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    số_nguyên sn_line_hl; // highlight ID for line
    số_nguyên sn_text_hl; // highlight ID for text
    số_nguyên sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    số_nguyên sn_num_hl; // highlight ID for line number
    số_nguyên sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

static sign_T *first_sign = NULL;
static số_nguyên next_sign_typenr = 1;

static void sign_list_defined(sign_T *sp);
static void sign_undefine(sign_T *sp, sign_T *sp_prev);

static char *cmds[] = { "define",
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
     cho ((sp) = first_sign; (sp) != NULL; (sp) = (sp)->sn_next)

static hashtab_T sg_table; // sign group (signgroup_T) hashtable
static số_nguyên next_sign_id = 1; // next sign id in the global group

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

    nếu (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        nếu (group == NULL)
            trả_về NULL;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    không_thì
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    trả_về group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
static void
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    nếu (HASHITEM_EMPTY(hi))
        trả_về;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    nếu (group->sg_refcount == 0)
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
static số_nguyên
sign_in_group(sign_entry_T *sign, char_u *group)
{
    trả_về ((group != NULL && STRCMP(group, "*") == 0) ||
            (group == NULL && sign->se_group == NULL) ||
            (group != NULL && sign->se_group != NULL &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
static số_nguyên
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    số_nguyên for_popup = sign->se_group != NULL &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    trả_về WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
static số_nguyên
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    số_nguyên id = 1;
    signgroup_T *group = NULL;
    sign_entry_T *sign = NULL;

    nếu (groupname != NULL)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        nếu (HASHITEM_EMPTY(hi))
            trả_về id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    số_nguyên found = FALSE;
    trong_khi (!found)
    {
        nếu (group == NULL)
            id = next_sign_id++; // global group
        không_thì
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            nếu (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                break;
            }
        }
    }

    trả_về id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
static void
insert_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            số_nguyên id, // sign ID
            char_u *group, // sign group; NULL for global group
            số_nguyên prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            số_nguyên typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(sizeof(sign_entry_T), FALSE, aid_insert_sign);
    nếu (newsign == NULL)
        trả_về;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    nếu (group != NULL)
    {
        newsign->se_group = sign_group_ref(group);
        nếu (newsign->se_group == NULL)
        {
            vim_free(newsign);
            trả_về;
        }
    }
    không_thì
    {
        newsign->se_group = NULL;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    nếu (next != NULL)
        next->se_prev = newsign;

    nếu (prev == NULL)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        nếu (buf->b_signlist == NULL)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_line_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        nếu (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    không_thì
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
                         số_nguyên id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         số_nguyên prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         số_nguyên typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    trong_khi (prev != NULL && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == NULL) ? buf->b_signlist : prev->se_next;

    insert_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
static sign_T *
find_sign_by_typenr(số_nguyên typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        nếu (sp->sn_typenr == typenr)
            trả_về sp;
    trả_về NULL;
}

/*
 * Get the name of a sign by its typenr.
 */
static char_u *
sign_typenr2name(số_nguyên typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        nếu (sp->sn_typenr == typenr)
            trả_về sp->sn_name;
    trả_về (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
static dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    nếu (d == NULL)
        trả_về NULL;

    dict_add_number(d, "id", sign->se_id);
    nếu (sign->se_group == NULL)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    không_thì
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_number(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_number(d, "priority", sign->se_priority);

    trả_về d;
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
    nếu ((sign->se_prev == NULL || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == NULL || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        trả_về;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    trong_khi (p->se_prev != NULL && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    nếu (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        trong_khi (p->se_next != NULL && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    nếu (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    nếu (sign->se_prev != NULL)
        sign->se_prev->se_next = sign->se_next;

    nếu (sign->se_next != NULL)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = NULL;
    sign->se_next = NULL;

    // Re-insert 'sign' at the right place
    nếu (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        nếu (sign->se_prev != NULL)
            sign->se_prev->se_next = sign;

        nếu (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    không_thì
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        nếu (sign->se_next != NULL)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
static void
buf_addsign(buf_T *buf, // buffer to store sign in
            số_nguyên id, // sign ID
            char_u *groupname, // sign group
            số_nguyên prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            số_nguyên typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    sign_entry_T *prev = NULL; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    nếu (buf->b_sign_finger != NULL && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    cho (sign = (prev == NULL) ? buf->b_signlist : prev->se_next; sign != NULL;
         sign = sign->se_next)
    {
        nếu (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            trả_về;
        }
        không_thì nếu (lnum < sign->se_lnum)
        {
            insert_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            trả_về;
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
                     số_nguyên markId, // sign ID
                     char_u *group, // sign group
                     số_nguyên typenr, // typenr of sign we are adding
                     số_nguyên prio) // sign priority
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        nếu (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            trả_về sign->se_lnum;
        }
    }

    trả_về (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
số_nguyên
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        nếu (sign->se_lnum > lnum)
            break;

        nếu (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            nếu (sp == NULL)
                trả_về FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            nếu (sattr->sat_text != NULL && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            nếu (sp->sn_line_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_line_hl);

            nếu (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            nếu (sp->sn_num_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_num_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            nếu (sign->se_next != NULL &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                nếu (next_sp == NULL)
                    trả_về FALSE;

                nếu (sattr->sat_icon == NULL && sattr->sat_text == NULL)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                nếu (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                nếu (sp->sn_line_hl <= 0 && next_sp->sn_line_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_line_hl);

                nếu (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                nếu (sp->sn_num_hl <= 0 && next_sp->sn_num_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_num_hl);
            }
            trả_về TRUE;
        }
    }
    trả_về FALSE;
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
            số_nguyên id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = NULL; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    cho (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;

        nếu ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            nếu (next != NULL)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            nếu (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_line_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            nếu (group == NULL || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                break;
        }
        không_thì
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    nếu (buf->b_signlist == NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    trả_về lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
số_nguyên
buf_findsign(buf_T *buf, // buffer to store sign in
             số_nguyên id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        nếu (sign->se_id == id && sign_in_group(sign, group))
            trả_về sign->se_lnum;

    trả_về 0;
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
        nếu (sign->se_lnum > lnum)
            break;

        nếu (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            trả_về sign;
    }

    trả_về NULL;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
số_nguyên
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_line(buf, lnum, groupname);
    nếu (sign != NULL)
        trả_về sign->se_id;

    trả_về 0;
}

# nếu defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
số_nguyên
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    số_nguyên typenr) // sign type number
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        nếu (sign->se_lnum > lnum)
            break;

        nếu (sign->se_lnum == lnum && sign->se_typenr == typenr)
            trả_về sign->se_id;
    }

    trả_về 0;
}

#  nếu defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
số_nguyên
buf_signcount(buf_T *buf, linenr_T lnum)
{
    số_nguyên count = 0;
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        nếu (sign->se_lnum > lnum)
            break;

        nếu (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != NULL)
            count++;
    }

    trả_về count;
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
    nếu (buf->b_signlist != NULL && curwin != NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL;

    cho (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;
        nếu (sign_in_group(sign, group))
        {
            *lastp = next;

            nếu (next != NULL)
                next->se_prev = sign->se_prev;

            nếu (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        không_thì
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
    char lbuf[MSG_BUF_LEN];
    char group[MSG_BUF_LEN];

    msg_puts_title(_("\n--- Signs ---"));
    msg_putchar('\n');

    buf_T *buf = (rbuf == NULL) ? firstbuf : rbuf;
    trong_khi (buf != NULL && !got_int)
    {
        nếu (buf->b_signlist != NULL)
        {
            vim_snprintf(lbuf, MSG_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            msg_puts_attr(lbuf, HL_ATTR(HLF_D));
            msg_putchar('\n');
        }

        sign_entry_T *sign = NULL;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            nếu (got_int)
                break;

            nếu (!sign_in_group(sign, sign_group))
                continue;

            nếu (sign->se_group != NULL)
                vim_snprintf(group, MSG_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            không_thì
                group[0] = '\0';

            vim_snprintf(lbuf, MSG_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            msg_puts(lbuf);
            msg_putchar('\n');
        }

        nếu (rbuf != NULL)
            break;

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
        nếu (sign->se_lnum < line1)
            continue;

        linenr_T new_lnum = sign->se_lnum;

        nếu (sign->se_lnum <= line2)
        {
            nếu (amount != MAXLNUM)
                new_lnum += amount;
        }
        không_thì nếu (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        nếu (new_lnum <= curbuf->b_ml.ml_line_count)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
static số_nguyên
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    số_nguyên idx = 0;
    char save = *end_cmd;
    *end_cmd = NUL;

    trong_khi (cmds[idx] != NULL && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    trả_về idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
static sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    nếu (sp_prev != NULL)
        *sp_prev = NULL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        nếu (STRCMP(sp->sn_name, name) == 0)
            break;

        nếu (sp_prev != NULL)
            *sp_prev = sp;
    }

    trả_về sp;
}

/*
 * Allocate a new sign
 */
static sign_T *
alloc_new_sign(char_u *name)
{
    số_nguyên start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(sizeof(sign_T), aid_sign_define_by_name);
    nếu (sp == NULL)
        trả_về NULL;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    trong_khi (lp != NULL)
    {
        nếu (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            nếu (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            nếu (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                trả_về NULL;
            }

            lp = first_sign; // start all over
            continue;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    nếu (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    nếu (sp->sn_name == NULL) // out of memory
    {
        vim_free(sp);
        trả_về NULL;
    }

    trả_về sp;
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
    nếu (gui.in_use)
    {
        out_flush();
        nếu (sp->sn_image != NULL)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
static số_nguyên
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = NULL;
    char_u *endp = text + (số_nguyên)STRLEN(text);
    số_nguyên cells = 0;

    // Remove backslashes so that it is possible to use a space.
    cho (s = text; s + 1 < endp; ++s)
    {
        nếu (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    nếu (has_mbyte)
    {
        cho (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            nếu (!vim_isprintc((*mb_ptr2char)(s)))
                break;

            cells += (*mb_ptr2cells)(s);
        }
    }
    không_thì
    {
        cho (s = text; s < endp; ++s)
        {
            nếu (!vim_isprintc(*s))
                break;
        }
        cells = (số_nguyên)(s - text);
    }

    // Currently sign text must be one or two display cells
    nếu (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        trả_về FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    số_nguyên len = (số_nguyên)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    nếu (sp->sn_text != NULL && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    trả_về OK;
}

/*
 * Define a new sign or update an existing sign
 */
số_nguyên
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    số_nguyên prio)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    nếu (sp == NULL)
    {
        sp = alloc_new_sign(name);
        nếu (sp == NULL)
            trả_về FAIL;

        // add the new sign to the list of signs
        nếu (sp_prev == NULL)
            first_sign = sp;
        không_thì
            sp_prev->sn_next = sp;
    }
    không_thì
    {
        win_T *wp = NULL;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            nếu (wp->w_buffer->b_signlist != NULL)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    nếu (icon != NULL)
        sign_define_init_icon(sp, icon);

    nếu (text != NULL && (sign_define_init_text(sp, text) == FAIL))
        trả_về FAIL;

    sp->sn_priority = prio;

    nếu (linehl != NULL)
    {
        nếu (*linehl == NUL)
            sp->sn_line_hl = 0;
        không_thì
            sp->sn_line_hl = syn_check_group(linehl, (số_nguyên)STRLEN(linehl));
    }

    nếu (texthl != NULL)
    {
        nếu (*texthl == NUL)
            sp->sn_text_hl = 0;
        không_thì
            sp->sn_text_hl = syn_check_group(texthl, (số_nguyên)STRLEN(texthl));
    }

    nếu (culhl != NULL)
    {
        nếu (*culhl == NUL)
            sp->sn_cul_hl = 0;
        không_thì
            sp->sn_cul_hl = syn_check_group(culhl, (số_nguyên)STRLEN(culhl));
    }

    nếu (numhl != NULL)
    {
        nếu (*numhl == NUL)
            sp->sn_num_hl = 0;
        không_thì
            sp->sn_num_hl = syn_check_group(numhl, (số_nguyên)STRLEN(numhl));
    }

    trả_về OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
số_nguyên
sign_exists_by_name(char_u *name)
{
    trả_về sign_find(name, NULL) != NULL;
}

/*
 * Free the sign specified by 'name'.
 */
số_nguyên
sign_undefine_by_name(char_u *name, số_nguyên give_error)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    nếu (sp == NULL)
    {
        nếu (give_error)
            semsg(_(e_unknown_sign_str), name);
        trả_về FAIL;
    }
    sign_undefine(sp, sp_prev);

    trả_về OK;
}

/*
 * List the signs matching 'name'
 */
static void
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, NULL);
    nếu (sp != NULL)
        sign_list_defined(sp);
    không_thì
        semsg(_(e_unknown_sign_str), name);
}

static void
may_force_numberwidth_recompute(buf_T *buf, số_nguyên unplace)
{
# nếu defined(FEAT_LINEBREAK)
    tabpage_T *tp = NULL;
    win_T *wp = NULL;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        nếu (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_line_count = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
số_nguyên
sign_place(số_nguyên *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           số_nguyên prio)
{
    // Check for reserved character '*' in group name
    nếu (sign_group != NULL && (*sign_group == '*' || *sign_group == '\0'))
        trả_về FAIL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        nếu (STRCMP(sp->sn_name, sign_name) == 0)
            break;
    }

    nếu (sp == NULL)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        trả_về FAIL;
    }

    nếu (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    nếu (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    nếu (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    không_thì
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    nếu (lnum > 0)
    {
        redraw_buf_line_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    không_thì
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        trả_về FAIL;
    }

    trả_về OK;
}

/*
 * Unplace the specified sign
 */
static số_nguyên
sign_unplace(số_nguyên sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    nếu (buf->b_signlist == NULL) // No signs in the buffer
        trả_về OK;

    nếu (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    không_thì
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        nếu (lnum == 0)
            trả_về FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    nếu (buf->b_signlist == NULL)
        may_force_numberwidth_recompute(buf, TRUE);

    trả_về OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
static void
sign_unplace_at_cursor(char_u *groupname)
{
    số_nguyên id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    nếu (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    không_thì
        emsg(_(e_missing_sign_number));
}

/*
 * Jump to a sign.
 */
static linenr_T
sign_jump(số_nguyên sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    nếu (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        trả_về -1;
    }

    // goto a sign ...
    nếu (buf_jump_open_win(buf) != NULL)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    không_thì
    { // ... not currently in a window
        nếu (buf->b_fname == NULL)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            trả_về -1;
        }
        setpcmark();
        nếu (do_ecmd(buf->b_fnum, NULL, NULL, NULL, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            trả_về -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    trả_về lnum;
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
    số_nguyên prio = -1;
    số_nguyên failed = FALSE;

    // set values for a defined sign.
    trong_khi (TRUE)
    {
        arg = skipwhite(p);
        nếu (*arg == NUL)
            break;

        p = skiptowhite_esc(arg);
        nếu (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        không_thì nếu (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((char *)arg);
        }
        không_thì
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            break;
        }
    }

    nếu (!failed)
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
               số_nguyên id,
               char_u *group,
               số_nguyên prio)
{
    nếu (id <= 0)
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
        nếu (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
            emsg(_(e_invalid_argument));
        không_thì
            sign_list_placed(buf, group);
    }
    không_thì
    {
        // Place a new sign
        nếu (sign_name == NULL || buf == NULL ||
            (group != NULL && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            trả_về;
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
                 số_nguyên id,
                 char_u *group)
{
    nếu (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        trả_về;
    }

    nếu (id == -2)
    {
        nếu (buf != NULL)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        không_thì
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                nếu (buf->b_signlist != NULL)
                    buf_delete_signs(buf, group);
            }
        }
    }
    không_thì
    {
        nếu (buf != NULL)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        không_thì
        {
            nếu (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            không_thì
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
              số_nguyên id,
              char_u *group)
{
    nếu (sign_name == NULL && group == NULL && id == -1)
    {
        emsg(_(e_argument_required));
        trả_về;
    }

    nếu (buf == NULL || (group != NULL && *group == '\0') || lnum >= 0 ||
        sign_name != NULL)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        trả_về;
    }

    (void)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
static số_nguyên
parse_sign_cmd_args(số_nguyên cmd,
                    char_u *arg,
                    char_u **sign_name,
                    số_nguyên *signid,
                    char_u **group,
                    số_nguyên *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = NULL;
    số_nguyên lnum_arg = FALSE;

    // first arg could be placed sign id
    nếu (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        nếu (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        không_thì
        {
            arg = skipwhite(arg);
        }
    }

    trong_khi (*arg != NUL)
    {
        nếu (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((char *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        không_thì nếu (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            nếu (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                trả_về FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        không_thì nếu (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            nếu (*arg != NUL)
                *arg++ = NUL;

            trong_khi (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        không_thì nếu (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            nếu (*arg != NUL)
                *arg++ = NUL;
        }
        không_thì nếu (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((char *)arg);
            arg = skiptowhite(arg);
        }
        không_thì nếu (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            break;
        }
        không_thì nếu (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((số_nguyên)getdigits(&arg));

            nếu (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            break;
        }
        không_thì
        {
            emsg(_(e_invalid_argument));
            trả_về FAIL;
        }

        arg = skipwhite(arg);
    }

    nếu (filename != NULL && *buf == NULL)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        trả_về FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    nếu (filename == NULL &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    trả_về OK;
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
    số_nguyên idx = sign_cmd_idx(arg, p);
    nếu (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        trả_về;
    }
    arg = skipwhite(p);

    nếu (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = NULL;
        số_nguyên id = -1;
        char_u *group = NULL;
        số_nguyên prio = -1;
        buf_T *buf = NULL;
        linenr_T lnum = -1;

        // Parse command line arguments
        nếu (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            trả_về;

        nếu (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        không_thì nếu (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        không_thì nếu (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        trả_về;
    }

    // Define, undefine or list signs.
    nếu (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        cho (sign_T *sp = first_sign; sp != NULL && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    không_thì nếu (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    không_thì
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        nếu (*p != NUL)
            *p++ = NUL;

        trong_khi (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        nếu (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        không_thì nếu (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        không_thì
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

    nếu (sp->sn_icon != NULL)
        dict_add_string(retdict, "icon", sp->sn_icon);

    nếu (sp->sn_text != NULL)
        dict_add_string(retdict, "text", sp->sn_text);

    nếu (sp->sn_priority > 0)
        dict_add_number(retdict, "priority", sp->sn_priority);

    nếu (sp->sn_line_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        nếu (p == NULL)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        không_thì
            dict_add_string(retdict, "linehl", p);
    }

    nếu (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        nếu (p == NULL)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        không_thì
            dict_add_string(retdict, "texthl", p);
    }

    nếu (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        nếu (p == NULL)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        không_thì
            dict_add_string(retdict, "culhl", p);
    }

    nếu (sp->sn_num_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        nếu (p == NULL)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        không_thì
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

    nếu (name != NULL)
    {
        sp = sign_find(name, NULL);
        nếu (sp == NULL)
            trả_về;
    }

    cho (; sp != NULL && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        nếu (dict == NULL)
            trả_về;

        nếu (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            trả_về;
        }

        sign_getinfo(sp, dict);

        nếu (name != NULL) // handle only the specified sign
            break;
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
        nếu (d != NULL && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
static void
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       số_nguyên sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    nếu (d == NULL)
        trả_về;

    nếu (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        trả_về;
    }

    dict_add_number(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    nếu (l == NULL)
        trả_về;

    nếu (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        trả_về;
    }

    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        nếu (!sign_in_group(sign, sign_group))
            continue;

        nếu ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            nếu (sdict != NULL && list_append_dict(l, sdict) == FAIL)
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
                số_nguyên sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    nếu (buf != NULL)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    không_thì
    {
        FOR_ALL_BUFFERS(buf)
        {
            nếu (buf->b_signlist != NULL)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# nếu defined(FEAT_SIGN_ICONS)
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
        nếu (sp->sn_icon != NULL)
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
    char lbuf[MSG_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    nếu (sp->sn_icon != NULL)
    {
        msg_puts(" icon=");
        msg_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        nếu (sp->sn_image == NULL)
            msg_puts(_(" (NOT FOUND)"));
# không_thì
        msg_puts(_(" (not supported)"));
# endif
    }

    nếu (sp->sn_text != NULL)
    {
        msg_puts(" text=");
        msg_outtrans(sp->sn_text);
    }

    nếu (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, MSG_BUF_LEN, " priority=%d", sp->sn_priority);
        msg_puts(lbuf);
    }

    nếu (sp->sn_line_hl > 0)
    {
        msg_puts(" linehl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        nếu (p == NULL)
            msg_puts("NONE");
        không_thì
            msg_puts((char *)p);
    }

    nếu (sp->sn_text_hl > 0)
    {
        msg_puts(" texthl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        nếu (p == NULL)
            msg_puts("NONE");
        không_thì
            msg_puts((char *)p);
    }

    nếu (sp->sn_cul_hl > 0)
    {
        msg_puts(" culhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        nếu (p == NULL)
            msg_puts("NONE");
        không_thì
            msg_puts((char *)p);
    }

    nếu (sp->sn_num_hl > 0)
    {
        msg_puts(" numhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        nếu (p == NULL)
            msg_puts("NONE");
        không_thì
            msg_puts((char *)p);
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
    nếu (sp->sn_image != NULL)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    nếu (sp_prev == NULL)
        first_sign = sp->sn_next;
    không_thì
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# nếu defined(FEAT_SIGN_ICONS)
void *
sign_get_image(số_nguyên typenr) // the attribute which may have a sign
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        nếu (sp->sn_typenr == typenr)
            trả_về sp->sn_image;
    }
    trả_về NULL;
}
# endif

/*
 * Undefine/free all signs.
 */
void
free_signs(void)
{
    trong_khi (first_sign != NULL)
        sign_undefine(first_sign, NULL);
}

static enum
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
get_nth_sign_name(số_nguyên idx)
{
    số_nguyên current_idx = 0;
    sign_T *sp = NULL;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        nếu (current_idx++ == idx)
            trả_về sp->sn_name;
    }
    trả_về NULL;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
static char_u *
get_nth_sign_group_name(số_nguyên idx)
{
    số_nguyên current_idx = 0;
    số_nguyên todo = (số_nguyên)sg_table.ht_used;
    hashitem_T *hi = NULL;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ITEMS(&sg_table, hi, todo)
    {
        nếu (!HASHITEM_EMPTY(hi))
        {
            --todo;
            nếu (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                trả_về group->sg_name;
            }
        }
    }
    trả_về NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, số_nguyên idx)
{
    switch (expand_what)
    {
        case EXP_SUBCMD:
            trả_về (char_u *)cmds[idx];
        case EXP_DEFINE:
        {
            char *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", NULL };
            trả_về (char_u *)define_arg[idx];
        }
        case EXP_PLACE:
        {
            char *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", NULL };
            trả_về (char_u *)place_arg[idx];
        }
        case EXP_LIST:
        {
            char *list_arg[] = { "group=", "file=", "buffer=", NULL };
            trả_về (char_u *)list_arg[idx];
        }
        case EXP_UNPLACE:
        {
            char *unplace_arg[] = { "group=", "file=", "buffer=", NULL };
            trả_về (char_u *)unplace_arg[idx];
        }
        case EXP_SIGN_NAMES:
            trả_về get_nth_sign_name(idx);
        case EXP_SIGN_GROUPS:
            trả_về get_nth_sign_group_name(idx);
        default:
            trả_về NULL;
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
    số_nguyên cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    nếu (*end_subcmd == NUL)
        trả_về;

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
    trong_khi (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    nếu (p == NULL)
    {
        // Expand last argument name (before equal sign).
        xp->xp_pattern = last;
        switch (cmd_idx)
        {
            case SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                break;
            case SIGNCMD_PLACE:
                // List placed signs
                nếu (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                không_thì
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                break;
            case SIGNCMD_LIST:
            case SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_NAMES;
                break;
            case SIGNCMD_JUMP:
            case SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                break;
            default:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    không_thì
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        switch (cmd_idx)
        {
            case SIGNCMD_DEFINE:
                nếu (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                không_thì nếu (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                không_thì
                    xp->xp_context = EXPAND_NOTHING;
                break;
            case SIGNCMD_PLACE:
                nếu (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                không_thì nếu (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                không_thì nếu (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                không_thì
                    xp->xp_context = EXPAND_NOTHING;
                break;
            case SIGNCMD_UNPLACE:
            case SIGNCMD_JUMP:
                nếu (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                không_thì nếu (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                không_thì
                    xp->xp_context = EXPAND_NOTHING;
                break;
            default:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
static số_nguyên
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = NULL;
    char_u *icon = NULL;
    char_u *linehl = NULL;
    char_u *text = NULL;
    char_u *texthl = NULL;
    char_u *culhl = NULL;
    char_u *numhl = NULL;
    số_nguyên prio = -1;
    số_nguyên retval = -1;

    nếu (name_arg == NULL && dict == NULL)
        trả_về retval;

    nếu (name_arg == NULL)
        name = dict_get_string(dict, "name", TRUE);
    không_thì
        name = vim_strsave(name_arg);

    nếu (name == NULL || name[0] == NUL)
        goto cleanup;

    nếu (dict != NULL)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_number_def(dict, "priority", -1);
    }

    nếu (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    trả_về retval;
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
        số_nguyên retval = -1;

        nếu (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(NULL, li->li_tv.vval.v_dict);
        không_thì
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
    nếu (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        trả_về;

    nếu (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        nếu (rettv_list_alloc(rettv) == FAIL)
            trả_về;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        trả_về;
    }

    // Define a single sign
    rettv->vval.v_number = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    nếu (name == NULL)
        trả_về;

    nếu (check_for_opt_dict_arg(argvars, 1) == FAIL)
        trả_về;

    rettv->vval.v_number = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : NULL);
}

/*
 * "sign_getdefined()" function
 */
void
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    nếu (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        trả_về;

    nếu (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        trả_về;

    char_u *name = NULL;
    nếu (argvars[0].v_type != VAR_UNKNOWN)
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
    số_nguyên sign_id = 0;
    char_u *group = NULL;

    nếu (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        trả_về;

    nếu (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        trả_về;

    nếu (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        nếu (buf == NULL)
            trả_về;

        nếu (argvars[1].v_type != VAR_UNKNOWN)
        {
            nếu (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                trả_về;

            dictitem_T *di = NULL;
            dict_T *dict = argvars[1].vval.v_dict;

            nếu ((di = dict_find(dict, (char_u *)"lnum", -1)) != NULL)
            {
                // get signs placed at this line
                số_nguyên notanum = FALSE;
                tv_get_number_chk(&di->di_tv, &notanum);
                nếu (notanum)
                    trả_về;

                lnum = tv_get_lnum(&di->di_tv);
            }

            nếu ((di = dict_find(dict, (char_u *)"id", -1)) != NULL)
            {
                // get sign placed with this identifier
                số_nguyên notanum = FALSE;
                sign_id = (số_nguyên)tv_get_number_chk(&di->di_tv, &notanum);
                nếu (notanum)
                    trả_về;
            }

            nếu ((di = dict_find(dict, (char_u *)"group", -1)) != NULL)
            {
                group = tv_get_string_chk(&di->di_tv);
                nếu (group == NULL)
                    trả_về;

                nếu (*group == '\0') // empty string means global group
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

    nếu (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        trả_về;

    số_nguyên notanum = FALSE;
    // Sign identifier
    số_nguyên sign_id = (số_nguyên)tv_get_number_chk(&argvars[0], &notanum);
    nếu (notanum)
        trả_về;

    nếu (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        trả_về;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    nếu (sign_group == NULL)
        trả_về;

    nếu (sign_group[0] == '\0')
    {
        sign_group = NULL; // global sign group
    }
    không_thì
    {
        sign_group = vim_strsave(sign_group);
        nếu (sign_group == NULL)
            trả_về;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    nếu (buf == NULL)
        goto cleanup;

    rettv->vval.v_number = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
static số_nguyên
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    số_nguyên sign_id = 0;
    char_u *group = NULL;
    char_u *sign_name = NULL;
    buf_T *buf = NULL;
    dictitem_T *di = NULL;
    linenr_T lnum = 0;
    số_nguyên prio = -1;
    số_nguyên ret_sign_id = -1;

    // sign identifier
    nếu (id_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        nếu (di != NULL)
            id_tv = &di->di_tv;
    }

    nếu (id_tv == NULL)
    {
        sign_id = 0;
    }
    không_thì
    {
        số_nguyên notanum = FALSE;
        sign_id = tv_get_number_chk(id_tv, &notanum);

        nếu (notanum)
            trả_về -1;

        nếu (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            trả_về -1;
        }
    }

    // sign group
    nếu (group_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        nếu (di != NULL)
            group_tv = &di->di_tv;
    }

    nếu (group_tv == NULL)
    {
        group = NULL; // global group
    }
    không_thì
    {
        group = tv_get_string_chk(group_tv);
        nếu (group == NULL)
            goto cleanup;

        nếu (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        không_thì
        {
            group = vim_strsave(group);
            nếu (group == NULL)
                trả_về -1;
        }
    }

    // sign name
    nếu (name_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        nếu (di != NULL)
            name_tv = &di->di_tv;
    }

    nếu (name_tv == NULL)
        goto cleanup;

    sign_name = tv_get_string_chk(name_tv);
    nếu (sign_name == NULL)
        goto cleanup;

    // buffer to place the sign
    nếu (buf_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        nếu (di != NULL)
            buf_tv = &di->di_tv;
    }

    nếu (buf_tv == NULL)
        goto cleanup;

    buf = get_buf_arg(buf_tv);
    nếu (buf == NULL)
        goto cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    nếu (di != NULL)
    {
        lnum = tv_get_lnum(&di->di_tv);
        nếu (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            goto cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    nếu (di != NULL)
    {
        số_nguyên notanum = FALSE;
        prio = (số_nguyên)tv_get_number_chk(&di->di_tv, &notanum);
        nếu (notanum)
            goto cleanup;
    }

    nếu (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    trả_về ret_sign_id;
}

/*
 * "sign_place()" function
 */
void
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = NULL;
    rettv->vval.v_number = -1;

    nếu (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        trả_về;

    nếu (argvars[4].v_type != VAR_UNKNOWN)
    {
        nếu (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            trả_về;
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
    nếu (rettv_list_alloc(rettv) == FAIL)
        trả_về;

    nếu (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        trả_về;

    nếu (check_for_list_arg(argvars, 0) == FAIL)
        trả_về;

    // Process the List of sign attributes
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        số_nguyên sign_id = -1;

        nếu (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(NULL, NULL, NULL, NULL,
                                           li->li_tv.vval.v_dict);
        không_thì
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
        số_nguyên retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        nếu (name != NULL && (sign_undefine_by_name(name, TRUE) == OK))
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

    nếu (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        trả_về;

    nếu (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        nếu (rettv_list_alloc(rettv) == FAIL)
            trả_về;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        trả_về;
    }

    rettv->vval.v_number = -1;

    nếu (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_number = 0;
    }
    không_thì
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        nếu (name == NULL)
            trả_về;

        nếu (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_number = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
static số_nguyên
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    số_nguyên sign_id = 0;
    buf_T *buf = NULL;
    char_u *group = NULL;
    số_nguyên retval = -1;

    // sign group
    group = (group_tv != NULL) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    nếu (group != NULL)
    {
        nếu (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        không_thì
        {
            group = vim_strsave(group);
            nếu (group == NULL)
                trả_về retval;
        }
    }

    nếu (dict != NULL)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        nếu (di != NULL)
        {
            buf = get_buf_arg(&di->di_tv);
            nếu (buf == NULL)
                goto cleanup;
        }

        nếu (dict_has_key(dict, "id"))
        {
            sign_id = dict_get_number(dict, "id");
            nếu (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                goto cleanup;
            }
        }
    }

    nếu (buf == NULL)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            nếu (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    không_thì nếu (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    trả_về retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    trong_khi (sign != NULL && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    trả_về sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
số_nguyên
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    nếu (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        trả_về get_first_valid_sign(wp) != NULL && !wp->w_p_nu && !wp->w_p_rnu;

    nếu (*wp->w_p_scl == 'n')
        trả_về FALSE;

    nếu (*wp->w_p_scl == 'y')
        trả_về TRUE;

    trả_về (get_first_valid_sign(wp) != NULL
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

    nếu ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_number = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
void
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    nếu (rettv_list_alloc(rettv) == FAIL)
        trả_về;

    nếu (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        trả_về;

    nếu (check_for_list_arg(argvars, 0) == FAIL)
        trả_về;

    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        số_nguyên retval = -1;

        nếu (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(NULL, li->li_tv.vval.v_dict);
        không_thì
            emsg(_(e_dictionary_required));

        list_append_number(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
