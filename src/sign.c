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

#ضمن "vim.h"

#إذا defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
تعريفنوع بنية sign sign_T;

بنية sign
{
    sign_T *sn_بل; // next sign in list
    صحيح sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    فراغ *sn_image; // icon image
# endif
    char_u *sn_متن; // text used instead of pixmap
    صحيح sn_کرښه_hl; // highlight ID for line
    صحيح sn_متن_hl; // highlight ID for text
    صحيح sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    صحيح sn_عدد_hl; // highlight ID for line number
    صحيح sn_لومړیتوب; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ساكن sign_T *لومړی_sign = عدم;
ساكن صحيح بل_sign_typenr = 1;

ساكن فراغ sign_list_defined(sign_T *sp);
ساكن فراغ sign_undefine(sign_T *sp, sign_T *sp_prev);

ساكن حرف *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_ځای_پرځای_کړه 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        عدم
# define SIGNCMD_وروستی 6
};

# define FOR_ALL_SIGNS(sp) \
     لكل ((sp) = لومړی_sign; (sp) != عدم; (sp) = (sp)->sn_بل)

ساكن hashtab_T sg_جدول; // sign group (signgroup_T) hashtable
ساكن صحيح بل_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
فراغ
init_signs(فراغ)
{
    hash_init(&sg_جدول); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ساكن signgroup_T *
sign_ډله_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_جدول, groupname, hash);
    signgroup_T *ډله = عدم;

    إذا (HASHITEM_تشه(hi))
    {
        // new group
        ډله = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        إذا (ډله == عدم)
            أرجع عدم;

        STRCPY(ډله->sg_name, groupname);
        ډله->sg_refcount = 1;
        ډله->sg_بل_sign_id = 1;
        hash_جمع_کړه_توکی(&sg_جدول, hi, ډله->sg_name, hash);
    }
    وإلا
    {
        // existing group
        ډله = HI2SG(hi);
        ډله->sg_refcount++;
    }

    أرجع ډله;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ساكن فراغ
