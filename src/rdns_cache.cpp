/*
Copyright (c) 2000
     The staff of Fatal Dimensions.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
     This product includes software developed by the staff of Fatal
     Dimensions and its contributors. For more information, see
     http://fataldimensions.nl.eu.org
4. Neither the name of Fatal Dimensions nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY FATAL DIMENSIONS AND CONTRIBUTORS ``AS
IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
STAFF OF FATAL DIMENSIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Version 3.3 (this is the 4rd release)
// Joost Bekkers
// $Id: rdns_cache.c,v 1.12 2001/05/01 06:51:08 joost Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <string.h>
// If you have problems with compiling and are running Linux, you might want
// to add this line, it worked for Debian Linux for me.
// #define __USE_UNIX98
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rdns_cache.h"

//#define debug(s) printf(s)
#define debug(s) /* empty */

// If your platform doesn't support pthread_rwlock_* calls correctly for
//  some reason or other uncomment the next line.
#define NORWLOCKS

// If your platform doesn't support asprintf() uncomment the next line
#define NOASPRINTF

#ifdef NORWLOCKS
#define pthread_rwlock_t pthread_mutex_t
#define pthread_rwlock_init pthread_mutex_init
#define pthread_rwlock_rdlock pthread_mutex_lock
#define pthread_rwlock_wrlock pthread_mutex_lock
#define pthread_rwlock_unlock pthread_mutex_unlock
#define pthread_rwlock_destroy pthread_mutex_destroy
#endif

//seems this isn't defined on all platforms
#ifndef PTHREAD_CANCEL_ASYNCHRONOUS
#define NOPTHREAD_CANCEL
#endif

typedef struct rdns_list {
   struct rdns_list     *next;
   
   unsigned char        *address;
   int                  address_len;
   int			address_format; // AF_INET or AF_INET6
   
   time_t               ttd;
   int                  valid;
   
   char                 *host_name;
} rdns_list_t;

// message to return while looking up th eip address.
char             *rdns_lookup_message = "looking up";
char		 rdns_address_string[40]; // reserve space to fit a INET6 ip-address

// item stay in the cache for 5 minutes.
int		 rdns_default_ttl = 300;

rdns_list_t      *rdns_hostlist;
rdns_list_t      **rdns_listptr=&rdns_hostlist;
pthread_rwlock_t rdns_head_lock, rdns_body_lock;
pthread_t        rdns_thread_info;
int              rdns_terminate=-1;

pthread_cond_t   rdns_cond;
pthread_mutex_t  rdns_mutex;

// ==============================================

char * add_to_list(const char *addr, int len, int af) {
  rdns_list_t *newitem;
  char *buf;
  if ((newitem=(rdns_list_t *)malloc(sizeof(rdns_list_t)))==NULL) return NULL;
  if ((newitem->address=(unsigned char *) malloc(len))==NULL) return NULL;
  memcpy(newitem->address,addr,len);
  newitem->address_len=len;
  newitem->valid=0;
  newitem->address_format=af;
  buf = (char *) malloc (40);
#ifndef NOASPRINTF
  asprintf(&newitem->host_name,"%s (%s)",inet_ntop(newitem->address_format,
						   newitem->address,
						   buf,40),
                                         rdns_lookup_message);
#else
  inet_ntop(newitem->address_format,
	    newitem->address,
	    buf,40);
  newitem->host_name=(char *)malloc(strlen(buf)+
				    strlen(rdns_lookup_message)+3);
  sprintf(newitem->host_name,"%s (%s)",buf,rdns_lookup_message);
#endif
    free(buf);
debug("<HWR add>\n");  
  pthread_mutex_lock(&rdns_mutex);
  pthread_rwlock_wrlock(&rdns_head_lock);
  newitem->next=rdns_hostlist;
  rdns_hostlist=newitem;
debug("</H>\n");
  pthread_rwlock_unlock(&rdns_head_lock);
  pthread_cond_signal(&rdns_cond);
  pthread_mutex_unlock(&rdns_mutex);
  return newitem->host_name;
}

