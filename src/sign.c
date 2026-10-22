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

#شامل "vim.h"

#اگر defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
قسمتعریف ساخت sign sign_T;

ساخت sign
{
    sign_T *sn_اگلا; // next sign in list
    صحیح sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    خالی *sn_image; // icon image
# endif
    char_u *sn_متن; // text used instead of pixmap
    صحیح sn_سطر_hl; // highlight ID for line
    صحیح sn_متن_hl; // highlight ID for text
    صحیح sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    صحیح sn_عدد_hl; // highlight ID for line number
    صحیح sn_ترجیح; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

ساکن sign_T *پہلا_sign = تہی;
ساکن صحیح اگلا_sign_typenr = 1;

ساکن خالی sign_list_defined(sign_T *sp);
ساکن خالی sign_undefine(sign_T *sp, sign_T *sp_پچھلا);

ساکن حرف *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_رکھو 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        تہی
# define SIGNCMD_آخری 6
};

# define FOR_ALL_SIGNS(sp) \
     برائے ((sp) = پہلا_sign; (sp) != تہی; (sp) = (sp)->sn_اگلا)

ساکن hashtab_T sg_جدول; // sign group (signgroup_T) hashtable
ساکن صحیح اگلا_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
خالی
init_signs(خالی)
{
    hash_init(&sg_جدول); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
ساکن signgroup_T *
sign_گروہ_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_جدول, groupname, hash);
    signgroup_T *گروہ = تہی;

    اگر (HASHITEM_خالی_خانے(hi))
    {
        // new group
        گروہ = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        اگر (گروہ == تہی)
            واپس تہی;

        STRCPY(گروہ->sg_name, groupname);
        گروہ->sg_refcount = 1;
        گروہ->sg_اگلا_sign_id = 1;
        hash_جوڑو_شے(&sg_جدول, hi, گروہ->sg_name, hash);
    }
    ورنہ
    {
        // existing group
        گروہ = HI2SG(hi);
        گروہ->sg_refcount++;
    }

    واپس گروہ;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
