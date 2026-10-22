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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SIGNS)

/*
 * Struct to hold the sign properties.
 */
రకనిర్వచనం నిర్మాణం sign sign_T;

నిర్మాణం sign
{
    sign_T *sn_తదుపరి; // next sign in list
    పూర్ణాంకం sn_typenr; // type number of sign
    char_u *sn_name; // name of sign
    char_u *sn_icon; // name of pixmap
# ifdef FEAT_SIGN_ICONS
    శూన్య *sn_image; // icon image
# endif
    char_u *sn_పాఠ్యం; // text used instead of pixmap
    పూర్ణాంకం sn_పంక్తి_hl; // highlight ID for line
    పూర్ణాంకం sn_పాఠ్యం_hl; // highlight ID for text
    పూర్ణాంకం sn_cul_hl; // highlight ID for text on current line when 'cursorline' is set
    పూర్ణాంకం sn_అంకె_hl; // highlight ID for line number
    పూర్ణాంకం sn_ప్రాధాన్యత; // default priority of this sign, -1 means SIGN_DEF_PRIO
};

స్థిర sign_T *మొదటి_sign = ఖాళీ;
స్థిర పూర్ణాంకం తదుపరి_sign_typenr = 1;

స్థిర శూన్య sign_list_defined(sign_T *sp);
స్థిర శూన్య sign_undefine(sign_T *sp, sign_T *sp_prev);

స్థిర అక్షరం *cmds[] = { "define",
# define SIGNCMD_DEFINE 0
                        "undefine",
# define SIGNCMD_UNDEFINE 1
                        "list",
# define SIGNCMD_LIST 2
                        "place",
# define SIGNCMD_ఉంచు 3
                        "unplace",
# define SIGNCMD_UNPLACE 4
                        "jump",
# define SIGNCMD_JUMP 5
                        ఖాళీ
# define SIGNCMD_చివరి 6
};

# define FOR_ALL_SIGNS(sp) \
     కొరకు ((sp) = మొదటి_sign; (sp) != ఖాళీ; (sp) = (sp)->sn_తదుపరి)

స్థిర hashtab_T sg_పట్టిక; // sign group (signgroup_T) hashtable
స్థిర పూర్ణాంకం తదుపరి_sign_id = 1; // next sign id in the global group

/*
 * Initialize data needed for managing signs
 */
శూన్య
init_signs(శూన్య)
{
    hash_init(&sg_పట్టిక); // sign group hash table
}

/*
 * A new sign in group 'groupname' is added. If the group is not present,
 * create it. Otherwise reference the group.
 */
స్థిర signgroup_T *
sign_సమూహం_ref(char_u *groupname)
{
    hash_T hash = hash_hash(groupname);
    hashitem_T *hi = hash_lookup(&sg_పట్టిక, groupname, hash);
    signgroup_T *సమూహం = ఖాళీ;

    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
        // new group
        సమూహం = alloc(offsetof(signgroup_T, sg_name) + STRLEN(groupname) + 1);
        ఒకవేళ (సమూహం == ఖాళీ)
            తిరిగి ఖాళీ;

        STRCPY(సమూహం->sg_name, groupname);
        సమూహం->sg_refcount = 1;
        సమూహం->sg_తదుపరి_sign_id = 1;
        hash_కూడు_అంశం(&sg_పట్టిక, hi, సమూహం->sg_name, hash);
    }
    లేకపోతే
    {
        // existing group
        సమూహం = HI2SG(hi);
        సమూహం->sg_refcount++;
    }

    తిరిగి సమూహం;
}

/*
 * A sign in group 'groupname' is removed. If all the signs in this group are
 * removed, then remove the group.
 */
స్థిర శూన్య
sign_సమూహం_unref(char_u *groupname)
{
    hashitem_T *hi = hash_కనుగొను(&sg_పట్టిక, groupname);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
        తిరిగి;

    signgroup_T *సమూహం = HI2SG(hi);
    సమూహం->sg_refcount--;
    ఒకవేళ (సమూహం->sg_refcount == 0)
    {
        // All the signs in this group are removed
        hash_remove(&sg_పట్టిక, hi, "sign remove");
        vim_free(సమూహం);
    }
}

/*
 * Returns TRUE if 'sign' is in 'group'.
 * A sign can either be in the global group (sign->se_group == NULL)
 * or in a named group. If 'group' is '*', then the sign is part of the group.
 */
స్థిర పూర్ణాంకం
sign_లోపల_సమూహం(sign_నమోదు_T *sign, char_u *సమూహం)
{
    తిరిగి ((సమూహం != ఖాళీ && STRCMP(సమూహం, "*") == 0) ||
            (సమూహం == ఖాళీ && sign->se_సమూహం == ఖాళీ) ||
            (సమూహం != ఖాళీ && sign->se_సమూహం != ఖాళీ &&
             STRCMP(సమూహం, sign->se_సమూహం->sg_name) == 0));
}

/*
 * Return TRUE if "sign" is to be displayed in window "wp".
 * If the group name starts with "PopUp" it only shows in a popup window.
 */
స్థిర పూర్ణాంకం
sign_సమూహం_for_కిటికీ(sign_నమోదు_T *sign, win_T *wp)
{
    పూర్ణాంకం for_popup = sign->se_సమూహం != ఖాళీ &&
                    STRNCMP("PopUp", sign->se_సమూహం->sg_name, 5) == 0;

    తిరిగి WIN_IS_POPUP(wp) ? for_popup : !for_popup;
}

/*
 * Get the next free sign identifier in the specified group
 */
స్థిర పూర్ణాంకం
sign_సమూహం_get_తదుపరి_signid(buf_T *buf, char_u *groupname)
{
    పూర్ణాంకం id = 1;
    signgroup_T *సమూహం = ఖాళీ;
    sign_నమోదు_T *sign = ఖాళీ;

    ఒకవేళ (groupname != ఖాళీ)
    {
        hashitem_T *hi = hash_కనుగొను(&sg_పట్టిక, groupname);
        ఒకవేళ (HASHITEM_ఖాళీలు(hi))
            తిరిగి id;
        సమూహం = HI2SG(hi);
    }

    // Search for the next usable sign identifier
    పూర్ణాంకం దొరికింది = FALSE;
    అయితే (!దొరికింది)
    {
        ఒకవేళ (సమూహం == ఖాళీ)
            id = తదుపరి_sign_id++; // global group
        లేకపోతే
            id = సమూహం->sg_తదుపరి_sign_id++;

        // Check whether this sign is already placed in the buffer
        దొరికింది = TRUE;
        FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
        {
            ఒకవేళ (id == sign->se_id && sign_లోపల_సమూహం(sign, groupname))
            {
                దొరికింది = FALSE; // sign identifier is in use
                ఆపు;
            }
        }
    }

    తిరిగి id;
}

/*
 * Insert a new sign into the signlist for buffer 'buf' between the 'prev' and
 * 'next' signs.
 */
స్థిర శూన్య
చొప్పించు_sign(buf_T *buf, // buffer to store sign in
            sign_నమోదు_T *prev, // previous sign entry
            sign_నమోదు_T *తదుపరి, // next sign entry
            పూర్ణాంకం id, // sign ID
            char_u *సమూహం, // sign group; NULL for global group
            పూర్ణాంకం prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            పూర్ణాంకం typenr) // typenr of sign we are adding
{
    sign_నమోదు_T *newsign =
        lalloc_id(పరిమాణం(sign_నమోదు_T), FALSE, aid_చొప్పించు_sign);
    ఒకవేళ (newsign == ఖాళీ)
        తిరిగి;

    newsign->se_id = id;
    newsign->se_lnum = lnum;
    newsign->se_typenr = typenr;

    ఒకవేళ (సమూహం != ఖాళీ)
    {
        newsign->se_సమూహం = sign_సమూహం_ref(సమూహం);
        ఒకవేళ (newsign->se_సమూహం == ఖాళీ)
        {
            vim_free(newsign);
            తిరిగి;
        }
    }
    లేకపోతే
    {
        newsign->se_సమూహం = ఖాళీ;
    }

    newsign->se_ప్రాధాన్యత = prio;
    newsign->se_తదుపరి = తదుపరి;
    newsign->se_prev = prev;
    ఒకవేళ (తదుపరి != ఖాళీ)
        తదుపరి->se_prev = newsign;

    ఒకవేళ (prev == ఖాళీ)
    {
        // When adding first sign need to redraw the windows to create the
        // column for signs.
        ఒకవేళ (buf->b_signlist == ఖాళీ)
        {
            redraw_buf_later(buf, UPD_NOT_VALID);
            మారింది_పంక్తి_abv_curs();
        }

        // first sign in signlist
        buf->b_signlist = newsign;
# ifdef FEAT_NETBEANS_INTG
        ఒకవేళ (netbeans_చురుకైన())
            buf->b_has_sign_స్తంభం = true;
# endif
    }
    లేకపోతే
    {
        prev->se_తదుపరి = newsign;
    }

    // Remember the newly inserted sign so a following insert on a later line
    // can resume the search from here.
    buf->b_sign_finger = newsign;
}

/*
 * Insert a new sign sorted by line number and sign priority.
 */
స్థిర శూన్య
చొప్పించు_sign_by_lnum_prio(buf_T *buf, // buffer to store sign in
                         sign_నమోదు_T *prev, // previous sign entry
                         పూర్ణాంకం id, // sign ID
                         char_u *సమూహం, // sign group; NULL for global group
                         పూర్ణాంకం prio, // sign priority
                         linenr_T lnum, // line number which gets the mark
                         పూర్ణాంకం typenr) // typenr of sign we are adding
{
    // keep signs sorted by lnum and by priority: insert new sign at
    // the proper position in the list for this lnum.
    అయితే (prev != ఖాళీ && prev->se_lnum == lnum && prev->se_ప్రాధాన్యత <= prio)
        prev = prev->se_prev;

    sign_నమోదు_T *sign = (prev == ఖాళీ) ? buf->b_signlist : prev->se_తదుపరి;

    చొప్పించు_sign(buf, prev, sign, id, సమూహం, prio, lnum, typenr);
}

/*
 * Lookup a sign by typenr. Returns NULL if sign is not found.
 */
స్థిర sign_T *
కనుగొను_sign_by_typenr(పూర్ణాంకం typenr)
{
    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
        ఒకవేళ (sp->sn_typenr == typenr)
            తిరిగి sp;
    తిరిగి ఖాళీ;
}

/*
 * Get the name of a sign by its typenr.
 */
స్థిర char_u *
sign_typenr2name(పూర్ణాంకం typenr)
{
    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
        ఒకవేళ (sp->sn_typenr == typenr)
            తిరిగి sp->sn_name;
    తిరిగి (char_u *)_("[Deleted]");
}

/*
 * Return information about a sign in a Dict
 */
స్థిర dict_T *
sign_get_info(sign_నమోదు_T *sign)
{
    dict_T *d = dict_alloc_id(aid_sign_getinfo);
    ఒకవేళ (d == ఖాళీ)
        తిరిగి ఖాళీ;

    dict_కూడు_సంఖ్య(d, "id", sign->se_id);
    ఒకవేళ (sign->se_సమూహం == ఖాళీ)
        dict_కూడు_string_len(d, "group", (char_u *)"", 0);
    లేకపోతే
        dict_కూడు_string(d, "group", sign->se_సమూహం->sg_name);
    dict_కూడు_సంఖ్య(d, "lnum", sign->se_lnum);
    dict_కూడు_string(d, "name", sign_typenr2name(sign->se_typenr));
    dict_కూడు_సంఖ్య(d, "priority", sign->se_ప్రాధాన్యత);

    తిరిగి d;
}

/*
 * Sort the signs placed on the same line as "sign" by priority.  Invoked after
 * changing the priority of an already placed sign.  Assumes the signs in the
 * buffer are sorted by line number and priority.
 */
