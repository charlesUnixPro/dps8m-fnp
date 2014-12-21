//
//  fnp_2.c
//  fnp
//
//  Created by Harry Reed on 12/9/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#include "fnp_2.h"

#include "fnp_utils.h"

FMTI *fmti = NULL;

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

void dumpFMTI(FMTI *p)
{
    if (!p)
        return;
    
    sim_printf("name:            %s\n", p->multics.name);
//    sim_printf("baud:            %s\n", p->multics.baud);
//    sim_printf("terminal_type:   %s\n", p->multics.terminal_type);
//    sim_printf("attributes:      %s\n", p->multics.attributes);
//    sim_printf("initial_command: %s\n", p->multics.initial_command);
//    sim_printf("comment:         %s\n", p->multics.comment);
    
    ATTRIBUTES *current, *tmp;
    
    HASH_ITER(hh, p->multics.attrs, current, tmp)
    {
        char temp[128];
        snprintf(temp, sizeof(temp), "%s:", current->Attribute);
        sim_printf("%-17s%s\n", trim(temp), current->Value);
    }
    
    sim_printf("\n");
}

char *strFMTI(FMTI *p, int line)
{
    if (!p)
        return "";
    static char str[1024];
    const int len = sizeof(str);
    
    snprintf(str, len,
    "\r\nLine %d connected as\r\n"
    "name:            %s\r\n",
//    "baud:            %s\r\n"
//    "terminal_type:   %s\r\n"
//    "attributes:      %s\r\n"
//    "initial_command: %s\r\n"
//    "comment:         %s\r\n"
//    "\n", line,
       line,      p->multics.name    //,  p->multics.baud, p->multics.terminal_type, p->multics.attributes, p->multics.initial_command, p->multics.comment
    );
    
    ATTRIBUTES *current, *tmp;
    HASH_ITER(hh, p->multics.attrs, current, tmp)
    {
        char temp[128], temp2[128];
        snprintf(temp,  sizeof(temp),  "%s:", current->Attribute);
        snprintf(temp2, sizeof(temp2), "%-17s%s\n", trim(temp), current->Value);
        
        strcat(str, temp2);
    }
    
    // XXX be careful of buffer overruns!
    strcat(str, "\n");

    
    return str;
}

#define FREE(t)     \
    if (t) free(t)

void freeFMTI(FMTI *p, bool bRecurse)
{
    if (!p)
        return;
    
    if (bRecurse)
    {
        while (p)
        {
            FREE(p->multics.name);
        
//            FREE(p->multics.baud);
//            FREE(p->multics.terminal_type);
//            FREE(p->multics.attributes);
//            FREE(p->multics.initial_command);
//            FREE(p->multics.comment);
            
            ATTRIBUTES *current, *tmp;
            HASH_ITER(hh, p->multics.attrs, current, tmp)
            {
                HASH_DEL(p->multics.attrs, current);  /* delete; users advances to next */
                free(current->Attribute);
                free(current->Value);
                free(current);            /* optional- if you want to free  */
            }
            
            FREE(p->uti);
            FMTI *nxt = p->next;
            
            FREE(p);
            
            p = nxt;
        }

    } else {
    
        FREE(p->multics.name);
    
//        FREE(p->multics.baud);
//        FREE(p->multics.terminal_type);
//        FREE(p->multics.attributes);
//        FREE(p->multics.initial_command);
//        FREE(p->multics.comment);
   
        ATTRIBUTES *current, *tmp;
        HASH_ITER(hh, p->multics.attrs, current, tmp)
        {
            HASH_DEL(p->multics.attrs, current);  /* delete; users advances to next */
            free(current->Attribute);
            free(current->Value);
            free(current);            /* optional- if you want to free  */
        }

        FREE(p->uti);
        
        FREE(p);
    }
    
}

FMTI *newFMTI()
{
    return calloc(1, sizeof(FMTI));
}

#if COMMENT
static void parse_ti(char *raw, FMTI *ti)
{
    memset(ti, 0, sizeof(FMTI));
    
    // 1st extract according to ';'
    char *buf = strdup(raw);
    ti->raw = buf;
    
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
}

int32 parseTI()
{
    FMTI *t = fmti;
    int n = 0;
    
    while (t->raw)
    {
        parse_ti(t->raw, t);
        
        //dumpFMTI(t);
        t++;
        n++;
    }
    
    return n;
}
#endif

char *
getDevList()
{
    static char buf[2048];
    strcpy(buf, "");
    
    FMTI *t = fmti;
    while (t)
    {
        if (t->inUse == false)
        {
            if (strlen(buf) > 0)
                strcat(buf, ",");
            strcat(buf, t->multics.name);
        }
        t = t->next;
    }
    return buf;
}


FMTI *searchForDevice(char *name)
{
    //sim_printf("looking for <%s>\n", dev);
    FMTI *t = fmti;
        
    while (t)
    {
        if (t->inUse == false)
            if (strcmp(name, t->multics.name) == 0)
                return t;
        t = t->next;
    }

    return NULL;
}

ATTRIBUTES *searchForAttribute(char *attrib, ATTRIBUTES *a)
{
    ATTRIBUTES *s;
    
    HASH_FIND_STR(a, attrib, s);
    return s;
}