ساکن خالی
sign_گروہ_unref(char_u *groupname)
{
    hashitem_T *hi = hash_ڈھونڈو(&sg_جدول, groupname);
    اگر (HASHITEM_خالی_خانے(hi))
        واپس;

    signgroup_T *گروہ = HI2SG(hi);
    گروہ->sg_refcount--;
    اگر (گروہ->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_جدول, hi, "sign remove");
        vim_free(گروہ);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
ساکن صحیح
sign_اندر_گروہ(sign_اندراج_T *sign, char_u *گروہ)
{
    واپس ((گروہ != تہی && STRCMP(گروہ, "*") == 0) ||
            (گروہ == تہی && sign->se_گروہ == تہی) ||
            (گروہ != تہی && sign->se_گروہ != تہی &&
             STRCMP(گروہ, sign->se_گروہ->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
ساکن صحیح
sign_گروہ_for_کھڑکی(sign_اندراج_T *sign, win_T *wp)
{
    صحیح for_popup = sign->se_گروہ != تہی &&
                    STRNCMP("PopUp", sign->se_گروہ->sg_name, 5) == 0;

    واپس WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
ساکن صحیح
sign_گروہ_get_اگلا_signid(buf_T *buf, char_u *groupname)
{
    صحیح id = 1;
    signgroup_T *گروہ = تہی;
    sign_اندراج_T *sign = تہی;

    اگر (groupname != تہی)
    {
        hashitem_T *hi = hash_ڈھونڈو(&sg_جدول, groupname);
        اگر (HASHITEM_خالی_خانے(hi))
            واپس id;
        گروہ = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    صحیح ملا = FALSE;
    جب (!ملا)
    {
        اگر (گروہ == تہی)
            id = اگلا_sign_id++; // global group
        ورنہ
            id = گروہ->sg_اگلا_sign_id++;

        // Check whether this sign is already placed in the buffer
        ملا = TRUE;
        FOR_ALL_SIGNS_اندر_BUF(buf, sign)
        {
            اگر (id == sign->se_id && sign_اندر_گروہ(sign, groupname))
            {
                ملا = FALSE; // sign identifier is in use
                توڑو;
            }
        }
    }

    واپس id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
ساکن خالی
داخل_کرو_sign(buf_T *buf, // buffer to store sign in
            sign_اندراج_T *پچھلا, // previous sign entry
            sign_اندراج_T *اگلا, // next sign entry
            صحیح id, // sign ID
            char_u *گروہ, // sign group; NULL for global group
            صحیح prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            صحیح typenr) // typenr of sign we are adding
{
    sign_اندراج_T *newsign =
        lalloc_id(حجم(sign_اندراج_T), FALSE, aid_داخل_کرو_sign);
    اگر (newsign == تہی)
        واپس;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    اگر (گروہ != تہی)
    {
        newsign->se_گروہ = sign_گروہ_ref(گروہ);
        اگر (newsign->se_گروہ == تہی)
        {
            vim_free(newsign);
            واپس;
        }
    }
    ورنہ
    {
        newsign->se_گروہ = تہی;
    }

    newsign->se_ترجیح = prio;
    newsign->se_اگلا = اگلا;
    newsign->se_پچھلا = پچھلا;
    اگر (اگلا != تہی)
        اگلا->se_پچھلا = newsign;

    اگر (پچھلا == تہی)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        اگر (buf->b_signlist == تہی)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            بدلا_سطر_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        اگر (netbeans_فعال())
            buf->b_has_sign_عمود = true;
# endif
    }
    ورنہ
    {
        پچھلا->se_اگلا = newsign;
    }

    // Remember the newly inserted sign so a following insert on a later line
    // can resume the search from here.
    buf->b_sign_finger = newsign;
}

/*
 * Insert a new sign sorted by line number and sign priority.
 */
ساکن خالی
داخل_کرو_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_اندراج_T *پچھلا, // previous sign entry
                         صحیح id, // sign ID
                         char_u *گروہ, // sign group; NULL for global group
                         صحیح prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         صحیح typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    جب (پچھلا != تہی && پچھلا->se_lnum == lnum && پچھلا->se_ترجیح <= prio)
        پچھلا = پچھلا->se_پچھلا;

    sign_اندراج_T *sign = (پچھلا == تہی) ? buf->b_signlist : پچھلا->se_اگلا;

    داخل_کرو_sign(buf, پچھلا, sign, id, گروہ, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
ساکن sign_T *
ڈھونڈو_sign_by_typenr(صحیح typenr)
{
    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
        اگر (sp->sn_typenr == typenr)
            واپس sp;
    واپس تہی;
}

/*
 * Get the name of a sign by its typenr.
 */
ساکن char_u *
sign_typenr2name(صحیح typenr)
{
    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
        اگر (sp->sn_typenr == typenr)
            واپس sp->sn_name;
    واپس (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
ساکن dict_T *
sign_get_info(sign_اندراج_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    اگر (d == تہی)
        واپس تہی;

    dict_جوڑو_نمبر(d, "id", sign->se_id);
    اگر (sign->se_گروہ == تہی)
        dict_جوڑو_string_len(d, "group", (char_u *)"", 0);
    ورنہ
        dict_جوڑو_string(d, "group", sign->se_گروہ->sg_name);
    dict_جوڑو_نمبر(d, "lnum", sign->se_lnum);
    dict_جوڑو_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_جوڑو_نمبر(d, "priority", sign->se_ترجیح);

    واپس d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
ساکن خالی
sign_ترتیب_دو_by_prio_on_سطر(buf_T *buf, sign_اندراج_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    اگر ((sign->se_پچھلا == تہی || sign->se_پچھلا->se_lnum != sign->se_lnum ||
         sign->se_پچھلا->se_ترجیح > sign->se_ترجیح) &&
        (sign->se_اگلا == تہی || sign->se_اگلا->se_lnum != sign->se_lnum ||
         sign->se_اگلا->se_ترجیح < sign->se_ترجیح))
        واپس;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_اندراج_T *p = sign;
    جب (p->se_پچھلا != تہی && p->se_پچھلا->se_lnum == sign->se_lnum &&
           p->se_پچھلا->se_ترجیح <= sign->se_ترجیح)
        p = p->se_پچھلا;

    اگر (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_اگلا;
        جب (p->se_اگلا != تہی && p->se_اگلا->se_lnum == sign->se_lnum &&
               p->se_اگلا->se_ترجیح > sign->se_ترجیح)
            p = p->se_اگلا;
    }

    // Remove 'sign' from the list
    اگر (buf->b_signlist == sign)
        buf->b_signlist = sign->se_اگلا;

    اگر (sign->se_پچھلا != تہی)
        sign->se_پچھلا->se_اگلا = sign->se_اگلا;

    اگر (sign->se_اگلا != تہی)
        sign->se_اگلا->se_پچھلا = sign->se_پچھلا;

    sign->se_پچھلا = تہی;
    sign->se_اگلا = تہی;

    // Re-insert 'sign' at the right place
    اگر (p->se_ترجیح <= sign->se_ترجیح)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_پچھلا = p->se_پچھلا;
        sign->se_اگلا = p;
        p->se_پچھلا = sign;
        اگر (sign->se_پچھلا != تہی)
            sign->se_پچھلا->se_اگلا = sign;

        اگر (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    ورنہ
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_پچھلا = p;
        sign->se_اگلا = p->se_اگلا;
        p->se_اگلا = sign;
        اگر (sign->se_اگلا != تہی)
            sign->se_اگلا->se_پچھلا = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
ساکن خالی
buf_addsign(buf_T *buf, // buffer to store sign in
            صحیح id, // sign ID
            char_u *groupname, // sign group
            صحیح prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            صحیح typenr) // typenr of sign we are adding
{
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    sign_اندراج_T *پچھلا = تہی; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    اگر (buf->b_sign_finger != تہی && buf->b_sign_finger->se_lnum < lnum)
        پچھلا = buf->b_sign_finger;

    برائے (sign = (پچھلا == تہی) ? buf->b_signlist : پچھلا->se_اگلا; sign != تہی;
         sign = sign->se_اگلا)
    {
        اگر (lnum == sign->se_lnum && id == sign->se_id &&
            sign_اندر_گروہ(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_ترجیح = prio;
            sign_ترتیب_دو_by_prio_on_سطر(buf, sign);
            واپس;
        }
        ورنہ اگر (lnum < sign->se_lnum)
        {
            داخل_کرو_sign_by_lnum_prio(buf, پچھلا, id, groupname, prio, lnum,
                                     typenr);
            واپس;
        }
        پچھلا = sign;
    }

    داخل_کرو_sign_by_lnum_prio(buf, پچھلا, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
ساکن linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     صحیح markId, // sign ID
                     char_u *گروہ, // sign group
                     صحیح typenr, // typenr of sign we are adding
                     صحیح prio) // sign priority
{
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        اگر (sign->se_id == markId && sign_اندر_گروہ(sign, گروہ))
        {
            sign->se_typenr = typenr;
            sign->se_ترجیح = prio;
            sign_ترتیب_دو_by_prio_on_سطر(buf, sign);
            واپس sign->se_lnum;
        }
    }

    واپس (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
صحیح
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_اشارہ_گر(sattr);

    buf_T *buf = wp->w_بفر;
    sign_اندراج_T *sign = تہی;
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        اگر (sign->se_lnum > lnum)
            توڑو;

        اگر (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_گروہ_for_کھڑکی(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = ڈھونڈو_sign_by_typenr(sign->se_typenr);
            اگر (sp == تہی)
                واپس FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_متن = sp->sn_متن;

            اگر (sattr->sat_متن != تہی && sp->sn_متن_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_متن_hl);

            اگر (sp->sn_سطر_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_سطر_hl);

            اگر (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            اگر (sp->sn_عدد_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_عدد_hl);

            sattr->sat_ترجیح = sign->se_ترجیح;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            اگر (sign->se_اگلا != تہی &&
                sign->se_اگلا->se_ترجیح == sign->se_ترجیح &&
                sign->se_اگلا->se_lnum == sign->se_lnum)
            {
                sign_T *اگلا_sp = ڈھونڈو_sign_by_typenr(sign->se_اگلا->se_typenr);
                اگر (اگلا_sp == تہی)
                    واپس FALSE;

                اگر (sattr->sat_icon == تہی && sattr->sat_متن == تہی)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = اگلا_sp->sn_image;
# endif
                    sattr->sat_متن = اگلا_sp->sn_متن;
                }

                اگر (sp->sn_متن_hl <= 0 && اگلا_sp->sn_متن_hl > 0)
                    sattr->sat_texthl = syn_id2attr(اگلا_sp->sn_متن_hl);

                اگر (sp->sn_سطر_hl <= 0 && اگلا_sp->sn_سطر_hl > 0)
                    sattr->sat_linehl = syn_id2attr(اگلا_sp->sn_سطر_hl);

                اگر (sp->sn_cul_hl <= 0 && اگلا_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(اگلا_sp->sn_cul_hl);

                اگر (sp->sn_عدد_hl <= 0 && اگلا_sp->sn_عدد_hl > 0)
                    sattr->sat_numhl = syn_id2attr(اگلا_sp->sn_عدد_hl);
            }
            واپس TRUE;
        }
    }
    واپس FALSE;
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
            صحیح id, // sign id
            char_u *گروہ) // sign group
{
    buf->b_sign_finger = تہی; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_اندراج_T **lastp = &buf->b_signlist;
    sign_اندراج_T *اگلا = تہی; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    برائے (sign_اندراج_T *sign = buf->b_signlist; sign != تہی; sign = اگلا)
    {
        اگلا = sign->se_اگلا;

        اگر ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_اندر_گروہ(sign, گروہ))
        {
            *lastp = اگلا;
            اگر (اگلا != تہی)
                اگلا->se_پچھلا = sign->se_پچھلا;

            lnum = sign->se_lnum;

            اگر (sign->se_گروہ != تہی)
                sign_گروہ_unref(sign->se_گروہ->sg_name);

            vim_free(sign);
            redraw_buf_سطر_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            اگر (گروہ == تہی || (*گروہ != '*' && id != 0) ||
                (*گروہ == '*' && atlnum != 0))
                توڑو;
        }
        ورنہ
        {
            lastp = &sign->se_اگلا;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    اگر (buf->b_signlist == تہی)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        بدلا_سطر_abv_curs();
    }

    واپس lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
صحیح
buf_findsign(buf_T *buf, // buffer to store sign in
             صحیح id, // sign ID
             char_u *گروہ) // sign group
{
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
        اگر (sign->se_id == id && sign_اندر_گروہ(sign, گروہ))
            واپس sign->se_lnum;

    واپس 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
ساکن sign_اندراج_T *
buf_getsign_at_سطر(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        اگر (sign->se_lnum > lnum)
            توڑو;

        اگر (sign->se_lnum == lnum && sign_اندر_گروہ(sign, groupname))
            واپس sign;
    }

    واپس تہی;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
صحیح
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_اندراج_T *sign = buf_getsign_at_سطر(buf, lnum, groupname);
    اگر (sign != تہی)
        واپس sign->se_id;

    واپس 0;
}

# اگر defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
صحیح
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    صحیح typenr) // sign type number
{
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        اگر (sign->se_lnum > lnum)
            توڑو;

        اگر (sign->se_lnum == lnum && sign->se_typenr == typenr)
            واپس sign->se_id;
    }

    واپس 0;
}

#  اگر defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
صحیح
buf_signcount(buf_T *buf, linenr_T lnum)
{
    صحیح گنتی = 0;
    sign_اندراج_T *sign = تہی; // a sign in the signlist
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        اگر (sign->se_lnum > lnum)
            توڑو;

        اگر (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != تہی)
            گنتی++;
    }

    واپس گنتی;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
خالی
buf_delete_signs(buf_T *buf, char_u *گروہ)
{
    buf->b_sign_finger = تہی; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    اگر (buf->b_signlist != تہی && curwin != تہی)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        بدلا_سطر_abv_curs();
    }

    // pointer to pointer to current sign
    sign_اندراج_T **lastp = &buf->b_signlist;
    sign_اندراج_T *اگلا = تہی;

    برائے (sign_اندراج_T *sign = buf->b_signlist; sign != تہی; sign = اگلا)
    {
        اگلا = sign->se_اگلا;
        اگر (sign_اندر_گروہ(sign, گروہ))
        {
            *lastp = اگلا;

            اگر (اگلا != تہی)
                اگلا->se_پچھلا = sign->se_پچھلا;

            اگر (sign->se_گروہ != تہی)
                sign_گروہ_unref(sign->se_گروہ->sg_name);

            vim_free(sign);
        }
        ورنہ
        {
            lastp = &sign->se_اگلا;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
ساکن خالی
sign_list_placed(buf_T *rbuf, char_u *sign_گروہ)
{
    حرف lbuf[پیغام_BUF_LEN];
    حرف گروہ[پیغام_BUF_LEN];

    پیغام_puts_title(_("\n--- Signs ---"));
    پیغام_putchar('\n');

    buf_T *buf = (rbuf == تہی) ? firstbuf : rbuf;
    جب (buf != تہی && !got_int)
    {
        اگر (buf->b_signlist != تہی)
        {
            vim_snprintf(lbuf, پیغام_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            پیغام_puts_attr(lbuf, HL_ATTR(HLF_D));
            پیغام_putchar('\n');
        }

        sign_اندراج_T *sign = تہی;
        FOR_ALL_SIGNS_اندر_BUF(buf, sign)
        {
            اگر (got_int)
                توڑو;

            اگر (!sign_اندر_گروہ(sign, sign_گروہ))
                جاری;

            اگر (sign->se_گروہ != تہی)
                vim_snprintf(گروہ, پیغام_BUF_LEN, _("  group=%s"),
                             sign->se_گروہ->sg_name);
            ورنہ
                گروہ[0] = '\0';

            vim_snprintf(lbuf, پیغام_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (طویل)sign->se_lnum, sign->se_id, گروہ,
                         sign_typenr2name(sign->se_typenr), sign->se_ترجیح);

            پیغام_puts(lbuf);
            پیغام_putchar('\n');
        }

        اگر (rbuf != تہی)
            توڑو;

        buf = buf->b_اگلا;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
خالی
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    طویل        amount,
    طویل        amount_after)
{
    curbuf->b_sign_finger = تہی; // line changes may reorder, drop the finger
    sign_اندراج_T *sign = تہی; // a sign in a b_signlist
    FOR_ALL_SIGNS_اندر_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        اگر (sign->se_lnum < line1)
            جاری;

        linenr_T new_lnum = sign->se_lnum;

        اگر (sign->se_lnum <= line2)
        {
            اگر (amount != MAXLNUM)
                new_lnum += amount;
        }
        ورنہ اگر (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        اگر (new_lnum <= curbuf->b_ml.ml_سطر_گنتی)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
ساکن صحیح
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *انتہا_cmd) // just after sign subcmd
{
    صحیح idx = 0;
    حرف save = *انتہا_cmd;
    *انتہا_cmd = NUL;

    جب (cmds[idx] != تہی && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *انتہا_cmd = save;
    واپس idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
ساکن sign_T *
sign_ڈھونڈو(char_u *name, sign_T **sp_پچھلا)
{
    اگر (sp_پچھلا != تہی)
        *sp_پچھلا = تہی;

    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
    {
        اگر (STRCMP(sp->sn_name, name) == 0)
            توڑو;

        اگر (sp_پچھلا != تہی)
            *sp_پچھلا = sp;
    }

    واپس sp;
}

/*
 * Allocate a new sign
 */
ساکن sign_T *
alloc_new_sign(char_u *name)
{
    صحیح آغاز = اگلا_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(حجم(sign_T), aid_sign_define_by_name);
    اگر (sp == تہی)
        واپس تہی;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = پہلا_sign;
    جب (lp != تہی)
    {
        اگر (lp->sn_typenr == اگلا_sign_typenr)
        {
            ++اگلا_sign_typenr;

            اگر (اگلا_sign_typenr == MAX_TYPENR)
                اگلا_sign_typenr = 1;

            اگر (اگلا_sign_typenr == آغاز)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                واپس تہی;
            }

            lp = پہلا_sign; // start all over
            جاری;
        }
        lp = lp->sn_اگلا;
    }

    sp->sn_typenr = اگلا_sign_typenr;

    اگر (++اگلا_sign_typenr == MAX_TYPENR)
        اگلا_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    اگر (sp->sn_name == تہی) // out of memory
    {
        vim_free(sp);
        واپس تہی;
    }

    واپس sp;
}

/*
 * Initialize the icon information for a new sign
 */
ساکن خالی
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    اگر (gui.اندر_use)
    {
        برآمد_flush();
        اگر (sp->sn_image != تہی)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_رجسٹر_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
ساکن صحیح
sign_define_init_متن(sign_T *sp, char_u *متن)
{
    char_u *s = تہی;
    char_u *endp = متن + (صحیح)STRLEN(متن);
    صحیح cells = 0;

    // Remove backslashes so that it is possible to use a space.
    برائے (s = متن; s + 1 < endp; ++s)
    {
        اگر (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    اگر (has_mbyte)
    {
        برائے (s = متن; s < endp; s += (*mb_ptr2len)(s))
        {
            اگر (!vim_isprintc((*mb_ptr2char)(s)))
                توڑو;

            cells += (*mb_ptr2cells)(s);
        }
    }
    ورنہ
    {
        برائے (s = متن; s < endp; ++s)
        {
            اگر (!vim_isprintc(*s))
                توڑو;
        }
        cells = (صحیح)(s - متن);
    }

    // Currently sign text must be one or two display cells
    اگر (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_متن_str), متن);
        واپس FAIL;
    }

    vim_free(sp->sn_متن);
    // Allocate one byte more if we need to pad up
    // with a space.
    صحیح len = (صحیح)(endp - متن + ((cells == 1) ? 1 : 0));
    sp->sn_متن = vim_strnsave(متن, len);

    // For single character sign text, pad with a space.
    اگر (sp->sn_متن != تہی && cells == 1)
        STRCPY(sp->sn_متن + len - 1, " ");

    واپس OK;
}

/*
 * Define a new sign or update an existing sign
 */
صحیح
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *متن,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    صحیح prio)
{
    sign_T *sp_پچھلا = تہی;
    sign_T *sp = sign_ڈھونڈو(name, &sp_پچھلا);
    اگر (sp == تہی)
    {
        sp = alloc_new_sign(name);
        اگر (sp == تہی)
            واپس FAIL;

        // add the new sign to the list of signs
        اگر (sp_پچھلا == تہی)
            پہلا_sign = sp;
        ورنہ
            sp_پچھلا->sn_اگلا = sp;
    }
    ورنہ
    {
        win_T *wp = تہی;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            اگر (wp->w_بفر->b_signlist != تہی)
                redraw_buf_later(wp->w_بفر, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    اگر (icon != تہی)
        sign_define_init_icon(sp, icon);

    اگر (متن != تہی && (sign_define_init_متن(sp, متن) == FAIL))
        واپس FAIL;

    sp->sn_ترجیح = prio;

    اگر (linehl != تہی)
    {
        اگر (*linehl == NUL)
            sp->sn_سطر_hl = 0;
        ورنہ
            sp->sn_سطر_hl = syn_check_گروہ(linehl, (صحیح)STRLEN(linehl));
    }

    اگر (texthl != تہی)
    {
        اگر (*texthl == NUL)
            sp->sn_متن_hl = 0;
        ورنہ
            sp->sn_متن_hl = syn_check_گروہ(texthl, (صحیح)STRLEN(texthl));
    }

    اگر (culhl != تہی)
    {
        اگر (*culhl == NUL)
            sp->sn_cul_hl = 0;
        ورنہ
            sp->sn_cul_hl = syn_check_گروہ(culhl, (صحیح)STRLEN(culhl));
    }

    اگر (numhl != تہی)
    {
        اگر (*numhl == NUL)
            sp->sn_عدد_hl = 0;
        ورنہ
            sp->sn_عدد_hl = syn_check_گروہ(numhl, (صحیح)STRLEN(numhl));
    }

    واپس OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
صحیح
sign_exists_by_name(char_u *name)
{
    واپس sign_ڈھونڈو(name, تہی) != تہی;
}

/*
 * Free the sign specified by 'name'.
 */
صحیح
sign_undefine_by_name(char_u *name, صحیح give_error)
{
    sign_T *sp_پچھلا = تہی;
    sign_T *sp = sign_ڈھونڈو(name, &sp_پچھلا);
    اگر (sp == تہی)
    {
        اگر (give_error)
            semsg(_(e_unknown_sign_str), name);
        واپس FAIL;
    }
    sign_undefine(sp, sp_پچھلا);

    واپس OK;
}

/*
 * List the signs matching 'name'
 */
ساکن خالی
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_ڈھونڈو(name, تہی);
    اگر (sp != تہی)
        sign_list_defined(sp);
    ورنہ
        semsg(_(e_unknown_sign_str), name);
}

ساکن خالی
may_force_numberwidth_recompute(buf_T *buf, صحیح unplace)
{
# اگر defined(FEAT_LINEBREAK)
    tabpage_T *tp = تہی;
    win_T *wp = تہی;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        اگر (wp->w_بفر == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_چوڑائی < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_سطر_گنتی = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
صحیح
sign_رکھو(صحیح *sign_id,
           char_u *sign_گروہ,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           صحیح prio)
{
    // Check for reserved character '*' in group name
    اگر (sign_گروہ != تہی && (*sign_گروہ == '*' || *sign_گروہ == '\0'))
        واپس FAIL;

    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
    {
        اگر (STRCMP(sp->sn_name, sign_name) == 0)
            توڑو;
    }

    اگر (sp == تہی)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        واپس FAIL;
    }

    اگر (*sign_id == 0)
        *sign_id = sign_گروہ_get_اگلا_signid(buf, sign_گروہ);

    // Use the default priority value for this sign.
    اگر (prio == -1)
        prio = (sp->sn_ترجیح != -1) ? sp->sn_ترجیح : SIGN_DEF_PRIO;

    اگر (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_گروہ, prio, lnum, sp->sn_typenr);
    }
    ورنہ
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_گروہ, sp->sn_typenr,
                                    prio);
    }

    اگر (lnum > 0)
    {
        redraw_buf_سطر_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    ورنہ
    {
        semsg(_(e_not_possible_مقصد_change_sign_str), sign_name);
        واپس FAIL;
    }

    واپس OK;
}

/*
 * Unplace the specified sign
 */
ساکن صحیح
sign_unplace(صحیح sign_id, char_u *sign_گروہ, buf_T *buf, linenr_T atlnum)
{
    اگر (buf->b_signlist == تہی) // No signs in the buffer
        واپس OK;

    اگر (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_گروہ);
    }
    ورنہ
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_گروہ);
        اگر (lnum == 0)
            واپس FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    اگر (buf->b_signlist == تہی)
        may_force_numberwidth_recompute(buf, TRUE);

    واپس OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
ساکن خالی
sign_unplace_at_cursor(char_u *groupname)
{
    صحیح id =
        buf_findsign_id(curwin->w_بفر, curwin->w_cursor.lnum, groupname);

    اگر (id > 0)
        sign_unplace(id, groupname, curwin->w_بفر, curwin->w_cursor.lnum);
    ورنہ
        emsg(_(e_missing_sign_نمبر));
}

/*
 * Jump to a sign.
 */
ساکن linenr_T
sign_jump(صحیح sign_id, char_u *sign_گروہ, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_گروہ);
    اگر (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        واپس -1;
    }

    // goto a sign ...
    اگر (buf_jump_open_win(buf) != تہی)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    ورنہ
    { // ... not currently in a window
        اگر (buf->b_fname == تہی)
        {
            emsg(_(e_cannot_jump_مقصد_بفر_that_does_not_have_name));
            واپس -1;
        }
        setpcmark();
        اگر (do_ecmd(buf->b_fnum, تہی, تہی, تہی, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            واپس -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    واپس lnum;
}

/*
 * ":sign define {name} ..." command
 */
ساکن خالی
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = تہی;
    char_u *p = cmdline;
    char_u *icon = تہی;
    char_u *متن = تہی;
    char_u *linehl = تہی;
    char_u *texthl = تہی;
    char_u *culhl = تہی;
    char_u *numhl = تہی;
    صحیح prio = -1;
    صحیح failed = FALSE;

    // set values for a defined sign.
    جب (TRUE)
    {
        arg = skipwhite(p);
        اگر (*arg == NUL)
            توڑو;

        p = skiptowhite_esc(arg);
        اگر (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            متن = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        ورنہ اگر (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((حرف *)arg);
        }
        ورنہ
        {
            semsg(_(e_invalid_دلیل_str), arg);
            failed = TRUE;
            توڑو;
        }
    }

    اگر (!failed)
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
ساکن خالی
sign_رکھو_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               صحیح id,
               char_u *گروہ,
               صحیح prio)
{
    اگر (id <= 0)
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
        اگر (lnum >= 0 || sign_name != تہی || (گروہ != تہی && *گروہ == '\0'))
            emsg(_(e_invalid_دلیل));
        ورنہ
            sign_list_placed(buf, گروہ);
    }
    ورنہ
    {
        // Place a new sign
        اگر (sign_name == تہی || buf == تہی ||
            (گروہ != تہی && *گروہ == '\0'))
        {
            emsg(_(e_invalid_دلیل));
            واپس;
        }

        sign_رکھو(&id, گروہ, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
ساکن خالی
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 صحیح id,
                 char_u *گروہ)
{
    اگر (lnum >= 0 || sign_name != تہی || (گروہ != تہی && *گروہ == '\0'))
    {
        emsg(_(e_invalid_دلیل));
        واپس;
    }

    اگر (id == -2)
    {
        اگر (buf != تہی)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, گروہ, buf, 0);
        }
        ورنہ
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                اگر (buf->b_signlist != تہی)
                    buf_delete_signs(buf, گروہ);
            }
        }
    }
    ورنہ
    {
        اگر (buf != تہی)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, گروہ, buf, 0);
        }
        ورنہ
        {
            اگر (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(گروہ);
            }
            ورنہ
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, گروہ, buf, 0);
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
ساکن خالی
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              صحیح id,
              char_u *گروہ)
{
    اگر (sign_name == تہی && گروہ == تہی && id == -1)
    {
        emsg(_(e_دلیل_required));
        واپس;
    }

    اگر (buf == تہی || (گروہ != تہی && *گروہ == '\0') || lnum >= 0 ||
        sign_name != تہی)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_دلیل));
        واپس;
    }

    (خالی)sign_jump(id, گروہ, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
ساکن صحیح
parse_sign_cmd_دلائل(صحیح cmd,
                    char_u *arg,
                    char_u **sign_name,
                    صحیح *signid,
                    char_u **گروہ,
                    صحیح *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = تہی;
    صحیح lnum_arg = FALSE;

    // first arg could be placed sign id
    اگر (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        اگر (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        ورنہ
        {
            arg = skipwhite(arg);
        }
    }

    جب (*arg != NUL)
    {
        اگر (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((حرف *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        ورنہ اگر (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            اگر (*signid != -1)
            {
                emsg(_(e_invalid_دلیل));
                واپس FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        ورنہ اگر (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            اگر (*arg != NUL)
                *arg++ = NUL;

            جب (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        ورنہ اگر (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *گروہ = arg;
            arg = skiptowhite(arg);
            اگر (*arg != NUL)
                *arg++ = NUL;
        }
        ورنہ اگر (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((حرف *)arg);
            arg = skiptowhite(arg);
        }
        ورنہ اگر (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            توڑو;
        }
        ورنہ اگر (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((صحیح)getdigits(&arg));

            اگر (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            توڑو;
        }
        ورنہ
        {
            emsg(_(e_invalid_دلیل));
            واپس FAIL;
        }

        arg = skipwhite(arg);
    }

    اگر (filename != تہی && *buf == تہی)
    {
        semsg(_(e_invalid_بفر_name_str), filename);
        واپس FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    اگر (filename == تہی &&
        ((cmd == SIGNCMD_رکھو && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_بفر;

    واپس OK;
}

/*
 * ":sign" command
 */
خالی
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    صحیح idx = sign_cmd_idx(arg, p);
    اگر (idx == SIGNCMD_آخری)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        واپس;
    }
    arg = skipwhite(p);

    اگر (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = تہی;
        صحیح id = -1;
        char_u *گروہ = تہی;
        صحیح prio = -1;
        buf_T *buf = تہی;
        linenr_T lnum = -1;

        // Parse command line arguments
        اگر (parse_sign_cmd_دلائل(idx, arg, &sign_name, &id, &گروہ, &prio, &buf,
                                &lnum) == FAIL)
            واپس;

        اگر (idx == SIGNCMD_رکھو)
            sign_رکھو_cmd(buf, lnum, sign_name, id, گروہ, prio);
        ورنہ اگر (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, گروہ);
        ورنہ اگر (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, گروہ);

        واپس;
    }

    // Define, undefine or list signs.
    اگر (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        برائے (sign_T *sp = پہلا_sign; sp != تہی && !got_int; sp = sp->sn_اگلا)
            sign_list_defined(sp);
    }
    ورنہ اگر (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    ورنہ
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        اگر (*p != NUL)
            *p++ = NUL;

        جب (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        اگر (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        ورنہ اگر (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        ورنہ
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
ساکن خالی
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_جوڑو_string(retdict, "name", sp->sn_name);

    اگر (sp->sn_icon != تہی)
        dict_جوڑو_string(retdict, "icon", sp->sn_icon);

    اگر (sp->sn_متن != تہی)
        dict_جوڑو_string(retdict, "text", sp->sn_متن);

    اگر (sp->sn_ترجیح > 0)
        dict_جوڑو_نمبر(retdict, "priority", sp->sn_ترجیح);

    اگر (sp->sn_سطر_hl > 0)
    {
        char_u *p = get_highlight_name_ext(تہی, sp->sn_سطر_hl - 1, FALSE);
        اگر (p == تہی)
            dict_جوڑو_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ورنہ
            dict_جوڑو_string(retdict, "linehl", p);
    }

    اگر (sp->sn_متن_hl > 0)
    {
        char_u *p = get_highlight_name_ext(تہی, sp->sn_متن_hl - 1, FALSE);
        اگر (p == تہی)
            dict_جوڑو_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ورنہ
            dict_جوڑو_string(retdict, "texthl", p);
    }

    اگر (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(تہی, sp->sn_cul_hl - 1, FALSE);
        اگر (p == تہی)
            dict_جوڑو_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ورنہ
            dict_جوڑو_string(retdict, "culhl", p);
    }

    اگر (sp->sn_عدد_hl > 0)
    {
        char_u *p = get_highlight_name_ext(تہی, sp->sn_عدد_hl - 1, FALSE);
        اگر (p == تہی)
            dict_جوڑو_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        ورنہ
            dict_جوڑو_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
ساکن خالی
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = پہلا_sign;

    اگر (name != تہی)
    {
        sp = sign_ڈھونڈو(name, تہی);
        اگر (sp == تہی)
            واپس;
    }

    برائے (; sp != تہی && !got_int; sp = sp->sn_اگلا)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        اگر (dict == تہی)
            واپس;

        اگر (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            واپس;
        }

        sign_getinfo(sp, dict);

        اگر (name != تہی) // handle only the specified sign
            توڑو;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
خالی
get_بفر_signs(buf_T *buf, list_T *l)
{
    sign_اندراج_T *sign = تہی;
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        اگر (d != تہی && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
ساکن خالی
sign_get_placed_اندر_buf(buf_T *buf,
                       linenr_T lnum,
                       صحیح sign_id,
                       char_u *sign_گروہ,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    اگر (d == تہی)
        واپس;

    اگر (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        واپس;
    }

    dict_جوڑو_نمبر(d, "bufnr", (طویل)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    اگر (l == تہی)
        واپس;

    اگر (dict_جوڑو_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        واپس;
    }

    sign_اندراج_T *sign = تہی;
    FOR_ALL_SIGNS_اندر_BUF(buf, sign)
    {
        اگر (!sign_اندر_گروہ(sign, sign_گروہ))
            جاری;

        اگر ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            اگر (sdict != تہی && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
ساکن خالی
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                صحیح sign_id,
                char_u *sign_گروہ,
                list_T *retlist)
{
    اگر (buf != تہی)
    {
        sign_get_placed_اندر_buf(buf, lnum, sign_id, sign_گروہ, retlist);
    }
    ورنہ
    {
        FOR_ALL_BUFFERS(buf)
        {
            اگر (buf->b_signlist != تہی)
                sign_get_placed_اندر_buf(buf, 0, sign_id, sign_گروہ, retlist);
        }
    }
}

# اگر defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
خالی
sign_gui_started(خالی)
{
    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
    {
        اگر (sp->sn_icon != تہی)
            sp->sn_image = gui_mch_رجسٹر_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
ساکن خالی
sign_list_defined(sign_T *sp)
{
    حرف lbuf[پیغام_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    اگر (sp->sn_icon != تہی)
    {
        پیغام_puts(" icon=");
        پیغام_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        اگر (sp->sn_image == تہی)
            پیغام_puts(_(" (NOT FOUND)"));
# ورنہ
        پیغام_puts(_(" (not supported)"));
# endif
    }

    اگر (sp->sn_متن != تہی)
    {
        پیغام_puts(" text=");
        پیغام_outtrans(sp->sn_متن);
    }

    اگر (sp->sn_ترجیح > 0)
    {
        vim_snprintf(lbuf, پیغام_BUF_LEN, " priority=%d", sp->sn_ترجیح);
        پیغام_puts(lbuf);
    }

    اگر (sp->sn_سطر_hl > 0)
    {
        پیغام_puts(" linehl=");

        char_u *p = get_highlight_name_ext(تہی, sp->sn_سطر_hl - 1, FALSE);
        اگر (p == تہی)
            پیغام_puts("NONE");
        ورنہ
            پیغام_puts((حرف *)p);
    }

    اگر (sp->sn_متن_hl > 0)
    {
        پیغام_puts(" texthl=");

        char_u *p = get_highlight_name_ext(تہی, sp->sn_متن_hl - 1, FALSE);
        اگر (p == تہی)
            پیغام_puts("NONE");
        ورنہ
            پیغام_puts((حرف *)p);
    }

    اگر (sp->sn_cul_hl > 0)
    {
        پیغام_puts(" culhl=");

        char_u *p = get_highlight_name_ext(تہی, sp->sn_cul_hl - 1, FALSE);
        اگر (p == تہی)
            پیغام_puts("NONE");
        ورنہ
            پیغام_puts((حرف *)p);
    }

    اگر (sp->sn_عدد_hl > 0)
    {
        پیغام_puts(" numhl=");

        char_u *p = get_highlight_name_ext(تہی, sp->sn_عدد_hl - 1, FALSE);
        اگر (p == تہی)
            پیغام_puts("NONE");
        ورنہ
            پیغام_puts((حرف *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
ساکن خالی
sign_undefine(sign_T *sp, sign_T *sp_پچھلا)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    اگر (sp->sn_image != تہی)
    {
        برآمد_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_متن);

    اگر (sp_پچھلا == تہی)
        پہلا_sign = sp->sn_اگلا;
    ورنہ
        sp_پچھلا->sn_اگلا = sp->sn_اگلا;

    vim_free(sp);
}

# اگر defined(FEAT_SIGN_ICONS)
خالی *
sign_get_image(صحیح typenr) // the attribute which may have a sign
{
    sign_T *sp = تہی;
    FOR_ALL_SIGNS(sp)
    {
        اگر (sp->sn_typenr == typenr)
            واپس sp->sn_image;
    }
    واپس تہی;
}
# endif

/*
 * Undefine/free all signs.
 */
خالی
free_signs(خالی)
{
    جب (پہلا_sign != تہی)
        sign_undefine(پہلا_sign, تہی);
}

ساکن شمار
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_رکھو, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_نام, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
ساکن char_u *
get_nth_sign_name(صحیح idx)
{
    صحیح current_idx = 0;
    sign_T *sp = تہی;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        اگر (current_idx++ == idx)
            واپس sp->sn_name;
    }
    واپس تہی;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
ساکن char_u *
get_nth_sign_گروہ_name(صحیح idx)
{
    صحیح current_idx = 0;
    صحیح todo = (صحیح)sg_جدول.ht_مستعمل;
    hashitem_T *hi = تہی;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_اشیاء(&sg_جدول, hi, todo)
    {
        اگر (!HASHITEM_خالی_خانے(hi))
        {
            --todo;
            اگر (current_idx++ == idx)
            {
                signgroup_T *گروہ = HI2SG(hi);
                واپس گروہ->sg_name;
            }
        }
    }
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    انتخاب (expand_what)
    {
        حالت EXP_SUBCMD:
            واپس (char_u *)cmds[idx];
        حالت EXP_DEFINE:
        {
            حرف *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", تہی };
            واپس (char_u *)define_arg[idx];
        }
        حالت EXP_رکھو:
        {
            حرف *رکھو_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", تہی };
            واپس (char_u *)رکھو_arg[idx];
        }
        حالت EXP_LIST:
        {
            حرف *list_arg[] = { "group=", "file=", "buffer=", تہی };
            واپس (char_u *)list_arg[idx];
        }
        حالت EXP_UNPLACE:
        {
            حرف *unplace_arg[] = { "group=", "file=", "buffer=", تہی };
            واپس (char_u *)unplace_arg[idx];
        }
        حالت EXP_SIGN_نام:
            واپس get_nth_sign_name(idx);
        حالت EXP_SIGN_GROUPS:
            واپس get_nth_sign_گروہ_name(idx);
        طےشدہ:
            واپس تہی;
    }
}

/*
 * Handle command line completion for :sign command.
 */
خالی
سیٹ_context_اندر_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *انتہا_subcmd;
    char_u *آخری;
    صحیح cmd_idx;
    char_u *begin_subcmd_دلائل;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_نمونہ = arg;

    انتہا_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    اگر (*انتہا_subcmd == NUL)
        واپس;

    cmd_idx = sign_cmd_idx(arg, انتہا_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_دلائل = skipwhite(انتہا_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_دلائل;
    کرو
    {
        p = skipwhite(p);
        آخری = p;
        p = skiptowhite(p);
    }
    جب (*p != NUL);

    p = vim_strchr(آخری, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    اگر (p == تہی)
    {
        // Expand last argument name (before equal sign).
        xp->xp_نمونہ = آخری;
        انتخاب (cmd_idx)
        {
            حالت SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                توڑو;
            حالت SIGNCMD_رکھو:
                // List placed signs
                اگر (VIM_ISDIGIT(*begin_subcmd_دلائل))
                    //   :sign place {id} {args}...
                    expand_what = EXP_رکھو;
                ورنہ
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                توڑو;
            حالت SIGNCMD_LIST:
            حالت SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_نام;
                توڑو;
            حالت SIGNCMD_JUMP:
            حالت SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                توڑو;
            طےشدہ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    ورنہ
    {
        // Expand last argument value (after equal sign).
        xp->xp_نمونہ = p + 1;
        انتخاب (cmd_idx)
        {
            حالت SIGNCMD_DEFINE:
                اگر (STRNCMP(آخری, "texthl", 6) == 0 ||
                    STRNCMP(آخری, "linehl", 6) == 0 ||
                    STRNCMP(آخری, "culhl", 5) == 0 ||
                    STRNCMP(آخری, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                ورنہ اگر (STRNCMP(آخری, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                ورنہ
                    xp->xp_context = EXPAND_NOTHING;
                توڑو;
            حالت SIGNCMD_رکھو:
                اگر (STRNCMP(آخری, "name", 4) == 0)
                    expand_what = EXP_SIGN_نام;
                ورنہ اگر (STRNCMP(آخری, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ورنہ اگر (STRNCMP(آخری, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ورنہ
                    xp->xp_context = EXPAND_NOTHING;
                توڑو;
            حالت SIGNCMD_UNPLACE:
            حالت SIGNCMD_JUMP:
                اگر (STRNCMP(آخری, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                ورنہ اگر (STRNCMP(آخری, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                ورنہ
                    xp->xp_context = EXPAND_NOTHING;
                توڑو;
            طےشدہ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
ساکن صحیح
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = تہی;
    char_u *icon = تہی;
    char_u *linehl = تہی;
    char_u *متن = تہی;
    char_u *texthl = تہی;
    char_u *culhl = تہی;
    char_u *numhl = تہی;
    صحیح prio = -1;
    صحیح retval = -1;

    اگر (name_arg == تہی && dict == تہی)
        واپس retval;

    اگر (name_arg == تہی)
        name = dict_get_string(dict, "name", TRUE);
    ورنہ
        name = vim_strsave(name_arg);

    اگر (name == تہی || name[0] == NUL)
        جاؤ cleanup;

    اگر (dict != تہی)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        متن = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_نمبر_def(dict, "priority", -1);
    }

    اگر (sign_define_by_name(name, icon, linehl, متن, texthl, culhl, numhl,
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

    واپس retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
ساکن خالی
sign_define_مضرب(list_T *l, list_T *retlist)
{
    listitem_T *li = تہی;
    FOR_ALL_LIST_اشیاء(l, li)
    {
        صحیح retval = -1;

        اگر (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(تہی, li->li_tv.vval.v_dict);
        ورنہ
            emsg(_(e_dictionary_required));

        list_append_نمبر(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
خالی
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        واپس;

    اگر (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        اگر (rettv_list_alloc(rettv) == FAIL)
            واپس;

        sign_define_مضرب(argvars[0].vval.v_list, rettv->vval.v_list);
        واپس;
    }

    // Define a single sign
    rettv->vval.v_نمبر = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    اگر (name == تہی)
        واپس;

    اگر (check_for_opt_dict_arg(argvars, 1) == FAIL)
        واپس;

    rettv->vval.v_نمبر = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : تہی);
}

/*
 * "sign_getdefined()" function
 */
خالی
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    اگر (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        واپس;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        واپس;

    char_u *name = تہی;
    اگر (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
خالی
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = تہی;
    linenr_T lnum = 0;
    صحیح sign_id = 0;
    char_u *گروہ = تہی;

    اگر (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        واپس;

    اگر (اندر_vim9script() && (check_for_opt_بفر_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        اگر (buf == تہی)
            واپس;

        اگر (argvars[1].v_type != VAR_UNKNOWN)
        {
            اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                واپس;

            dictitem_T *di = تہی;
            dict_T *dict = argvars[1].vval.v_dict;

            اگر ((di = dict_ڈھونڈو(dict, (char_u *)"lnum", -1)) != تہی)
            {
                // get signs placed at this line
                صحیح notanum = FALSE;
                tv_get_نمبر_chk(&di->di_tv, &notanum);
                اگر (notanum)
                    واپس;

                lnum = tv_get_lnum(&di->di_tv);
            }

            اگر ((di = dict_ڈھونڈو(dict, (char_u *)"id", -1)) != تہی)
            {
                // get sign placed with this identifier
                صحیح notanum = FALSE;
                sign_id = (صحیح)tv_get_نمبر_chk(&di->di_tv, &notanum);
                اگر (notanum)
                    واپس;
            }

            اگر ((di = dict_ڈھونڈو(dict, (char_u *)"group", -1)) != تہی)
            {
                گروہ = tv_get_string_chk(&di->di_tv);
                اگر (گروہ == تہی)
                    واپس;

                اگر (*گروہ == '\0') // empty string means global group
                    گروہ = تہی;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, گروہ, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
خالی
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_گروہ = تہی;
    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script() && (check_for_نمبر_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_بفر_arg(argvars, 2) == FAIL))
        واپس;

    صحیح notanum = FALSE;
    // Sign identifier
    صحیح sign_id = (صحیح)tv_get_نمبر_chk(&argvars[0], &notanum);
    اگر (notanum)
        واپس;

    اگر (sign_id <= 0)
    {
        emsg(_(e_invalid_دلیل));
        واپس;
    }

    // Sign group
    sign_گروہ = tv_get_string_chk(&argvars[1]);
    اگر (sign_گروہ == تہی)
        واپس;

    اگر (sign_گروہ[0] == '\0')
    {
        sign_گروہ = تہی; // global sign group
    }
    ورنہ
    {
        sign_گروہ = vim_strsave(sign_گروہ);
        اگر (sign_گروہ == تہی)
            واپس;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    اگر (buf == تہی)
        جاؤ cleanup;

    rettv->vval.v_نمبر = sign_jump(sign_id, sign_گروہ, buf);

cleanup:
    vim_free(sign_گروہ);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
ساکن صحیح
sign_رکھو_from_dict(typval_T *id_tv,
                     typval_T *گروہ_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    صحیح sign_id = 0;
    char_u *گروہ = تہی;
    char_u *sign_name = تہی;
    buf_T *buf = تہی;
    dictitem_T *di = تہی;
    linenr_T lnum = 0;
    صحیح prio = -1;
    صحیح ret_sign_id = -1;

    // sign identifier
    اگر (id_tv == تہی)
    {
        di = dict_ڈھونڈو(dict, (char_u *)"id", -1);
        اگر (di != تہی)
            id_tv = &di->di_tv;
    }

    اگر (id_tv == تہی)
    {
        sign_id = 0;
    }
    ورنہ
    {
        صحیح notanum = FALSE;
        sign_id = tv_get_نمبر_chk(id_tv, &notanum);

        اگر (notanum)
            واپس -1;

        اگر (sign_id < 0)
        {
            emsg(_(e_invalid_دلیل));
            واپس -1;
        }
    }

    // sign group
    اگر (گروہ_tv == تہی)
    {
        di = dict_ڈھونڈو(dict, (char_u *)"group", -1);
        اگر (di != تہی)
            گروہ_tv = &di->di_tv;
    }

    اگر (گروہ_tv == تہی)
    {
        گروہ = تہی; // global group
    }
    ورنہ
    {
        گروہ = tv_get_string_chk(گروہ_tv);
        اگر (گروہ == تہی)
            جاؤ cleanup;

        اگر (گروہ[0] == '\0') // global sign group
        {
            گروہ = تہی;
        }
        ورنہ
        {
            گروہ = vim_strsave(گروہ);
            اگر (گروہ == تہی)
                واپس -1;
        }
    }

    // sign name
    اگر (name_tv == تہی)
    {
        di = dict_ڈھونڈو(dict, (char_u *)"name", -1);
        اگر (di != تہی)
            name_tv = &di->di_tv;
    }

    اگر (name_tv == تہی)
        جاؤ cleanup;

    sign_name = tv_get_string_chk(name_tv);
    اگر (sign_name == تہی)
        جاؤ cleanup;

    // buffer to place the sign
    اگر (buf_tv == تہی)
    {
        di = dict_ڈھونڈو(dict, (char_u *)"buffer", -1);
        اگر (di != تہی)
            buf_tv = &di->di_tv;
    }

    اگر (buf_tv == تہی)
        جاؤ cleanup;

    buf = get_buf_arg(buf_tv);
    اگر (buf == تہی)
        جاؤ cleanup;

    // line number of the sign
    di = dict_ڈھونڈو(dict, (char_u *)"lnum", -1);
    اگر (di != تہی)
    {
        lnum = tv_get_lnum(&di->di_tv);
        اگر (lnum <= 0)
        {
            emsg(_(e_invalid_دلیل));
            جاؤ cleanup;
        }
    }

    // sign priority
    di = dict_ڈھونڈو(dict, (char_u *)"priority", -1);
    اگر (di != تہی)
    {
        صحیح notanum = FALSE;
        prio = (صحیح)tv_get_نمبر_chk(&di->di_tv, &notanum);
        اگر (notanum)
            جاؤ cleanup;
    }

    اگر (sign_رکھو(&sign_id, گروہ, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(گروہ);

    واپس ret_sign_id;
}

/*
 * "sign_place()" function
 */
خالی
f_sign_رکھو(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = تہی;
    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script() && (check_for_نمبر_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_بفر_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        واپس;

    اگر (argvars[4].v_type != VAR_UNKNOWN)
    {
        اگر (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            واپس;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_نمبر = sign_رکھو_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
خالی
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
        واپس;

    اگر (اندر_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        واپس;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
        واپس;

    // Process the List of sign attributes
    listitem_T *li = تہی;
    FOR_ALL_LIST_اشیاء(argvars[0].vval.v_list, li)
    {
        صحیح sign_id = -1;

        اگر (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_رکھو_from_dict(تہی, تہی, تہی, تہی,
                                           li->li_tv.vval.v_dict);
        ورنہ
            emsg(_(e_dictionary_required));

        list_append_نمبر(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
ساکن خالی
sign_undefine_مضرب(list_T *l, list_T *retlist)
{
    listitem_T *li = تہی;
    FOR_ALL_LIST_اشیاء(l, li)
    {
        صحیح retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        اگر (name != تہی && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_نمبر(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
خالی
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    اگر (اندر_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        واپس;

    اگر (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        اگر (rettv_list_alloc(rettv) == FAIL)
            واپس;

        sign_undefine_مضرب(argvars[0].vval.v_list, rettv->vval.v_list);
        واپس;
    }

    rettv->vval.v_نمبر = -1;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_نمبر = 0;
    }
    ورنہ
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        اگر (name == تہی)
            واپس;

        اگر (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_نمبر = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
ساکن صحیح
sign_unplace_from_dict(typval_T *گروہ_tv, dict_T *dict)
{
    صحیح sign_id = 0;
    buf_T *buf = تہی;
    char_u *گروہ = تہی;
    صحیح retval = -1;

    // sign group
    گروہ = (گروہ_tv != تہی) ? tv_get_string(گروہ_tv)
                               : dict_get_string(dict, "group", FALSE);

    اگر (گروہ != تہی)
    {
        اگر (گروہ[0] == '\0') // global sign group
        {
            گروہ = تہی;
        }
        ورنہ
        {
            گروہ = vim_strsave(گروہ);
            اگر (گروہ == تہی)
                واپس retval;
        }
    }

    اگر (dict != تہی)
    {
        dictitem_T *di = dict_ڈھونڈو(dict, (char_u *)"buffer", -1);
        اگر (di != تہی)
        {
            buf = get_buf_arg(&di->di_tv);
            اگر (buf == تہی)
                جاؤ cleanup;
        }

        اگر (dict_has_کلید(dict, "id"))
        {
            sign_id = dict_get_نمبر(dict, "id");
            اگر (sign_id <= 0)
            {
                emsg(_(e_invalid_دلیل));
                جاؤ cleanup;
            }
        }
    }

    اگر (buf == تہی)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            اگر (sign_unplace(sign_id, گروہ, buf, 0) != OK)
                retval = -1;
    }
    ورنہ اگر (sign_unplace(sign_id, گروہ, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(گروہ);

    واپس retval;
}

sign_اندراج_T *
get_پہلا_valid_sign(win_T *wp)
{
    sign_اندراج_T *sign = wp->w_بفر->b_signlist;

# ifdef FEAT_PROP_POPUP
    جب (sign != تہی && !sign_گروہ_for_کھڑکی(sign, wp))
        sign = sign->se_اگلا;
# endif
    واپس sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
صحیح
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    اگر (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        واپس get_پہلا_valid_sign(wp) != تہی && !wp->w_p_nu && !wp->w_p_rnu;

    اگر (*wp->w_p_scl == 'n')
        واپس FALSE;

    اگر (*wp->w_p_scl == 'y')
        واپس TRUE;

    واپس (get_پہلا_valid_sign(wp) != تہی
# ifdef FEAT_NETBEANS_INTG
            || wp->w_بفر->b_has_sign_عمود
# endif
    );
}

/*
 * "sign_unplace()" function
 */
خالی
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = تہی;
    rettv->vval.v_نمبر = -1;

    اگر ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_نمبر = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
خالی
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
        واپس;

    اگر (اندر_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        واپس;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
        واپس;

    listitem_T *li = تہی;
    FOR_ALL_LIST_اشیاء(argvars[0].vval.v_list, li)
    {
        صحیح retval = -1;

        اگر (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(تہی, li->li_tv.vval.v_dict);
        ورنہ
            emsg(_(e_dictionary_required));

        list_append_نمبر(rettv->vval.v_list, retval);
    }
}

#اگر_ختم // FEAT_SIGNS