స్థిర శూన్య
sign_క్రమీకరించు_by_prio_on_పంక్తి(buf_T *buf, sign_నమోదు_T *sign)
{
    // If there is only one sign in the buffer or only one sign on the line or
    // the sign is already sorted by priority, then return.
    ఒకవేళ ((sign->se_prev == ఖాళీ || sign->se_prev->se_lnum != sign->se_lnum ||
         sign->se_prev->se_ప్రాధాన్యత > sign->se_ప్రాధాన్యత) &&
        (sign->se_తదుపరి == ఖాళీ || sign->se_తదుపరి->se_lnum != sign->se_lnum ||
         sign->se_తదుపరి->se_ప్రాధాన్యత < sign->se_ప్రాధాన్యత))
        తిరిగి;

    // One or more signs on the same line as 'sign'
    // Find a sign after which 'sign' should be inserted

    // First search backward for a sign with higher priority on the same line
    sign_నమోదు_T *p = sign;
    అయితే (p->se_prev != ఖాళీ && p->se_prev->se_lnum == sign->se_lnum &&
           p->se_prev->se_ప్రాధాన్యత <= sign->se_ప్రాధాన్యత)
        p = p->se_prev;

    ఒకవేళ (p == sign)
    {
        // Sign not found. Search forward for a sign with priority just before
        // 'sign'.
        p = sign->se_తదుపరి;
        అయితే (p->se_తదుపరి != ఖాళీ && p->se_తదుపరి->se_lnum == sign->se_lnum &&
               p->se_తదుపరి->se_ప్రాధాన్యత > sign->se_ప్రాధాన్యత)
            p = p->se_తదుపరి;
    }

    // Remove 'sign' from the list
    ఒకవేళ (buf->b_signlist == sign)
        buf->b_signlist = sign->se_తదుపరి;

    ఒకవేళ (sign->se_prev != ఖాళీ)
        sign->se_prev->se_తదుపరి = sign->se_తదుపరి;

    ఒకవేళ (sign->se_తదుపరి != ఖాళీ)
        sign->se_తదుపరి->se_prev = sign->se_prev;

    sign->se_prev = ఖాళీ;
    sign->se_తదుపరి = ఖాళీ;

    // Re-insert 'sign' at the right place
    ఒకవేళ (p->se_ప్రాధాన్యత <= sign->se_ప్రాధాన్యత)
    {
        // 'sign' has a higher priority and should be inserted before 'p'
        sign->se_prev = p->se_prev;
        sign->se_తదుపరి = p;
        p->se_prev = sign;
        ఒకవేళ (sign->se_prev != ఖాళీ)
            sign->se_prev->se_తదుపరి = sign;

        ఒకవేళ (buf->b_signlist == p)
            buf->b_signlist = sign;
    }
    లేకపోతే
    {
        // 'sign' has a lower priority and should be inserted after 'p'
        sign->se_prev = p;
        sign->se_తదుపరి = p->se_తదుపరి;
        p->se_తదుపరి = sign;
        ఒకవేళ (sign->se_తదుపరి != ఖాళీ)
            sign->se_తదుపరి->se_prev = sign;
    }
}

/*
 * Add the sign into the signlist. Find the right spot to do it though.
 */
స్థిర శూన్య
buf_addsign(buf_T *buf, // buffer to store sign in
            పూర్ణాంకం id, // sign ID
            char_u *groupname, // sign group
            పూర్ణాంకం prio, // sign priority
            linenr_T lnum, // line number which gets the mark
            పూర్ణాంకం typenr) // typenr of sign we are adding
{
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    sign_నమోదు_T *prev = ఖాళీ; // the previous sign

    // The list is sorted by line number.  When the last inserted sign is on a
    // strictly earlier line, resume the search from there instead of the head;
    // this makes placing many signs on distinct, ascending lines close to
    // linear.  Signs on the same line fall back to a scan from the head.
    ఒకవేళ (buf->b_sign_finger != ఖాళీ && buf->b_sign_finger->se_lnum < lnum)
        prev = buf->b_sign_finger;

    కొరకు (sign = (prev == ఖాళీ) ? buf->b_signlist : prev->se_తదుపరి; sign != ఖాళీ;
         sign = sign->se_తదుపరి)
    {
        ఒకవేళ (lnum == sign->se_lnum && id == sign->se_id &&
            sign_లోపల_సమూహం(sign, groupname))
        {
            // Update an existing sign
            sign->se_typenr = typenr;
            sign->se_ప్రాధాన్యత = prio;
            sign_క్రమీకరించు_by_prio_on_పంక్తి(buf, sign);
            తిరిగి;
        }
        లేకపోతే ఒకవేళ (lnum < sign->se_lnum)
        {
            చొప్పించు_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum,
                                     typenr);
            తిరిగి;
        }
        prev = sign;
    }

    చొప్పించు_sign_by_lnum_prio(buf, prev, id, groupname, prio, lnum, typenr);
}

/*
 * For an existing, placed sign "markId" change the type to "typenr".
 * Returns the line number of the sign, or zero if the sign is not found.
 */
స్థిర linenr_T
buf_change_sign_type(buf_T *buf, // buffer to store sign in
                     పూర్ణాంకం markId, // sign ID
                     char_u *సమూహం, // sign group
                     పూర్ణాంకం typenr, // typenr of sign we are adding
                     పూర్ణాంకం prio) // sign priority
{
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        ఒకవేళ (sign->se_id == markId && sign_లోపల_సమూహం(sign, సమూహం))
        {
            sign->se_typenr = typenr;
            sign->se_ప్రాధాన్యత = prio;
            sign_క్రమీకరించు_by_prio_on_పంక్తి(buf, sign);
            తిరిగి sign->se_lnum;
        }
    }

    తిరిగి (linenr_T)0;
}

/*
 * Return the attributes of the first sign placed on line 'lnum' in buffer
 * 'buf'. Used when refreshing the screen. Returns TRUE if a sign is found on
 * 'lnum', FALSE otherwise.
 */
పూర్ణాంకం
buf_get_signattrs(win_T *wp, linenr_T lnum, sign_attrs_T *sattr)
{
    CLEAR_సూచిక(sattr);

    buf_T *buf = wp->w_బఫరు;
    sign_నమోదు_T *sign = ఖాళీ;
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ఒకవేళ (sign->se_lnum > lnum)
            ఆపు;

        ఒకవేళ (sign->se_lnum == lnum
# ifdef FEAT_PROP_POPUP
            && sign_సమూహం_for_కిటికీ(sign, wp)
# endif
        )
        {
            sattr->sat_typenr = sign->se_typenr;
            sign_T *sp = కనుగొను_sign_by_typenr(sign->se_typenr);
            ఒకవేళ (sp == ఖాళీ)
                తిరిగి FALSE;

# ifdef FEAT_SIGN_ICONS
            sattr->sat_icon = sp->sn_image;
# endif
            sattr->sat_పాఠ్యం = sp->sn_పాఠ్యం;

            ఒకవేళ (sattr->sat_పాఠ్యం != ఖాళీ && sp->sn_పాఠ్యం_hl > 0)
                sattr->sat_texthl = syn_id2attr(sp->sn_పాఠ్యం_hl);

            ఒకవేళ (sp->sn_పంక్తి_hl > 0)
                sattr->sat_linehl = syn_id2attr(sp->sn_పంక్తి_hl);

            ఒకవేళ (sp->sn_cul_hl > 0)
                sattr->sat_culhl = syn_id2attr(sp->sn_cul_hl);

            ఒకవేళ (sp->sn_అంకె_hl > 0)
                sattr->sat_numhl = syn_id2attr(sp->sn_అంకె_hl);

            sattr->sat_ప్రాధాన్యత = sign->se_ప్రాధాన్యత;

            // If there is another sign next with the same priority, may
            // combine the text and the line highlighting.
            ఒకవేళ (sign->se_తదుపరి != ఖాళీ &&
                sign->se_తదుపరి->se_ప్రాధాన్యత == sign->se_ప్రాధాన్యత &&
                sign->se_తదుపరి->se_lnum == sign->se_lnum)
            {
                sign_T *తదుపరి_sp = కనుగొను_sign_by_typenr(sign->se_తదుపరి->se_typenr);
                ఒకవేళ (తదుపరి_sp == ఖాళీ)
                    తిరిగి FALSE;

                ఒకవేళ (sattr->sat_icon == ఖాళీ && sattr->sat_పాఠ్యం == ఖాళీ)
                {
# ifdef FEAT_SIGN_ICONS
                    sattr->sat_icon = తదుపరి_sp->sn_image;
# endif
                    sattr->sat_పాఠ్యం = తదుపరి_sp->sn_పాఠ్యం;
                }

                ఒకవేళ (sp->sn_పాఠ్యం_hl <= 0 && తదుపరి_sp->sn_పాఠ్యం_hl > 0)
                    sattr->sat_texthl = syn_id2attr(తదుపరి_sp->sn_పాఠ్యం_hl);

                ఒకవేళ (sp->sn_పంక్తి_hl <= 0 && తదుపరి_sp->sn_పంక్తి_hl > 0)
                    sattr->sat_linehl = syn_id2attr(తదుపరి_sp->sn_పంక్తి_hl);

                ఒకవేళ (sp->sn_cul_hl <= 0 && తదుపరి_sp->sn_cul_hl > 0)
                    sattr->sat_culhl = syn_id2attr(తదుపరి_sp->sn_cul_hl);

                ఒకవేళ (sp->sn_అంకె_hl <= 0 && తదుపరి_sp->sn_అంకె_hl > 0)
                    sattr->sat_numhl = syn_id2attr(తదుపరి_sp->sn_అంకె_hl);
            }
            తిరిగి TRUE;
        }
    }
    తిరిగి FALSE;
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
            పూర్ణాంకం id, // sign id
            char_u *సమూహం) // sign group
{
    buf->b_sign_finger = ఖాళీ; // a removal invalidates the insertion finger
    // pointer to pointer to current sign
    sign_నమోదు_T **lastp = &buf->b_signlist;
    sign_నమోదు_T *తదుపరి = ఖాళీ; // the next sign in a b_signlist
    linenr_T lnum = 0; // line number whose sign was deleted

    కొరకు (sign_నమోదు_T *sign = buf->b_signlist; sign != ఖాళీ; sign = తదుపరి)
    {
        తదుపరి = sign->se_తదుపరి;

        ఒకవేళ ((id == 0 || sign->se_id == id) &&
            (atlnum == 0 || sign->se_lnum == atlnum) &&
            sign_లోపల_సమూహం(sign, సమూహం))
        {
            *lastp = తదుపరి;
            ఒకవేళ (తదుపరి != ఖాళీ)
                తదుపరి->se_prev = sign->se_prev;

            lnum = sign->se_lnum;

            ఒకవేళ (sign->se_సమూహం != ఖాళీ)
                sign_సమూహం_unref(sign->se_సమూహం->sg_name);

            vim_free(sign);
            redraw_buf_పంక్తి_later(buf, lnum);

            // Check whether only one sign needs to be deleted
            // If deleting a sign with a specific identifier in a particular
            // group or deleting any sign at a particular line number, delete
            // only one sign.
            ఒకవేళ (సమూహం == ఖాళీ || (*సమూహం != '*' && id != 0) ||
                (*సమూహం == '*' && atlnum != 0))
                ఆపు;
        }
        లేకపోతే
        {
            lastp = &sign->se_తదుపరి;
        }
    }

    // When deleting the last sign the cursor position may change, because the
    // sign columns no longer shows.  And the 'signcolumn' may be hidden.
    ఒకవేళ (buf->b_signlist == ఖాళీ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        మారింది_పంక్తి_abv_curs();
    }

    తిరిగి lnum;
}

/*
 * Find the line number of the sign with the requested id in group 'group'. If
 * the sign does not exist, return 0 as the line number. This will still let
 * the correct file get loaded.
 */
