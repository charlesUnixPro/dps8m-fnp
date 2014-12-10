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

struct terminf
{
    struct
    {
        char *raw;
        char *name;     // Multics device name
        char *baud;     // baud rate
        char *comment;  // comment
        char *terminal_type;
        char *attributes;
        char *initial_command;
    } multics;
    char *uti;          // UNIX terminfo terminal type
};

typedef struct terminf TERMINFO;

TERMINFO ti[] ={
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

static void parse_ti(char *raw)
{
    // 1st  extract according to ';'
    char *buf = strdup(raw);
    char **bp = &buf;
    char *tok;
    while ((tok = Strsep(bp, ";\r\n")))
    {
        sim_printf("tok = `%s'\n", tok);
        // now according to ':'
        
        char *buf2 = strdup(tok);
        char **bp2 = &buf2;
        char *tok2;

        while ((tok2 = Strsep(bp2, ":")))
            sim_printf("    tok2 = `%s'\n", tok2);
        
        free(buf2);
    }
    free(buf);
}

void parseTI()
{
    TERMINFO *t = ti;
    while (t->multics.raw)
    {
        parse_ti(t->multics.raw);
        
        t++;
    }
}