sign_ډله_unref(char_u *groupname)
{
    hashitem_T *hi = hash_پیدا_کړه(&sg_جدول, groupname);
    إذا (HASHITEM_تشه(hi))
        أرجع;

    signgroup_T *ډله = HI2SG(hi);
    ډله->sg_refcount--;
    إذا (ډله->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_جدول, hi, "sign remove");
        vim_free(ډله);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
ساكن صحيح
sign_دننه_ډله(sign_ثبت_T *sign, char_u *ډله)
{
    أرجع ((ډله != عدم && STRCMP(ډله, "*") == 0) ||
            (ډله == عدم && sign->se_ډله == عدم) ||
            (ډله != عدم && sign->se_ډله != عدم &&
             STRCMP(ډله, sign->se_ډله->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ساكن صحيح
sign_ډله_for_کړکۍ(sign_ثبت_T *sign, win_T *wp)
{
    صحيح for_popup = sign->se_ډله != عدم &&
                    STRNCMP("PopUp", sign->se_ډله->sg_name, 5) == 0;

    أرجع WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ساكن صحيح
sign_ډله_get_بل_signid(buf_T *buf, char_u *groupname)
{
    صحيح id = 1;
    signgroup_T *ډله = عدم;
    sign_ثبت_T *sign = عدم;

    إذا (groupname != عدم)
    {
        hashitem_T *hi = hash_پیدا_کړه(&sg_جدول, groupname);
        إذا (HASHITEM_تشه(hi))
            أرجع id;
        ډله = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    صحيح وموندل = FALSE;
    طالما (!وموندل)
    {
        إذا (ډله == عدم)
            id = بل_sign_id++; // global group
        وإلا
            id = ډله->sg_بل_sign_id++;

        // Check whether this sign is already placed in the buffer
        وموندل = TRUE;
        FOR_ALL_SIGNS_دننه_BUF(buf, sign)
        {
            إذا (id == sign->se_id && sign_دننه_ډله(sign, groupname))
            {
                وموندل = FALSE; // sign identifier is in use
                اكسر;
            }
        }
    }

    أرجع id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ساكن فراغ
ورننباسه_sign(buf_T *buf, // buffer to store sign in
            sign_ثبت_T *prev, // previous sign entry
            sign_ثبت_T *بل, // next sign entry
            صحيح id, // sign ID
            char_u *ډله, // sign group; NULL for global group
            صحيح prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            صحيح typenr) // typenr of sign we are adding
{
    sign_ثبت_T *newsign =
        lalloc_id(حجم(sign_ثبت_T), FALSE, aid_ورننباسه_sign);
    إذا (newsign == عدم)
        أرجع;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    إذا (ډله != عدم)
    {
        newsign->se_ډله = sign_ډله_ref(ډله);
        إذا (newsign->se_ډله == عدم)
        {
            vim_free(newsign);
            أرجع;
        }
    }
    وإلا
    {
        newsign->se_ډله = عدم;
    }

    newsign->se_لومړیتوب = prio;
    newsign->se_بل = بل;
    newsign->se_prev = prev;
    إذا (بل != عدم)
        بل->se_prev = newsign;

    إذا (prev == عدم)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        إذا (buf->b_signlist == عدم)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            بدل_شوی_کرښه_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        إذا (netbeans_فعال())
            buf->b_has_sign_ستنه = true;
# endif
    }
    وإلا
    {
        prev->se_بل = newsign;
    }

    // Remember the newly inserted sign so a following insert on a later line
    // can resume the search from here.
    buf->b_sign_finger = newsign;
}

/*
 * Insert a new sign sorted by line number and sign priority.
 */
ساكن فراغ
ورننباسه_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_ثبت_T *prev, // previous sign entry
                         صحيح id, // sign ID
                         char_u *ډله, // sign group; NULL for global group
                         صحيح prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         صحيح typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    طالما (prev != عدم && prev->se_lnum == lnum && prev->se_لومړیتوب <= prio)
        prev = prev->se_prev;

    sign_ثبت_T *sign = (prev == عدم) ? buf->b_signlist : prev->se_بل;

    ورننباسه_sign(buf, prev, sign, id, ډله, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ساكن sign_T *
پیدا_کړه_sign_by_typenr(صحيح typenr)
{
    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
        إذا (sp->sn_typenr == typenr)
            أرجع sp;
    أرجع عدم;
}

/*
 * Get the name of a sign by its typenr.
 */
ساكن char_u *
sign_typenr2name(صحيح typenr)
{
    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
        إذا (sp->sn_typenr == typenr)
            أرجع sp->sn_name;
    أرجع (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ساكن dict_T *
sign_get_info(sign_ثبت_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    إذا (d == عدم)
        أرجع عدم;

    dict_جمع_کړه_شمېره(d, "id", sign->se_id);
    إذا (sign->se_ډله == عدم)
        dict_جمع_کړه_string_len(d, "group", (char_u *)"", 0);
    وإلا
        dict_جمع_کړه_string(d, "group", sign->se_ډله->sg_name);
    dict_جمع_کړه_شمېره(d, "lnum", sign->se_lnum);
    dict_جمع_کړه_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_جمع_کړه_شمېره(d, "priority", sign->se_لومړیتوب);

    أرجع d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ساكن فراغ
sign_ترتیبول_by_prio_on_کرښه(buf_T *buf, sign_ثبت_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    إذا ((sign->se_prev == عدم || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_لومړیتوب > sign->se_لومړیتوب) &&
        (sign->se_بل == عدم || sign->se_بل->se_lnum != sign->se_lnum ||
         sign->se_بل->se_لومړیتوب < sign->se_لومړیتوب))
        أرجع;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_ثبت_T *p = sign;
    طالما (p->se_prev != عدم && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_لومړیتوب <= sign->se_لومړیتوب)
        p = p->se_prev;

    إذا (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_بل;
        طالما (p->se_بل != عدم && p->se_بل->se_lnum == sign->se_lnum &&
               p->se_بل->se_لومړیتوب > sign->se_لومړیتوب)
            p = p->se_بل;
    }

    // Remove 'sign' from the list
    إذا (buf->b_signlist == sign)
        buf->b_signlist = sign->se_بل;

    إذا (sign->se_prev != عدم)
        sign->se_prev->se_بل = sign->se_بل;

    إذا (sign->se_بل != عدم)
        sign->se_بل->se_prev = sign->se_prev;

    sign->se_prev = عدم;
    sign->se_بل = عدم;

    // Re-insert 'sign' at the right place
    إذا (p->se_لومړیتوب <= sign->se_لومړیتوب)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_بل = p;
        p->se_prev = sign;
        إذا (sign->se_prev != عدم)
            sign->se_prev->se_بل = sign;

        إذا (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    وإلا
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_بل = p->se_بل;
        p->se_بل = sign;
        إذا (sign->se_بل != عدم)
            sign->se_بل->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ساكن فراغ
buf_addsign(buf_T *buf, // buffer to store sign in
            صحيح id, // sign ID
            char_u *groupname, // sign group
            صحيح prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            صحيح typenr) // typenr of sign we are adding
{
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    sign_ثبت_T *prev = عدم; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    إذا (buf->b_sign_finger != عدم && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    لكل (sign = (prev == عدم) ? buf->b_signlist : prev->se_بل; sign != عدم;
         sign = sign->se_بل)
    {
        إذا (lnum == sign->se_lnum && id == sign->se_id &&
            sign_دننه_ډله(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_لومړیتوب = prio;
            sign_ترتیبول_by_prio_on_کرښه(buf, sign);
            أرجع;
        }
        وإلا إذا (lnum < sign->se_lnum)
        {
            ورننباسه_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            أرجع;
        }
        prev = sign;
    }

    ورننباسه_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
ساكن linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     صحيح markId, // sign ID
                     char_u *ډله, // sign group
                     صحيح typenr, // typenr of sign we are adding
                     صحيح prio) // sign priority
{
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        إذا (sign->se_id == markId && sign_دننه_ډله(sign, ډله))
        {
            sign->se_typenr = typenr;
            sign->se_لومړیتوب = prio;
            sign_ترتیبول_by_prio_on_کرښه(buf, sign);
            أرجع sign->se_lnum;
        }
    }

    أرجع (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
صحيح
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_اشاره_ګر(sattr);

    buf_T *buf = wp->w_بفر;
    sign_ثبت_T *sign = عدم;
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        إذا (sign->se_lnum > lnum)
            اكسر;

        إذا (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_ډله_for_کړکۍ(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = پیدا_کړه_sign_by_typenr(sign->se_typenr);
            إذا (sp == عدم)
                أرجع FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_متن = sp->sn_متن;

            إذا (sattr->sat_متن != عدم && sp->sn_متن_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_متن_hl);

            إذا (sp->sn_کرښه_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_کرښه_hl);

            إذا (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            إذا (sp->sn_عدد_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_عدد_hl);

            sattr->sat_لومړیتوب = sign->se_لومړیتوب;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            إذا (sign->se_بل != عدم &&
                sign->se_بل->se_لومړیتوب == sign->se_لومړیتوب &&
                sign->se_بل->se_lnum == sign->se_lnum)
            {
                sign_T *بل_sp = پیدا_کړه_sign_by_typenr(sign->se_بل->se_typenr);
                إذا (بل_sp == عدم)
                    أرجع FALSE;

                إذا (sattr->sat_icon == عدم && sattr->sat_متن == عدم)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = بل_sp->sn_image;
# endif
                    sattr->sat_متن = بل_sp->sn_متن;
                }

                إذا (sp->sn_متن_hl <= 0 && بل_sp->sn_متن_hl > 0)
                    sattr->sat_texthl = syn_id2attr(بل_sp->sn_متن_hl);

                إذا (sp->sn_کرښه_hl <= 0 && بل_sp->sn_کرښه_hl > 0)
                    sattr->sat_linehl = syn_id2attr(بل_sp->sn_کرښه_hl);

                إذا (sp->sn_cul_hl <= 0 && بل_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(بل_sp->sn_cul_hl);

                إذا (sp->sn_عدد_hl <= 0 && بل_sp->sn_عدد_hl > 0)
                    sattr->sat_numhl = syn_id2attr(بل_sp->sn_عدد_hl);
            }
            أرجع TRUE;
        }
    }
    أرجع FALSE;
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
            صحيح id, // sign id
            char_u *ډله) // sign group
{
    buf->b_sign_finger = عدم; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_ثبت_T **lastp = &buf->b_signlist;
    sign_ثبت_T *بل = عدم; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    لكل (sign_ثبت_T *sign = buf->b_signlist; sign != عدم; sign = بل)
    {
        بل = sign->se_بل;

        إذا ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_دننه_ډله(sign, ډله))
        {
            *lastp = بل;
            إذا (بل != عدم)
                بل->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            إذا (sign->se_ډله != عدم)
                sign_ډله_unref(sign->se_ډله->sg_name);

            vim_free(sign);
            redraw_buf_کرښه_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            إذا (ډله == عدم || (*ډله != '*' && id != 0) ||
                (*ډله == '*' && atlnum != 0))
                اكسر;
        }
        وإلا
        {
            lastp = &sign->se_بل;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    إذا (buf->b_signlist == عدم)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        بدل_شوی_کرښه_abv_curs();
    }

    أرجع lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
صحيح
buf_findsign(buf_T *buf, // buffer to store sign in
             صحيح id, // sign ID
             char_u *ډله) // sign group
{
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
        إذا (sign->se_id == id && sign_دننه_ډله(sign, ډله))
            أرجع sign->se_lnum;

    أرجع 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ساكن sign_ثبت_T *
buf_getsign_at_کرښه(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        إذا (sign->se_lnum > lnum)
            اكسر;

        إذا (sign->se_lnum == lnum && sign_دننه_ډله(sign, groupname))
            أرجع sign;
    }

    أرجع عدم;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
صحيح
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_ثبت_T *sign = buf_getsign_at_کرښه(buf, lnum, groupname);
    إذا (sign != عدم)
        أرجع sign->se_id;

    أرجع 0;
}

# إذا defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
صحيح
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    صحيح typenr) // sign type number
{
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        إذا (sign->se_lnum > lnum)
            اكسر;

        إذا (sign->se_lnum == lnum && sign->se_typenr == typenr)
            أرجع sign->se_id;
    }

    أرجع 0;
}

#  إذا defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
صحيح
buf_signcount(buf_T *buf, linenr_T lnum)
{
    صحيح شمېر = 0;
    sign_ثبت_T *sign = عدم; // a sign in the signlist
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        إذا (sign->se_lnum > lnum)
            اكسر;

        إذا (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != عدم)
            شمېر++;
    }

    أرجع شمېر;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
فراغ
buf_delete_signs(buf_T *buf, char_u *ډله)
{
    buf->b_sign_finger = عدم; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    إذا (buf->b_signlist != عدم && curwin != عدم)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        بدل_شوی_کرښه_abv_curs();
    }

    // pointer to pointer to current sign
    sign_ثبت_T **lastp = &buf->b_signlist;
    sign_ثبت_T *بل = عدم;

    لكل (sign_ثبت_T *sign = buf->b_signlist; sign != عدم; sign = بل)
    {
        بل = sign->se_بل;
        إذا (sign_دننه_ډله(sign, ډله))
        {
            *lastp = بل;

            إذا (بل != عدم)
                بل->se_prev = sign->se_prev;

            إذا (sign->se_ډله != عدم)
                sign_ډله_unref(sign->se_ډله->sg_name);

            vim_free(sign);
        }
        وإلا
        {
            lastp = &sign->se_بل;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ساكن فراغ
sign_list_placed(buf_T *rbuf, char_u *sign_ډله)
{
    حرف lbuf[پیغام_BUF_LEN];
    حرف ډله[پیغام_BUF_LEN];

    پیغام_puts_title(_("\n--- Signs ---"));
    پیغام_putchar('\n');

    buf_T *buf = (rbuf == عدم) ? firstbuf : rbuf;
    طالما (buf != عدم && !got_int)
    {
        إذا (buf->b_signlist != عدم)
        {
            vim_snprintf(lbuf, پیغام_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            پیغام_puts_attr(lbuf, HL_ATTR(HLF_D));
            پیغام_putchar('\n');
        }

        sign_ثبت_T *sign = عدم;
        FOR_ALL_SIGNS_دننه_BUF(buf, sign)
        {
            إذا (got_int)
                اكسر;

            إذا (!sign_دننه_ډله(sign, sign_ډله))
                استمر;

            إذا (sign->se_ډله != عدم)
                vim_snprintf(ډله, پیغام_BUF_LEN, _("  group=%s"),
                             sign->se_ډله->sg_name);
            وإلا
                ډله[0] = '\0';

            vim_snprintf(lbuf, پیغام_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (اوږد)sign->se_lnum, sign->se_id, ډله,
                         sign_typenr2name(sign->se_typenr), sign->se_لومړیتوب);

            پیغام_puts(lbuf);
            پیغام_putchar('\n');
        }

        إذا (rbuf != عدم)
            اكسر;

        buf = buf->b_بل;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
فراغ
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    اوږد        amount,
    اوږد        amount_after)
{
    curbuf->b_sign_finger = عدم; // line changes may reorder, drop the finger
    sign_ثبت_T *sign = عدم; // a sign in a b_signlist
    FOR_ALL_SIGNS_دننه_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        إذا (sign->se_lnum < line1)
            استمر;

        linenr_T new_lnum = sign->se_lnum;

        إذا (sign->se_lnum <= line2)
        {
            إذا (amount != MAXLNUM)
                new_lnum += amount;
        }
        وإلا إذا (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        إذا (new_lnum <= curbuf->b_ml.ml_کرښه_شمېر)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ساكن صحيح
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *پای_ټکی_cmd) // just after sign subcmd
{
    صحيح idx = 0;
    حرف save = *پای_ټکی_cmd;
    *پای_ټکی_cmd = NUL;

    طالما (cmds[idx] != عدم && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *پای_ټکی_cmd = save;
    أرجع idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ساكن sign_T *
sign_پیدا_کړه(char_u *name, sign_T **sp_prev)
{
    إذا (sp_prev != عدم)
        *sp_prev = عدم;

    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
    {
        إذا (STRCMP(sp->sn_name, name) == 0)
            اكسر;

        إذا (sp_prev != عدم)
            *sp_prev = sp;
    }

    أرجع sp;
}

/*
 * Allocate a new sign
 */
ساكن sign_T *
alloc_new_sign(char_u *name)
{
    صحيح پیل = بل_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(حجم(sign_T), aid_sign_define_by_name);
    إذا (sp == عدم)
        أرجع عدم;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = لومړی_sign;
    طالما (lp != عدم)
    {
        إذا (lp->sn_typenr == بل_sign_typenr)
        {
            ++بل_sign_typenr;

            إذا (بل_sign_typenr == MAX_TYPENR)
                بل_sign_typenr = 1;

            إذا (بل_sign_typenr == پیل)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                أرجع عدم;
            }

            lp = لومړی_sign; // start all over
            استمر;
        }
        lp = lp->sn_بل;
    }

    sp->sn_typenr = بل_sign_typenr;

    إذا (++بل_sign_typenr == MAX_TYPENR)
        بل_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    إذا (sp->sn_name == عدم) // out of memory
    {
        vim_free(sp);
        أرجع عدم;
    }

    أرجع sp;
}

/*
 * Initialize the icon information for a new sign
 */
ساكن فراغ
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    إذا (gui.دننه_use)
    {
        بهر_flush();
        إذا (sp->sn_image != عدم)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_ثبتوونکی_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ساكن صحيح
sign_define_init_متن(sign_T *sp, char_u *متن)
{
    char_u *s = عدم;
    char_u *endp = متن + (صحيح)STRLEN(متن);
    صحيح cells = 0;

    // Remove backslashes so that it is possible to use a space.
    لكل (s = متن; s + 1 < endp; ++s)
    {
        إذا (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    إذا (has_mbyte)
    {
        لكل (s = متن; s < endp; s += (*mb_ptr2len)(s))
        {
            إذا (!vim_isprintc((*mb_ptr2char)(s)))
                اكسر;

            cells += (*mb_ptr2cells)(s);
        }
    }
    وإلا
    {
        لكل (s = متن; s < endp; ++s)
        {
            إذا (!vim_isprintc(*s))
                اكسر;
        }
        cells = (صحيح)(s - متن);
    }

    // Currently sign text must be one or two display cells
    إذا (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_متن_str), متن);
        أرجع FAIL;
    }

    vim_free(sp->sn_متن);
    // Allocate one byte more if we need to pad up
    // with a space.
    صحيح len = (صحيح)(endp - متن + ((cells == 1) ? 1 : 0));
    sp->sn_متن = vim_strnsave(متن, len);

    // For single character sign text, pad with a space.
    إذا (sp->sn_متن != عدم && cells == 1)
        STRCPY(sp->sn_متن + len - 1, " ");

    أرجع OK;
}

/*
 * Define a new sign or update an existing sign
 */
صحيح
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *متن,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    صحيح prio)
{
    sign_T *sp_prev = عدم;
    sign_T *sp = sign_پیدا_کړه(name, &sp_prev);
    إذا (sp == عدم)
    {
        sp = alloc_new_sign(name);
        إذا (sp == عدم)
            أرجع FAIL;

        // add the new sign to the list of signs
        إذا (sp_prev == عدم)
            لومړی_sign = sp;
        وإلا
            sp_prev->sn_بل = sp;
    }
    وإلا
    {
        win_T *wp = عدم;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            إذا (wp->w_بفر->b_signlist != عدم)
                redraw_buf_later(wp->w_بفر, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    إذا (icon != عدم)
        sign_define_init_icon(sp, icon);

    إذا (متن != عدم && (sign_define_init_متن(sp, متن) == FAIL))
        أرجع FAIL;

    sp->sn_لومړیتوب = prio;

    إذا (linehl != عدم)
    {
        إذا (*linehl == NUL)
            sp->sn_کرښه_hl = 0;
        وإلا
            sp->sn_کرښه_hl = syn_check_ډله(linehl, (صحيح)STRLEN(linehl));
    }

    إذا (texthl != عدم)
    {
        إذا (*texthl == NUL)
            sp->sn_متن_hl = 0;
        وإلا
            sp->sn_متن_hl = syn_check_ډله(texthl, (صحيح)STRLEN(texthl));
    }

    إذا (culhl != عدم)
    {
        إذا (*culhl == NUL)
            sp->sn_cul_hl = 0;
        وإلا
            sp->sn_cul_hl = syn_check_ډله(culhl, (صحيح)STRLEN(culhl));
    }

    إذا (numhl != عدم)
    {
        إذا (*numhl == NUL)
            sp->sn_عدد_hl = 0;
        وإلا
            sp->sn_عدد_hl = syn_check_ډله(numhl, (صحيح)STRLEN(numhl));
    }

    أرجع OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
صحيح
sign_exists_by_name(char_u *name)
{
    أرجع sign_پیدا_کړه(name, عدم) != عدم;
}

/*
 * Free the sign specified by 'name'.
 */
صحيح
sign_undefine_by_name(char_u *name, صحيح give_error)
{
    sign_T *sp_prev = عدم;
    sign_T *sp = sign_پیدا_کړه(name, &sp_prev);
    إذا (sp == عدم)
    {
        إذا (give_error)
            semsg(_(e_unknown_sign_str), name);
        أرجع FAIL;
    }
    sign_undefine(sp, sp_prev);

    أرجع OK;
}

/*
 * List the signs matching 'name'
 */
ساكن فراغ
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_پیدا_کړه(name, عدم);
    إذا (sp != عدم)
        sign_list_defined(sp);
    وإلا
        semsg(_(e_unknown_sign_str), name);
}

ساكن فراغ
may_force_numberwidth_recompute(buf_T *buf, صحيح unplace)
{
# إذا defined(FEAT_LINEBREAK)
    tabpage_T *tp = عدم;
    win_T *wp = عدم;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        إذا (wp->w_بفر == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_پلنوالی < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_کرښه_شمېر = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
صحيح
sign_ځای_پرځای_کړه(صحيح *sign_id,
           char_u *sign_ډله,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           صحيح prio)
{
    // Check for reserved character '*' in group name
    إذا (sign_ډله != عدم && (*sign_ډله == '*' || *sign_ډله == '\0'))
        أرجع FAIL;

    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
    {
        إذا (STRCMP(sp->sn_name, sign_name) == 0)
            اكسر;
    }

    إذا (sp == عدم)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        أرجع FAIL;
    }

    إذا (*sign_id == 0)
        *sign_id = sign_ډله_get_بل_signid(buf, sign_ډله);

    // Use the default priority value for this sign.
    إذا (prio == -1)
        prio = (sp->sn_لومړیتوب != -1) ? sp->sn_لومړیتوب : SIGN_DEF_PRIO;

    إذا (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_ډله, prio, lnum, sp->sn_typenr);
    }
    وإلا
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_ډله, sp->sn_typenr,
                                    prio);
    }

    إذا (lnum > 0)
    {
        redraw_buf_کرښه_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    وإلا
    {
        semsg(_(e_not_possible_منزل_change_sign_str), sign_name);
        أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Unplace the specified sign
 */
ساكن صحيح
sign_unplace(صحيح sign_id, char_u *sign_ډله, buf_T *buf, linenr_T atlnum)
{
    إذا (buf->b_signlist == عدم) // No signs in the buffer
        أرجع OK;

    إذا (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_ډله);
    }
    وإلا
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_ډله);
        إذا (lnum == 0)
            أرجع FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    إذا (buf->b_signlist == عدم)
        may_force_numberwidth_recompute(buf, TRUE);

    أرجع OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ساكن فراغ
sign_unplace_at_cursor(char_u *groupname)
{
    صحيح id =
        buf_findsign_id(curwin->w_بفر, curwin->w_cursor.lnum, groupname);

    إذا (id > 0)
        sign_unplace(id, groupname, curwin->w_بفر, curwin->w_cursor.lnum);
    وإلا
        emsg(_(e_missing_sign_شمېره));
}

/*
 * Jump to a sign.
 */
ساكن linenr_T
sign_jump(صحيح sign_id, char_u *sign_ډله, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_ډله);
    إذا (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        أرجع -1;
    }

    // goto a sign ...
    إذا (buf_jump_open_win(buf) != عدم)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    وإلا
    { // ... not currently in a window
        إذا (buf->b_fname == عدم)
        {
            emsg(_(e_cannot_jump_منزل_بفر_that_does_not_have_name));
            أرجع -1;
        }
        setpcmark();
        إذا (do_ecmd(buf->b_fnum, عدم, عدم, عدم, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            أرجع -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    أرجع lnum;
}

/*
 * ":sign define {name} ..." command
 */
ساكن فراغ
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = عدم;
    char_u *p = cmdline;
    char_u *icon = عدم;
    char_u *متن = عدم;
    char_u *linehl = عدم;
    char_u *texthl = عدم;
    char_u *culhl = عدم;
    char_u *numhl = عدم;
    صحيح prio = -1;
    صحيح failed = FALSE;

    // set values for a defined sign.
    طالما (TRUE)
    {
        arg = skipwhite(p);
        إذا (*arg == NUL)
            اكسر;

        p = skiptowhite_esc(arg);
        إذا (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            متن = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        وإلا إذا (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((حرف *)arg);
        }
        وإلا
        {
            semsg(_(e_invalid_دلیل_str), arg);
            failed = TRUE;
            اكسر;
        }
    }

    إذا (!failed)
        sign_define_by_name(sign_name, icon, linehl, متن, texthl, culhl, numhl,
                            prio);

    vim_free(icon);
    vim_free(متن);
    vim_free(linehl);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);
}

/*
 * ":sign place" command
 */
ساكن فراغ
sign_ځای_پرځای_کړه_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               صحيح id,
               char_u *ډله,
               صحيح prio)
{
    إذا (id <= 0)
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
        إذا (lnum >= 0 || sign_name != عدم || (ډله != عدم && *ډله == '\0'))
            emsg(_(e_invalid_دلیل));
        وإلا
            sign_list_placed(buf, ډله);
    }
    وإلا
    {
        // Place a new sign
        إذا (sign_name == عدم || buf == عدم ||
            (ډله != عدم && *ډله == '\0'))
        {
            emsg(_(e_invalid_دلیل));
            أرجع;
        }

        sign_ځای_پرځای_کړه(&id, ډله, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ساكن فراغ
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 صحيح id,
                 char_u *ډله)
{
    إذا (lnum >= 0 || sign_name != عدم || (ډله != عدم && *ډله == '\0'))
    {
        emsg(_(e_invalid_دلیل));
        أرجع;
    }

    إذا (id == -2)
    {
        إذا (buf != عدم)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, ډله, buf, 0);
        }
        وإلا
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                إذا (buf->b_signlist != عدم)
                    buf_delete_signs(buf, ډله);
            }
        }
    }
    وإلا
    {
        إذا (buf != عدم)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, ډله, buf, 0);
        }
        وإلا
        {
            إذا (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(ډله);
            }
            وإلا
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, ډله, buf, 0);
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
ساكن فراغ
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              صحيح id,
              char_u *ډله)
{
    إذا (sign_name == عدم && ډله == عدم && id == -1)
    {
        emsg(_(e_دلیل_required));
        أرجع;
    }

    إذا (buf == عدم || (ډله != عدم && *ډله == '\0') || lnum >= 0 ||
        sign_name != عدم)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_دلیل));
        أرجع;
    }

    (فراغ)sign_jump(id, ډله, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ساكن صحيح
parse_sign_cmd_دلیلونه(صحيح cmd,
                    char_u *arg,
                    char_u **sign_name,
                    صحيح *signid,
                    char_u **ډله,
                    صحيح *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = عدم;
    صحيح lnum_arg = FALSE;

    // first arg could be placed sign id
    إذا (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        إذا (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        وإلا
        {
            arg = skipwhite(arg);
        }
    }

    طالما (*arg != NUL)
    {
        إذا (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((حرف *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        وإلا إذا (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            إذا (*signid != -1)
            {
                emsg(_(e_invalid_دلیل));
                أرجع FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        وإلا إذا (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            إذا (*arg != NUL)
                *arg++ = NUL;

            طالما (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        وإلا إذا (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *ډله = arg;
            arg = skiptowhite(arg);
            إذا (*arg != NUL)
                *arg++ = NUL;
        }
        وإلا إذا (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((حرف *)arg);
            arg = skiptowhite(arg);
        }
        وإلا إذا (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            اكسر;
        }
        وإلا إذا (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((صحيح)getdigits(&arg));

            إذا (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            اكسر;
        }
        وإلا
        {
            emsg(_(e_invalid_دلیل));
            أرجع FAIL;
        }

        arg = skipwhite(arg);
    }

    إذا (filename != عدم && *buf == عدم)
    {
        semsg(_(e_invalid_بفر_name_str), filename);
        أرجع FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    إذا (filename == عدم &&
        ((cmd == SIGNCMD_ځای_پرځای_کړه && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_بفر;

    أرجع OK;
}

/*
 * ":sign" command
 */
فراغ
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    صحيح idx = sign_cmd_idx(arg, p);
    إذا (idx == SIGNCMD_وروستی)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        أرجع;
    }
    arg = skipwhite(p);

    إذا (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = عدم;
        صحيح id = -1;
        char_u *ډله = عدم;
        صحيح prio = -1;
        buf_T *buf = عدم;
        linenr_T lnum = -1;

        // Parse command line arguments
        إذا (parse_sign_cmd_دلیلونه(idx, arg, &sign_name, &id, &ډله, &prio, &buf,
                                &lnum) == FAIL)
            أرجع;

        إذا (idx == SIGNCMD_ځای_پرځای_کړه)
            sign_ځای_پرځای_کړه_cmd(buf, lnum, sign_name, id, ډله, prio);
        وإلا إذا (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, ډله);
        وإلا إذا (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, ډله);

        أرجع;
    }

    // Define, undefine or list signs.
    إذا (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        لكل (sign_T *sp = لومړی_sign; sp != عدم && !got_int; sp = sp->sn_بل)
            sign_list_defined(sp);
    }
    وإلا إذا (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    وإلا
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        إذا (*p != NUL)
            *p++ = NUL;

        طالما (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        إذا (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        وإلا إذا (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        وإلا
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
ساكن فراغ
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_جمع_کړه_string(retdict, "name", sp->sn_name);

    إذا (sp->sn_icon != عدم)
        dict_جمع_کړه_string(retdict, "icon", sp->sn_icon);

    إذا (sp->sn_متن != عدم)
        dict_جمع_کړه_string(retdict, "text", sp->sn_متن);

    إذا (sp->sn_لومړیتوب > 0)
        dict_جمع_کړه_شمېره(retdict, "priority", sp->sn_لومړیتوب);

    إذا (sp->sn_کرښه_hl > 0)
    {
        char_u *p = get_highlight_name_ext(عدم, sp->sn_کرښه_hl - 1, FALSE);
        إذا (p == عدم)
            dict_جمع_کړه_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        وإلا
            dict_جمع_کړه_string(retdict, "linehl", p);
    }

    إذا (sp->sn_متن_hl > 0)
    {
        char_u *p = get_highlight_name_ext(عدم, sp->sn_متن_hl - 1, FALSE);
        إذا (p == عدم)
            dict_جمع_کړه_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        وإلا
            dict_جمع_کړه_string(retdict, "texthl", p);
    }

    إذا (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(عدم, sp->sn_cul_hl - 1, FALSE);
        إذا (p == عدم)
            dict_جمع_کړه_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        وإلا
            dict_جمع_کړه_string(retdict, "culhl", p);
    }

    إذا (sp->sn_عدد_hl > 0)
    {
        char_u *p = get_highlight_name_ext(عدم, sp->sn_عدد_hl - 1, FALSE);
        إذا (p == عدم)
            dict_جمع_کړه_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        وإلا
            dict_جمع_کړه_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ساكن فراغ
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = لومړی_sign;

    إذا (name != عدم)
    {
        sp = sign_پیدا_کړه(name, عدم);
        إذا (sp == عدم)
            أرجع;
    }

    لكل (; sp != عدم && !got_int; sp = sp->sn_بل)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        إذا (dict == عدم)
            أرجع;

        إذا (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            أرجع;
        }

        sign_getinfo(sp, dict);

        إذا (name != عدم) // handle only the specified sign
            اكسر;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
فراغ
get_بفر_signs(buf_T *buf, list_T *l)
{
    sign_ثبت_T *sign = عدم;
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        إذا (d != عدم && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ساكن فراغ
sign_get_placed_دننه_buf(buf_T *buf,
                       linenr_T lnum,
                       صحيح sign_id,
                       char_u *sign_ډله,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    إذا (d == عدم)
        أرجع;

    إذا (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        أرجع;
    }

    dict_جمع_کړه_شمېره(d, "bufnr", (اوږد)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    إذا (l == عدم)
        أرجع;

    إذا (dict_جمع_کړه_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        أرجع;
    }

    sign_ثبت_T *sign = عدم;
    FOR_ALL_SIGNS_دننه_BUF(buf, sign)
    {
        إذا (!sign_دننه_ډله(sign, sign_ډله))
            استمر;

        إذا ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            إذا (sdict != عدم && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
ساكن فراغ
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                صحيح sign_id,
                char_u *sign_ډله,
                list_T *retlist)
{
    إذا (buf != عدم)
    {
        sign_get_placed_دننه_buf(buf, lnum, sign_id, sign_ډله, retlist);
    }
    وإلا
    {
        FOR_ALL_BUFFERS(buf)
        {
            إذا (buf->b_signlist != عدم)
                sign_get_placed_دننه_buf(buf, 0, sign_id, sign_ډله, retlist);
        }
    }
}

# إذا defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
فراغ
sign_gui_started(فراغ)
{
    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
    {
        إذا (sp->sn_icon != عدم)
            sp->sn_image = gui_mch_ثبتوونکی_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ساكن فراغ
sign_list_defined(sign_T *sp)
{
    حرف lbuf[پیغام_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    إذا (sp->sn_icon != عدم)
    {
        پیغام_puts(" icon=");
        پیغام_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        إذا (sp->sn_image == عدم)
            پیغام_puts(_(" (NOT FOUND)"));
# وإلا
        پیغام_puts(_(" (not supported)"));
# endif
    }

    إذا (sp->sn_متن != عدم)
    {
        پیغام_puts(" text=");
        پیغام_outtrans(sp->sn_متن);
    }

    إذا (sp->sn_لومړیتوب > 0)
    {
        vim_snprintf(lbuf, پیغام_BUF_LEN, " priority=%d", sp->sn_لومړیتوب);
        پیغام_puts(lbuf);
    }

    إذا (sp->sn_کرښه_hl > 0)
    {
        پیغام_puts(" linehl=");

        char_u *p = get_highlight_name_ext(عدم, sp->sn_کرښه_hl - 1, FALSE);
        إذا (p == عدم)
            پیغام_puts("NONE");
        وإلا
            پیغام_puts((حرف *)p);
    }

    إذا (sp->sn_متن_hl > 0)
    {
        پیغام_puts(" texthl=");

        char_u *p = get_highlight_name_ext(عدم, sp->sn_متن_hl - 1, FALSE);
        إذا (p == عدم)
            پیغام_puts("NONE");
        وإلا
            پیغام_puts((حرف *)p);
    }

    إذا (sp->sn_cul_hl > 0)
    {
        پیغام_puts(" culhl=");

        char_u *p = get_highlight_name_ext(عدم, sp->sn_cul_hl - 1, FALSE);
        إذا (p == عدم)
            پیغام_puts("NONE");
        وإلا
            پیغام_puts((حرف *)p);
    }

    إذا (sp->sn_عدد_hl > 0)
    {
        پیغام_puts(" numhl=");

        char_u *p = get_highlight_name_ext(عدم, sp->sn_عدد_hl - 1, FALSE);
        إذا (p == عدم)
            پیغام_puts("NONE");
        وإلا
            پیغام_puts((حرف *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
ساكن فراغ
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    إذا (sp->sn_image != عدم)
    {
        بهر_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_متن);

    إذا (sp_prev == عدم)
        لومړی_sign = sp->sn_بل;
    وإلا
        sp_prev->sn_بل = sp->sn_بل;

    vim_free(sp);
}

# إذا defined(FEAT_SIGN_ICONS)
فراغ *
sign_get_image(صحيح typenr) // the attribute which may have a sign
{
    sign_T *sp = عدم;
    FOR_ALL_SIGNS(sp)
    {
        إذا (sp->sn_typenr == typenr)
            أرجع sp->sn_image;
    }
    أرجع عدم;
}
# endif

/*
 * Undefine/free all signs.
 */
فراغ
free_signs(فراغ)
{
    طالما (لومړی_sign != عدم)
        sign_undefine(لومړی_sign, عدم);
}

ساكن تعداد
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_ځای_پرځای_کړه, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_نومونه, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
ساكن char_u *
get_nth_sign_name(صحيح idx)
{
    صحيح current_idx = 0;
    sign_T *sp = عدم;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        إذا (current_idx++ == idx)
            أرجع sp->sn_name;
    }
    أرجع عدم;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ساكن char_u *
get_nth_sign_ډله_name(صحيح idx)
{
    صحيح current_idx = 0;
    صحيح todo = (صحيح)sg_جدول.ht_کارول_شوی;
    hashitem_T *hi = عدم;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_توکي(&sg_جدول, hi, todo)
    {
        إذا (!HASHITEM_تشه(hi))
        {
            --todo;
            إذا (current_idx++ == idx)
            {
                signgroup_T *ډله = HI2SG(hi);
                أرجع ډله->sg_name;
            }
        }
    }
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp ناکارول_شوی, صحيح idx)
{
    بدل (expand_what)
    {
        حالة EXP_SUBCMD:
            أرجع (char_u *)cmds[idx];
        حالة EXP_DEFINE:
        {
            حرف *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", عدم };
            أرجع (char_u *)define_arg[idx];
        }
        حالة EXP_ځای_پرځای_کړه:
        {
            حرف *ځای_پرځای_کړه_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", عدم };
            أرجع (char_u *)ځای_پرځای_کړه_arg[idx];
        }
        حالة EXP_LIST:
        {
            حرف *list_arg[] = { "group=", "file=", "buffer=", عدم };
            أرجع (char_u *)list_arg[idx];
        }
        حالة EXP_UNPLACE:
        {
            حرف *unplace_arg[] = { "group=", "file=", "buffer=", عدم };
            أرجع (char_u *)unplace_arg[idx];
        }
        حالة EXP_SIGN_نومونه:
            أرجع get_nth_sign_name(idx);
        حالة EXP_SIGN_GROUPS:
            أرجع get_nth_sign_ډله_name(idx);
        افتراضي:
            أرجع عدم;
    }
}

/*
 * Handle command line completion for :sign command.
 */
فراغ
سېټ_context_دننه_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *پای_ټکی_subcmd;
    char_u *وروستی;
    صحيح cmd_idx;
    char_u *begin_subcmd_دلیلونه;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_نمونه = arg;

    پای_ټکی_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    إذا (*پای_ټکی_subcmd == NUL)
        أرجع;

    cmd_idx = sign_cmd_idx(arg, پای_ټکی_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_دلیلونه = skipwhite(پای_ټکی_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_دلیلونه;
    افعل
    {
        p = skipwhite(p);
        وروستی = p;
        p = skiptowhite(p);
    }
    طالما (*p != NUL);

    p = vim_strchr(وروستی, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    إذا (p == عدم)
    {
        // Expand last argument name (before equal sign).
        xp->xp_نمونه = وروستی;
        بدل (cmd_idx)
        {
            حالة SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                اكسر;
            حالة SIGNCMD_ځای_پرځای_کړه:
                // List placed signs
                إذا (VIM_ISDIGIT(*begin_subcmd_دلیلونه))
                    //   :sign place {id} {args}...
                    expand_what = EXP_ځای_پرځای_کړه;
                وإلا
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                اكسر;
            حالة SIGNCMD_LIST:
            حالة SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_نومونه;
                اكسر;
            حالة SIGNCMD_JUMP:
            حالة SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                اكسر;
            افتراضي:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    وإلا
    {
        // Expand last argument value (after equal sign).
        xp->xp_نمونه = p + 1;
        بدل (cmd_idx)
        {
            حالة SIGNCMD_DEFINE:
                إذا (STRNCMP(وروستی, "texthl", 6) == 0 ||
                    STRNCMP(وروستی, "linehl", 6) == 0 ||
                    STRNCMP(وروستی, "culhl", 5) == 0 ||
                    STRNCMP(وروستی, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                وإلا إذا (STRNCMP(وروستی, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                وإلا
                    xp->xp_context = EXPAND_NOTHING;
                اكسر;
            حالة SIGNCMD_ځای_پرځای_کړه:
                إذا (STRNCMP(وروستی, "name", 4) == 0)
                    expand_what = EXP_SIGN_نومونه;
                وإلا إذا (STRNCMP(وروستی, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                وإلا إذا (STRNCMP(وروستی, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                وإلا
                    xp->xp_context = EXPAND_NOTHING;
                اكسر;
            حالة SIGNCMD_UNPLACE:
            حالة SIGNCMD_JUMP:
                إذا (STRNCMP(وروستی, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                وإلا إذا (STRNCMP(وروستی, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                وإلا
                    xp->xp_context = EXPAND_NOTHING;
                اكسر;
            افتراضي:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ساكن صحيح
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = عدم;
    char_u *icon = عدم;
    char_u *linehl = عدم;
    char_u *متن = عدم;
    char_u *texthl = عدم;
    char_u *culhl = عدم;
    char_u *numhl = عدم;
    صحيح prio = -1;
    صحيح retval = -1;

    إذا (name_arg == عدم && dict == عدم)
        أرجع retval;

    إذا (name_arg == عدم)
        name = dict_get_string(dict, "name", TRUE);
    وإلا
        name = vim_strsave(name_arg);

    إذا (name == عدم || name[0] == NUL)
        اذهب cleanup;

    إذا (dict != عدم)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        متن = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_شمېره_def(dict, "priority", -1);
    }

    إذا (sign_define_by_name(name, icon, linehl, متن, texthl, culhl, numhl,
                            prio) == OK)
        retval = 0;

cleanup:
    vim_free(name);
    vim_free(icon);
    vim_free(linehl);
    vim_free(متن);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);

    أرجع retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ساكن فراغ
sign_define_مضرب(list_T *l, list_T *retlist)
{
    listitem_T *li = عدم;
    FOR_ALL_LIST_توکي(l, li)
    {
        صحيح retval = -1;

        إذا (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(عدم, li->li_tv.vval.v_dict);
        وإلا
            emsg(_(e_dictionary_required));

        list_append_شمېره(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
فراغ
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        أرجع;

    إذا (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        إذا (rettv_list_alloc(rettv) == FAIL)
            أرجع;

        sign_define_مضرب(argvars[0].vval.v_list, rettv->vval.v_list);
        أرجع;
    }

    // Define a single sign
    rettv->vval.v_شمېره = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    إذا (name == عدم)
        أرجع;

    إذا (check_for_opt_dict_arg(argvars, 1) == FAIL)
        أرجع;

    rettv->vval.v_شمېره = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : عدم);
}

/*
 * "sign_getdefined()" function
 */
فراغ
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    إذا (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        أرجع;

    إذا (دننه_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        أرجع;

    char_u *name = عدم;
    إذا (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
فراغ
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = عدم;
    linenr_T lnum = 0;
    صحيح sign_id = 0;
    char_u *ډله = عدم;

    إذا (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        أرجع;

    إذا (دننه_vim9script() && (check_for_opt_بفر_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        إذا (buf == عدم)
            أرجع;

        إذا (argvars[1].v_type != VAR_UNKNOWN)
        {
            إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                أرجع;

            dictitem_T *di = عدم;
            dict_T *dict = argvars[1].vval.v_dict;

            إذا ((di = dict_پیدا_کړه(dict, (char_u *)"lnum", -1)) != عدم)
            {
                // get signs placed at this line
                صحيح notanum = FALSE;
                tv_get_شمېره_chk(&di->di_tv, &notanum);
                إذا (notanum)
                    أرجع;

                lnum = tv_get_lnum(&di->di_tv);
            }

            إذا ((di = dict_پیدا_کړه(dict, (char_u *)"id", -1)) != عدم)
            {
                // get sign placed with this identifier
                صحيح notanum = FALSE;
                sign_id = (صحيح)tv_get_شمېره_chk(&di->di_tv, &notanum);
                إذا (notanum)
                    أرجع;
            }

            إذا ((di = dict_پیدا_کړه(dict, (char_u *)"group", -1)) != عدم)
            {
                ډله = tv_get_string_chk(&di->di_tv);
                إذا (ډله == عدم)
                    أرجع;

                إذا (*ډله == '\0') // empty string means global group
                    ډله = عدم;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, ډله, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
فراغ
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_ډله = عدم;
    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script() && (check_for_شمېره_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_بفر_arg(argvars, 2) == FAIL))
        أرجع;

    صحيح notanum = FALSE;
    // Sign identifier
    صحيح sign_id = (صحيح)tv_get_شمېره_chk(&argvars[0], &notanum);
    إذا (notanum)
        أرجع;

    إذا (sign_id <= 0)
    {
        emsg(_(e_invalid_دلیل));
        أرجع;
    }

    // Sign group
    sign_ډله = tv_get_string_chk(&argvars[1]);
    إذا (sign_ډله == عدم)
        أرجع;

    إذا (sign_ډله[0] == '\0')
    {
        sign_ډله = عدم; // global sign group
    }
    وإلا
    {
        sign_ډله = vim_strsave(sign_ډله);
        إذا (sign_ډله == عدم)
            أرجع;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    إذا (buf == عدم)
        اذهب cleanup;

    rettv->vval.v_شمېره = sign_jump(sign_id, sign_ډله, buf);

cleanup:
    vim_free(sign_ډله);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ساكن صحيح
sign_ځای_پرځای_کړه_from_dict(typval_T *id_tv,
                     typval_T *ډله_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    صحيح sign_id = 0;
    char_u *ډله = عدم;
    char_u *sign_name = عدم;
    buf_T *buf = عدم;
    dictitem_T *di = عدم;
    linenr_T lnum = 0;
    صحيح prio = -1;
    صحيح ret_sign_id = -1;

    // sign identifier
    إذا (id_tv == عدم)
    {
        di = dict_پیدا_کړه(dict, (char_u *)"id", -1);
        إذا (di != عدم)
            id_tv = &di->di_tv;
    }

    إذا (id_tv == عدم)
    {
        sign_id = 0;
    }
    وإلا
    {
        صحيح notanum = FALSE;
        sign_id = tv_get_شمېره_chk(id_tv, &notanum);

        إذا (notanum)
            أرجع -1;

        إذا (sign_id < 0)
        {
            emsg(_(e_invalid_دلیل));
            أرجع -1;
        }
    }

    // sign group
    إذا (ډله_tv == عدم)
    {
        di = dict_پیدا_کړه(dict, (char_u *)"group", -1);
        إذا (di != عدم)
            ډله_tv = &di->di_tv;
    }

    إذا (ډله_tv == عدم)
    {
        ډله = عدم; // global group
    }
    وإلا
    {
        ډله = tv_get_string_chk(ډله_tv);
        إذا (ډله == عدم)
            اذهب cleanup;

        إذا (ډله[0] == '\0') // global sign group
        {
            ډله = عدم;
        }
        وإلا
        {
            ډله = vim_strsave(ډله);
            إذا (ډله == عدم)
                أرجع -1;
        }
    }

    // sign name
    إذا (name_tv == عدم)
    {
        di = dict_پیدا_کړه(dict, (char_u *)"name", -1);
        إذا (di != عدم)
            name_tv = &di->di_tv;
    }

    إذا (name_tv == عدم)
        اذهب cleanup;

    sign_name = tv_get_string_chk(name_tv);
    إذا (sign_name == عدم)
        اذهب cleanup;

    // buffer to place the sign
    إذا (buf_tv == عدم)
    {
        di = dict_پیدا_کړه(dict, (char_u *)"buffer", -1);
        إذا (di != عدم)
            buf_tv = &di->di_tv;
    }

    إذا (buf_tv == عدم)
        اذهب cleanup;

    buf = get_buf_arg(buf_tv);
    إذا (buf == عدم)
        اذهب cleanup;

    // line number of the sign
    di = dict_پیدا_کړه(dict, (char_u *)"lnum", -1);
    إذا (di != عدم)
    {
        lnum = tv_get_lnum(&di->di_tv);
        إذا (lnum <= 0)
        {
            emsg(_(e_invalid_دلیل));
            اذهب cleanup;
        }
    }

    // sign priority
    di = dict_پیدا_کړه(dict, (char_u *)"priority", -1);
    إذا (di != عدم)
    {
        صحيح notanum = FALSE;
        prio = (صحيح)tv_get_شمېره_chk(&di->di_tv, &notanum);
        إذا (notanum)
            اذهب cleanup;
    }

    إذا (sign_ځای_پرځای_کړه(&sign_id, ډله, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(ډله);

    أرجع ret_sign_id;
}

/*
 * "sign_place()" function
 */
فراغ
f_sign_ځای_پرځای_کړه(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = عدم;
    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script() && (check_for_شمېره_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_بفر_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        أرجع;

    إذا (argvars[4].v_type != VAR_UNKNOWN)
    {
        إذا (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            أرجع;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_شمېره = sign_ځای_پرځای_کړه_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
فراغ
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) == FAIL)
        أرجع;

    إذا (دننه_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
        أرجع;

    // Process the List of sign attributes
    listitem_T *li = عدم;
    FOR_ALL_LIST_توکي(argvars[0].vval.v_list, li)
    {
        صحيح sign_id = -1;

        إذا (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_ځای_پرځای_کړه_from_dict(عدم, عدم, عدم, عدم,
                                           li->li_tv.vval.v_dict);
        وإلا
            emsg(_(e_dictionary_required));

        list_append_شمېره(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ساكن فراغ
sign_undefine_مضرب(list_T *l, list_T *retlist)
{
    listitem_T *li = عدم;
    FOR_ALL_LIST_توکي(l, li)
    {
        صحيح retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        إذا (name != عدم && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_شمېره(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
فراغ
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    إذا (دننه_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        أرجع;

    إذا (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        إذا (rettv_list_alloc(rettv) == FAIL)
            أرجع;

        sign_undefine_مضرب(argvars[0].vval.v_list, rettv->vval.v_list);
        أرجع;
    }

    rettv->vval.v_شمېره = -1;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_شمېره = 0;
    }
    وإلا
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        إذا (name == عدم)
            أرجع;

        إذا (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_شمېره = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ساكن صحيح
sign_unplace_from_dict(typval_T *ډله_tv, dict_T *dict)
{
    صحيح sign_id = 0;
    buf_T *buf = عدم;
    char_u *ډله = عدم;
    صحيح retval = -1;

    // sign group
    ډله = (ډله_tv != عدم) ? tv_get_string(ډله_tv)
                               : dict_get_string(dict, "group", FALSE);

    إذا (ډله != عدم)
    {
        إذا (ډله[0] == '\0') // global sign group
        {
            ډله = عدم;
        }
        وإلا
        {
            ډله = vim_strsave(ډله);
            إذا (ډله == عدم)
                أرجع retval;
        }
    }

    إذا (dict != عدم)
    {
        dictitem_T *di = dict_پیدا_کړه(dict, (char_u *)"buffer", -1);
        إذا (di != عدم)
        {
            buf = get_buf_arg(&di->di_tv);
            إذا (buf == عدم)
                اذهب cleanup;
        }

        إذا (dict_has_کلی(dict, "id"))
        {
            sign_id = dict_get_شمېره(dict, "id");
            إذا (sign_id <= 0)
            {
                emsg(_(e_invalid_دلیل));
                اذهب cleanup;
            }
        }
    }

    إذا (buf == عدم)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            إذا (sign_unplace(sign_id, ډله, buf, 0) != OK)
                retval = -1;
    }
    وإلا إذا (sign_unplace(sign_id, ډله, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(ډله);

    أرجع retval;
}

sign_ثبت_T *
get_لومړی_valid_sign(win_T *wp)
{
    sign_ثبت_T *sign = wp->w_بفر->b_signlist;

# ifdef FEAT_PROP_POPUP
    طالما (sign != عدم && !sign_ډله_for_کړکۍ(sign, wp))
        sign = sign->se_بل;
# endif
    أرجع sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
صحيح
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    إذا (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        أرجع get_لومړی_valid_sign(wp) != عدم && !wp->w_p_nu && !wp->w_p_rnu;

    إذا (*wp->w_p_scl == 'n')
        أرجع FALSE;

    إذا (*wp->w_p_scl == 'y')
        أرجع TRUE;

    أرجع (get_لومړی_valid_sign(wp) != عدم
# ifdef FEAT_NETBEANS_INTG
            || wp->w_بفر->b_has_sign_ستنه
# endif
    );
}

/*
 * "sign_unplace()" function
 */
فراغ
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = عدم;
    rettv->vval.v_شمېره = -1;

    إذا ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_شمېره = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
فراغ
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) == FAIL)
        أرجع;

    إذا (دننه_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
        أرجع;

    listitem_T *li = عدم;
    FOR_ALL_LIST_توکي(argvars[0].vval.v_list, li)
    {
        صحيح retval = -1;

        إذا (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(عدم, li->li_tv.vval.v_dict);
        وإلا
            emsg(_(e_dictionary_required));

        list_append_شمېره(rettv->vval.v_list, retval);
    }
}

#د_که_پای // FEAT_SIGNS