పూర్ణాంకం
buf_findsign(buf_T *buf, // buffer to store sign in
             పూర్ణాంకం id, // sign ID
             char_u *సమూహం) // sign group
{
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
        ఒకవేళ (sign->se_id == id && sign_లోపల_సమూహం(sign, సమూహం))
            తిరిగి sign->se_lnum;

    తిరిగి 0;
}

/*
 * Return the sign at line 'lnum' in buffer 'buf'. Returns NULL if a sign is
 * not found at the line. If 'groupname' is NULL, searches in the global group.
 */
స్థిర sign_నమోదు_T *
buf_getsign_at_పంక్తి(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    char_u *groupname) // sign group name
{
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ఒకవేళ (sign->se_lnum > lnum)
            ఆపు;

        ఒకవేళ (sign->se_lnum == lnum && sign_లోపల_సమూహం(sign, groupname))
            తిరిగి sign;
    }

    తిరిగి ఖాళీ;
}

/*
 * Return the identifier of the sign at line number 'lnum' in buffer 'buf'.
 */
పూర్ణాంకం
buf_findsign_id(buf_T *buf, // buffer whose sign we are searching for
                linenr_T lnum, // line number of sign
                char_u *groupname) // sign group name
{
    // a sign in the signlist
    sign_నమోదు_T *sign = buf_getsign_at_పంక్తి(buf, lnum, groupname);
    ఒకవేళ (sign != ఖాళీ)
        తిరిగి sign->se_id;

    తిరిగి 0;
}

# ఒకవేళ defined(FEAT_NETBEANS_INTG)
/*
 * See if a given type of sign exists on a specific line.
 */
పూర్ణాంకం
buf_findsigntype_id(buf_T *buf, // buffer whose sign we are searching for
                    linenr_T lnum, // line number of sign
                    పూర్ణాంకం typenr) // sign type number
{
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ఒకవేళ (sign->se_lnum > lnum)
            ఆపు;

        ఒకవేళ (sign->se_lnum == lnum && sign->se_typenr == typenr)
            తిరిగి sign->se_id;
    }

    తిరిగి 0;
}

#  ఒకవేళ defined(FEAT_SIGN_ICONS)
/*
 * Return the number of icons on the given line.
 */
పూర్ణాంకం
buf_signcount(buf_T *buf, linenr_T lnum)
{
    పూర్ణాంకం లెక్క = 0;
    sign_నమోదు_T *sign = ఖాళీ; // a sign in the signlist
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        // Signs are sorted by line number in the buffer. No need to check
        // for signs after the specified line number 'lnum'.
        ఒకవేళ (sign->se_lnum > lnum)
            ఆపు;

        ఒకవేళ (sign->se_lnum == lnum && sign_get_image(sign->se_typenr) != ఖాళీ)
            లెక్క++;
    }

    తిరిగి లెక్క;
}
#  endif // FEAT_SIGN_ICONS
# endif // FEAT_NETBEANS_INTG

/*
 * Delete signs in group 'group' in buffer "buf". If 'group' is '*', then
 * delete all the signs.
 */
శూన్య
buf_delete_signs(buf_T *buf, char_u *సమూహం)
{
    buf->b_sign_finger = ఖాళీ; // a removal invalidates the insertion finger
    // When deleting the last sign need to redraw the windows to remove the
    // sign column. Not when curwin is NULL (this means we're exiting).
    ఒకవేళ (buf->b_signlist != ఖాళీ && curwin != ఖాళీ)
    {
        redraw_buf_later(buf, UPD_NOT_VALID);
        మారింది_పంక్తి_abv_curs();
    }

    // pointer to pointer to current sign
    sign_నమోదు_T **lastp = &buf->b_signlist;
    sign_నమోదు_T *తదుపరి = ఖాళీ;

    కొరకు (sign_నమోదు_T *sign = buf->b_signlist; sign != ఖాళీ; sign = తదుపరి)
    {
        తదుపరి = sign->se_తదుపరి;
        ఒకవేళ (sign_లోపల_సమూహం(sign, సమూహం))
        {
            *lastp = తదుపరి;

            ఒకవేళ (తదుపరి != ఖాళీ)
                తదుపరి->se_prev = sign->se_prev;

            ఒకవేళ (sign->se_సమూహం != ఖాళీ)
                sign_సమూహం_unref(sign->se_సమూహం->sg_name);

            vim_free(sign);
        }
        లేకపోతే
        {
            lastp = &sign->se_తదుపరి;
        }
    }
}

/*
 * List placed signs for "rbuf".  If "rbuf" is NULL do it for all buffers.
 */
స్థిర శూన్య
sign_list_placed(buf_T *rbuf, char_u *sign_సమూహం)
{
    అక్షరం lbuf[సందేశం_BUF_LEN];
    అక్షరం సమూహం[సందేశం_BUF_LEN];

    సందేశం_puts_title(_("\n--- Signs ---"));
    సందేశం_putchar('\n');

    buf_T *buf = (rbuf == ఖాళీ) ? firstbuf : rbuf;
    అయితే (buf != ఖాళీ && !got_int)
    {
        ఒకవేళ (buf->b_signlist != ఖాళీ)
        {
            vim_snprintf(lbuf, సందేశం_BUF_LEN, _("Signs for %s:"), buf->b_fname);
            సందేశం_puts_attr(lbuf, HL_ATTR(HLF_D));
            సందేశం_putchar('\n');
        }

        sign_నమోదు_T *sign = ఖాళీ;
        FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
        {
            ఒకవేళ (got_int)
                ఆపు;

            ఒకవేళ (!sign_లోపల_సమూహం(sign, sign_సమూహం))
                కొనసాగు;

            ఒకవేళ (sign->se_సమూహం != ఖాళీ)
                vim_snprintf(సమూహం, సందేశం_BUF_LEN, _("  group=%s"),
                             sign->se_సమూహం->sg_name);
            లేకపోతే
                సమూహం[0] = '\0';

            vim_snprintf(lbuf, సందేశం_BUF_LEN,
                         _("    line=%ld  id=%d%s  name=%s  priority=%d"),
                         (దీర్ఘ)sign->se_lnum, sign->se_id, సమూహం,
                         sign_typenr2name(sign->se_typenr), sign->se_ప్రాధాన్యత);

            సందేశం_puts(lbuf);
            సందేశం_putchar('\n');
        }

        ఒకవేళ (rbuf != ఖాళీ)
            ఆపు;

        buf = buf->b_తదుపరి;
    }
}

/*
 * Adjust a placed sign for inserted/deleted lines.
 */
