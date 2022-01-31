//MARK CURRENTLY DISABLED
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"
#include "mark.h"
#include "lookup.h"

bool run_mark_editor( DESCRIPTOR_DATA *d )
{
    if (d->editor == ED_MARK)
    {
        mark_edit(d->character, d->incomm);
        return TRUE;
    }
    return FALSE;
}

const struct mark_cmd mark_cmd_table [] = {
    {"show", mark_show_edit},
    //{"type", mark_set_type},
    {"target", mark_set_target},
    {"price", mark_set_price},
    {"extra", mark_set_extra},
    {"status", mark_set_status},
    {"desc", mark_set_descr},
    {"accept", mark_accept},
    {"help", mark_help},
    {"?", mark_help},
    {"", 0}
};

const struct mark_tbl_type mark_type_table [] = {
    {"None", MARK_TYPE_NONE},
    {"Player Death", MARK_TYPE_PLAYER_DEATH},
    {"Player Loot", MARK_TYPE_PLAYER_LOOTS},
    {"Spec Mob Eq", MARK_TYPE_SPEC_MOB_EQ},
    {"Mob Eq", MARK_TYPE_MOB_EQ},
    {"Guild", MARK_TYPE_GUILD},
    {"", -2}   
};

const struct mark_status mark_status_table [] = {
    {"Writing",MARK_STAT_WRITE},
    {"Negotiation",MARK_STAT_NEGOT},
    {"Accepted",MARK_STAT_ACCEPT },
    {"Rejected",MARK_STAT_REJECT},
    {"Completed",MARK_STAT_COMPLET},
    {"Paided",MARK_STAT_PAID},
    {"",-2}
};

bool IS_FURIES (CHAR_DATA *ch)
{
    if (IS_NPC(ch))
        return FALSE;
    if (ch->clan == clan_lookup("Furies"))
        return TRUE;
    else
        return FALSE;
}

bool IS_CONTRACTOR (CHAR_DATA *ch, MARK_DATA * mark)
{
    if (IS_NPC(ch))
        return FALSE;
    if (!str_cmp(mark->contractor,ch->name))
        return TRUE;
    else
        return FALSE;
}

bool check_player( char * name )
{
    char buf[100];

    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( access( buf, F_OK ) == 0 ) 
        return TRUE;

    return FALSE;
}
char * str_mark_type (int type)
{
    int stype;
    
    for (stype = -1; mark_type_table[stype].type!= -2;stype++)
        if (stype == type)
            return mark_type_table[stype].name;
    
    return "Undefined";
}

char * str_mark_status (int status)
{
    int stat;
    
    for (stat = -1; mark_status_table[stat].status!= -2;stat++)
        if (stat == status)
            return mark_status_table[stat].name;
    
    return "Undefined";
}


void save_marks( void )
{
    MARK_DATA * mark;
    FILE * fp;

    if (( fp = fopen("../data/mark.save", "w"))==NULL )
    {
    	bug("Save Mark: fopen",0);
	return;
    }
    else
    {
    	fprintf(fp, "#MARKS\n" );
        for ( mark = mark_list; mark!=NULL; mark = mark->next )
        {
            fwrite_mark (mark,fp);
        }
    }
    fprintf( fp, "#End\n" );
    fclose( fp );
    return;
}
void fwrite_mark (MARK_DATA *mark, FILE * fp)
{
    fprintf( fp, "CONTRACT\n" );
    fprintf( fp, "Cont %s~\n", mark->contractor);
    fprintf( fp, "Type %d~\n", mark->type);	    
    fprintf( fp, "Targ %s~\n", mark->target);
    fprintf( fp, "Desc %s~\n", mark->desc);
    fprintf( fp, "Stat %d~\n", mark->status);
    fprintf( fp, "Pric %d~\n", mark->price);
    fprintf( fp, "Extr %d~\n", mark->extra);  
    fprintf( fp, "End\n" );

}


