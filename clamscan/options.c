/*
 *  Copyright (C) 2001-2002 Tomasz Kojm <zolw@konarski.edu.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Sat Sep 14 22:18:20 CEST 2002: included getfirst*(), getnext*() functions
 *			from Alejandro Dubrovsky <s328940@student.uq.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clamav.h>
#define _GNU_SOURCE
#include "getopt.h"


#include "options.h"
#include "others.h"
#include "shared.h"

void clamscan(struct optstruct *opt);

int main(int argc, char **argv)
{
	int ret, opt_index, i, len;
	struct optstruct *opt;

	const char *getopt_parameters = "hvd:wriVl:m";

	static struct option long_options[] = {
	    {"help", 0, 0, 'h'},
	    {"quiet", 0, 0, 0},
	    {"verbose", 0, 0, 'v'},
	    {"debug", 0, 0, 0},
	    {"version", 0, 0, 'V'},
	    {"tempdir", 1, 0, 0},
	    {"config-file", 1, 0, 0}, /* clamdscan */
	    {"database", 1, 0, 'd'},
	    {"whole-file", 0, 0, 'w'}, /* not used */
	    {"force", 0, 0, 0},
	    {"recursive", 0, 0, 'r'},
	    {"disable-summary", 0, 0, 0},
	    {"infected", 0, 0, 'i'},
	    {"log", 1, 0, 'l'},
	    {"log-verbose", 0, 0, 0},
	    {"threads", 1, 0, 0}, /* not used */
	    {"one-virus", 0, 0, 0}, /* not used */
	    {"move", 1, 0, 0},
	    {"remove", 0, 0, 0},
	    {"exclude", 1, 0, 0},
	    {"include", 1, 0, 0},
	    {"max-files", 1, 0, 0},
	    {"max-space", 1, 0, 0},
	    {"max-recursion", 1, 0, 0},
	    {"disable-archive", 0, 0, 0},

	    {"mbox", 0, 0, 'm'},
	    {"stdout", 0, 0, 0},
	    {"unzip", 2, 0, 0},
	    {"unrar", 2, 0, 0},
	    {"unace", 2, 0, 0},
	    {"unarj", 2, 0, 0}, /* not used */
	    {"arj", 2, 0, 0},
	    {"zoo", 2, 0, 0}, /* not used */
	    {"unzoo", 2, 0, 0},
	    {"lha", 2, 0, 0},
	    {"jar", 2, 0, 0},
	    {"tar", 2, 0, 0},
	    {"tgz", 2, 0, 0},
	    {"deb", 2, 0, 0},
	    {0, 0, 0, 0}
    	};


    opt=(struct optstruct*) mmalloc(sizeof(struct optstruct));
    opt->optlist = NULL;

    while(1) {

	opt_index=0;
	ret=getopt_long(argc, argv, getopt_parameters, long_options, &opt_index);

	if (ret == -1)
	    break;

	switch (ret) {
	    case 0:
		register_long_option(opt, long_options[opt_index].name);
		break;

    	    default:
		if(strchr(getopt_parameters, ret))
		    register_char_option(opt, ret);
		else {
		    mprintf("!Unknown option passed.\n");
		    free_opt(opt);
		    exit(40);
		}
        }
    }

    if (optind < argc) {

        len=0;

	/* count length of non-option arguments */

	for(i=optind; i<argc; i++)
	    len+=strlen(argv[i]);

	len=len+argc-optind-1; /* add spaces between arguments */
	opt->filename=(char*)mcalloc(len + 256, sizeof(char));

        for(i=optind; i<argc; i++) {
	    strncat(opt->filename, argv[i], strlen(argv[i]));
	    if(i != argc-1)
		strncat(opt->filename, "\t", 1);
	}

    }
    clamscan(opt);

    free_opt(opt);

    return(0);
}

void register_char_option(struct optstruct *opt, char ch)
{
	struct optnode *newnode;

    newnode = (struct optnode *) mmalloc(sizeof(struct optnode));
    newnode->optchar = ch;
    if(optarg != NULL) {
	newnode->optarg = (char *) mcalloc(strlen(optarg) + 1, sizeof(char));
	strcpy(newnode->optarg, optarg);
    } else newnode->optarg = NULL;

    newnode->optname = NULL;
    newnode->next = opt->optlist;
    opt->optlist = newnode;
}

void register_long_option(struct optstruct *opt, const char *optname)
{
	struct optnode *newnode;

    newnode = (struct optnode *) mmalloc(sizeof(struct optnode));
    newnode->optchar = 0;
    if(optarg != NULL) {
	newnode->optarg = (char *) mcalloc(strlen(optarg) + 1, sizeof(char));
	strcpy(newnode->optarg, optarg);
    } else newnode->optarg = NULL;

    newnode->optname = (char *) mcalloc(strlen(optname) + 1, sizeof(char));
    strcpy(newnode->optname, optname);
    newnode->next = opt->optlist;
    opt->optlist = newnode;
}

int optc(const struct optstruct *opt, char ch)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optchar == ch) return 1;
	} else break;
	handler = handler->next;
    }

    return(0);
}

int optl(const struct optstruct *opt, const char *optname)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optname)
		if(!strcmp(handler->optname, optname)) return 1;
	} else break;
	handler = handler->next;
    }

    return(0);
}

char *getargc(const struct optstruct *opt, char ch)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optchar == ch) return handler->optarg;
	} else break;
	handler = handler->next;
    }

    return(NULL);
}

char *getfirstargc(const struct optstruct *opt, char ch, struct optnode **optnode)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optchar == ch) {
	    	*optnode = handler;
	    	return handler->optarg;
	    }
	} else break;
	handler = handler->next;
    }
    *optnode = NULL;
    return(NULL);
}

char *getnextargc(struct optnode **optnode, char ch)
{
	struct optnode *handler;

    handler = (*optnode)->next;

    while(1) {
	if(handler) {
	    if(handler->optchar == ch) {
	    	*optnode = handler;
	    	return handler->optarg;
	    }
	} else break;
	handler = handler->next;
    }
    *optnode = NULL;
    return(NULL);
}

char *getargl(const struct optstruct *opt, const char *optname)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optname)
		if(!strcmp(handler->optname, optname)) return handler->optarg;
	} else break;
	handler = handler->next;
    }

    return(NULL);
}

char *getfirstargl(const struct optstruct *opt, const char *optname, struct optnode **optnode)
{
	struct optnode *handler;

    handler = opt->optlist;

    while(1) {
	if(handler) {
	    if(handler->optname)
		if(!strcmp(handler->optname, optname)) {
			*optnode = handler;
			return handler->optarg;
		}
	} else break;
	handler = handler->next;
    }
    
    *optnode = NULL;
    return(NULL);
}

char *getnextargl(struct optnode **optnode, const char *optname)
{
	struct optnode *handler;

    handler = (*optnode)->next;

    while(1) {
	if(handler) {
	    if(handler->optname)
		if(!strcmp(handler->optname, optname)) {
			*optnode = handler;
			return handler->optarg;
		}
	} else break;
	handler = handler->next;
    }
    
    *optnode = NULL;
    return(NULL);
}

void free_opt(struct optstruct *opt)
{
	struct optnode *handler, *prev;

    if(!opt)
	return;

    handler = opt->optlist;

    while(handler != NULL) {
	handler->optchar = 0;
	if(handler->optarg) free(handler->optarg);
	if(handler->optname) free(handler->optname);
	prev = handler;
	handler = handler->next;
	free(prev);
    }

    if (opt->filename)
    	free(opt->filename);
    free(opt);
}