శూన్య
sign_mark_adjust(
    linenr_T    line1,
    linenr_T    line2,
    దీర్ఘ        amount,
    దీర్ఘ        amount_after)
{
    curbuf->b_sign_finger = ఖాళీ; // line changes may reorder, drop the finger
    sign_నమోదు_T *sign = ఖాళీ; // a sign in a b_signlist
    FOR_ALL_SIGNS_లోపల_BUF(curbuf, sign)
    {
        // Ignore changes to lines after the sign
        ఒకవేళ (sign->se_lnum < line1)
            కొనసాగు;

        linenr_T new_lnum = sign->se_lnum;

        ఒకవేళ (sign->se_lnum <= line2)
        {
            ఒకవేళ (amount != MAXLNUM)
                new_lnum += amount;
        }
        లేకపోతే ఒకవేళ (sign->se_lnum > line2)
        {
            // Lines inserted or deleted before the sign
            new_lnum += amount_after;
        }

        // If the new sign line number is past the last line in the buffer,
        // then don't adjust the line number. Otherwise, it will always be past
        // the last line and will not be visible.
        ఒకవేళ (new_lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
            sign->se_lnum = new_lnum;
    }
}

/*
 * Find index of a ":sign" subcmd from its name.
 * "*end_cmd" must be writable.
 */
స్థిర పూర్ణాంకం
sign_cmd_idx(char_u *begin_cmd, // begin of sign subcmd
             char_u *చివర_cmd) // just after sign subcmd
{
    పూర్ణాంకం idx = 0;
    అక్షరం save = *చివర_cmd;
    *చివర_cmd = NUL;

    అయితే (cmds[idx] != ఖాళీ && STRCMP(begin_cmd, cmds[idx]) != 0)
        ++idx;

    *చివర_cmd = save;
    తిరిగి idx;
}

/*
 * Find a sign by name. Also returns pointer to the previous sign.
 */
స్థిర sign_T *
sign_కనుగొను(char_u *name, sign_T **sp_prev)
{
    ఒకవేళ (sp_prev != ఖాళీ)
        *sp_prev = ఖాళీ;

    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
    {
        ఒకవేళ (STRCMP(sp->sn_name, name) == 0)
            ఆపు;

        ఒకవేళ (sp_prev != ఖాళీ)
            *sp_prev = sp;
    }

    తిరిగి sp;
}

/*
 * Allocate a new sign
 */
స్థిర sign_T *
alloc_new_sign(char_u *name)
{
    పూర్ణాంకం ప్రారంభం = తదుపరి_sign_typenr;

    // Allocate a new sign.
    sign_T *sp = alloc_clear_id(పరిమాణం(sign_T), aid_sign_define_by_name);
    ఒకవేళ (sp == ఖాళీ)
        తిరిగి ఖాళీ;

    // Check that next_sign_typenr is not already being used.
    // This only happens after wrapping around.  Hopefully
    // another one got deleted and we can use its number.
    sign_T *lp = మొదటి_sign;
    అయితే (lp != ఖాళీ)
    {
        ఒకవేళ (lp->sn_typenr == తదుపరి_sign_typenr)
        {
            ++తదుపరి_sign_typenr;

            ఒకవేళ (తదుపరి_sign_typenr == MAX_TYPENR)
                తదుపరి_sign_typenr = 1;

            ఒకవేళ (తదుపరి_sign_typenr == ప్రారంభం)
            {
                vim_free(sp);
                emsg(_(e_too_many_signs_defined));
                తిరిగి ఖాళీ;
            }

            lp = మొదటి_sign; // start all over
            కొనసాగు;
        }
        lp = lp->sn_తదుపరి;
    }

    sp->sn_typenr = తదుపరి_sign_typenr;

    ఒకవేళ (++తదుపరి_sign_typenr == MAX_TYPENR)
        తదుపరి_sign_typenr = 1; // wrap around

    sp->sn_name = vim_strsave(name);
    ఒకవేళ (sp->sn_name == ఖాళీ) // out of memory
    {
        vim_free(sp);
        తిరిగి ఖాళీ;
    }

    తిరిగి sp;
}

/*
 * Initialize the icon information for a new sign
 */
స్థిర శూన్య
sign_define_init_icon(sign_T *sp, char_u *icon)
{
    vim_free(sp->sn_icon);
    sp->sn_icon = vim_strsave(icon);
    backslash_halve(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ఒకవేళ (gui.లోపల_use)
    {
        బహిర్గతం_flush();
        ఒకవేళ (sp->sn_image != ఖాళీ)
            gui_mch_destroy_sign(sp->sn_image);
        sp->sn_image = gui_mch_రిజిస్టరు_sign(sp->sn_icon);
    }
# endif
}

/*
 * Initialize the text for a new sign
 */
స్థిర పూర్ణాంకం
sign_define_init_పాఠ్యం(sign_T *sp, char_u *పాఠ్యం)
{
    char_u *s = ఖాళీ;
    char_u *endp = పాఠ్యం + (పూర్ణాంకం)STRLEN(పాఠ్యం);
    పూర్ణాంకం cells = 0;

    // Remove backslashes so that it is possible to use a space.
    కొరకు (s = పాఠ్యం; s + 1 < endp; ++s)
    {
        ఒకవేళ (*s == '\\')
        {
            STRMOVE(s, s + 1);
            --endp;
        }
    }

    // Count cells and check for non-printable chars
    ఒకవేళ (has_mbyte)
    {
        కొరకు (s = పాఠ్యం; s < endp; s += (*mb_ptr2len)(s))
        {
            ఒకవేళ (!vim_isprintc((*mb_ptr2char)(s)))
                ఆపు;

            cells += (*mb_ptr2cells)(s);
        }
    }
    లేకపోతే
    {
        కొరకు (s = పాఠ్యం; s < endp; ++s)
        {
            ఒకవేళ (!vim_isprintc(*s))
                ఆపు;
        }
        cells = (పూర్ణాంకం)(s - పాఠ్యం);
    }

    // Currently sign text must be one or two display cells
    ఒకవేళ (s != endp || cells < 1 || cells > 2)
    {
        semsg(_(e_invalid_sign_పాఠ్యం_str), పాఠ్యం);
        తిరిగి FAIL;
    }

    vim_free(sp->sn_పాఠ్యం);
    // Allocate one byte more if we need to pad up
    // with a space.
    పూర్ణాంకం len = (పూర్ణాంకం)(endp - పాఠ్యం + ((cells == 1) ? 1 : 0));
    sp->sn_పాఠ్యం = vim_strnsave(పాఠ్యం, len);

    // For single character sign text, pad with a space.
    ఒకవేళ (sp->sn_పాఠ్యం != ఖాళీ && cells == 1)
        STRCPY(sp->sn_పాఠ్యం + len - 1, " ");

    తిరిగి OK;
}

/*
 * Define a new sign or update an existing sign
 */
పూర్ణాంకం
sign_define_by_name(char_u *name,
                    char_u *icon,
                    char_u *linehl,
                    char_u *పాఠ్యం,
                    char_u *texthl,
                    char_u *culhl,
                    char_u *numhl,
                    పూర్ణాంకం prio)
{
    sign_T *sp_prev = ఖాళీ;
    sign_T *sp = sign_కనుగొను(name, &sp_prev);
    ఒకవేళ (sp == ఖాళీ)
    {
        sp = alloc_new_sign(name);
        ఒకవేళ (sp == ఖాళీ)
            తిరిగి FAIL;

        // add the new sign to the list of signs
        ఒకవేళ (sp_prev == ఖాళీ)
            మొదటి_sign = sp;
        లేకపోతే
            sp_prev->sn_తదుపరి = sp;
    }
    లేకపోతే
    {
        win_T *wp = ఖాళీ;
        // Signs may already exist, a redraw is needed in windows with a
        // non-empty sign list.
        FOR_ALL_WINDOWS(wp)
        {
            ఒకవేళ (wp->w_బఫరు->b_signlist != ఖాళీ)
                redraw_buf_later(wp->w_బఫరు, UPD_NOT_VALID);
        }
    }

    // set values for a defined sign.
    ఒకవేళ (icon != ఖాళీ)
        sign_define_init_icon(sp, icon);

    ఒకవేళ (పాఠ్యం != ఖాళీ && (sign_define_init_పాఠ్యం(sp, పాఠ్యం) == FAIL))
        తిరిగి FAIL;

    sp->sn_ప్రాధాన్యత = prio;

    ఒకవేళ (linehl != ఖాళీ)
    {
        ఒకవేళ (*linehl == NUL)
            sp->sn_పంక్తి_hl = 0;
        లేకపోతే
            sp->sn_పంక్తి_hl = syn_check_సమూహం(linehl, (పూర్ణాంకం)STRLEN(linehl));
    }

    ఒకవేళ (texthl != ఖాళీ)
    {
        ఒకవేళ (*texthl == NUL)
            sp->sn_పాఠ్యం_hl = 0;
        లేకపోతే
            sp->sn_పాఠ్యం_hl = syn_check_సమూహం(texthl, (పూర్ణాంకం)STRLEN(texthl));
    }

    ఒకవేళ (culhl != ఖాళీ)
    {
        ఒకవేళ (*culhl == NUL)
            sp->sn_cul_hl = 0;
        లేకపోతే
            sp->sn_cul_hl = syn_check_సమూహం(culhl, (పూర్ణాంకం)STRLEN(culhl));
    }

    ఒకవేళ (numhl != ఖాళీ)
    {
        ఒకవేళ (*numhl == NUL)
            sp->sn_అంకె_hl = 0;
        లేకపోతే
            sp->sn_అంకె_hl = syn_check_సమూహం(numhl, (పూర్ణాంకం)STRLEN(numhl));
    }

    తిరిగి OK;
}

/*
 * Return TRUE if sign "name" exists.
 */
పూర్ణాంకం
sign_exists_by_name(char_u *name)
{
    తిరిగి sign_కనుగొను(name, ఖాళీ) != ఖాళీ;
}

/*
 * Free the sign specified by 'name'.
 */
పూర్ణాంకం
sign_undefine_by_name(char_u *name, పూర్ణాంకం give_error)
{
    sign_T *sp_prev = ఖాళీ;
    sign_T *sp = sign_కనుగొను(name, &sp_prev);
    ఒకవేళ (sp == ఖాళీ)
    {
        ఒకవేళ (give_error)
            semsg(_(e_unknown_sign_str), name);
        తిరిగి FAIL;
    }
    sign_undefine(sp, sp_prev);

    తిరిగి OK;
}

/*
 * List the signs matching 'name'
 */
స్థిర శూన్య
sign_list_by_name(char_u *name)
{
    sign_T *sp = sign_కనుగొను(name, ఖాళీ);
    ఒకవేళ (sp != ఖాళీ)
        sign_list_defined(sp);
    లేకపోతే
        semsg(_(e_unknown_sign_str), name);
}

స్థిర శూన్య
may_force_numberwidth_recompute(buf_T *buf, పూర్ణాంకం unplace)
{
# ఒకవేళ defined(FEAT_LINEBREAK)
    tabpage_T *tp = ఖాళీ;
    win_T *wp = ఖాళీ;

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
        ఒకవేళ (wp->w_బఫరు == buf && (wp->w_p_nu || wp->w_p_rnu) &&
            (unplace || wp->w_nrwidth_వెడల్పు < 2) &&
            (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
            wp->w_nrwidth_పంక్తి_లెక్క = 0;
    }
# endif
}

/*
 * Place a sign at the specified file location or update a sign.
 */
పూర్ణాంకం
sign_ఉంచు(పూర్ణాంకం *sign_id,
           char_u *sign_సమూహం,
           char_u *sign_name,
           buf_T *buf,
           linenr_T lnum,
           పూర్ణాంకం prio)
{
    // Check for reserved character '*' in group name
    ఒకవేళ (sign_సమూహం != ఖాళీ && (*sign_సమూహం == '*' || *sign_సమూహం == '\0'))
        తిరిగి FAIL;

    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
    {
        ఒకవేళ (STRCMP(sp->sn_name, sign_name) == 0)
            ఆపు;
    }

    ఒకవేళ (sp == ఖాళీ)
    {
        semsg(_(e_unknown_sign_str), sign_name);
        తిరిగి FAIL;
    }

    ఒకవేళ (*sign_id == 0)
        *sign_id = sign_సమూహం_get_తదుపరి_signid(buf, sign_సమూహం);

    // Use the default priority value for this sign.
    ఒకవేళ (prio == -1)
        prio = (sp->sn_ప్రాధాన్యత != -1) ? sp->sn_ప్రాధాన్యత : SIGN_DEF_PRIO;

    ఒకవేళ (lnum > 0)
    {
        // ":sign place {id} line={lnum} name={name} file={fname}":
        // place a sign
        buf_addsign(buf, *sign_id, sign_సమూహం, prio, lnum, sp->sn_typenr);
    }
    లేకపోతే
    {
        // ":sign place {id} file={fname}": change sign type and/or priority
        lnum = buf_change_sign_type(buf, *sign_id, sign_సమూహం, sp->sn_typenr,
                                    prio);
    }

    ఒకవేళ (lnum > 0)
    {
        redraw_buf_పంక్తి_later(buf, lnum);

        // When displaying signs in the 'number' column, if the width of the
        // number column is less than 2, then force recomputing the width.
        may_force_numberwidth_recompute(buf, FALSE);
    }
    లేకపోతే
    {
        semsg(_(e_not_possible_గమ్యం_change_sign_str), sign_name);
        తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Unplace the specified sign
 */
స్థిర పూర్ణాంకం
sign_unplace(పూర్ణాంకం sign_id, char_u *sign_సమూహం, buf_T *buf, linenr_T atlnum)
{
    ఒకవేళ (buf->b_signlist == ఖాళీ) // No signs in the buffer
        తిరిగి OK;

    ఒకవేళ (sign_id == 0)
    {
        // Delete all the signs in the specified buffer
        redraw_buf_later(buf, UPD_NOT_VALID);
        buf_delete_signs(buf, sign_సమూహం);
    }
    లేకపోతే
    {
        // Delete only the specified signs
        linenr_T lnum = buf_delsign(buf, atlnum, sign_id, sign_సమూహం);
        ఒకవేళ (lnum == 0)
            తిరిగి FAIL;
    }

    // When all the signs in a buffer are removed, force recomputing the
    // number column width (if enabled) in all the windows displaying the
    // buffer if 'signcolumn' is set to 'number' in that window.
    ఒకవేళ (buf->b_signlist == ఖాళీ)
        may_force_numberwidth_recompute(buf, TRUE);

    తిరిగి OK;
}

/*
 * Unplace the sign at the current cursor line.
 */
స్థిర శూన్య
sign_unplace_at_cursor(char_u *groupname)
{
    పూర్ణాంకం id =
        buf_findsign_id(curwin->w_బఫరు, curwin->w_cursor.lnum, groupname);

    ఒకవేళ (id > 0)
        sign_unplace(id, groupname, curwin->w_బఫరు, curwin->w_cursor.lnum);
    లేకపోతే
        emsg(_(e_missing_sign_సంఖ్య));
}

/*
 * Jump to a sign.
 */
స్థిర linenr_T
sign_jump(పూర్ణాంకం sign_id, char_u *sign_సమూహం, buf_T *buf)
{
    linenr_T lnum = buf_findsign(buf, sign_id, sign_సమూహం);
    ఒకవేళ (lnum <= 0)
    {
        semsg(_(e_invalid_sign_id_nr), sign_id);
        తిరిగి -1;
    }

    // goto a sign ...
    ఒకవేళ (buf_jump_open_win(buf) != ఖాళీ)
    { // ... in a current window
        curwin->w_cursor.lnum = lnum;
        check_cursor_lnum();
        beginline(BL_WHITE);
    }
    లేకపోతే
    { // ... not currently in a window
        ఒకవేళ (buf->b_fname == ఖాళీ)
        {
            emsg(_(e_cannot_jump_గమ్యం_బఫరు_that_does_not_have_name));
            తిరిగి -1;
        }
        setpcmark();
        ఒకవేళ (do_ecmd(buf->b_fnum, ఖాళీ, ఖాళీ, ఖాళీ, lnum,
                    buf_hide(curbuf) ? ECMD_HIDE : 0, curwin) == FAIL)
            తిరిగి -1;
    }
# ifdef FEAT_FOLDING
    foldOpenCursor();
# endif

    తిరిగి lnum;
}

/*
 * ":sign define {name} ..." command
 */
స్థిర శూన్య
sign_define_cmd(char_u *sign_name, char_u *cmdline)
{
    char_u *arg = ఖాళీ;
    char_u *p = cmdline;
    char_u *icon = ఖాళీ;
    char_u *పాఠ్యం = ఖాళీ;
    char_u *linehl = ఖాళీ;
    char_u *texthl = ఖాళీ;
    char_u *culhl = ఖాళీ;
    char_u *numhl = ఖాళీ;
    పూర్ణాంకం prio = -1;
    పూర్ణాంకం failed = FALSE;

    // set values for a defined sign.
    అయితే (TRUE)
    {
        arg = skipwhite(p);
        ఒకవేళ (*arg == NUL)
            ఆపు;

        p = skiptowhite_esc(arg);
        ఒకవేళ (STRNCMP(arg, "icon=", 5) == 0)
        {
            arg += 5;
            icon = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "text=", 5) == 0)
        {
            arg += 5;
            పాఠ్యం = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "linehl=", 7) == 0)
        {
            arg += 7;
            linehl = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "texthl=", 7) == 0)
        {
            arg += 7;
            texthl = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "culhl=", 6) == 0)
        {
            arg += 6;
            culhl = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "numhl=", 6) == 0)
        {
            arg += 6;
            numhl = vim_strnsave(arg, p - arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            prio = atoi((అక్షరం *)arg);
        }
        లేకపోతే
        {
            semsg(_(e_invalid_వాదం_str), arg);
            failed = TRUE;
            ఆపు;
        }
    }

    ఒకవేళ (!failed)
        sign_define_by_name(sign_name, icon, linehl, పాఠ్యం, texthl, culhl, numhl,
                            prio);

    vim_free(icon);
    vim_free(పాఠ్యం);
    vim_free(linehl);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);
}