void load_marks( void )
{
    MARK_DATA * mark;
    FILE * fp;
    char * word;
    bool fmatch;
    

    
    if ( ( fp = fopen( "../data/mark.save", "r" ) ) == NULL )
    {
    	bug("Load_Mark: fopen", 0);
	return;
    }
    word = fread_word(fp);
    
    if (!str_cmp(word,"#END"))
        return;
        
    for ( ; ; )
    {
    	if ( feof(fp) )
        {
            fclose(fp);
            return;
        }
        mark = new_mark();
	word   = fread_word( fp );
        fmatch = FALSE;
	switch ( UPPER( word[0] ) )
	{
	    case '#':
	        if ( !str_cmp (word, "#MARKS" ))
	        {
	            fmatch = TRUE;	            
	            break;
	        }
	        
	        if ( !str_cmp (word ,"#CONTRACT"))
	        {
	            fmatch = TRUE;
	            mark = new_mark();
	            mark->next = mark_list;
	            mark_list = mark;
	            fread_mark (mark,fp);
	            break;
	        }
	        if (!str_cmp("#END", word))
                {
                    fmatch = TRUE;
                    return;
                }           
		break;

    	}
        if (!fmatch)
        {
    	    bug ("MARK : Bad Match",0);
        }
    	
    }
    free_string (word);
    fclose( fp );
    return;
}
void fread_mark (MARK_DATA *mark,FILE * fp)
{
    fread_word(fp);
    mark->contractor = fread_string(fp);
    fread_word(fp);
    mark->type = fread_number(fp);
    fread_word(fp);
    mark->target = fread_string(fp);
    fread_word(fp);
    mark->desc = fread_string(fp);
    fread_word(fp);
    mark->status = fread_number(fp);
    fread_word(fp);
    mark->price = fread_number(fp);
    fread_word(fp);
    mark->extra = fread_number(fp);
}

void do_mark (CHAR_DATA *ch, char * argument)
{
    MARK_DATA *mark;
    char arg1 [MIL];
    char arg2 [MIL];
    //char buf [MSL];
    int tmpint;
    int count = 0;
    bool found = FALSE;
    
    if (IS_NPC(ch))
        return;
    
    argument = one_argument (argument,arg1);
    argument = one_argument (argument,arg2);
    if (!str_cmp(arg1, "load")&&IS_IMMORTAL(ch))
    {
    	load_marks();
    	return;
    }

    if (!str_cmp(arg1, "list"))
    {
    	mark_do_list(ch);
    	return;
    }
    if (!str_cmp (arg1, "create"))
    {
    	mark = new_mark();
    	VALIDATE(mark);
    	mark->contractor = ch->name;
    	mark->next = mark_list;
    	mark_list = mark;
    	ch->desc->editor = ED_MARK;
    	ch->desc->pEdit = mark;
    	//mark_edit(ch,mark,NULL);
    	return;
    }
    if (!str_cmp (arg1, "edit"))
    {
    	if (!is_number(arg2))
    	{
    	   send_to_char("Syntax: mark edit <number>\n\r",ch);
    	   return;
    	}
    	tmpint = atoi(arg2);
    	for (mark = mark_list;mark != NULL;mark = mark->next)
    	{
    	    if (!IS_FURIES(ch) && !IS_CONTRACTOR(ch,mark))
    	        continue;
    	        
    	    if (tmpint == count)
    	    {
    	    	found = TRUE;
    	        break;
    	    }
    	    count++;
    	}
    	if (!found)
    	{
    	    send_to_char("Contract not found.\n\r",ch);
    	    return;
    	}
    	if (!IS_CONTRACTOR(ch,mark) && !IS_FURIES(ch))
    	{
    	    send_to_char("You aren't the contractor of that mark.\n\r",ch);
    	    return;
    	}
    	    	
    	ch->desc->editor = ED_MARK;
    	ch->desc->pEdit = mark;
    	//mark_edit(ch,mark,NULL);
    	return;
    }
    if (!str_cmp (arg1, "read"))
    {
    	if (!is_number(arg2))
    	{
    	   send_to_char("Syntax: mark read <number>\n\r",ch);
    	   return;
    	}
    	tmpint = atoi(arg2);
    	for (mark = mark_list;mark != NULL;mark = mark->next)
    	{
    	    if (!IS_FURIES(ch) && !IS_CONTRACTOR(ch,mark))
    	        continue;
    	    
    	    if (tmpint == count)
    	    {
    	    	found = TRUE;
    	        break;
    	    }
    	    count++;
    	}
    	if (!found)
    	{
    	    send_to_char("Contract not found.\n\r",ch);
    	    return;
    	}    	
    	mark_show(ch,mark);
    	return;
    }

}
void mark_show (CHAR_DATA *ch,MARK_DATA * mark)
{
    char buf [MSL];
    sprintf (buf, "Contractor : %13s  Type: %10s  Target: %13s\n",
        mark->contractor,str_mark_type(mark->type),mark->target);
    send_to_char (buf,ch);
    sprintf (buf, "Price: %12d           STATUS: %s\n", mark->price,str_mark_status(mark->status));
    send_to_char (buf,ch);
    sprintf (buf, "Extra: %12d\n", mark->extra);
    send_to_char (buf,ch);
    sprintf (buf, "       %18d\n Total\n\n",mark->price+mark->extra);
    send_to_char (buf,ch);
    sprintf (buf, "Contract Description :\n%s\n\r",mark->desc);
    send_to_char (buf,ch);
    return;
    
    
}
void mark_show_edit (CHAR_DATA *ch, char * argument)
{
    char buf [MSL];
    MARK_DATA *mark;
    
    EDIT_MARK(ch, mark);
    
    
    sprintf (buf, "Contractor : %13s  Type: %10s  Target: %13s\n",
        mark->contractor,str_mark_type(mark->type),mark->target);
    send_to_char (buf,ch);
    sprintf (buf, "Price: %12d           STATUS: %s\n", mark->price,str_mark_status(mark->status));
    send_to_char (buf,ch);
    sprintf (buf, "Extra: %12d\n", mark->extra);
    send_to_char (buf,ch);
    sprintf (buf, "       %18d\n Total\n\n",mark->price+mark->extra);
    send_to_char (buf,ch);
    sprintf (buf, "Contract Description :\n%s\n\r",mark->desc);
    send_to_char (buf,ch);
    return;
    
    
}

