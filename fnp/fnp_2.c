//
//  fnp_2.c
//  fnp
//
//  Created by Harry Reed on 12/9/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#include "fnp_2.h"

#include "fnp_utils.h"

#ifdef COMMENT
On Tue, Dec 9, 2014 at 1:58 PM, Harry Reed <doon386@cox.net> wrote:

>> Yup, makes sense. I assume that the line/device database is downloaded into the FNP? and in the format you game?


> No such luck. The database is a collection of macros, and is cross-compiled into the firmware, which is pushed into the FNP at multi-user startup time.
> The cross-compiler is currently not working under Multics, so we are temporarily constrained to that particular configuration. The configuration data
> is well defined, and it should be possible to extract it from the download, but I don't view that as critical path; all we need
> to get working is emulation of hardwired terminals.

> -- C

name: d.h002;
baud:               auto;
comment:            "cable 12319, X-6400, Vadic VA3467.";

name: d.h006;
baud:               9600;
comment:            "Define a 60ma current loop.";
terminal_type:      VIP7801;
attributes:         hardwired, dont_read_answerback;
initial_command:    "modes echoplex,^tabs,tabecho,crecho,lfecho";

name: d.h012-d.h013;
baud:               2400;
comment:            "hardwired to service system channels c.h212,3.";
terminal_type:      VIP7801;
attributes:         hardwired, dont_read_answerback;
initial_command:    "modes echoplex,^tabs,tabecho,crecho,lfecho";

name: d.h014;
baud:               9600;
comment:            "cable 72012, Rm 254, hardwired VIP7801, PMDC";
terminal_type:      VIP7801;
attributes:         hardwired, dont_read_answerback;
initial_command:    "modes echoplex,^tabs,tabecho,crecho,lfecho";

name: d.h015;
baud:               9600;
comment:            "cable 71688, Rm 254, hardwired VIP7802, PMDC.";
terminal_type:      VIP7801;
attributes:         hardwired, dont_read_answerback;
initial_command:    "modes echoplex,^tabs,tabecho,crecho,lfecho";

name: d.h022;
baud:               1200;
comment:            "cable 57070, TYMNET, port 60.";

name: d.h023;
baud:               1200;
comment:            "cable 57073, TYMNET, prot 61.";
#endif

//struct terminf
//{
//    struct
//    {
//        char *raw;
//        char *name;     // Multics device name
//        char *baud;     // baud rate
//        char *comment;  // comment
//        char *terminal_type;
//        char *attributes;
//        char *initial_command;
//    } multics;
//    char *uti;          // UNIX terminfo terminal type
//};


MUXTERMIO ti[] = {
{
    "name:  d.h002;"
    "baud:               auto;"
    "comment:            \"cable 12319, X-6400, Vadic VA3467.\";"
},
{
    "name: d.h006;"
    "baud:               9600;"
    "comment:            \"Define a 60ma current loop.\";"
    "terminal_type:      VIP7801;"
    "attributes:         hardwired, dont_read_answerback;"
    "initial_command:    \"modes echoplex,^tabs,tabecho,crecho,lfecho\";"
},
{
    "name: d.h012-d.h013;"
    "baud:               2400;"
    "comment:            \"hardwired to service system channels c.h212,3.\";"
    "terminal_type:      VIP7801;"
    "attributes:         hardwired, dont_read_answerback;"
    "initial_command:    \"modes echoplex,^tabs,tabecho,crecho,lfecho\";"
},
{
    "name: d.h014;"
    "baud:               9600;"
    "comment:            \"cable 72012, Rm 254, hardwired VIP7801, PMDC\";"
    "terminal_type:      VIP7801;"
    "attributes:         hardwired, dont_read_answerback;"
    "initial_command:    \"modes echoplex,^tabs,tabecho,crecho,lfecho\";"
},
{
    "name: d.h015;"
    "baud:               9600;"
    "comment:            \"cable 71688, Rm 254, hardwired VIP7802, PMDC.\";"
    "terminal_type:      VIP7801;"
    "attributes:         hardwired, dont_read_answerback;"
    "initial_command:    \"modes echoplex,^tabs,tabecho,crecho,lfecho\";"
},
{
    "name: d.h022;"
    "baud:               1200;"
    "comment:            \"cable 57070, TYMNET, port 60.\";"
},
{
    "name: d.h023;"
    "baud:               1200;"
    "comment:            \"cable 57073, TYMNET, prot 61.\";"
},
{ 0 }
};


void dumpMUXTERMIO(MUXTERMIO *p)
{
    if (!p)
        return;
    
    sim_printf("name:            %s\n", p->multics.name);
    sim_printf("baud:            %s\n", p->multics.baud);
    sim_printf("terminal_type:   %s\n", p->multics.terminal_type);
    sim_printf("attributes:      %s\n", p->multics.attributes);
    sim_printf("initial_command: %s\n", p->multics.initial_command);
    sim_printf("comment:         %s\n", p->multics.comment);
    sim_printf("\n");
    
}

#define FREE(t)     \
    if (t) free(t)

void freeMUXTERMIO(MUXTERMIO *p)
{
    if (!p)
        return;
    
    FREE(p->multics.name);
    
    FREE(p->multics.baud);
    FREE(p->multics.terminal_type);
    FREE(p->multics.attributes);
    FREE(p->multics.initial_command);
    FREE(p->multics.comment);
    
    FREE(p->uti);
}

static void parse_ti(char *raw, MUXTERMIO *ti)
{
    memset(ti, 0, sizeof(MUXTERMIO));
    ti->mux_line = -1;
    
    // 1st extract according to ';'
    char *buf = strdup(raw);
    char **bp = &buf;
    char *tok;
    while ((tok = Strsep(bp, "\n")))
    {
        if (strlen(tok) == 0)
            continue;
        
        //sim_printf("tok = `%s'\n", tok);
        
        // 2nd, now according to ';'
        char *buf2 = strdup(tok);
        char **bp2 = &buf2;
        char *tok2;

        while ((tok2 = Strsep(bp2, ";")))
        {
            if (strlen(tok2) == 0)
                continue;
            
            //sim_printf("    tok2 = `%s'\n", trim(tok2));
            
            if (strchr(tok2, ':') == 0)
            {
                sim_printf("malformed entry '%s'\n", tok2);
                continue;
            }
            char *t = strdup(tok2);

            char *first = Strtok(t, ":");
            char *second = Strtok(NULL, ":");
            
            //sim_printf("%s %s\n", first, second);
            
            if (strcmp(first, "name") == 0)
                ti->multics.name = strdup(trim(second));
            else if (strcmp(first, "baud") == 0)
                ti->multics.baud  = strdup(trim(second));
            else if (strcmp(first, "comment") == 0)
                ti->multics.comment  = strdup(trim(second));
            else if (strcmp(first, "terminal_type") == 0)
                ti->multics.terminal_type  = strdup(trim(second));
            else if (strcmp(first, "attributes") == 0)
                ti->multics.attributes  = strdup(trim(second));
            else if (strcmp(first, "initial_command") == 0)
                ti->multics.initial_command  = strdup(trim(second));
            else
                sim_printf("Unknown entry '%s'\n", first);
            
            free(t);
        }
        
        free(buf2);
    }
    free(buf);
}

void parseTI()
{
    MUXTERMIO *t = ti;
    while (t->raw)
    {
        parse_ti(t->raw, t);
        
        dumpMUXTERMIO(t);
        t++;
    }
}

char *
getPortList()
{
    return "1,2,3,4,5";
}