/*
 * ":sign place" command
 */
స్థిర శూన్య
sign_ఉంచు_cmd(buf_T *buf,
               linenr_T lnum,
               char_u *sign_name,
               పూర్ణాంకం id,
               char_u *సమూహం,
               పూర్ణాంకం prio)
{
    ఒకవేళ (id <= 0)
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
        ఒకవేళ (lnum >= 0 || sign_name != ఖాళీ || (సమూహం != ఖాళీ && *సమూహం == '\0'))
            emsg(_(e_invalid_వాదం));
        లేకపోతే
            sign_list_placed(buf, సమూహం);
    }
    లేకపోతే
    {
        // Place a new sign
        ఒకవేళ (sign_name == ఖాళీ || buf == ఖాళీ ||
            (సమూహం != ఖాళీ && *సమూహం == '\0'))
        {
            emsg(_(e_invalid_వాదం));
            తిరిగి;
        }

        sign_ఉంచు(&id, సమూహం, sign_name, buf, lnum, prio);
    }
}

/*
 * ":sign unplace" command
 */
స్థిర శూన్య
sign_unplace_cmd(buf_T *buf,
                 linenr_T lnum,
                 char_u *sign_name,
                 పూర్ణాంకం id,
                 char_u *సమూహం)
{
    ఒకవేళ (lnum >= 0 || sign_name != ఖాళీ || (సమూహం != ఖాళీ && *సమూహం == '\0'))
    {
        emsg(_(e_invalid_వాదం));
        తిరిగి;
    }

    ఒకవేళ (id == -2)
    {
        ఒకవేళ (buf != ఖాళీ)
        {
            // :sign unplace * file={fname}
            // :sign unplace * group={group} file={fname}
            // :sign unplace * group=* file={fname}
            // :sign unplace * buffer={nr}
            // :sign unplace * group={group} buffer={nr}
            // :sign unplace * group=* buffer={nr}
            sign_unplace(0, సమూహం, buf, 0);
        }
        లేకపోతే
        {
            // :sign unplace *
            // :sign unplace * group={group}
            // :sign unplace * group=*
            FOR_ALL_BUFFERS(buf)
            {
                ఒకవేళ (buf->b_signlist != ఖాళీ)
                    buf_delete_signs(buf, సమూహం);
            }
        }
    }
    లేకపోతే
    {
        ఒకవేళ (buf != ఖాళీ)
        {
            // :sign unplace {id} file={fname}
            // :sign unplace {id} group={group} file={fname}
            // :sign unplace {id} group=* file={fname}
            // :sign unplace {id} buffer={nr}
            // :sign unplace {id} group={group} buffer={nr}
            // :sign unplace {id} group=* buffer={nr}
            sign_unplace(id, సమూహం, buf, 0);
        }
        లేకపోతే
        {
            ఒకవేళ (id == -1)
            {
                // :sign unplace group={group}
                // :sign unplace group=*
                sign_unplace_at_cursor(సమూహం);
            }
            లేకపోతే
            {
                // :sign unplace {id}
                // :sign unplace {id} group={group}
                // :sign unplace {id} group=*
                FOR_ALL_BUFFERS(buf)
                    sign_unplace(id, సమూహం, buf, 0);
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
స్థిర శూన్య
sign_jump_cmd(buf_T *buf,
              linenr_T lnum,
              char_u *sign_name,
              పూర్ణాంకం id,
              char_u *సమూహం)
{
    ఒకవేళ (sign_name == ఖాళీ && సమూహం == ఖాళీ && id == -1)
    {
        emsg(_(e_వాదం_required));
        తిరిగి;
    }

    ఒకవేళ (buf == ఖాళీ || (సమూహం != ఖాళీ && *సమూహం == '\0') || lnum >= 0 ||
        sign_name != ఖాళీ)
    {
        // File or buffer is not specified or an empty group is used
        // or a line number or a sign name is specified.
        emsg(_(e_invalid_వాదం));
        తిరిగి;
    }

    (శూన్య)sign_jump(id, సమూహం, buf);
}

/*
 * Parse the command line arguments for the ":sign place", ":sign unplace" and
 * ":sign jump" commands.
 * The supported arguments are: line={lnum} name={name} group={group}
 * priority={prio} and file={fname} or buffer={nr}.
 */
స్థిర పూర్ణాంకం
parse_sign_cmd_వాదనలు(పూర్ణాంకం cmd,
                    char_u *arg,
                    char_u **sign_name,
                    పూర్ణాంకం *signid,
                    char_u **సమూహం,
                    పూర్ణాంకం *prio,
                    buf_T **buf,
                    linenr_T *lnum)
{
    char_u *arg1 = arg;
    char_u *filename = ఖాళీ;
    పూర్ణాంకం lnum_arg = FALSE;

    // first arg could be placed sign id
    ఒకవేళ (VIM_ISDIGIT(*arg))
    {
        *signid = getdigits(&arg);
        ఒకవేళ (!VIM_ISWHITE(*arg) && *arg != NUL)
        {
            *signid = -1;
            arg = arg1;
        }
        లేకపోతే
        {
            arg = skipwhite(arg);
        }
    }

    అయితే (*arg != NUL)
    {
        ఒకవేళ (STRNCMP(arg, "line=", 5) == 0)
        {
            arg += 5;
            *lnum = atoi((అక్షరం *)arg);
            arg = skiptowhite(arg);
            lnum_arg = TRUE;
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "*", 1) == 0 && cmd == SIGNCMD_UNPLACE)
        {
            ఒకవేళ (*signid != -1)
            {
                emsg(_(e_invalid_వాదం));
                తిరిగి FAIL;
            }
            *signid = -2;
            arg = skiptowhite(arg + 1);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "name=", 5) == 0)
        {
            arg += 5;
            char_u *name = arg;
            arg = skiptowhite(arg);
            ఒకవేళ (*arg != NUL)
                *arg++ = NUL;

            అయితే (name[0] == '0' && name[1] != NUL)
                ++name;

            *sign_name = name;
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "group=", 6) == 0)
        {
            arg += 6;
            *సమూహం = arg;
            arg = skiptowhite(arg);
            ఒకవేళ (*arg != NUL)
                *arg++ = NUL;
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "priority=", 9) == 0)
        {
            arg += 9;
            *prio = atoi((అక్షరం *)arg);
            arg = skiptowhite(arg);
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "file=", 5) == 0)
        {
            arg += 5;
            filename = arg;
            *buf = buflist_findname_exp(arg);
            ఆపు;
        }
        లేకపోతే ఒకవేళ (STRNCMP(arg, "buffer=", 7) == 0)
        {
            arg += 7;
            filename = arg;
            *buf = buflist_findnr((పూర్ణాంకం)getdigits(&arg));

            ఒకవేళ (*skipwhite(arg) != NUL)
                semsg(_(e_trailing_characters_str), arg);

            ఆపు;
        }
        లేకపోతే
        {
            emsg(_(e_invalid_వాదం));
            తిరిగి FAIL;
        }

        arg = skipwhite(arg);
    }

    ఒకవేళ (filename != ఖాళీ && *buf == ఖాళీ)
    {
        semsg(_(e_invalid_బఫరు_name_str), filename);
        తిరిగి FAIL;
    }

    // If the filename is not supplied for the sign place or the sign jump
    // command, then use the current buffer.
    ఒకవేళ (filename == ఖాళీ &&
        ((cmd == SIGNCMD_ఉంచు && lnum_arg) || cmd == SIGNCMD_JUMP))
        *buf = curwin->w_బఫరు;

    తిరిగి OK;
}

/*
 * ":sign" command
 */
శూన్య
ex_sign(exarg_T *eap)
{
    char_u *arg = eap->arg;

    // Parse the subcommand.
    char_u *p = skiptowhite(arg);
    పూర్ణాంకం idx = sign_cmd_idx(arg, p);
    ఒకవేళ (idx == SIGNCMD_చివరి)
    {
        semsg(_(e_unknown_sign_command_str), arg);
        తిరిగి;
    }
    arg = skipwhite(p);

    ఒకవేళ (idx > SIGNCMD_LIST)
    {
        char_u *sign_name = ఖాళీ;
        పూర్ణాంకం id = -1;
        char_u *సమూహం = ఖాళీ;
        పూర్ణాంకం prio = -1;
        buf_T *buf = ఖాళీ;
        linenr_T lnum = -1;

        // Parse command line arguments
        ఒకవేళ (parse_sign_cmd_వాదనలు(idx, arg, &sign_name, &id, &సమూహం, &prio, &buf,
                                &lnum) == FAIL)
            తిరిగి;

        ఒకవేళ (idx == SIGNCMD_ఉంచు)
            sign_ఉంచు_cmd(buf, lnum, sign_name, id, సమూహం, prio);
        లేకపోతే ఒకవేళ (idx == SIGNCMD_UNPLACE)
            sign_unplace_cmd(buf, lnum, sign_name, id, సమూహం);
        లేకపోతే ఒకవేళ (idx == SIGNCMD_JUMP)
            sign_jump_cmd(buf, lnum, sign_name, id, సమూహం);

        తిరిగి;
    }

    // Define, undefine or list signs.
    ఒకవేళ (idx == SIGNCMD_LIST && *arg == NUL)
    {
        // ":sign list": list all defined signs
        కొరకు (sign_T *sp = మొదటి_sign; sp != ఖాళీ && !got_int; sp = sp->sn_తదుపరి)
            sign_list_defined(sp);
    }
    లేకపోతే ఒకవేళ (*arg == NUL)
    {
        emsg(_(e_missing_sign_name));
    }
    లేకపోతే
    {

        // Isolate the sign name.  If it's a number skip leading zeroes,
        // so that "099" and "99" are the same sign.  But keep "0".
        p = skiptowhite(arg);
        ఒకవేళ (*p != NUL)
            *p++ = NUL;

        అయితే (arg[0] == '0' && arg[1] != NUL)
            ++arg;

        char_u *name = vim_strsave(arg);

        ఒకవేళ (idx == SIGNCMD_DEFINE)
            sign_define_cmd(name, p);
        లేకపోతే ఒకవేళ (idx == SIGNCMD_LIST)
            // ":sign list {name}"
            sign_list_by_name(name);
        లేకపోతే
            // ":sign undefine {name}"
            sign_undefine_by_name(name, TRUE);

        vim_free(name);
    }
}

/*
 * Return information about a specified sign
 */
