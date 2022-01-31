#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

bool IS_DISGUISED (CHAR_DATA * ch)
{
   bool disguise = FALSE;
   if (IS_AFFECTED (ch, AFF_DISGUISE))
       disguise = TRUE;
   return disguise;
}

void REM_DISGUISE (CHAR_DATA * ch)
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;
    if (IS_NPC(ch))
        return;
    
    if (!IS_DISGUISED (ch))
        return;
    
    
    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
    	paf_next = paf->next;
    	if (paf->bitvector != AFF_DISGUISE)
    	    continue;
    	else
    	{
            STR_REMOVE_BIT(ch->affected_by, paf->bitvector); 
            affect_remove (ch, paf);  
    	}
    	
    }
    ch->name = ch->pcdata->disguise.orig_name;
    ch->pcdata->title = ch->pcdata->disguise.orig_title;
    ch->description = ch->pcdata->disguise.orig_desc;
    ch->short_descr = ch->pcdata->disguise.orig_short;
    ch->long_descr = ch->pcdata->disguise.orig_long;
    ch->sex = ch->pcdata->disguise.orig_sex;
    act ("$n removes $s disguise.\n\r",ch,NULL,NULL,TO_ROOM);
    send_to_char ("You remove your disguise.\n\r",ch);
    return;
}

void do_disguiseself (CHAR_DATA * ch, char * argument)
{
    CHAR_DATA * victim;
    AFFECT_DATA af;
    
    char tmpname[MSL];
    int skill;
    
    if (IS_NPC(ch))
        return;
    
    skill = get_skill (ch,gsn_cloak);
    
    if (skill < 1)
    {
    	send_to_char ("You don't know where to begin to disguise yourself properly.\n\r",ch);
    	return;
    }
    
    if (argument[0] == '\0')
    {
    	send_to_char ("Disguise yourself like who?\n\r",ch);
    	return;
    }
    
    if ((victim = get_char_room (ch, NULL,argument)) == NULL)
    {
    	send_to_char ("That person isn't here.\n\r",ch);
    	return;
    }
    if (!IS_NPC(victim)
    || IS_SET(victim->act,ACT_TRAIN)
    || IS_SET(victim->act,ACT_PRACTICE)
    || IS_SET(victim->act,ACT_IS_HEALER)
    || (victim->spec_fun == spec_lookup( "spec_questmaster" )))
    {
     
    	send_to_char ("That individual is too distinct to disguise yourself into.\n\r",ch);
    	return;
    }

    if (IS_DISGUISED(ch))
        REM_DISGUISE(ch);
    
    WAIT_STATE (ch, skill_table[gsn_cloak].beats);
    if (number_percent()<skill)
    {
    	//Backup original character data
        ch->pcdata->disguise.orig_name = ch->name;
        ch->pcdata->disguise.orig_title = ch->pcdata->title;
        ch->pcdata->disguise.orig_desc = ch->description;
        ch->pcdata->disguise.orig_short = ch->short_descr;
        ch->pcdata->disguise.orig_long = ch->long_descr;
        ch->pcdata->disguise.orig_sex = ch->sex;
        
        //Set new disguise char
        
        sprintf (tmpname, "%s ",ch->name);
        strcat (tmpname,victim->name);
        
        ch->name = str_dup(tmpname);
        ch->description = str_dup(victim->description);
        ch->pcdata->title = ("");
        ch->short_descr = str_dup(victim->short_descr);
        ch->long_descr = str_dup(victim->long_descr);
        ch->sex = victim->sex;
        af.where = TO_AFFECTS;
        af.type = gsn_cloak;
        af.level = ch->level;
        af.duration = number_fuzzy (ch->level / 6);
        af.modifier = 0;
        af.bitvector = AFF_DISGUISE;
        af.location = APPLY_NONE;
        affect_to_char (ch, &af);
        act ("$n disguises $mself.\n\r",ch,NULL,NULL,TO_ROOM);
        send_to_char ("You disguise yourself!\n\r",ch);
        return;
    
    }
    act ("$n tries disguise $mself but fails.\n\r",ch,NULL,NULL,TO_ROOM);
    send_to_char ("You fail to achieve your disguise.\n\r",ch);
    return;
}
