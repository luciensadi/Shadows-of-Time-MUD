/* 
There's been some talk about this recently, so I finally got round to at
least partially cleaning up what I'd done with this.

This is code based on that made public by Erwin Andreasen to give an
ispell command for online spellchecking.  This version differs in that it
only uses output functions found in base Rom, but more importantly has an
ispell_string function which can be used to spellcheck room descs and
such.  The ispell.c file follows, and after it is info on how to add
supprt to the string editor found in the ILAB OLC.  I ported the 2.3 OLC
to 2.4 myself, so I don't know how applicable this part is to OLC's such
as Ivan's..suspect it works fine.. YMMV as always.

Erwin's code gives a do_ispell command, which means you need to add a line
in the command table for it.  Additionally, the calls to the functions
that initialise and shutdown the ispell engine need added.  ispell_init
should be before the call to boot_db in comm.c, and ispell_done is needed
before a copyover is done if you have that coded added.  Add appropriate
function prototypes to make things happy here. 

The function ispell_string passes each word of a string to the ispell
engine (avoiding duplicates though) and gathers the output which is then
paged to the char.  I turned this into a new dot command, insprationally
called '.i'.  A prototype for the ispell_string function also needs added,
either in merc.h or string.c.
*/

#define Stringify(x) Str(x)
#define Str(x) #x

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include "merc.h"
#include "recycle.h"
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

static FILE *ispell_out;
static pid_t ispell_pid = -1;
static int to[2], from[2];

#define ISPELL_BUF_SIZE 1024


void ispell_init()
{
    char ignore_buf[1024];

    pipe(to);
    pipe(from);

    ispell_pid = fork();
    if (ispell_pid < 0)
    {
	char errmsg_buf[1024];
        char *errmsg = errmsg_buf;
        sprintf(errmsg, "ispell_init: fork: %s", strerror(errno));
        bug(errmsg, 0);
    }
    else if (ispell_pid == 0)   /* child */
    {
        int i;
        dup2(to[0], 0);         /* this is where we read commands from - make
                                   it stdin */
        close(to[0]);
        close(to[1]);

        dup2(from[1], 1);       /* this is where we write stuff to */
        close(from[0]);
        close(from[1]);

        /* Close all the other files */
        for (i = 2; i < 255; i++)
            close(i);

        execlp("ispell", "ispell", "-a", (char *)
               NULL);
        exit(1);
    }
    else
    {                           /* ok ! */
        close(to[0]);
        close(from[1]);
        ispell_out = fdopen(to[1], "w");
        setbuf(ispell_out, NULL);
#if !defined( sun )             /* that ispell on sun gives no (c) msg */
        read(from[0], ignore_buf, 1024);
#endif
    }
}

void do_ispell_done(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  sprintf(buf, "PID %d\n\r", ispell_pid);
  send_to_char(buf, ch); 
  ispell_done();
  sprintf(buf, "PID %d\n\r", ispell_pid);
  send_to_char(buf, ch); 
}

void do_ispell_start(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  sprintf(buf, "PID %d\n\r", ispell_pid);
  send_to_char(buf, ch); 
  ispell_done();
  ispell_init();
  sprintf(buf, "PID %d\n\r", ispell_pid);
  send_to_char(buf, ch); 
}
  
void ispell_done()
{
    if (ispell_pid != -1)
    {
        fprintf(ispell_out, "#\n");
        fclose(ispell_out);
        close(from[0]);
        waitpid(ispell_pid, NULL, 0);
        logf("Ispell Done.", 0);
        ispell_pid = -1;
    }
}

char *get_ispell_line(char *word)
{
    static char buf[ISPELL_BUF_SIZE];
    char buf2[MSL];
    int len;

    if (ispell_pid == -1)
    {
      logf("No pid", 0);
        return NULL;
    }

    if (word)
    {
          fprintf(ispell_out, "^%s\n\r", word);
          fflush(ispell_out);
    }

    len = read(from[0], buf2, ISPELL_BUF_SIZE);
    logf("Ispell len %d", len);
    buf2[len] = '\0';

    /* Read up to max 1024 characters here */
    if (sscanf(buf2, "%" Stringify(ISPELL_BUF_SIZE) "[^\n]\n\n", buf) != 1)
    {
      logf("Ispell: Scanf %s", buf2);
        return NULL;
    }

    return buf;
}