స్థిర శూన్య
sign_getinfo(sign_T *sp, dict_T *retdict)
{
    dict_కూడు_string(retdict, "name", sp->sn_name);

    ఒకవేళ (sp->sn_icon != ఖాళీ)
        dict_కూడు_string(retdict, "icon", sp->sn_icon);

    ఒకవేళ (sp->sn_పాఠ్యం != ఖాళీ)
        dict_కూడు_string(retdict, "text", sp->sn_పాఠ్యం);

    ఒకవేళ (sp->sn_ప్రాధాన్యత > 0)
        dict_కూడు_సంఖ్య(retdict, "priority", sp->sn_ప్రాధాన్యత);

    ఒకవేళ (sp->sn_పంక్తి_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_పంక్తి_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            dict_కూడు_string_len(retdict, "linehl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        లేకపోతే
            dict_కూడు_string(retdict, "linehl", p);
    }

    ఒకవేళ (sp->sn_పాఠ్యం_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_పాఠ్యం_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            dict_కూడు_string_len(retdict, "texthl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        లేకపోతే
            dict_కూడు_string(retdict, "texthl", p);
    }

    ఒకవేళ (sp->sn_cul_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_cul_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            dict_కూడు_string_len(retdict, "culhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        లేకపోతే
            dict_కూడు_string(retdict, "culhl", p);
    }

    ఒకవేళ (sp->sn_అంకె_hl > 0)
    {
        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_అంకె_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            dict_కూడు_string_len(retdict, "numhl",
                (char_u *)"NONE", STRLEN_LITERAL("NONE"));
        లేకపోతే
            dict_కూడు_string(retdict, "numhl", p);
    }
}

/*
 * If 'name' is NULL, return a list of all the defined signs.
 * Otherwise, return information about the specified sign.
 */
స్థిర శూన్య
sign_getlist(char_u *name, list_T *retlist)
{
    sign_T *sp = మొదటి_sign;

    ఒకవేళ (name != ఖాళీ)
    {
        sp = sign_కనుగొను(name, ఖాళీ);
        ఒకవేళ (sp == ఖాళీ)
            తిరిగి;
    }

    కొరకు (; sp != ఖాళీ && !got_int; sp = sp->sn_తదుపరి)
    {
        dict_T *dict = dict_alloc_id(aid_sign_getlist);
        ఒకవేళ (dict == ఖాళీ)
            తిరిగి;

        ఒకవేళ (list_append_dict(retlist, dict) == FAIL)
        {
            dict_unref(dict);
            తిరిగి;
        }

        sign_getinfo(sp, dict);

        ఒకవేళ (name != ఖాళీ) // handle only the specified sign
            ఆపు;
    }
}

/*
 * Returns information about signs placed in a buffer as list of dicts.
 */
శూన్య
get_బఫరు_signs(buf_T *buf, list_T *l)
{
    sign_నమోదు_T *sign = ఖాళీ;
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        dict_T *d = sign_get_info(sign);
        ఒకవేళ (d != ఖాళీ && list_append_dict(l, d) == FAIL)
            dict_unref(d);
    }
}

/*
 * Return information about all the signs placed in a buffer
 */
స్థిర శూన్య
sign_get_placed_లోపల_buf(buf_T *buf,
                       linenr_T lnum,
                       పూర్ణాంకం sign_id,
                       char_u *sign_సమూహం,
                       list_T *retlist)
{
    dict_T *d = dict_alloc_id(aid_sign_getplaced_dict);
    ఒకవేళ (d == ఖాళీ)
        తిరిగి;

    ఒకవేళ (list_append_dict(retlist, d) == FAIL)
    {
        dict_unref(d);
        తిరిగి;
    }

    dict_కూడు_సంఖ్య(d, "bufnr", (దీర్ఘ)buf->b_fnum);

    list_T *l = list_alloc_id(aid_sign_getplaced_list);
    ఒకవేళ (l == ఖాళీ)
        తిరిగి;

    ఒకవేళ (dict_కూడు_list(d, "signs", l) == FAIL)
    {
        list_unref(l);
        తిరిగి;
    }

    sign_నమోదు_T *sign = ఖాళీ;
    FOR_ALL_SIGNS_లోపల_BUF(buf, sign)
    {
        ఒకవేళ (!sign_లోపల_సమూహం(sign, sign_సమూహం))
            కొనసాగు;

        ఒకవేళ ((lnum == 0 && sign_id == 0) ||
            (sign_id == 0 && lnum == sign->se_lnum) ||
            (lnum == 0 && sign_id == sign->se_id) ||
            (lnum == sign->se_lnum && sign_id == sign->se_id))
        {
            dict_T *sdict = sign_get_info(sign);
            ఒకవేళ (sdict != ఖాళీ && list_append_dict(l, sdict) == FAIL)
                dict_unref(sdict);
        }
    }
}

/*
 * Get a list of signs placed in buffer 'buf'. If 'num' is non-zero, return the
 * sign placed at the line number. If 'lnum' is zero, return all the signs
 * placed in 'buf'. If 'buf' is NULL, return signs placed in all the buffers.
 */
స్థిర శూన్య
sign_get_placed(buf_T *buf,
                linenr_T lnum,
                పూర్ణాంకం sign_id,
                char_u *sign_సమూహం,
                list_T *retlist)
{
    ఒకవేళ (buf != ఖాళీ)
    {
        sign_get_placed_లోపల_buf(buf, lnum, sign_id, sign_సమూహం, retlist);
    }
    లేకపోతే
    {
        FOR_ALL_BUFFERS(buf)
        {
            ఒకవేళ (buf->b_signlist != ఖాళీ)
                sign_get_placed_లోపల_buf(buf, 0, sign_id, sign_సమూహం, retlist);
        }
    }
}

# ఒకవేళ defined(FEAT_SIGN_ICONS)
/*
 * Allocate the icons.  Called when the GUI has started.  Allows defining
 * signs before it starts.
 */
శూన్య
sign_gui_started(శూన్య)
{
    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
    {
        ఒకవేళ (sp->sn_icon != ఖాళీ)
            sp->sn_image = gui_mch_రిజిస్టరు_sign(sp->sn_icon);
    }
}
# endif

/*
 * List one sign.
 */
స్థిర శూన్య
sign_list_defined(sign_T *sp)
{
    అక్షరం lbuf[సందేశం_BUF_LEN];

    smsg("sign %s", sp->sn_name);

    ఒకవేళ (sp->sn_icon != ఖాళీ)
    {
        సందేశం_puts(" icon=");
        సందేశం_outtrans(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
        ఒకవేళ (sp->sn_image == ఖాళీ)
            సందేశం_puts(_(" (NOT FOUND)"));
# లేకపోతే
        సందేశం_puts(_(" (not supported)"));
# endif
    }

    ఒకవేళ (sp->sn_పాఠ్యం != ఖాళీ)
    {
        సందేశం_puts(" text=");
        సందేశం_outtrans(sp->sn_పాఠ్యం);
    }

    ఒకవేళ (sp->sn_ప్రాధాన్యత > 0)
    {
        vim_snprintf(lbuf, సందేశం_BUF_LEN, " priority=%d", sp->sn_ప్రాధాన్యత);
        సందేశం_puts(lbuf);
    }

    ఒకవేళ (sp->sn_పంక్తి_hl > 0)
    {
        సందేశం_puts(" linehl=");

        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_పంక్తి_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            సందేశం_puts("NONE");
        లేకపోతే
            సందేశం_puts((అక్షరం *)p);
    }

    ఒకవేళ (sp->sn_పాఠ్యం_hl > 0)
    {
        సందేశం_puts(" texthl=");

        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_పాఠ్యం_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            సందేశం_puts("NONE");
        లేకపోతే
            సందేశం_puts((అక్షరం *)p);
    }

    ఒకవేళ (sp->sn_cul_hl > 0)
    {
        సందేశం_puts(" culhl=");

        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_cul_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            సందేశం_puts("NONE");
        లేకపోతే
            సందేశం_puts((అక్షరం *)p);
    }

    ఒకవేళ (sp->sn_అంకె_hl > 0)
    {
        సందేశం_puts(" numhl=");

        char_u *p = get_highlight_name_ext(ఖాళీ, sp->sn_అంకె_hl - 1, FALSE);
        ఒకవేళ (p == ఖాళీ)
            సందేశం_puts("NONE");
        లేకపోతే
            సందేశం_puts((అక్షరం *)p);
    }
}

/*
 * Undefine a sign and free its memory.
 */
స్థిర శూన్య
sign_undefine(sign_T *sp, sign_T *sp_prev)
{
    vim_free(sp->sn_name);
    vim_free(sp->sn_icon);
# ifdef FEAT_SIGN_ICONS
    ఒకవేళ (sp->sn_image != ఖాళీ)
    {
        బహిర్గతం_flush();
        gui_mch_destroy_sign(sp->sn_image);
    }
# endif
    vim_free(sp->sn_పాఠ్యం);

    ఒకవేళ (sp_prev == ఖాళీ)
        మొదటి_sign = sp->sn_తదుపరి;
    లేకపోతే
        sp_prev->sn_తదుపరి = sp->sn_తదుపరి;

    vim_free(sp);
}

# ఒకవేళ defined(FEAT_SIGN_ICONS)
శూన్య *
sign_get_image(పూర్ణాంకం typenr) // the attribute which may have a sign
{
    sign_T *sp = ఖాళీ;
    FOR_ALL_SIGNS(sp)
    {
        ఒకవేళ (sp->sn_typenr == typenr)
            తిరిగి sp->sn_image;
    }
    తిరిగి ఖాళీ;
}
# endif

/*
 * Undefine/free all signs.
 */
శూన్య
free_signs(శూన్య)
{
    అయితే (మొదటి_sign != ఖాళీ)
        sign_undefine(మొదటి_sign, ఖాళీ);
}

స్థిర గణన
{
    EXP_SUBCMD, // expand :sign sub-commands
    EXP_DEFINE, // expand :sign define {name} args
    EXP_ఉంచు, // expand :sign place {id} args
    EXP_LIST, // expand :sign place args
    EXP_UNPLACE, // expand :sign unplace"
    EXP_SIGN_పేర్లు, // expand with name of placed signs
    EXP_SIGN_GROUPS // expand with name of placed sign groups
} expand_what;

/*
 * Return the n'th sign name (used for command line completion)
 */
స్థిర char_u *
get_nth_sign_name(పూర్ణాంకం idx)
{
    పూర్ణాంకం current_idx = 0;
    sign_T *sp = ఖాళీ;

    // Complete with name of signs already defined
    FOR_ALL_SIGNS(sp)
    {
        ఒకవేళ (current_idx++ == idx)
            తిరిగి sp->sn_name;
    }
    తిరిగి ఖాళీ;
}

/*
 * Return the n'th sign group name (used for command line completion)
 */
స్థిర char_u *
get_nth_sign_సమూహం_name(పూర్ణాంకం idx)
{
    పూర్ణాంకం current_idx = 0;
    పూర్ణాంకం todo = (పూర్ణాంకం)sg_పట్టిక.ht_వాడిన;
    hashitem_T *hi = ఖాళీ;

    // Complete with name of sign groups already defined
    FOR_ALL_HASHTAB_వస్తువులు(&sg_పట్టిక, hi, todo)
    {
        ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
        {
            --todo;
            ఒకవేళ (current_idx++ == idx)
            {
                signgroup_T *సమూహం = HI2SG(hi);
                తిరిగి సమూహం->sg_name;
            }
        }
    }
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the sign command
 * expansion.
 */
char_u *
get_sign_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఎంపిక (expand_what)
    {
        స్థితి EXP_SUBCMD:
            తిరిగి (char_u *)cmds[idx];
        స్థితి EXP_DEFINE:
        {
            అక్షరం *define_arg[] = { "culhl=", "icon=",   "linehl=",   "numhl=",
                                   "text=",  "texthl=", "priority=", ఖాళీ };
            తిరిగి (char_u *)define_arg[idx];
        }
        స్థితి EXP_ఉంచు:
        {
            అక్షరం *ఉంచు_arg[] = { "line=", "name=",   "group=", "priority=",
                                  "file=", "buffer=", ఖాళీ };
            తిరిగి (char_u *)ఉంచు_arg[idx];
        }
        స్థితి EXP_LIST:
        {
            అక్షరం *list_arg[] = { "group=", "file=", "buffer=", ఖాళీ };
            తిరిగి (char_u *)list_arg[idx];
        }
        స్థితి EXP_UNPLACE:
        {
            అక్షరం *unplace_arg[] = { "group=", "file=", "buffer=", ఖాళీ };
            తిరిగి (char_u *)unplace_arg[idx];
        }
        స్థితి EXP_SIGN_పేర్లు:
            తిరిగి get_nth_sign_name(idx);
        స్థితి EXP_SIGN_GROUPS:
            తిరిగి get_nth_sign_సమూహం_name(idx);
        అప్రమేయ:
            తిరిగి ఖాళీ;
    }
}

/*
 * Handle command line completion for :sign command.
 */
శూన్య
సమితి_context_లోపల_sign_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    char_u *చివర_subcmd;
    char_u *చివరి;
    పూర్ణాంకం cmd_idx;
    char_u *begin_subcmd_వాదనలు;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_SIGN;
    expand_what = EXP_SUBCMD;
    xp->xp_నమూనా = arg;

    చివర_subcmd = skiptowhite(arg);
    // expand subcmd name
    // :sign {subcmd}<CTRL-D>
    ఒకవేళ (*చివర_subcmd == NUL)
        తిరిగి;

    cmd_idx = sign_cmd_idx(arg, చివర_subcmd);

    // :sign {subcmd} {subcmd_args}
    //                |
    //                begin_subcmd_args
    begin_subcmd_వాదనలు = skipwhite(చివర_subcmd);

    // expand last argument of subcmd

    // :sign define {name} {args}...
    //              |
    //              p

    // Loop until reaching last argument.
    p = begin_subcmd_వాదనలు;
    చేయి
    {
        p = skipwhite(p);
        చివరి = p;
        p = skiptowhite(p);
    }
    అయితే (*p != NUL);

    p = vim_strchr(చివరి, '=');

    // :sign define {name} {args}... {last}=
    //                               |     |
    //                            last     p
    ఒకవేళ (p == ఖాళీ)
    {
        // Expand last argument name (before equal sign).
        xp->xp_నమూనా = చివరి;
        ఎంపిక (cmd_idx)
        {
            స్థితి SIGNCMD_DEFINE:
                expand_what = EXP_DEFINE;
                ఆపు;
            స్థితి SIGNCMD_ఉంచు:
                // List placed signs
                ఒకవేళ (VIM_ISDIGIT(*begin_subcmd_వాదనలు))
                    //   :sign place {id} {args}...
                    expand_what = EXP_ఉంచు;
                లేకపోతే
                    //   :sign place {args}...
                    expand_what = EXP_LIST;
                ఆపు;
            స్థితి SIGNCMD_LIST:
            స్థితి SIGNCMD_UNDEFINE:
                // :sign list <CTRL-D>
                // :sign undefine <CTRL-D>
                expand_what = EXP_SIGN_పేర్లు;
                ఆపు;
            స్థితి SIGNCMD_JUMP:
            స్థితి SIGNCMD_UNPLACE:
                expand_what = EXP_UNPLACE;
                ఆపు;
            అప్రమేయ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
    లేకపోతే
    {
        // Expand last argument value (after equal sign).
        xp->xp_నమూనా = p + 1;
        ఎంపిక (cmd_idx)
        {
            స్థితి SIGNCMD_DEFINE:
                ఒకవేళ (STRNCMP(చివరి, "texthl", 6) == 0 ||
                    STRNCMP(చివరి, "linehl", 6) == 0 ||
                    STRNCMP(చివరి, "culhl", 5) == 0 ||
                    STRNCMP(చివరి, "numhl", 5) == 0)
                    xp->xp_context = EXPAND_HIGHLIGHT;
                లేకపోతే ఒకవేళ (STRNCMP(చివరి, "icon", 4) == 0)
                    xp->xp_context = EXPAND_FILES;
                లేకపోతే
                    xp->xp_context = EXPAND_NOTHING;
                ఆపు;
            స్థితి SIGNCMD_ఉంచు:
                ఒకవేళ (STRNCMP(చివరి, "name", 4) == 0)
                    expand_what = EXP_SIGN_పేర్లు;
                లేకపోతే ఒకవేళ (STRNCMP(చివరి, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                లేకపోతే ఒకవేళ (STRNCMP(చివరి, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                లేకపోతే
                    xp->xp_context = EXPAND_NOTHING;
                ఆపు;
            స్థితి SIGNCMD_UNPLACE:
            స్థితి SIGNCMD_JUMP:
                ఒకవేళ (STRNCMP(చివరి, "group", 5) == 0)
                    expand_what = EXP_SIGN_GROUPS;
                లేకపోతే ఒకవేళ (STRNCMP(చివరి, "file", 4) == 0)
                    xp->xp_context = EXPAND_BUFFERS;
                లేకపోతే
                    xp->xp_context = EXPAND_NOTHING;
                ఆపు;
            అప్రమేయ:
                xp->xp_context = EXPAND_NOTHING;
        }
    }
}

/*
 * Define a sign using the attributes in 'dict'. Returns 0 on success and -1 on
 * failure.
 */
స్థిర పూర్ణాంకం
sign_define_from_dict(char_u *name_arg, dict_T *dict)
{
    char_u *name = ఖాళీ;
    char_u *icon = ఖాళీ;
    char_u *linehl = ఖాళీ;
    char_u *పాఠ్యం = ఖాళీ;
    char_u *texthl = ఖాళీ;
    char_u *culhl = ఖాళీ;
    char_u *numhl = ఖాళీ;
    పూర్ణాంకం prio = -1;
    పూర్ణాంకం retval = -1;

    ఒకవేళ (name_arg == ఖాళీ && dict == ఖాళీ)
        తిరిగి retval;

    ఒకవేళ (name_arg == ఖాళీ)
        name = dict_get_string(dict, "name", TRUE);
    లేకపోతే
        name = vim_strsave(name_arg);

    ఒకవేళ (name == ఖాళీ || name[0] == NUL)
        వెళ్ళు cleanup;

    ఒకవేళ (dict != ఖాళీ)
    {
        icon = dict_get_string(dict, "icon", TRUE);
        linehl = dict_get_string(dict, "linehl", TRUE);
        పాఠ్యం = dict_get_string(dict, "text", TRUE);
        texthl = dict_get_string(dict, "texthl", TRUE);
        culhl = dict_get_string(dict, "culhl", TRUE);
        numhl = dict_get_string(dict, "numhl", TRUE);
        prio = dict_get_సంఖ్య_def(dict, "priority", -1);
    }

    ఒకవేళ (sign_define_by_name(name, icon, linehl, పాఠ్యం, texthl, culhl, numhl,
                            prio) == OK)
        retval = 0;

cleanup:
    vim_free(name);
    vim_free(icon);
    vim_free(linehl);
    vim_free(పాఠ్యం);
    vim_free(texthl);
    vim_free(culhl);
    vim_free(numhl);

    తిరిగి retval;
}

/*
 * Define multiple signs using attributes from list 'l' and store the return
 * values in 'retlist'.
 */
స్థిర శూన్య
sign_define_గుణిజం(list_T *l, list_T *retlist)
{
    listitem_T *li = ఖాళీ;
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
        పూర్ణాంకం retval = -1;

        ఒకవేళ (li->li_tv.v_type == VAR_DICT)
            retval = sign_define_from_dict(ఖాళీ, li->li_tv.vval.v_dict);
        లేకపోతే
            emsg(_(e_dictionary_required));

        list_append_సంఖ్య(retlist, retval);
    }
}

/*
 * "sign_define()" function
 */
శూన్య
f_sign_define(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && (check_for_string_or_list_arg(argvars, 0) == FAIL ||
                            check_for_opt_dict_arg(argvars, 1) == FAIL))
        తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Define multiple signs
        ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
            తిరిగి;

        sign_define_గుణిజం(argvars[0].vval.v_list, rettv->vval.v_list);
        తిరిగి;
    }

    // Define a single sign
    rettv->vval.v_సంఖ్య = -1;

    char_u *name = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (name == ఖాళీ)
        తిరిగి;

    ఒకవేళ (check_for_opt_dict_arg(argvars, 1) == FAIL)
        తిరిగి;

    rettv->vval.v_సంఖ్య = sign_define_from_dict(
        name, argvars[1].v_type == VAR_DICT ? argvars[1].vval.v_dict : ఖాళీ);
}

/*
 * "sign_getdefined()" function
 */
శూన్య
f_sign_getdefined(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc_id(rettv, aid_sign_getdefined) == FAIL)
        తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
        తిరిగి;

    char_u *name = ఖాళీ;
    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
        name = tv_get_string(&argvars[0]);

    sign_getlist(name, rettv->vval.v_list);
}

/*
 * "sign_getplaced()" function
 */
శూన్య
f_sign_getplaced(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ఖాళీ;
    linenr_T lnum = 0;
    పూర్ణాంకం sign_id = 0;
    char_u *సమూహం = ఖాళీ;

    ఒకవేళ (rettv_list_alloc_id(rettv, aid_sign_getplaced) == FAIL)
        తిరిగి;

    ఒకవేళ (లోపల_vim9script() && (check_for_opt_బఫరు_arg(argvars, 0) == FAIL ||
                            (argvars[0].v_type != VAR_UNKNOWN &&
                             check_for_opt_dict_arg(argvars, 1) == FAIL)))
        తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
        // get signs placed in the specified buffer
        buf = get_buf_arg(&argvars[0]);
        ఒకవేళ (buf == ఖాళీ)
            తిరిగి;

        ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
        {
            ఒకవేళ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
                తిరిగి;

            dictitem_T *di = ఖాళీ;
            dict_T *dict = argvars[1].vval.v_dict;

            ఒకవేళ ((di = dict_కనుగొను(dict, (char_u *)"lnum", -1)) != ఖాళీ)
            {
                // get signs placed at this line
                పూర్ణాంకం notanum = FALSE;
                tv_get_సంఖ్య_chk(&di->di_tv, &notanum);
                ఒకవేళ (notanum)
                    తిరిగి;

                lnum = tv_get_lnum(&di->di_tv);
            }

            ఒకవేళ ((di = dict_కనుగొను(dict, (char_u *)"id", -1)) != ఖాళీ)
            {
                // get sign placed with this identifier
                పూర్ణాంకం notanum = FALSE;
                sign_id = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&di->di_tv, &notanum);
                ఒకవేళ (notanum)
                    తిరిగి;
            }

            ఒకవేళ ((di = dict_కనుగొను(dict, (char_u *)"group", -1)) != ఖాళీ)
            {
                సమూహం = tv_get_string_chk(&di->di_tv);
                ఒకవేళ (సమూహం == ఖాళీ)
                    తిరిగి;

                ఒకవేళ (*సమూహం == '\0') // empty string means global group
                    సమూహం = ఖాళీ;
            }
        }
    }

    sign_get_placed(buf, lnum, sign_id, సమూహం, rettv->vval.v_list);
}

/*
 * "sign_jump()" function
 */
శూన్య
f_sign_jump(typval_T *argvars, typval_T *rettv)
{
    char_u *sign_సమూహం = ఖాళీ;
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script() && (check_for_సంఖ్య_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_బఫరు_arg(argvars, 2) == FAIL))
        తిరిగి;

    పూర్ణాంకం notanum = FALSE;
    // Sign identifier
    పూర్ణాంకం sign_id = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], &notanum);
    ఒకవేళ (notanum)
        తిరిగి;

    ఒకవేళ (sign_id <= 0)
    {
        emsg(_(e_invalid_వాదం));
        తిరిగి;
    }

    // Sign group
    sign_సమూహం = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (sign_సమూహం == ఖాళీ)
        తిరిగి;

    ఒకవేళ (sign_సమూహం[0] == '\0')
    {
        sign_సమూహం = ఖాళీ; // global sign group
    }
    లేకపోతే
    {
        sign_సమూహం = vim_strsave(sign_సమూహం);
        ఒకవేళ (sign_సమూహం == ఖాళీ)
            తిరిగి;
    }

    // Buffer to place the sign
    buf_T *buf = get_buf_arg(&argvars[2]);
    ఒకవేళ (buf == ఖాళీ)
        వెళ్ళు cleanup;

    rettv->vval.v_సంఖ్య = sign_jump(sign_id, sign_సమూహం, buf);