MUXTERMIO ttys[MAX_LINES];
extern TMLN mux_ldsc[MAX_LINES];

MUXTERMSTATE processUserInput(TMXR *mp, TMLN *tmln, MUXTERMIO *tty, int32 line, int32 kar)
{
    if (kar == 0x1b || kar == 0x03)             // ESCape ('\e') | ^C
    {
//        char n[132];
//        snprintf(n, sizeof(n), "%d", line);
//        tmxr_dscln(&mux_unit, !0, n, mp);       // disconnect line
        
        tmxr_reset_ln( &mux_ldsc[line] ) ;
        
        tty->mux_line = -1;
        tty->state = eDisconnected;
        tty->tmln = NULL;
        if (tty->fmti)
            tty->fmti->inUse = false;
        tty->fmti = NULL;
        
        return eDisconnected;                   // line disconnect
    }
    
    // buffer too full for anything more?
    if (tty->nPos >= sizeof(tty->buffer))
    {
        // yes. Only allow \n, \r, ^H, ^R
        switch (kar)
        {
            case '\b':  // backspace
            case 127:   // delete
                tmxr_linemsg(tmln, "\b \b");    // remove char from line
                tty->buffer[tty->nPos] = 0;     // remove char from buffer
                tty->nPos -= 1;                 // back up buffer pointer
                break;
                
            case '\n':
            case '\r':
                tty->buffer[tty->nPos] = 0;
                return eEndOfLine;              // EOL found

            case 0x12:  // ^R
                tmxr_linemsg  (tmln, "^R\r\n");       // echo ^R
                tmxr_linemsgf (tmln, PROMPT, getDevList());
                tmxr_linemsg  (tmln, tty->buffer);
                break;
                
            default:
                break;
        }
        return eInput;  // stay in input mode
    }

    if (isprint(kar))   // printable?
    {
        MuxWrite(line, kar);
        tty->buffer[tty->nPos++] = kar;
    } else {
        switch (kar)
        {
            case '\b':  // backspace
            case 127:   // delete
                if (tty->nPos > 0)
                {
                    tmxr_linemsg(tmln, "\b \b");    // remove char from line
                    tty->buffer[tty->nPos] = 0;     // remove char from buffer
                    tty->nPos -= 1;                 // back up buffer pointer
                } else
                    tmxr_linemsg(tmln, "\a");
                
                break;
                
            case '\n':
            case '\r':
                tty->buffer[tty->nPos] = 0;
                return eEndOfLine;
                
            case 0x12:  // ^R
                tmxr_linemsg  (tmln, "^R\r\n");       // echo ^R
                tmxr_linemsgf (tmln, PROMPT, getDevList());
                tty->buffer[tty->nPos] = 0;
                tmxr_linemsg  (tmln, tty->buffer);
                break;

                
            default:
                break;
        }

    }
    
    return eInput;  // stay in input mode
}

FMTI * readDevInfo(FILE *src)
{
    char buff[1024];

    FMTI *head = NULL;      // head of linked list
    FMTI *current = NULL;   // currrent entry (tail)

    int nLines = 0;
    
    while (fgets(buff, sizeof(buff), src))
    {
        nLines += 1;
        
        char *p = trim(buff);   // trim leading and trailing whitespace
        
        if (p[0] == '#')        // a '#' as first non-white charater is a comment line
            continue;
        
        if (p[0] == 0)          // blank line
            continue;;
        
        if (strncmp(p, "name:", 5) == 0)
        {
            if (!head)
            {
                head = newFMTI();
                current = head;
            } else {
                current->next = newFMTI();
                current = current->next;
            }
        }
    
        char *first  = trim(Strtok(p, ":"));       // stuff to the left of ':'
        char *second = trim(Strtok(NULL, ":;"));    // stuff to the right of ':'
        
        //sim_printf("%s %s\n", first, second);
        
        if (strcmp(first, "name") == 0)
            current->multics.name = strdup(trim(second));
//        else if (strcmp(first, "baud") == 0)
//            current->multics.baud  = strdup(trim(second));
//        else if (strcmp(first, "comment") == 0)
//            current->multics.comment  = strdup(trim(second));
//        else if (strcmp(first, "terminal_type") == 0)
//            current->multics.terminal_type  = strdup(trim(second));
//        else if (strcmp(first, "attributes") == 0)
//            current->multics.attributes  = strdup(trim(second));
//        else if (strcmp(first, "initial_command") == 0)
//            current->multics.initial_command  = strdup(trim(second));
//        else
//            sim_printf("Unknown terminal attribute '%s'\n", first);
        else
        {
            if (searchForAttribute(first, current->multics.attrs))
                sim_printf("Warning: Ignoring duplicate attribute <%s> at line %d\n", first, nLines);
            else
            {
                ATTRIBUTES *a  = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES));
                a->Attribute = strdup(first);
                a->Value = strdup(second);
                HASH_ADD_KEYPTR(hh, current->multics.attrs, a->Attribute, strlen(a->Attribute), a);
            }
        }
        
    }
    
    return head;
}

FMTI *readAndPrint(char *file)
{
    FILE *in = fopen(file, "r");
    FMTI *p = readDevInfo(in);
    fclose(in);
    
    while (p)
    {
        dumpFMTI(p);
        p = p->next;
    }
    
    return p;
}