void mark_do_list (CHAR_DATA *ch)
{
    MARK_DATA *mark;
    char buf [MSL];
    int count = 0;
    
    for (mark = mark_list; mark != NULL; mark = mark->next)
    {
    	if (!IS_CONTRACTOR(ch,mark) && !IS_FURIES(ch))
    	    continue;
    	sprintf (buf,"[ %3d ] %13s  Contract Type: %10s  Target: %13s\n",
    	    count,mark->contractor,str_mark_type(mark->type),mark->target);
    	send_to_char (buf,ch);
    	count++;    	
    }

}

void mark_edit(CHAR_DATA *ch, char *argument)
{
    MARK_DATA *mark;
    
    EDIT_MARK(ch, mark);
    
    if (argument[0] == '\0' || argument [1] == '\0')
    {
    	mark_show_edit(ch,"");
    	return;
    }
    if (!str_cmp(argument, "done"))
    {
    	if (mark->type == -1)
    	{
    	    send_to_char ("Type not set.\n\r",ch);
    	    return;
    	}
    	if (IS_FURIES(ch) && mark->price == 0)
    	{
    	    send_to_char ("Price not set.\n\r",ch);
    	    return;
    	
    	}
    	if (!str_cmp(mark->desc,"None"))
    	{
    	    send_to_char ("Description not set.\n\r",ch);
    	    return;
    	
    	}
    	
    	if (!IS_FURIES(ch))
    	{
            send_to_char ("Sending your offer to the Furies.\n\r",ch);
            mark->status = MARK_STAT_NEGOT;
        }
        else
            send_to_char ("Sending your offer back to Contractor.\n\r",ch);
    	
    	ch->desc->editor = ED_NONE;
    	save_marks();
    	return;    	
    }
    
    mark_interpret (ch, argument);
    return;
}

void mark_interpret (CHAR_DATA *ch, char * argument)
{
   char arg [MIL];
   int cmd;
   bool found = FALSE;
   
   argument = one_argument (argument, arg);
   
   for (cmd = 0; mark_cmd_table[cmd].name[0] != '\0';cmd++)
   {
   	if (!str_prefix(arg, mark_cmd_table[cmd].name))
   	{
   	    found = TRUE;
   	    break;
   	}
   }

   if (found)
       (mark_cmd_table[cmd].mark_fun) (ch, argument);
   else
       send_to_char("Command not found\n\r",ch);
   return;
}