cleanup:
    vim_free(sign_సమూహం);
}

/*
 * Place a new sign using the values specified in dict 'dict'. Returns the sign
 * identifier if successfully placed, otherwise returns 0.
 */
స్థిర పూర్ణాంకం
sign_ఉంచు_from_dict(typval_T *id_tv,
                     typval_T *సమూహం_tv,
                     typval_T *name_tv,
                     typval_T *buf_tv,
                     dict_T *dict)
{
    పూర్ణాంకం sign_id = 0;
    char_u *సమూహం = ఖాళీ;
    char_u *sign_name = ఖాళీ;
    buf_T *buf = ఖాళీ;
    dictitem_T *di = ఖాళీ;
    linenr_T lnum = 0;
    పూర్ణాంకం prio = -1;
    పూర్ణాంకం ret_sign_id = -1;

    // sign identifier
    ఒకవేళ (id_tv == ఖాళీ)
    {
        di = dict_కనుగొను(dict, (char_u *)"id", -1);
        ఒకవేళ (di != ఖాళీ)
            id_tv = &di->di_tv;
    }

    ఒకవేళ (id_tv == ఖాళీ)
    {
        sign_id = 0;
    }
    లేకపోతే
    {
        పూర్ణాంకం notanum = FALSE;
        sign_id = tv_get_సంఖ్య_chk(id_tv, &notanum);

        ఒకవేళ (notanum)
            తిరిగి -1;

        ఒకవేళ (sign_id < 0)
        {
            emsg(_(e_invalid_వాదం));
            తిరిగి -1;
        }
    }

    // sign group
    ఒకవేళ (సమూహం_tv == ఖాళీ)
    {
        di = dict_కనుగొను(dict, (char_u *)"group", -1);
        ఒకవేళ (di != ఖాళీ)
            సమూహం_tv = &di->di_tv;
    }

    ఒకవేళ (సమూహం_tv == ఖాళీ)
    {
        సమూహం = ఖాళీ; // global group
    }
    లేకపోతే
    {
        సమూహం = tv_get_string_chk(సమూహం_tv);
        ఒకవేళ (సమూహం == ఖాళీ)
            వెళ్ళు cleanup;

        ఒకవేళ (సమూహం[0] == '\0') // global sign group
        {
            సమూహం = ఖాళీ;
        }
        లేకపోతే
        {
            సమూహం = vim_strsave(సమూహం);
            ఒకవేళ (సమూహం == ఖాళీ)
                తిరిగి -1;
        }
    }

    // sign name
    ఒకవేళ (name_tv == ఖాళీ)
    {
        di = dict_కనుగొను(dict, (char_u *)"name", -1);
        ఒకవేళ (di != ఖాళీ)
            name_tv = &di->di_tv;
    }

    ఒకవేళ (name_tv == ఖాళీ)
        వెళ్ళు cleanup;

    sign_name = tv_get_string_chk(name_tv);
    ఒకవేళ (sign_name == ఖాళీ)
        వెళ్ళు cleanup;

    // buffer to place the sign
    ఒకవేళ (buf_tv == ఖాళీ)
    {
        di = dict_కనుగొను(dict, (char_u *)"buffer", -1);
        ఒకవేళ (di != ఖాళీ)
            buf_tv = &di->di_tv;
    }

    ఒకవేళ (buf_tv == ఖాళీ)
        వెళ్ళు cleanup;

    buf = get_buf_arg(buf_tv);
    ఒకవేళ (buf == ఖాళీ)
        వెళ్ళు cleanup;

    // line number of the sign
    di = dict_కనుగొను(dict, (char_u *)"lnum", -1);
    ఒకవేళ (di != ఖాళీ)
    {
        lnum = tv_get_lnum(&di->di_tv);
        ఒకవేళ (lnum <= 0)
        {
            emsg(_(e_invalid_వాదం));
            వెళ్ళు cleanup;
        }
    }

    // sign priority
    di = dict_కనుగొను(dict, (char_u *)"priority", -1);
    ఒకవేళ (di != ఖాళీ)
    {
        పూర్ణాంకం notanum = FALSE;
        prio = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&di->di_tv, &notanum);
        ఒకవేళ (notanum)
            వెళ్ళు cleanup;
    }

    ఒకవేళ (sign_ఉంచు(&sign_id, సమూహం, sign_name, buf, lnum, prio) == OK)
        ret_sign_id = sign_id;

