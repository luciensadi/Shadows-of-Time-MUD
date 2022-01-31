/*
 * This slave does iptoname conversions, and identquery lookups.
 * 
 * The philosophy is to keep this program as simple/small as possible.
 * It does normal fork()s, so the smaller it is, the faster it goes.
 */

/* Taken from the Crystal Shard codebase, derived from Sillymud 1.1b.
 * Ported for Smaug compatibility by Samson, Ident query sections removed
 * as Smaug comes with its own ident routine. This should prevent the
 * dreaded lag associated with reverse DNS lookups.
 *
 * This is a standalone program. To compile changes, use 'make dns' in your src dir.
 * It will be booted up by the main mud process during startup.
 */

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SVR4
#include <sys/time.h>
#endif				/* SVR4 */

#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "dns_slave.h"

#ifdef sun
#define RETSIGTYPE void
#endif

#define MAX_STRING 4096

char *arg_for_errors;

char *format_inet_addr(char *buf, struct in_addr addr)
{
  char *tmp;

  tmp = inet_ntoa( addr );
  strcpy( buf, tmp );
  return( buf + strlen(buf) );
}


#ifndef HAS_STPCPY
char *stpcpy(char *dest, char *src)
{
  strcpy( dest, src );
  return(dest + strlen(dest) );
}
#endif				/* HAS_STPCPY */

int my_write(int s, char *buf, size_t len)
{
  return( 1 ? write( s, buf, len ) : send( s, buf, len, 0 ) );
}

int my_read(int s, char *buf, size_t len)
{
  return( 1 ? read( s, buf, len ) : recv( s, buf, len, 0 ) );
}

#ifdef SVR4
RETSIGTYPE
slave_signal()
#else
void slave_signal()
#endif				/* SVR4 */
{
  if( getppid() == 1 )
  {
    if( !arg_for_errors )
      fprintf( stderr, "[Slave] Parenting died, exiting.\n" );
    else
    {
	fprintf( stderr, "[Slave] '%s' parent died, exiting.\n", arg_for_errors );
    }
    exit(1);
  }
  signal(SIGALRM, slave_signal);
}

#ifdef SVR4
RETSIGTYPE
timeout_signal()
#else
void timeout_signal()
#endif				/* SVR4 */
{
  fprintf( stderr, "'%s' 5 minute timeout\n", arg_for_errors );
  exit(1);
}

/* This is for requests that fail for reasons OTHER than */
/* failing to talk to the main server     */
int req_failed( int type, char *addr, char *port_pair )
{
  char buf[MAX_STRING];
  int len = 0;

  switch (type)
  {
    case SLAVE_IPTONAME_FAIL:
      len = sprintf( buf, "%c%s\n", (char)type, addr );
      break;
  }

  my_write( 1, buf, len );
  return (0);
}

int iptoname( char *arg )
{
  struct hostent *hp;
  unsigned long addr;
  char *p, buf[MAX_STRING];

  addr = inet_addr( arg );
  if( addr == -1 )
  {
    fprintf( stderr, "[Slave] %s is not a valid decimal IP address\n", arg );
    req_failed( SLAVE_IPTONAME_FAIL, arg, 0 );
    return (-1);
  }
  hp = gethostbyaddr( (char *)&addr, sizeof (addr), AF_INET );
  if( hp )
  {
    buf[0] = SLAVE_IPTONAME;
    p = stpcpy( buf + 1, arg );
    *p++ = ' ';
    p = stpcpy( p, hp->h_name );
    *p++ = '\n';
    my_write( 1, buf, p - buf );
  }
  else
    req_failed( SLAVE_IPTONAME_FAIL, arg, 0 );

  return (0);
}

#ifdef SVR4
RETSIGTYPE
child_signal()
#else
void child_signal()
#endif				/* SVR4 */
{
  /* collect any children */
  while( waitpid(0, NULL, WNOHANG) > 0 );
  signal( SIGCHLD, child_signal );
}

int main(int argc, char **argv)
{
  struct itimerval itime;
  struct timeval interval;
  char *p, arg[MAX_STRING + 1];
  int len, timeout;

  arg_for_errors = 0;
  fprintf( stderr, "[Slave] DNS Slave process booted\n" );
  signal( SIGCHLD, child_signal );
  signal( SIGPIPE, SIG_DFL );
  if( argc > 1 )
    timeout = atoi( argv[1] );
  else
    timeout = 120;

  /* Let's limit those timeouts */
  if( timeout < 0 )
    timeout = 10;

  if( timeout > 300 )
    timeout = 300;

  /* Wake up every 30 seconds and see if we have a parent */
  interval.tv_sec = 30;
  interval.tv_usec = 0;
  itime.it_interval = interval;
  itime.it_value = interval;
  setitimer( ITIMER_REAL, &itime, 0 );
  signal( SIGALRM, slave_signal );

  for (;;)
  {
    len = my_read(0, arg, MAX_STRING);
    if (!len)
      break;

    if (len < 0)
    {
      if (errno == EINTR)
      {
	   errno = 0;
	   continue;
      }
	fprintf( stderr, "[Slave] Read: %s\n", strerror(errno) );
      break;
    }
    arg[len] = 0;
    p = strchr(arg, '\n');
    if (p)
      *p = 0;

    fprintf( stderr, "[Slave] Received: '%s'\n", arg );
    switch( fork() )
    {
      case -1:
	   fprintf( stderr, "[Slave] '%s': fork: %s\n", arg, strerror(errno) );
	   exit(1);

      case 0:			/* child */
	   interval.tv_sec = timeout;	/* X minutes */
	   interval.tv_usec = 0;
	   itime.it_interval = interval;
	   itime.it_value = interval;
	   setitimer( ITIMER_VIRTUAL, &itime, 0 );
	   signal( SIGVTALRM, timeout_signal );

	   switch( arg[0] )
	   {
	      case SLAVE_IPTONAME:
	        exit( iptoname(arg + 1) != 0 );

	      default:
		  fprintf( stderr, "[Slave] Invalid arg: %s\n", arg );
	   }
    }

    /* collect any children */
    while( waitpid(0, NULL, WNOHANG) > 0 );
  }

  fprintf( stderr, "[Slave] Exiting\n" );
  exit(0);
}