void mark_set_type (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    char arg [MIL];
    sh_int find;
    
    argument = one_argument (argument, arg);
    
    EDIT_MARK(ch, mark);
    if (mark->type != -1
    && mark->status >= MARK_STAT_ACCEPT
    && !IS_FURIES(ch))
    {
    	send_to_char ("Contract type has already been accepted.\nTalk to Furies if you wish for this to change.\n\r",ch);
    	return;
    }

    if (arg[0] != '\0')
    for (find = -1;mark_type_table[find].name[0] != '\0';find++)
        if (!str_cmp (argument, mark_type_table[find].name))
            mark->type = mark_type_table[find].type;
    
    
        
}
void mark_set_target (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    char arg [MIL];
    
    argument = one_argument (argument, arg);
    
    EDIT_MARK(ch, mark);
    if (arg[0] == '\0')
    {
    	send_to_char("Syntax: target <name>",ch);
    	return;
    }
    
    if (str_cmp(mark->target, "None") 
    && mark->status >= MARK_STAT_ACCEPT
    && !IS_FURIES(ch))
    {
    	send_to_char ("Target has already been set.\nTalk to Furies if you wish this to change.\n\r",ch);
    	return;
    }


    if (check_player(arg))
    {
    	mark->target = capitalize(arg);
    }
    else
    {
    	send_to_char("Did not find that player",ch);
    	return;
    }
}
void mark_set_price (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    char arg [MIL];
    int price;
    
    argument = one_argument (argument, arg);
    
    EDIT_MARK(ch, mark);
    
    if (!IS_FURIES(ch))
    {
    	send_to_char("This is a Furies only command.\n\r",ch);
    	return;
    }
    
    if (mark->price != 0
    && mark->status >= MARK_STAT_ACCEPT)
    {
    	send_to_char("Contract price as already been accepted and finalized.\n\r",ch);
    	return;
    }
    
    
    price = atoi(arg);
    if (price < 0 || price > 10000)
    {
    	send_to_char ("Base price is out of range. ( 0 - 10000)",ch);
    	return;
    }
    mark->price = price;
}
void mark_set_status (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    char arg [MIL];
    sh_int find;
    
    argument = one_argument (argument, arg);
    
    EDIT_MARK(ch, mark);
    if (IS_FURIES(ch))
    {
    	send_to_char("This is a furies only command.\n\r",ch);
    	return;
    }

    
    if (arg[0] != '\0')
    for (find = -1;mark_status_table[find].name[0] != '\0';find++)
        if (!str_cmp (argument, mark_status_table[find].name))
            mark->status = mark_status_table[find].status;
}
void mark_set_extra (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    char arg [MIL];
    int price;
    
    argument = one_argument (argument, arg);
    
    EDIT_MARK(ch, mark);
    if (IS_FURIES(ch))
    {
    	send_to_char("This is a furies only command.\n\r",ch);
    	return;
    }
    if (mark->extra != 0 
    && mark->status >= MARK_STAT_ACCEPT)
    {
    	send_to_char("Contract extra costs have already been accepted.",ch);
    	return;
    }

    price = atoi(arg);
    if (price < 0 || price > 10000)
    {
    	send_to_char ("Extra price is out of range. ( 0 - 10000)",ch);
    	return;
    }
    mark->price = price;
}
void mark_set_descr (CHAR_DATA * ch, char * argument)
{
    MARK_DATA *mark;
    
    EDIT_MARK(ch, mark);
    if (mark->status >= MARK_STAT_ACCEPT)
    {
    	send_to_char ("Contract details already have been accepted.\n\r",ch);
    	return;
    }

    if (argument[0] == '\0')    
        string_append( ch, &mark->desc );
  
    send_to_char("Syntax: desc\n\r", ch);
}

void mark_help (CHAR_DATA * ch, char* argument)
{
    send_to_char("God help me indeed\n\r",ch);
    return;
}

void mark_accept (CHAR_DATA *ch, char * argument)
{
    MARK_DATA * mark;
    
    EDIT_MARK(ch,mark);
    if (!IS_CONTRACTOR(ch,mark))
    {
    	send_to_char("Only a contractor can accept a Furies offer.\n\r",ch);
    	return;
    }
    if (mark->status != MARK_STAT_NEGOT && mark->price != 0)
    {
    	send_to_char ("Contract is no longer in negotiation phase.\n\r",ch);
        return;
    }
    mark->status = MARK_STAT_ACCEPT;
    return;
}