cleanup:
    vim_free(సమూహం);

    తిరిగి ret_sign_id;
}

/*
 * "sign_place()" function
 */
శూన్య
f_sign_ఉంచు(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ఖాళీ;
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script() && (check_for_సంఖ్య_arg(argvars, 0) == FAIL ||
                            check_for_string_arg(argvars, 1) == FAIL ||
                            check_for_string_arg(argvars, 2) == FAIL ||
                            check_for_బఫరు_arg(argvars, 3) == FAIL ||
                            check_for_opt_dict_arg(argvars, 4) == FAIL))
        తిరిగి;

    ఒకవేళ (argvars[4].v_type != VAR_UNKNOWN)
    {
        ఒకవేళ (check_for_nonnull_dict_arg(argvars, 4) == FAIL)
            తిరిగి;
        dict = argvars[4].vval.v_dict;
    }

    rettv->vval.v_సంఖ్య = sign_ఉంచు_from_dict(&argvars[0], &argvars[1],
                                                &argvars[2], &argvars[3], dict);
}

/*
 * "sign_placelist()" function.  Place multiple signs.
 */
శూన్య
f_sign_placelist(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
        తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
        తిరిగి;

    // Process the List of sign attributes
    listitem_T *li = ఖాళీ;
    FOR_ALL_LIST_వస్తువులు(argvars[0].vval.v_list, li)
    {
        పూర్ణాంకం sign_id = -1;

        ఒకవేళ (li->li_tv.v_type == VAR_DICT)
            sign_id = sign_ఉంచు_from_dict(ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ,
                                           li->li_tv.vval.v_dict);
        లేకపోతే
            emsg(_(e_dictionary_required));

        list_append_సంఖ్య(rettv->vval.v_list, sign_id);
    }
}

/*
 * Undefine multiple signs
 */
స్థిర శూన్య
sign_undefine_గుణిజం(list_T *l, list_T *retlist)
{
    listitem_T *li = ఖాళీ;
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
        పూర్ణాంకం retval = -1;
        char_u *name = tv_get_string_chk(&li->li_tv);
        ఒకవేళ (name != ఖాళీ && (sign_undefine_by_name(name, TRUE) == OK))
            retval = 0;

        list_append_సంఖ్య(retlist, retval);
    }
}

/*
 * "sign_undefine()" function
 */
శూన్య
f_sign_undefine(typval_T *argvars, typval_T *rettv)
{

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_or_list_arg(argvars, 0) == FAIL)
        తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_UNKNOWN)
    {
        // Undefine multiple signs
        ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
            తిరిగి;

        sign_undefine_గుణిజం(argvars[0].vval.v_list, rettv->vval.v_list);
        తిరిగి;
    }

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
        // Free all the signs
        free_signs();
        rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే
    {
        // Free only the specified sign
        char_u *name = tv_get_string_chk(&argvars[0]);
        ఒకవేళ (name == ఖాళీ)
            తిరిగి;

        ఒకవేళ (sign_undefine_by_name(name, TRUE) == OK)
            rettv->vval.v_సంఖ్య = 0;
    }
}

/*
 * Unplace the sign with attributes specified in 'dict'. Returns 0 on success
 * and -1 on failure.
 */
స్థిర పూర్ణాంకం
sign_unplace_from_dict(typval_T *సమూహం_tv, dict_T *dict)
{
    పూర్ణాంకం sign_id = 0;
    buf_T *buf = ఖాళీ;
    char_u *సమూహం = ఖాళీ;
    పూర్ణాంకం retval = -1;

    // sign group
    సమూహం = (సమూహం_tv != ఖాళీ) ? tv_get_string(సమూహం_tv)
                               : dict_get_string(dict, "group", FALSE);

    ఒకవేళ (సమూహం != ఖాళీ)
    {
        ఒకవేళ (సమూహం[0] == '\0') // global sign group
        {
            సమూహం = ఖాళీ;
        }
        లేకపోతే
        {
            సమూహం = vim_strsave(సమూహం);
            ఒకవేళ (సమూహం == ఖాళీ)
                తిరిగి retval;
        }
    }

    ఒకవేళ (dict != ఖాళీ)
    {
        dictitem_T *di = dict_కనుగొను(dict, (char_u *)"buffer", -1);
        ఒకవేళ (di != ఖాళీ)
        {
            buf = get_buf_arg(&di->di_tv);
            ఒకవేళ (buf == ఖాళీ)
                వెళ్ళు cleanup;
        }

        ఒకవేళ (dict_has_కీలకం(dict, "id"))
        {
            sign_id = dict_get_సంఖ్య(dict, "id");
            ఒకవేళ (sign_id <= 0)
            {
                emsg(_(e_invalid_వాదం));
                వెళ్ళు cleanup;
            }
        }
    }

    ఒకవేళ (buf == ఖాళీ)
    {
        // Delete the sign in all the buffers
        retval = 0;
        FOR_ALL_BUFFERS(buf)
            ఒకవేళ (sign_unplace(sign_id, సమూహం, buf, 0) != OK)
                retval = -1;
    }
    లేకపోతే ఒకవేళ (sign_unplace(sign_id, సమూహం, buf, 0) == OK)
        retval = 0;

cleanup:
    vim_free(సమూహం);

    తిరిగి retval;
}

sign_నమోదు_T *
get_మొదటి_valid_sign(win_T *wp)
{
    sign_నమోదు_T *sign = wp->w_బఫరు->b_signlist;

# ifdef FEAT_PROP_POPUP
    అయితే (sign != ఖాళీ && !sign_సమూహం_for_కిటికీ(sign, wp))
        sign = sign->se_తదుపరి;
# endif
    తిరిగి sign;
}

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
పూర్ణాంకం
signcolumn_on(win_T *wp)
{
    // If 'signcolumn' is set to 'number', signs are displayed in the 'number'
    // column (if present). Otherwise signs are to be displayed in the sign
    // column.
    ఒకవేళ (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u')
        తిరిగి get_మొదటి_valid_sign(wp) != ఖాళీ && !wp->w_p_nu && !wp->w_p_rnu;

    ఒకవేళ (*wp->w_p_scl == 'n')
        తిరిగి FALSE;

    ఒకవేళ (*wp->w_p_scl == 'y')
        తిరిగి TRUE;

    తిరిగి (get_మొదటి_valid_sign(wp) != ఖాళీ
# ifdef FEAT_NETBEANS_INTG
            || wp->w_బఫరు->b_has_sign_స్తంభం
# endif
    );
}

/*
 * "sign_unplace()" function
 */
శూన్య
f_sign_unplace(typval_T *argvars, typval_T *rettv)
{
    dict_T *dict = ఖాళీ;
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ ((check_for_string_arg(argvars, 0) == FAIL ||
         check_for_opt_dict_arg(argvars, 1) == FAIL))
        తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
        dict = argvars[1].vval.v_dict;

    rettv->vval.v_సంఖ్య = sign_unplace_from_dict(&argvars[0], dict);
}

/*
 * "sign_unplacelist()" function
 */
శూన్య
f_sign_unplacelist(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
        తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
        తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
        తిరిగి;

    listitem_T *li = ఖాళీ;
    FOR_ALL_LIST_వస్తువులు(argvars[0].vval.v_list, li)
    {
        పూర్ణాంకం retval = -1;

        ఒకవేళ (li->li_tv.v_type == VAR_DICT)
            retval = sign_unplace_from_dict(ఖాళీ, li->li_tv.vval.v_dict);
        లేకపోతే
            emsg(_(e_dictionary_required));

        list_append_సంఖ్య(rettv->vval.v_list, retval);
    }
}

#అయితే_ముగింపు // FEAT_SIGNS