void free_item(rdns_list_t *item) {
  free(item->host_name);
  free(item->address);
  free(item);
}

// =Lookup thread================================

void *rdns_lookup() {

  rdns_list_t *item;
  rdns_list_t *last_start=NULL;
  time_t      first_to_expire = 0;
  struct timespec fte_spec;    

#ifndef NOPTHREAD_CANCEL
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#endif
  while (rdns_terminate!=1) {
debug("<START LOOP>\n");    
    pthread_mutex_lock(&rdns_mutex);
    pthread_rwlock_rdlock(&rdns_head_lock);
    if (rdns_hostlist != last_start) {
      pthread_rwlock_unlock(&rdns_head_lock);
    } else {
      pthread_rwlock_unlock(&rdns_head_lock);
      if (first_to_expire==0) {
debug("<SUSPEND indef>\n");
        pthread_cond_wait(&rdns_cond,&rdns_mutex);
      } else {
debug("<SUSPEND timed>\n");
        fte_spec.tv_sec = first_to_expire;
        fte_spec.tv_nsec = 0;
        pthread_cond_timedwait(&rdns_cond,&rdns_mutex,&fte_spec);
      }
debug("<RESUME>\n");
    }
    pthread_mutex_unlock(&rdns_mutex);
//printf("RUNNING\n");
debug("<HRD lookup>\n");
    pthread_rwlock_rdlock(&rdns_head_lock);
    item = rdns_hostlist;
    last_start = item;
debug("<BRD lookup>\n");
    pthread_rwlock_rdlock(&rdns_body_lock);
debug("</H>\n");
    pthread_rwlock_unlock(&rdns_head_lock);
    while (item != NULL) {
      if (item->valid == 0) {
        struct hostent *host_info;
        char   *oldname=item->host_name;
        char   *newtext;
        host_info = gethostbyaddr((char *)item->address,item->address_len,item->address_format);
        
        if (host_info == NULL) {
          const char *str;
	  char *buf;
          str = hstrerror(h_errno);
	  buf=(char *)malloc(40);
#ifndef NOASPRINTF
          asprintf(&newtext,"%s (%s)",inet_ntop(item->address_format,
						item->address,
						buf,40),
                                      str);
#else
	  inet_ntop(item->address_format,
		    item->address,
		    buf,40);
	  newtext=(char *)malloc(strlen(buf)+
				 strlen(str)+3);
	  sprintf(newtext,
		  "%s (%s)",buf,str);
#endif
          free(buf);

        } else {
          newtext=(char *)malloc(strlen(host_info->h_name)+1);
          strcpy(newtext,host_info->h_name);
        }

        item->host_name = newtext;
        free(oldname);
        item->ttd = time(NULL) + rdns_default_ttl;
        item->valid = 1;
        if ((first_to_expire == 0) || (item->ttd < first_to_expire)) {
          first_to_expire = item->ttd;
        }
      }
      item=item->next;
    }
debug("</B>\n");
    pthread_rwlock_unlock(&rdns_body_lock);
    
    if (first_to_expire <= time(NULL)) {
      rdns_list_t *prev;
      time_t      now = time(NULL);
      first_to_expire = 0;
debug("<HWR purge>\n");
      pthread_rwlock_wrlock(&rdns_head_lock);
debug("<BWR purge>\n");
      pthread_rwlock_wrlock(&rdns_body_lock);
      
      item = rdns_hostlist;
      
      prev=NULL;
      while (item != NULL) {
      
        if (item->ttd <= now) {
          if (prev==NULL) {
            *rdns_listptr=item->next;
            free_item(item);    
            item=rdns_hostlist;                        
          } else {
            prev->next = item->next;
            free_item(item);
            item = prev->next;
          }
        } else {
          if ((first_to_expire == 0) || (item->ttd < first_to_expire)) {
            first_to_expire = item->ttd;
          }
          prev = item;
          item = item->next;
        }  
      }
debug("</B>\n");
      pthread_rwlock_unlock(&rdns_body_lock);
debug("</H>\n");
      pthread_rwlock_unlock(&rdns_head_lock);
    }  
    
//    sleep(1);
  }

  rdns_terminate=2;
  return NULL;
}

