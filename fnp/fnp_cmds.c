//
//  fnp_cmds.c
//  fnp
//
//  Created by Harry Reed on 5/17/15.
//  Copyright (c) 2015 Harry Reed. All rights reserved.
//

//#include "fnp_defs.h"
#include "fnp_cmds.h"
#include "fnp_2.h"
#include "fnp_mux.h"

t_MState MState;

static char * unpack (char * buffer)
  {
    char * p = strstr (buffer, "data:");
    if (! p)
      return NULL;
    p += 5; // strlen ("data:");
    char * q;
    int nBytes = strtol (p, & q, 10);
    if (p == q)
      return NULL;
    if (* q != ':')
      return NULL;
    q ++;
    char * out = malloc (nBytes);
    if (! out)
      goto fail;
    char * o = out;
    while (nBytes --)
      {
        int val;

        char ch = * q ++;
        if (ch >= '0' && ch <= '9')
          val = (ch - '0') << 4;
        else if (ch >= 'a' && ch<= 'f')
          val = (ch - 'a' + 10) << 4;
        else
          goto fail;

        ch = * q ++;
        if (ch >= '0' && ch <= '9')
          val |= (ch - '0');
        else if (ch >= 'a' && ch<= 'f')
          val |= (ch - 'a' + 10);
        else
          goto fail;

        //ipc_printf ("%c%c %02x\n", *(q - 2), *(q - 1), val);
        * o ++ = val;
      }
    return out;
fail:
    free (out);
    return NULL;
  }

