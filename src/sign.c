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

#если defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
typedef struct sign sign_T;

struct sign
{
    sign_T *sn_next; // next sign in list
    целое sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    void *sn_image; // icon image
# endif
    char_u *sn_text; // text used instead of pixmap
    целое sn_line_hl; // highlight ID for line
    целое sn_text_hl; // highlight ID for text
    целое sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    целое sn_num_hl; // highlight ID for line number
    целое sn_priority; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

static sign_T *first_sign = NULL;
static целое next_sign_typenr = 1;

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
     для ((sp) = first_sign; (sp) != NULL; (sp) = (sp)->sn_next)

static hashtab_T sg_table; // sign group (signgroup_T) hashtable
static целое next_sign_id = 1; // next sign id in the global group

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

    если (HASHITEM_EMPTY(hi))
    {
        // new group
        group = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        если (group == NULL)
            вернуть NULL;

        STRCPY(group->sg_name, groupname);
        group->sg_refcount = 1;
        group->sg_next_sign_id = 1;
        hash_add_item(&sg_table, hi, group->sg_name, hash);
    }
    иначе
    {
        // existing group
        group = HI2SG(hi);
        group->sg_refcount++;
    }

    вернуть group;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
static void
sign_group_unref(char_u *groupname)
{
    hashitem_T *hi = hash_find(&sg_table, groupname);
    если (HASHITEM_EMPTY(hi))
        вернуть;

    signgroup_T *group = HI2SG(hi);
    group->sg_refcount--;
    если (group->sg_refcount == 0)
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
static целое
sign_in_group(sign_entry_T *sign, char_u *group)
{
    вернуть ((group != NULL && STRCMP(group, "*") == 0) ||
            (group == NULL && sign->se_group == NULL) ||
            (group != NULL && sign->se_group != NULL &&
             STRCMP(group, sign->se_group->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
static целое
sign_group_for_window(sign_entry_T *sign, win_T *wp)
{
    целое for_popup = sign->se_group != NULL &&
                    STRNCMP("PopUp", sign->se_group->sg_name, 5) == 0;

    вернуть WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
static целое
sign_group_get_next_signid(buf_T *buf, char_u *groupname)
{
    целое id = 1;
    signgroup_T *group = NULL;
    sign_entry_T *sign = NULL;

    если (groupname != NULL)
    {
        hashitem_T *hi = hash_find(&sg_table, groupname);
        если (HASHITEM_EMPTY(hi))
            вернуть id;
        group = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    целое found = FALSE;
    пока (!found)
    {
        если (group == NULL)
            id = next_sign_id++; // global group
        иначе
            id = group->sg_next_sign_id++;

        // Check whether this sign is already placed in the buffer
        found = TRUE;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            если (id == sign->se_id && sign_in_group(sign, groupname))
            {
                found = FALSE; // sign identifier is in use
                break;
            }
        }
    }

    вернуть id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
static void
insert_sign(buf_T *buf, // buffer to store sign in
            sign_entry_T *prev, // previous sign entry
            sign_entry_T *next, // next sign entry
            целое id, // sign ID
            char_u *group, // sign group; NULL for global group
            целое prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            целое typenr) // typenr of sign we are adding
{
    sign_entry_T *newsign =
        lalloc_id(sizeof(sign_entry_T), FALSE, aid_insert_sign);
    если (newsign == NULL)
        вернуть;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    если (group != NULL)
    {
        newsign->se_group = sign_group_ref(group);
        если (newsign->se_group == NULL)
        {
            vim_free(newsign);
            вернуть;
        }
    }
    иначе
    {
        newsign->se_group = NULL;
    }

    newsign->se_priority = prio;
    newsign->se_next = next;
    newsign->se_prev = prev;
    если (next != NULL)
        next->se_prev = newsign;

    если (prev == NULL)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        если (buf->b_signlist == NULL)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            changed_line_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        если (netbeans_active())
            buf->b_has_sign_column = true;
# endif
    }
    иначе
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
                         целое id, // sign ID
                         char_u *group, // sign group; NULL for global group
                         целое prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         целое typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    пока (prev != NULL && prev->se_lnum == lnum && prev->se_priority <= prio)
        prev = prev->se_prev;

    sign_entry_T *sign = (prev == NULL) ? buf->b_signlist : prev->se_next;

    insert_sign(buf, prev, sign, id, group, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
static sign_T *
find_sign_by_typenr(целое typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        если (sp->sn_typenr == typenr)
            вернуть sp;
    вернуть NULL;
}

/*
 * Get the name of a sign by its typenr.
 */
static char_u *
sign_typenr2name(целое typenr)
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
        если (sp->sn_typenr == typenr)
            вернуть sp->sn_name;
    вернуть (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
static dict_T *
sign_get_info(sign_entry_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    если (d == NULL)
        вернуть NULL;

    dict_add_number(d, "id", sign->se_id);
    если (sign->se_group == NULL)
        dict_add_string_len(d, "group", (char_u *)"", 0);
    иначе
        dict_add_string(d, "group", sign->se_group->sg_name);
    dict_add_number(d, "lnum", sign->se_lnum);
    dict_add_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_add_number(d, "priority", sign->se_priority);

    вернуть d;
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
    если ((sign->se_prev == NULL || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_priority > sign->se_priority) &&
        (sign->se_next == NULL || sign->se_next->se_lnum != sign->se_lnum ||
         sign->se_next->se_priority < sign->se_priority))
        вернуть;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_entry_T *p = sign;
    пока (p->se_prev != NULL && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_priority <= sign->se_priority)
        p = p->se_prev;

    если (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_next;
        пока (p->se_next != NULL && p->se_next->se_lnum == sign->se_lnum &&
               p->se_next->se_priority > sign->se_priority)
            p = p->se_next;
    }

    // Remove 'sign' from the list
    если (buf->b_signlist == sign)
        buf->b_signlist = sign->se_next;

    если (sign->se_prev != NULL)
        sign->se_prev->se_next = sign->se_next;

    если (sign->se_next != NULL)
        sign->se_next->se_prev = sign->se_prev;

    sign->se_prev = NULL;
    sign->se_next = NULL;

    // Re-insert 'sign' at the right place
    если (p->se_priority <= sign->se_priority)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_next = p;
        p->se_prev = sign;
        если (sign->se_prev != NULL)
            sign->se_prev->se_next = sign;

        если (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    иначе
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_next = p->se_next;
        p->se_next = sign;
        если (sign->se_next != NULL)
            sign->se_next->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
static void
buf_addsign(buf_T *buf, // buffer to store sign in
            целое id, // sign ID
            char_u *groupname, // sign group
            целое prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            целое typenr) // typenr of sign we are adding
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    sign_entry_T *prev = NULL; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    если (buf->b_sign_finger != NULL && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    для (sign = (prev == NULL) ? buf->b_signlist : prev->se_next; sign != NULL;
         sign = sign->se_next)
    {
        если (lnum == sign->se_lnum && id == sign->se_id &&
            sign_in_group(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            вернуть;
        }
        иначе если (lnum < sign->se_lnum)
        {
            insert_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            вернуть;
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
                     целое markId, // sign ID
                     char_u *group, // sign group
                     целое typenr, // typenr of sign we are adding
                     целое prio) // sign priority
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        если (sign->se_id == markId && sign_in_group(sign, group))
        {
            sign->se_typenr = typenr;
            sign->se_priority = prio;
            sign_sort_by_prio_on_line(buf, sign);
            вернуть sign->se_lnum;
        }
    }

    вернуть (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
целое
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_POINTER(sattr);

    buf_T *buf = wp->w_buffer;
    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        если (sign->se_lnum > lnum)
            break;

        если (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_group_for_window(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = find_sign_by_typenr(sign->se_typenr);
            если (sp == NULL)
                вернуть FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_text = sp->sn_text;

            если (sattr->sat_text != NULL && sp->sn_text_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_text_hl);

            если (sp->sn_line_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_line_hl);

            если (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            если (sp->sn_num_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_num_hl);

            sattr->sat_priority = sign->se_priority;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            если (sign->se_next != NULL &&
                sign->se_next->se_priority == sign->se_priority &&
                sign->se_next->se_lnum == sign->se_lnum)
            {
                sign_T *next_sp = find_sign_by_typenr(sign->se_next->se_typenr);
                если (next_sp == NULL)
                    вернуть FALSE;

                если (sattr->sat_icon == NULL && sattr->sat_text == NULL)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = next_sp->sn_image;
# endif
                    sattr->sat_text = next_sp->sn_text;
                }

                если (sp->sn_text_hl <= 0 && next_sp->sn_text_hl > 0)
                    sattr->sat_texthl = syn_id2attr(next_sp->sn_text_hl);

                если (sp->sn_line_hl <= 0 && next_sp->sn_line_hl > 0)
                    sattr->sat_linehl = syn_id2attr(next_sp->sn_line_hl);

                если (sp->sn_cul_hl <= 0 && next_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(next_sp->sn_cul_hl);

                если (sp->sn_num_hl <= 0 && next_sp->sn_num_hl > 0)
                    sattr->sat_numhl = syn_id2attr(next_sp->sn_num_hl);
            }
            вернуть TRUE;
        }
    }
    вернуть FALSE;
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
            целое id, // sign id
            char_u *group) // sign group
{
    buf->b_sign_finger = NULL; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    для (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;

        если ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_in_group(sign, group))
        {
            *lastp = next;
            если (next != NULL)
                next->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            если (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
            redraw_buf_line_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            если (group == NULL || (*group != '*' && id != 0) ||
                (*group == '*' && atlnum != 0))
                break;
        }
        иначе
        {
            lastp = &sign->se_next;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    если (buf->b_signlist == NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    вернуть lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
целое
buf_findsign(buf_T *buf, // buffer to store sign in
             целое id, // sign ID
             char_u *group) // sign group
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
        если (sign->se_id == id && sign_in_group(sign, group))
            вернуть sign->se_lnum;

    вернуть 0;
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
        если (sign->se_lnum > lnum)
            break;

        если (sign->se_lnum == lnum && sign_in_group(sign, groupname))
            вернуть sign;
    }

    вернуть NULL;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
целое
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_entry_T *sign = buf_getsign_at_line(buf, lnum, groupname);
    если (sign != NULL)
        вернуть sign->se_id;

    вернуть 0;
}

# если defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
целое
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    целое typenr) // sign type number
{
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        если (sign->se_lnum > lnum)
            break;

        если (sign->se_lnum == lnum && sign->se_typenr == typenr)
            вернуть sign->se_id;
    }

    вернуть 0;
}

#  если defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
целое
buf_signcount(buf_T *buf, linenr_T lnum)
{
    целое count = 0;
    sign_entry_T *sign = NULL; // a sign in the signlist
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        если (sign->se_lnum > lnum)
            break;

        если (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != NULL)
            count++;
    }

    вернуть count;
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
    если (buf->b_signlist != NULL && curwin != NULL)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        changed_line_abv_curs();
    }

    // pointer to pointer to current sign
    sign_entry_T **lastp = &buf->b_signlist;
    sign_entry_T *next = NULL;

    для (sign_entry_T *sign = buf->b_signlist; sign != NULL; sign = next)
    {
        next = sign->se_next;
        если (sign_in_group(sign, group))
        {
            *lastp = next;

            если (next != NULL)
                next->se_prev = sign->se_prev;

            если (sign->se_group != NULL)
                sign_group_unref(sign->se_group->sg_name);

            vim_free(sign);
        }
        иначе
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
    пока (buf != NULL && !got_int)
    {
        если (buf->b_signlist != NULL)
        {
            vim_snprintf(lbuf, MSG_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            msg_puts_attr(lbuf, HL_ATTR(HLF_D));
            msg_putchar('\n');
        }

        sign_entry_T *sign = NULL;
        FOR_ALL_SIGNS_IN_BUF(buf, sign)
        {
            если (got_int)
                break;

            если (!sign_in_group(sign, sign_group))
                continue;

            если (sign->se_group != NULL)
                vim_snprintf(group, MSG_BUF_LEN, _("  group=%s"),
                             sign->se_group->sg_name);
            иначе
                group[0] = '\0';

            vim_snprintf(lbuf, MSG_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (long)sign->se_lnum, sign->se_id, group,
                         sign_typenr2name(sign->se_typenr), sign->se_priority);

            msg_puts(lbuf);
            msg_putchar('\n');
        }

        если (rbuf != NULL)
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
        если (sign->se_lnum < line1)
            continue;

        linenr_T new_lnum = sign->se_lnum;

        если (sign->se_lnum <= line2)
        {
            если (amount != MAXLNUM)
                new_lnum += amount;
        }
        иначе если (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        если (new_lnum <= curbuf->b_ml.ml_line_count)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
static целое
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *end_cmd) // just after sign subcmd
{
    целое idx = 0;
    char save = *end_cmd;
    *end_cmd = NUL;

    пока (cmds[idx] != NULL && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *end_cmd = save;
    вернуть idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
static sign_T *
sign_find(char_u *name, sign_T **sp_prev)
{
    если (sp_prev != NULL)
        *sp_prev = NULL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        если (STRCMP(sp->sn_name, name) == 0)
            break;

        если (sp_prev != NULL)
            *sp_prev = sp;
    }

    вернуть sp;
}

/*
 * Allocate a new sign
 */
static sign_T *
alloc_new_sign(char_u *name)
{
    целое start = next_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(sizeof(sign_T), aid_sign_define_by_name);
    если (sp == NULL)
        вернуть NULL;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = first_sign;
    пока (lp != NULL)
    {
        если (lp->sn_typenr == next_sign_typenr)
        {
            ++next_sign_typenr;

            если (next_sign_typenr == MAX_TYPENR)
                next_sign_typenr = 1;

            если (next_sign_typenr == start)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                вернуть NULL;
            }

            lp = first_sign; // start all over
            continue;
        }
        lp = lp->sn_next;
    }

    sp->sn_typenr = next_sign_typenr;

    если (++next_sign_typenr == MAX_TYPENR)
        next_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    если (sp->sn_name == NULL) // out of memory
    {
        vim_free(sp);
        вернуть NULL;
    }

    вернуть sp;
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
    если (gui.in_use)
    {
        out_flush();
        если (sp->sn_image != NULL)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_register_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
static целое
sign_define_init_text(sign_T *sp, char_u *text)
{
    char_u *s = NULL;
    char_u *endp = text + (целое)STRLEN(text);
    целое cells = 0;

    // Remove backslashes so that it is possible to use a space.
    для (s = text; s + 1 < endp; ++s)
    {
        если (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    если (has_mbyte)
    {
        для (s = text; s < endp; s += (*mb_ptr2len)(s))
        {
            если (!vim_isprintc((*mb_ptr2char)(s)))
                break;

            cells += (*mb_ptr2cells)(s);
        }
    }
    иначе
    {
        для (s = text; s < endp; ++s)
        {
            если (!vim_isprintc(*s))
                break;
        }
        cells = (целое)(s - text);
    }

    // Currently sign text must be one or two display cells
    если (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_text_str), text);
        вернуть FAIL;
    }

    vim_free(sp->sn_text);
    // Allocate one byte more if we need to pad up
    // with a space.
    целое len = (целое)(endp - text + ((cells == 1) ? 1 : 0));
    sp->sn_text = vim_strnsave(text, len);

    // For single character sign text, pad with a space.
    если (sp->sn_text != NULL && cells == 1)
        STRCPY(sp->sn_text + len - 1, " ");

    вернуть OK;
}

/*
 * Define a new sign or update an existing sign
 */
целое
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *text,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    целое prio)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    если (sp == NULL)
    {
        sp = alloc_new_sign(name);
        если (sp == NULL)
            вернуть FAIL;

        // add the new sign to the list of signs
        если (sp_prev == NULL)
            first_sign = sp;
        иначе
            sp_prev->sn_next = sp;
    }
    иначе
    {
        win_T *wp = NULL;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            если (wp->w_buffer->b_signlist != NULL)
                redraw_buf_later(wp->w_buffer, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    если (icon != NULL)
        sign_define_init_icon(sp, icon);

    если (text != NULL && (sign_define_init_text(sp, text) == FAIL))
        вернуть FAIL;

    sp->sn_priority = prio;

    если (linehl != NULL)
    {
        если (*linehl == NUL)
            sp->sn_line_hl = 0;
        иначе
            sp->sn_line_hl = syn_check_group(linehl, (целое)STRLEN(linehl));
    }

    если (texthl != NULL)
    {
        если (*texthl == NUL)
            sp->sn_text_hl = 0;
        иначе
            sp->sn_text_hl = syn_check_group(texthl, (целое)STRLEN(texthl));
    }

    если (culhl != NULL)
    {
        если (*culhl == NUL)
            sp->sn_cul_hl = 0;
        иначе
            sp->sn_cul_hl = syn_check_group(culhl, (целое)STRLEN(culhl));
    }

    если (numhl != NULL)
    {
        если (*numhl == NUL)
            sp->sn_num_hl = 0;
        иначе
            sp->sn_num_hl = syn_check_group(numhl, (целое)STRLEN(numhl));
    }

    вернуть OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
целое
sign_exists_by_name(char_u *name)
{
    вернуть sign_find(name, NULL) != NULL;
}

/*
 * Free the sign specified by 'name'.
 */
целое
sign_undefine_by_name(char_u *name, целое give_error)
{
    sign_T *sp_prev = NULL;
    sign_T *sp = sign_find(name, &sp_prev);
    если (sp == NULL)
    {
        если (give_error)
            semsg(_(e_unknown_sign_str), name);
        вернуть FAIL;
    }
    sign_undefine(sp, sp_prev);

    вернуть OK;
}

/*
 * List the signs matching 'name'
 */
static void
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_find(name, NULL);
    если (sp != NULL)
        sign_list_defined(sp);
    иначе
        semsg(_(e_unknown_sign_str), name);
}

static void
may_force_numberwidth_recompute(buf_T *buf, целое unplace)
{
# если defined(FEAT_LINEBREAK)
    tabpage_T *tp = NULL;
    win_T *wp = NULL;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        если (wp->w_buffer == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_width < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_line_count = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
целое
sign_place(целое *sign_id,
           char_u *sign_group,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           целое prio)
{
    // Check for reserved character '*' in group name
    если (sign_group != NULL && (*sign_group == '*' || *sign_group == '\0'))
        вернуть FAIL;

    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        если (STRCMP(sp->sn_name, sign_name) == 0)
            break;
    }

    если (sp == NULL)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        вернуть FAIL;
    }

    если (*sign_id == 0)
        *sign_id = sign_group_get_next_signid(buf, sign_group);

    // Use the default priority value for this sign.
    если (prio == -1)
        prio = (sp->sn_priority != -1) ? sp->sn_priority : SIGN_DEF_PRIO;

    если (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_group, prio, lnum, sp->sn_typenr);
    }
    иначе
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_group, sp->sn_typenr,
                                    prio);
    }

    если (lnum > 0)
    {
        redraw_buf_line_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    иначе
    {
        semsg(_(e_not_possible_to_change_sign_str), sign_name);
        вернуть FAIL;
    }

    вернуть OK;
}

/*
 * Unplace the specified sign
 */
static целое
sign_unplace(целое sign_id, char_u *sign_group, buf_T *buf, linenr_T atlnum)
{
    если (buf->b_signlist == NULL) // No signs in the buffer
        вернуть OK;

    если (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_group);
    }
    иначе
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_group);
        если (lnum == 0)
            вернуть FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    если (buf->b_signlist == NULL)
        may_force_numberwidth_recompute(buf, TRUE);

    вернуть OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
static void
sign_unplace_at_cursor(char_u *groupname)
{
    целое id =
        buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum, groupname);

    если (id > 0)
        sign_unplace(id, groupname, curwin->w_buffer, curwin->w_cursor.lnum);
    иначе
        emsg(_(e_missing_sign_number));
}

/*
 * Jump to a sign.
 */
static linenr_T
sign_jump(целое sign_id, char_u *sign_group, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_group);
    если (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        вернуть -1;
    }

    // goto a sign ...
    если (buf_jump_open_win(buf) != NULL)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    иначе
    { // ... not currently in a window
        если (buf->b_fname == NULL)
        {
            emsg(_(e_cannot_jump_to_buffer_that_does_not_have_name));
            вернуть -1;
        }
        setpcmark();
        если (do_ecmd(buf->b_fnum, NULL, NULL, NULL, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            вернуть -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    вернуть lnum;
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
    целое prio = -1;
    целое failed = FALSE;

    // set values for a defined sign.
    пока (TRUE)
    {
        arg = skipwhite(p);
        если (*arg == NUL)
            break;

        p = skiptowhite_esc(arg);
        если (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            text = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        иначе если (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((char *)arg);
        }
        иначе
        {
            semsg(_(e_invalid_argument_str), arg);
            failed = TRUE;
            break;
        }
    }

    если (!failed)
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
               целое id,
               char_u *group,
               целое prio)
{
    если (id <= 0)
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
        если (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
            emsg(_(e_invalid_argument));
        иначе
            sign_list_placed(buf, group);
    }
    иначе
    {
        // Place a new sign
        если (sign_name == NULL || buf == NULL ||
            (group != NULL && *group == '\0'))
        {
            emsg(_(e_invalid_argument));
            вернуть;
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
                 целое id,
                 char_u *group)
{
    если (lnum >= 0 || sign_name != NULL || (group != NULL && *group == '\0'))
    {
        emsg(_(e_invalid_argument));
        вернуть;
    }

    если (id == -2)
    {
        если (buf != NULL)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, group, buf, 0);
        }
        иначе
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                если (buf->b_signlist != NULL)
                    buf_delete_signs(buf, group);
            }
        }
    }
    иначе
    {
        если (buf != NULL)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, group, buf, 0);
        }
        иначе
        {
            если (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(group);
            }
            иначе
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
              целое id,
              char_u *group)
{
    если (sign_name == NULL && group == NULL && id == -1)
    {
        emsg(_(e_argument_required));
        вернуть;
    }

    если (buf == NULL || (group != NULL && *group == '\0') || lnum >= 0 ||
        sign_name != NULL)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_argument));
        вернуть;
    }

    (void)sign_jump(id, group, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
static целое
parse_sign_cmd_args(целое cmd,
                    char_u *arg,
                    char_u **sign_name,
                    целое *signid,
                    char_u **group,
                    целое *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = NULL;
    целое lnum_arg = FALSE;

    // first arg could be placed sign id
    если (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        если (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        иначе
        {
            arg = skipwhite(arg);
        }
    }

    пока (*arg != NUL)
    {
        если (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((char *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        иначе если (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            если (*signid != -1)
            {
                emsg(_(e_invalid_argument));
                вернуть FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        иначе если (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            если (*arg != NUL)
                *arg++ = NUL;

            пока (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        иначе если (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *group = arg;
            arg = skiptowhite(arg);
            если (*arg != NUL)
                *arg++ = NUL;
        }
        иначе если (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((char *)arg);
            arg = skiptowhite(arg);
        }
        иначе если (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            break;
        }
        иначе если (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((целое)getdigits(&arg));

            если (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            break;
        }
        иначе
        {
            emsg(_(e_invalid_argument));
            вернуть FAIL;
        }

        arg = skipwhite(arg);
    }

    если (filename != NULL && *buf == NULL)
    {
        semsg(_(e_invalid_buffer_name_str), filename);
        вернуть FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    если (filename == NULL &&
        ((cmd == SIGNCMD_PLACE && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_buffer;

    вернуть OK;
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
    целое idx = sign_cmd_idx(arg, p);
    если (idx == SIGNCMD_LAST)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        вернуть;
    }
    arg = skipwhite(p);

    если (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = NULL;
        целое id = -1;
        char_u *group = NULL;
        целое prio = -1;
        buf_T *buf = NULL;
        linenr_T lnum = -1;

        // Parse command line arguments
        если (parse_sign_cmd_args(idx, arg, &sign_name, &id, &group, &prio, &buf,
                                &lnum) == FAIL)
            вернуть;

        если (idx == SIGNCMD_PLACE)
            sign_place_cmd(buf, lnum, sign_name, id, group, prio);
        иначе если (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, group);
        иначе если (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, group);

        вернуть;
    }

    // Define, undefine or list signs.
    если (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        для (sign_T *sp = first_sign; sp != NULL && !got_int; sp = sp->sn_next)
            sign_list_defined(sp);
    }
    иначе если (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    иначе
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        если (*p != NUL)
            *p++ = NUL;

        пока (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        если (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        иначе если (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        иначе
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

    если (sp->sn_icon != NULL)
        dict_add_string(retdict, "icon", sp->sn_icon);

    если (sp->sn_text != NULL)
        dict_add_string(retdict, "text", sp->sn_text);

    если (sp->sn_priority > 0)
        dict_add_number(retdict, "priority", sp->sn_priority);

    если (sp->sn_line_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        если (p == NULL)
            dict_add_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        иначе
            dict_add_string(retdict, "linehl", p);
    }

    если (sp->sn_text_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        если (p == NULL)
            dict_add_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        иначе
            dict_add_string(retdict, "texthl", p);
    }

    если (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        если (p == NULL)
            dict_add_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        иначе
            dict_add_string(retdict, "culhl", p);
    }

    если (sp->sn_num_hl > 0)
    {
        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        если (p == NULL)
            dict_add_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        иначе
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

    если (name != NULL)
    {
        sp = sign_find(name, NULL);
        если (sp == NULL)
            вернуть;
    }

    для (; sp != NULL && !got_int; sp = sp->sn_next)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        если (dict == NULL)
            вернуть;

        если (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            вернуть;
        }

        sign_getinfo(sp, dict);

        если (name != NULL) // handle only the specified sign
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
        если (d != NULL && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
static void
sign_get_placed_in_buf(buf_T *buf,
                       linenr_T lnum,
                       целое sign_id,
                       char_u *sign_group,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    если (d == NULL)
        вернуть;

    если (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        вернуть;
    }

    dict_add_number(d, "bufnr", (long)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    если (l == NULL)
        вернуть;

    если (dict_add_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        вернуть;
    }

    sign_entry_T *sign = NULL;
    FOR_ALL_SIGNS_IN_BUF(buf, sign)
    {
        если (!sign_in_group(sign, sign_group))
            continue;

        если ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            если (sdict != NULL && list_append_dict(l, sdict) == FAIL)
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
                целое sign_id,
                char_u *sign_group,
                list_T *retlist)
{
    если (buf != NULL)
    {
        sign_get_placed_in_buf(buf, lnum, sign_id, sign_group, retlist);
    }
    иначе
    {
        FOR_ALL_BUFFERS(buf)
        {
            если (buf->b_signlist != NULL)
                sign_get_placed_in_buf(buf, 0, sign_id, sign_group, retlist);
        }
    }
}

# если defined(FEAT_SIGN_ICONS)
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
        если (sp->sn_icon != NULL)
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

    если (sp->sn_icon != NULL)
    {
        msg_puts(" icon=");
        msg_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        если (sp->sn_image == NULL)
            msg_puts(_(" (NOT FOUND)"));
# иначе
        msg_puts(_(" (not supported)"));
# endif
    }

    если (sp->sn_text != NULL)
    {
        msg_puts(" text=");
        msg_outtrans(sp->sn_text);
    }

    если (sp->sn_priority > 0)
    {
        vim_snprintf(lbuf, MSG_BUF_LEN, " priority=%d", sp->sn_priority);
        msg_puts(lbuf);
    }

    если (sp->sn_line_hl > 0)
    {
        msg_puts(" linehl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_line_hl - 1, FALSE);
        если (p == NULL)
            msg_puts("NONE");
        иначе
            msg_puts((char *)p);
    }

    если (sp->sn_text_hl > 0)
    {
        msg_puts(" texthl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_text_hl - 1, FALSE);
        если (p == NULL)
            msg_puts("NONE");
        иначе
            msg_puts((char *)p);
    }

    если (sp->sn_cul_hl > 0)
    {
        msg_puts(" culhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_cul_hl - 1, FALSE);
        если (p == NULL)
            msg_puts("NONE");
        иначе
            msg_puts((char *)p);
    }

    если (sp->sn_num_hl > 0)
    {
        msg_puts(" numhl=");

        char_u *p = get_highlight_name_ext(NULL, sp->sn_num_hl - 1, FALSE);
        если (p == NULL)
            msg_puts("NONE");
        иначе
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
    если (sp->sn_image != NULL)
    {
        out_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_text);

    если (sp_prev == NULL)
        first_sign = sp->sn_next;
    иначе
        sp_prev->sn_next = sp->sn_next;

    vim_free(sp);
}

# если defined(FEAT_SIGN_ICONS)
void *
sign_get_image(целое typenr) // the attribute which may have a sign
{
    sign_T *sp = NULL;
    FOR_ALL_SIGNS(sp)
    {
        если (sp->sn_typenr == typenr)
            вернуть sp->sn_image;
    }
    вернуть NULL;
}
# endif

/*
 * Undefine/free all signs.
 */
void
free_signs(void)
{
    пока (first_sign != NULL)
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
get_nth_sign_name(целое idx)
{
    целое current_idx = 0;
    sign_T *sp = NULL;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        если (current_idx++ == idx)
            вернуть sp->sn_name;
    }
    вернуть NULL;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
static char_u *
get_nth_sign_group_name(целое idx)
{
    целое current_idx = 0;
    целое todo = (целое)sg_table.ht_used;
    hashitem_T *hi = NULL;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_ITEMS(&sg_table, hi, todo)
    {
        если (!HASHITEM_EMPTY(hi))
        {
            --todo;
            если (current_idx++ == idx)
            {
                signgroup_T *group = HI2SG(hi);
                вернуть group->sg_name;
            }
        }
    }
    вернуть NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp UNUSED, целое idx)
{
    switch (expand_what)
    {
        case EXP_SUBCMD:
            вернуть (char_u *)cmds[idx];
        case EXP_DEFINE:
        {
            char *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", NULL };
            вернуть (char_u *)define_arg[idx];
        }
        case EXP_PLACE:
        {
            char *place_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", NULL };
            вернуть (char_u *)place_arg[idx];
        }
        case EXP_LIST:
        {
            char *list_arg[] = { "group=", "file=", "buffer=", NULL };
            вернуть (char_u *)list_arg[idx];
        }
        case EXP_UNPLACE:
        {
            char *unplace_arg[] = { "group=", "file=", "buffer=", NULL };
            вернуть (char_u *)unplace_arg[idx];
        }
        case EXP_SIGN_NAMES:
            вернуть get_nth_sign_name(idx);
        case EXP_SIGN_GROUPS:
            вернуть get_nth_sign_group_name(idx);
        default:
            вернуть NULL;
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
    целое cmd_idx;
    char_u *begin_subcmd_args;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    если (*end_subcmd == NUL)
        вернуть;

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
    пока (*p != NUL);

    p = vim_strchr(last, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    если (p == NULL)
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
                если (VIM_ISDIGIT(*begin_subcmd_args))
                    //   :sign place {id} {args}...
                    expand_what = EXP_PLACE;
                иначе
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
    иначе
    {
        // Expand last argument value (after equal sign).
        xp->xp_pattern = p + 1;
        switch (cmd_idx)
        {
            case SIGNCMD_DEFINE:
                если (STRNCMP(last, "texthl", 6) == 0 ||
                    STRNCMP(last, "linehl", 6) == 0 ||
                    STRNCMP(last, "culhl", 5) == 0 ||
                    STRNCMP(last, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                иначе если (STRNCMP(last, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                иначе
                    xp->xp_context = EXPAND_NOTHING;
                break;
            case SIGNCMD_PLACE:
                если (STRNCMP(last, "name", 4) == 0)
                    expand_what = EXP_SIGN_NAMES;
                иначе если (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                иначе если (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                иначе
                    xp->xp_context = EXPAND_NOTHING;
                break;
            case SIGNCMD_UNPLACE:
            case SIGNCMD_JUMP:
                если (STRNCMP(last, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                иначе если (STRNCMP(last, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                иначе
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
static целое
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = NULL;
    char_u *icon = NULL;
    char_u *linehl = NULL;
    char_u *text = NULL;
    char_u *texthl = NULL;
    char_u *culhl = NULL;
    char_u *numhl = NULL;
    целое prio = -1;
    целое retval = -1;

    если (name_arg == NULL && dict == NULL)
        вернуть retval;

    если (name_arg == NULL)
        name = dict_get_string(dict, "name", TRUE);
    иначе
        name = vim_strsave(name_arg);

    если (name == NULL || name[0] == NUL)
        goto cleanup;

    если (dict != NULL)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        text = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_number_def(dict, "priority", -1);
    }

    если (sign_define_by_name(name, icon, linehl, text, texthl, culhl, numhl,
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

    вернуть retval;
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
        целое retval = -1;

        если (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(NULL, li->li_tv.vval.v_dict);
        иначе
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
    если (in_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        вернуть;

    если (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        если (rettv_list_alloc(rettv) == FAIL)
            вернуть;

        sign_define_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        вернуть;
    }

    // Define a single sign
    rettv->vval.v_number = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    если (name == NULL)
        вернуть;

    если (check_for_opt_dict_arg(argvars, 1) == FAIL)
        вернуть;

    rettv->vval.v_number = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : NULL);
}

/*
 * "sign_getdefined()" function
 */
void
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    если (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        вернуть;

    если (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        вернуть;

    char_u *name = NULL;
    если (argvars[0].v_type != VAR_UNKNOWN)
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
    целое sign_id = 0;
    char_u *group = NULL;

    если (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        вернуть;

    если (in_vim9script() && (check_for_opt_buffer_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        вернуть;

    если (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        если (buf == NULL)
            вернуть;

        если (argvars[1].v_type != VAR_UNKNOWN)
        {
            если (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                вернуть;

            dictitem_T *di = NULL;
            dict_T *dict = argvars[1].vval.v_dict;

            если ((di = dict_find(dict, (char_u *)"lnum", -1)) != NULL)
            {
                // get signs placed at this line
                целое notanum = FALSE;
                tv_get_number_chk(&di->di_tv, &notanum);
                если (notanum)
                    вернуть;

                lnum = tv_get_lnum(&di->di_tv);
            }

            если ((di = dict_find(dict, (char_u *)"id", -1)) != NULL)
            {
                // get sign placed with this identifier
                целое notanum = FALSE;
                sign_id = (целое)tv_get_number_chk(&di->di_tv, &notanum);
                если (notanum)
                    вернуть;
            }

            если ((di = dict_find(dict, (char_u *)"group", -1)) != NULL)
            {
                group = tv_get_string_chk(&di->di_tv);
                если (group == NULL)
                    вернуть;

                если (*group == '\0') // empty string means global group
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

    если (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_buffer_arg(argvars, 2) == FAIL))
        вернуть;

    целое notanum = FALSE;
    // Sign identifier
    целое sign_id = (целое)tv_get_number_chk(&argvars[0], &notanum);
    если (notanum)
        вернуть;

    если (sign_id <= 0)
    {
        emsg(_(e_invalid_argument));
        вернуть;
    }

    // Sign group
    sign_group = tv_get_string_chk(&argvars[1]);
    если (sign_group == NULL)
        вернуть;

    если (sign_group[0] == '\0')
    {
        sign_group = NULL; // global sign group
    }
    иначе
    {
        sign_group = vim_strsave(sign_group);
        если (sign_group == NULL)
            вернуть;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    если (buf == NULL)
        goto cleanup;

    rettv->vval.v_number = sign_jump(sign_id, sign_group, buf);

cleanup:
    vim_free(sign_group);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
static целое
sign_place_from_dict(typval_T *id_tv,
                     typval_T *group_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    целое sign_id = 0;
    char_u *group = NULL;
    char_u *sign_name = NULL;
    buf_T *buf = NULL;
    dictitem_T *di = NULL;
    linenr_T lnum = 0;
    целое prio = -1;
    целое ret_sign_id = -1;

    // sign identifier
    если (id_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"id", -1);
        если (di != NULL)
            id_tv = &di->di_tv;
    }

    если (id_tv == NULL)
    {
        sign_id = 0;
    }
    иначе
    {
        целое notanum = FALSE;
        sign_id = tv_get_number_chk(id_tv, &notanum);

        если (notanum)
            вернуть -1;

        если (sign_id < 0)
        {
            emsg(_(e_invalid_argument));
            вернуть -1;
        }
    }

    // sign group
    если (group_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"group", -1);
        если (di != NULL)
            group_tv = &di->di_tv;
    }

    если (group_tv == NULL)
    {
        group = NULL; // global group
    }
    иначе
    {
        group = tv_get_string_chk(group_tv);
        если (group == NULL)
            goto cleanup;

        если (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        иначе
        {
            group = vim_strsave(group);
            если (group == NULL)
                вернуть -1;
        }
    }

    // sign name
    если (name_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"name", -1);
        если (di != NULL)
            name_tv = &di->di_tv;
    }

    если (name_tv == NULL)
        goto cleanup;

    sign_name = tv_get_string_chk(name_tv);
    если (sign_name == NULL)
        goto cleanup;

    // buffer to place the sign
    если (buf_tv == NULL)
    {
        di = dict_find(dict, (char_u *)"buffer", -1);
        если (di != NULL)
            buf_tv = &di->di_tv;
    }

    если (buf_tv == NULL)
        goto cleanup;

    buf = get_buf_arg(buf_tv);
    если (buf == NULL)
        goto cleanup;

    // line number of the sign
    di = dict_find(dict, (char_u *)"lnum", -1);
    если (di != NULL)
    {
        lnum = tv_get_lnum(&di->di_tv);
        если (lnum <= 0)
        {
            emsg(_(e_invalid_argument));
            goto cleanup;
        }
    }

    // sign priority
    di = dict_find(dict, (char_u *)"priority", -1);
    если (di != NULL)
    {
        целое notanum = FALSE;
        prio = (целое)tv_get_number_chk(&di->di_tv, &notanum);
        если (notanum)
            goto cleanup;
    }

    если (sign_place(&sign_id, group, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(group);

    вернуть ret_sign_id;
}

/*
 * "sign_place()" function
 */
void
f_sign_place(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = NULL;
    rettv->vval.v_number = -1;

    если (in_vim9script() && (check_for_number_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_buffer_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        вернуть;

    если (argvars[4].v_type != VAR_UNKNOWN)
    {
        если (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            вернуть;
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
    если (rettv_list_alloc(rettv) == FAIL)
        вернуть;

    если (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        вернуть;

    если (check_for_list_arg(argvars, 0) == FAIL)
        вернуть;

    // Process the List of sign attributes
    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        целое sign_id = -1;

        если (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_place_from_dict(NULL, NULL, NULL, NULL,
                                           li->li_tv.vval.v_dict);
        иначе
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
        целое retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        если (name != NULL && (sign_undefine_by_name(name, TRUE) == OK))
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

    если (in_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        вернуть;

    если (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        если (rettv_list_alloc(rettv) == FAIL)
            вернуть;

        sign_undefine_multiple(argvars[0].vval.v_list, rettv->vval.v_list);
        вернуть;
    }

    rettv->vval.v_number = -1;

    если (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_number = 0;
    }
    иначе
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        если (name == NULL)
            вернуть;

        если (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_number = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
static целое
sign_unplace_from_dict(typval_T *group_tv, dict_T *dict)
{
    целое sign_id = 0;
    buf_T *buf = NULL;
    char_u *group = NULL;
    целое retval = -1;

    // sign group
    group = (group_tv != NULL) ? tv_get_string(group_tv)
                               : dict_get_string(dict, "group", FALSE);

    если (group != NULL)
    {
        если (group[0] == '\0') // global sign group
        {
            group = NULL;
        }
        иначе
        {
            group = vim_strsave(group);
            если (group == NULL)
                вернуть retval;
        }
    }

    если (dict != NULL)
    {
        dictitem_T *di = dict_find(dict, (char_u *)"buffer", -1);
        если (di != NULL)
        {
            buf = get_buf_arg(&di->di_tv);
            если (buf == NULL)
                goto cleanup;
        }

        если (dict_has_key(dict, "id"))
        {
            sign_id = dict_get_number(dict, "id");
            если (sign_id <= 0)
            {
                emsg(_(e_invalid_argument));
                goto cleanup;
            }
        }
    }

    если (buf == NULL)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            если (sign_unplace(sign_id, group, buf, 0) != OK)
                retval = -1;
    }
    иначе если (sign_unplace(sign_id, group, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(group);

    вернуть retval;
}

sign_entry_T *
get_first_valid_sign(win_T *wp)
{
    sign_entry_T *sign = wp->w_buffer->b_signlist;

# ifdef FEAT_PROP_POPUP
    пока (sign != NULL && !sign_group_for_window(sign, wp))
        sign = sign->se_next;
# endif
    вернуть sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
целое
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    если (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        вернуть get_first_valid_sign(wp) != NULL && !wp->w_p_nu && !wp->w_p_rnu;

    если (*wp->w_p_scl == 'n')
        вернуть FALSE;

    если (*wp->w_p_scl == 'y')
        вернуть TRUE;

    вернуть (get_first_valid_sign(wp) != NULL
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

    если ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        вернуть;

    если (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_number = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
void
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    если (rettv_list_alloc(rettv) == FAIL)
        вернуть;

    если (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        вернуть;

    если (check_for_list_arg(argvars, 0) == FAIL)
        вернуть;

    listitem_T *li = NULL;
    FOR_ALL_LIST_ITEMS(argvars[0].vval.v_list, li)
    {
        целое retval = -1;

        если (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(NULL, li->li_tv.vval.v_dict);
        иначе
            emsg(_(e_dictionary_required));

        list_append_number(rettv->vval.v_list, retval);
    }
}

#endif // FEAT_SIGNS