// ==============================================

void rdns_cache_destroy(void) {
  // terminate thread
  if (rdns_terminate!=0) return;
#ifndef NOPTHREAD_CANCEL
debug("<THREAD cancel>\n");
  pthread_cancel(rdns_thread_info);
#else
debug("<THREAD terminate>\n");
  rdns_terminate=1;
  pthread_mutex_lock(&rdns_mutex);
  pthread_cond_signal(&rdns_cond);
  pthread_mutex_unlock(&rdns_mutex);
#endif
// if you experiense lockups during copyover add a /* here (check 13 lines down)
  pthread_join(rdns_thread_info,NULL);
  pthread_rwlock_wrlock(&rdns_head_lock);
  pthread_rwlock_wrlock(&rdns_body_lock);
  while (rdns_hostlist != NULL) {
    rdns_list_t *item;
    item=rdns_hostlist;
    rdns_hostlist = rdns_hostlist->next;
    free_item(item);
  }
  pthread_rwlock_unlock(&rdns_body_lock);
  pthread_rwlock_unlock(&rdns_head_lock);
  pthread_rwlock_destroy(&rdns_body_lock);
  pthread_rwlock_destroy(&rdns_head_lock);
// and a */ here.
  pthread_mutex_destroy(&rdns_mutex);
  pthread_cond_destroy(&rdns_cond);
  rdns_terminate=-1;
  return;
}

int rdns_cache_init(void) {
  int res;
  if ((rdns_terminate!=2) && (rdns_terminate!=-1)) return -1;
  rdns_hostlist=NULL;
  res = pthread_rwlock_init(&rdns_head_lock,NULL);
  if (res != 0) return res;
  res = pthread_rwlock_init(&rdns_body_lock,NULL);
  if (res != 0) return res;
  res = pthread_mutex_init(&rdns_mutex,NULL);
  if (res != 0) return res;
  res = pthread_cond_init(&rdns_cond,NULL);
  if (res != 0) return res;
  res = 0;
// pthread_create(&rdns_thread_info,NULL, (void * ((void *) rdns_lookup)),NULL);
  if (res != 0) return res;
/*  res = pthread_detach(rdns_thread_info);
  if (res != 0) return res;*/
  if (rdns_terminate==-1) atexit(rdns_cache_destroy);
  rdns_terminate=0;
  return 0;
}

void rdns_cache_set_ttl(int new_ttl) {
  rdns_default_ttl = new_ttl;
}

char * gethostname_cached(const unsigned char *addr, int len, int ttl_refresh) {

  rdns_list_t *item;
  int address_format;

  if (rdns_terminate!=2) {
    rdns_cache_init();
  }
  if (len==4) { // INET
    address_format = AF_INET;
  } else if (len==16) { // INET6
    address_format = AF_INET6;
  } else {
debug("INVALID address length\n");  
    return NULL;
  }
debug("<HRD gethost>\n");
  pthread_rwlock_rdlock(&rdns_head_lock);
  item=rdns_hostlist;
  while (item != NULL) {
    if ((address_format==item->address_format) && 
	(len==item->address_len) && 
	(memcmp(addr,item->address,len)==0)) {
      char *res = NULL;
debug("</H cached>\n");
      
      if (item->valid) {
        if (res==NULL) res = rdns_lookup_message;
        if (ttl_refresh) item->ttd = time(NULL) + rdns_default_ttl;
      } 
      res = item->host_name;
      pthread_rwlock_unlock(&rdns_head_lock);
      return res;
    }
    item=item->next;
  }
debug("</H n/a>\n");
  pthread_rwlock_unlock(&rdns_head_lock);

// not found in list, queue.
  return add_to_list((const char *) addr,len,address_format);
}