void do_ispell(CHAR_DATA * ch, char *argument)
{
    char *pc;
    char buf3[MSL];

    if (ispell_pid <= 0)
    {
        send_to_char("ispell is not running.\n\r", ch);
        return;
    }

//    sprintf(buf, "PID %d\n\r", ispell_pid);
//    send_to_char(buf, ch);
    if (!argument[0] || strchr(argument, ' '))
    {
        send_to_char("Invalid input.\n\r", ch);
        return;
    }

    if (argument[0] == '+' && IS_IMMORTAL(ch))
    {
        for (pc = argument + 1; *pc; pc++)
            if (!isalpha(*pc))
            {
                sprintf(buf3, "'%c' is not a letter.\n\r",
                        *pc);
                send_to_char(buf3, ch);
                return;
            }
        fprintf(ispell_out, "*%s\n", argument + 1);
        fflush(ispell_out);
        return;                 /* we assume everything is OK.. better be so! */
    }

    pc = get_ispell_line(argument);
    if (!pc)
    {
        send_to_char("ispell: failed.\n\r", ch);
//        ispell_done();
//	ispell_init();
        return;
    }

    switch (pc[0])
    {
        case '*':
        case '+':               /* root */
        case '-':               /* compound */
            send_to_char("Correct.\n\r", ch);
            break;

        case '&':               /* miss */
            sprintf(buf3, "Not found. Possible words: %s\n\r", strchr(pc, ':') + 1);
            send_to_char(buf3, ch);
            break;

        case '?':               /* guess */
            sprintf(buf3, "Not found. Possible words: %s\n\r",
                    strchr(pc, ':') + 1);
            send_to_char(buf3, ch);
            break;

        case '#':               /* none */
            send_to_char("Unable to find anything that matches.\n\r", ch);
            break;

        default:
            sprintf(buf3, "Weird output from ispell: %s\n\r",
                    pc);
            send_to_char(buf3, ch);
    }
}

/*
* Function to add ispell support within an editor
* It takes the string the char is currently editing as it's source,
* and passes it a word at a time to ispell.  A check is kept on
* what words have been checked to prevent repetition of the same
* 'error', mainly useful with proper names etc.
*/
void ispell_string(CHAR_DATA * ch)
{
    char *result;
    char word[MSL];
    char checked_list[MSL];
    char buf[MSL];
    char *str;
    int i = 0;
    bool errors = FALSE;
    BUFFER *buffer;

    buffer = new_buf();
    strcpy(checked_list, "");

    str = *ch->desc->pString;

    while (*str != '\0')
    {
        while (*str != ' ' && *str != '\0' && *str != '\n' && *str != '\r')
        {
            word[i++] = *str;

            str++;
        }

        word[i++] = '\0';

        if (!is_exact_name(word, checked_list))
        {
            sprintf(checked_list + strlen(checked_list), "%s ", word);

            result = get_ispell_line(word);

            if (!IS_NULLSTR(result))
            {
                if (result[0] == '&')
                {
                    sprintf(buf, "%s failed - Possible words : %s\n\r",
                            word, strchr(result, ':'));
                    errors = TRUE;
                    add_buf(buffer, buf);
                }

                if (result[0] == '#')
                {
                    sprintf(buf, "%s failed - no similar words found.\n\r", word);
                    errors = TRUE;
                    add_buf(buffer, buf);
                }
            }
        }
        str++;
        i = 0;
    }
    send_to_char("\n\r", ch);
    if (errors)
        page_to_char(buf_string(buffer), ch);
    else
        send_to_char("No errors found.\n\r", ch);

    free_buf(buffer);

    return;
}



          

