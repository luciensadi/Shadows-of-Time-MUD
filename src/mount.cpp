/** Mount code by Asmodeus **/
#include <stdio.h>
#include <sys/types.h>
#include "merc.h"

void affect_strip_bv args ((CHAR_DATA *ch, int sn));

void do_mount(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *mount;

  if (get_skill(ch, gsn_horseback) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NPC(ch))
  {
    send_to_char("You don't have thumbs.\n\r", ch);
    return;
  }

  if ((mount = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("You don't see that here.\n\r", ch);
    return;
  }

  if (!IS_NPC(mount))
  {
    act("You try to mount $N unsuccesfully.", ch, NULL, mount, TO_CHAR);
    act("$n just tried to mount you.", ch, NULL, mount, TO_VICT);
    act("$n just tried to mount $N.", ch, NULL, mount, TO_ROOM);
    return;
  }

  if (!IS_SET(mount->act, ACT_MOUNT))
  {
    if (ch->pcdata->condition[COND_DRUNK] < 10 )
    {
      send_to_char("You're not drunk enough to try that.\n\r", ch);
      return;
    }
    else 
    {
      act("You try to mount $N unsuccesfully.", ch, NULL, mount, TO_CHAR);
      act("$n just tried to mount you.", ch, NULL, mount, TO_VICT);
      act("$n just tried to mount $N.", ch, NULL, mount, TO_ROOM);
      return;
    }
  }

  if (ch->is_mounted)
  {
     send_to_char("You are already mounted.\n\r", ch);
     return;
  }    

  if (mount->is_mounted)
  {
     send_to_char("That beast already has a passenger.\n\r", ch);
     return;
  }    

  if (mount->mount != ch)
  {
    send_to_char("That's not your mount.\n\r", ch);
    return;
  }

  if (mount->mount == ch)
  {
    act("You hop up on top of $N", ch, NULL, mount, TO_CHAR);
    act("$n hops up on top of $N", ch, NULL, mount, TO_CHAR);
    ch->is_mounted = TRUE;
    ch->mount = mount;
    mount->is_mounted = TRUE;
    mount->leader = ch;
  }
}

void do_spur(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *mount;
  AFFECT_DATA af;
  int sn;

  if (!IS_MOUNTED(ch))
  {
    send_to_char("Spur what, yourself?\n\r", ch);
    return;
  }

  if ((mount = ch->mount) == NULL)
  {
    send_to_char("Mount bug, report.\n\r", ch);
    return;
  } 

  sn = skill_lookup("Galloping");

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = ch->level;
  af.duration = ch->level / 5;
  af.location = APPLY_NONE;
  af.bitvector = AFF_GALLOPING;
  affect_to_char(mount, &af);
  send_to_char("You spur the horse into a rage.\n\r", ch);
 
}    

void do_reinin(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *mount;
  int sn;

  if (!IS_MOUNTED(ch))
  {
    send_to_char("Rein what, yourself?\n\r", ch);
    return;
  }

  if ((mount = ch->mount) == NULL)
  {
    send_to_char("Mount bug, report.\n\r", ch);
    return;
  } 

  if (!IS_AFFECTED(mount, AFF_GALLOPING))
  {
    send_to_char("Your mount is already at a walk.\n\r", ch);
    return;
  }

  sn = skill_lookup("Galloping");

  affect_strip(mount, sn);
  send_to_char("You rein in the mount.\n\r", ch);
      
}    

void do_tame(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *mount;
   
   if ((mount = get_char_room (ch, NULL, argument)) == NULL)
   {
     send_to_char("You don't see that mount here.\n\r", ch);
     return;
   }

   if (!IS_NPC(mount))
   {
     send_to_char("They are way too wild to be tamed.\n\r", ch);
     return;
   }

   if (!IS_SET(mount->act, ACT_MOUNT))
   {
     send_to_char("That wouldn't make a good mount.\n\r", ch);
     return;
   }
 
   if (number_percent() > get_skill(ch, gsn_tame) / 1.3)
   {
     act("You fail to tame $M and $S attacks.\n\r", ch, NULL, mount, TO_CHAR);
     act("$n tries to tame $N, but fails.\n\r", ch, NULL, mount, TO_ROOM); 
     check_improve(ch, gsn_tame, FALSE, 1); 
     multi_hit(mount, ch, TYPE_UNDEFINED);
     return;
   }

   if (mount->mount)
   {
     send_to_char("That mount has already been tamed.\n\r", ch);
     return;
   }

   mount->mount = ch;
   act("You tame $N and make $M your own.\n\r", ch, NULL, mount, TO_CHAR);
   act("$n tames $N", ch, NULL, mount, TO_ROOM);
   check_improve(ch, gsn_tame, TRUE, 3); 
}


void do_dismount(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch))
    return;

  if (!ch->is_mounted)
  {
    send_to_char("Dismount from what, you're on your feet silly.\n\r", ch);
    return;
  }

  if (ch->mount)
  {
    act("$n dismounts from $N", ch, NULL, ch->mount, TO_ROOM);
    act("You dismount from $N", ch, NULL, ch->mount, TO_CHAR);
    ch->is_mounted = FALSE;
    ch->mount->is_mounted = FALSE;
    ch->mount->leader = NULL;
    return;
  } 
  
  ch->is_mounted = FALSE;
  send_to_char("You dismount.\n\r", ch);
}

void throw_mount(CHAR_DATA *mount)
{
  CHAR_DATA *ch;

  if (!mount->mount)
    return;

  ch = mount->mount;

  ch->mount = NULL;
  ch->is_mounted = FALSE;

  if (number_percent() > get_skill(ch, gsn_horseback))
  {
    act("$N throws you from $M back and falls on top of you crushing you under $M weight.", ch, NULL, mount, TO_CHAR);
    act("$N throws $n from it's back and lands on $m.", ch, NULL, mount, TO_ROOM);
    damage(mount, ch, mount->size * 70, TYPE_UNDEFINED, DAM_BASH, TRUE, FALSE);
    WAIT_STATE(ch, PULSE_VIOLENCE * 3);
  }
  else
  {  
    act("$N throws you from $M back, but you fall safely clear.", ch, NULL, mount, TO_CHAR);
    act("$N throws $n from it's back.", ch, NULL, mount, TO_ROOM);
    WAIT_STATE(ch, PULSE_VIOLENCE);
  }
}