t_stat fnp_command(char *nodename, char *id, char *arg3)
{
    ipc_printf("fnp_command(\"%s\", \"%s\", \"%s\")\n", nodename, id, arg3);
    
    size_t arg3_len = strlen (arg3);
    char keyword [arg3_len];
    sscanf (arg3, "%s", keyword);

    if (strcmp(keyword, "bootload") == 0)
    {
        ipc_printf("Received BOOTLOAD command...\n");
        



    } else if (strcmp(keyword, "accept_calls") == 0)
    {
        ipc_printf("Received ACCEPT_CALLS command...\n");
        MState . accept_calls = true;



    } else if (strcmp(keyword, "dont_accept_calls") == 0)
    {
        ipc_printf("Received DONT_ACCEPT_CALLS command...\n");
        MState . accept_calls = false;



    } else if (strcmp(keyword, "listen") == 0)
    {
        int p1, p2, p3;
        int n = sscanf(arg3, "%*s %d %d %d", &p1, &p2, &p3);
        if (n != 3)
            return SCPE_ARG; // listen is supposed to have 3 args
        ipc_printf("received LISTEN %d %d %d ...\n", p1, p2, p3);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: LISTEN p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: LISTEN p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . listen = p2 != 0;
        MState . line [p1] . inputBufferSize = p3;
#if 0
// Can't report listening on line # because if you weren't listening you can't hace a line #
// Need to rework the logic a bit;the user needs to be abe to grab a line even if Multics is 
// not accepting calls, or not listening to a line. When the listen comes, then we send the
// accept_new_terminal.
        if (p2)
        {
            int muxLineNum = MState.line[p1].muxLineNum;
            if (muxLineNum != -1 && ttys[muxLineNum].state == eUnassigned)
            {
                
                connectPrompt (ttys[muxLineNum].tmln);
            }
        }
#endif

    } else if (strcmp(keyword, "change_control_string") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG; // change_control_string is supposed to have 2 args
        ipc_printf("received CHANGE_CONTROL_STRING %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: CHANGE_CONTROL_STRING p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        MState . line [p1] . ctrlStrIdx = p2;



    } else if (strcmp(keyword, "dump_input") == 0)
    {
        int p1;
        int n = sscanf(arg3, "%*s %d", &p1);
        if (n != 1)
            return SCPE_ARG;
        ipc_printf("received dump_input %d ...\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: DUMP_INPUT p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // dump the input
       int muxLineNo = MState . line [p1] . muxLineNum;
       //ipc_printf ("dumping mux line %d\n");
       ttys [muxLineNo] . nPos = 0; 


    } else if (strcmp(keyword, "wru") == 0)
    {
        int p1;
        int n = sscanf(arg3, "%*s %d", &p1);
        if (n != 1)
            return SCPE_ARG;
        ipc_printf("received WRU %d ...\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: WRU p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // XXX send wru
        // XXX for now, just reply that there was a timeout
        char msg [256];
        sprintf (msg, "wru_timeout %d", p1);
        tellCPU (0, msg);



    } else if (strcmp(keyword, "terminal_accepted") == 0)
    {
        int p1;
        int n = sscanf(arg3, "%*s %d", &p1);
        if (n != 1)
            return SCPE_ARG;
        ipc_printf("received terminal_accepted %d ...\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: terminal_accepted p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // XXX terminal accepted ... init stuff?
        char msg [256];
        sprintf (msg, "send_output %d", p1);
        tellCPU (0, msg);



    } else if (strcmp(keyword, "break_all") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received break_all %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: break_all p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: break_all p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        // set break all on terminal
        MState . line [p1] . breakAll = !! p2;



    } else if (strcmp(keyword, "handle_quit") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received handle_quit %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: handle_quit p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: full_duplex p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . handleQuit = !! p2;



    } else if (strcmp(keyword, "full_duplex") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received full_duplex %d ...\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: full_duplex p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: full_duplex p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . fullDuplex = !! p2;



    } else if (strcmp(keyword, "echoplex") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received echoplex %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: echoplex p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: full_duplex p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . echoPlex = !! p2;



    } else if (strcmp(keyword, "crecho") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received crecho %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: crecho p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: crecho p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . crecho = !! p2;



    } else if (strcmp(keyword, "lfecho") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received lfecho %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: lfecho p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: lfecho p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . lfecho = !! p2;



    } else if (strcmp(keyword, "tabecho") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received tabecho %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: tabecho p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: tabecho p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . tabecho = !! p2;



    } else if (strcmp(keyword, "replay") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received replay %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: replay p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: replay p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . replay = !! p2;



    } else if (strcmp(keyword, "polite") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received polite %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: polite p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: polite p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . polite = !! p2;



    } else if (strcmp(keyword, "prefixnl") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received prefixnl %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: prefixnl p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: prefixnl p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . prefixnl = !! p2;



    } else if (strcmp(keyword, "eight_bit_out") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received eight_bit_out %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: eight_bit_out p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: eight_bit_out p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . eight_bit_out = !! p2;



    } else if (strcmp(keyword, "eight_bit_in") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received eight_bit_in %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: eight_bit_in p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: eight_bit_in p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . eight_bit_in = !! p2;



    } else if (strcmp(keyword, "odd_parity") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received odd_parity %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: odd_parity p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: odd_parity p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . odd_parity = !! p2;



    } else if (strcmp(keyword, "output_flow_control") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received output_flow_control %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: output_flow_control p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: output_flow_control p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . output_flow_control = !! p2;



    } else if (strcmp(keyword, "input_flow_control") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received input_flow_control %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: input_flow_control p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        if (p2 != 0 && p2 != 1)
        {
            ipc_printf("err: input_flow_control p2 (%d) != [0..1]\n", p2);
            return SCPE_ARG; 
        }
        MState . line [p1] . input_flow_control = !! p2;



    } else if (strcmp(keyword, "block_xfer") == 0)
    {
        int p1, p2, p3;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2, &p3);
        if (n != 3)
            return SCPE_ARG;
        ipc_printf("received block_xfer %d %d %d ...\n", p1, p2, p3);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: block_xfer p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        MState . line [p1] . block_xfer_in_frame = p2;
        MState . line [p1] . block_xfer_out_of_frame = p3;



    } else if (strcmp(keyword, "set_delay_table") == 0)
    {
        int p1, d1, d2, d3, d4, d5, d6;
        int n = sscanf(arg3, "%*s %d %d %d %d %d %d %d", &p1, &d1, &d2, &d3, &d4, &d5, &d6);
        if (n != 7)
            return SCPE_ARG;
        ipc_printf("received set_delay_table %d %d %d %d %d %d %d...\n", p1, d1, d2, d3, d4, d5, d6);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: set_delay_table p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        MState . line [p1] . delay_table [0] = d1;
        MState . line [p1] . delay_table [1] = d2;
        MState . line [p1] . delay_table [2] = d3;
        MState . line [p1] . delay_table [3] = d4;
        MState . line [p1] . delay_table [4] = d5;
        MState . line [p1] . delay_table [5] = d6;




    } else if (strcmp(keyword, "output") == 0)
    {
        int p1, p2;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2);
        if (n != 2)
            return SCPE_ARG;
        ipc_printf("received output %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: output p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        char * data = unpack (arg3);
ipc_printf ("msg:<%s>\n", data);
        if (! data)
        {
            sim_printf ("illformatted output message data; dropping\n");
            return SCPE_ARG;
        }
        // delete NULs
        char * clean = malloc (p2);
        char * p = data;
        char * q = clean;
        for (int i = 0; i < p2; i ++)
          {
            char c = * p ++;
            if (c)
              * q ++ = c;
          }
        * q ++ = 0;

        int muxLineNum = MState . line [p1] . muxLineNum;
        tmxr_linemsg (& mux_ldsc [muxLineNum], clean);
        free (data);
        free (clean);
        char msg [256];
        sprintf (msg, "send_output %d", p1);
ipc_printf ("tell CPU to send_output\n");
        tellCPU (0, msg);




    } else if (strcmp(keyword, "disconnect_line") == 0)
    {
        int p1;
        int n = sscanf(arg3, "%*s %d", &p1);
        if (n != 1)
            return SCPE_ARG;
        ipc_printf("received disconnect_line %d\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: disconnect_line p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        int muxLineNum = MState.line[p1].muxLineNum;
        tmxr_linemsg(ttys[muxLineNum].tmln, "Multics has disconnected you\r\n");

        ttys[muxLineNum].fmti->inUse = false;
        ttys[muxLineNum].fmti = NULL;

        connectPrompt(ttys[muxLineNum].tmln);
        ttys[muxLineNum].state = eUnassigned;
        char msg [256];
        sprintf (msg, "line_disconnected %d", p1);
        tellCPU (0, msg);
        




    } else {
       ipc_printf ("dropping <%s>\n", arg3);
       return SCPE_ARG;
    }
    
    return SCPE_OK;
}


void sendInputLine (int hsla_line_num, char * buffer, int nChars)
  {
    // Our encoding scheme is 2 hex digits/char

    // temporary until the logic is in place XXX
    int outputChainPresent = 0;

    // temporary until the logic is in place XXX
    int breakPresent = 1;

    // Our encoding scheme is 2 hex digits/char
    char cmd [256 + 2 * nChars];
    sprintf (cmd, "input %d %d %d %d data:%d:", hsla_line_num, nChars, outputChainPresent, breakPresent, nChars);
    char * tail = cmd;
    while (* tail)
      tail ++;
    for (int i = 0; i < nChars; i ++)
       {
         * tail ++ = "0123456789abcdef" [(buffer [i] >> 4) % 16];
         * tail ++ = "0123456789abcdef" [(buffer [i]     ) % 16];
       }
    tellCPU (0, cmd);
  }
