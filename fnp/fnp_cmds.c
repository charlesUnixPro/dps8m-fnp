//
//  fnp_cmds.c
//  fnp
//
//  Created by Harry Reed on 5/17/15.
//  Copyright (c) 2015 Harry Reed. All rights reserved.
//

#include "fnp_cmds.h"

bool MS_accept_calls = false;
bool MS_listen [MAX_LINES];
int MS_input_buffer_size [MAX_LINES];
int MS_csi [MAX_LINES];

t_stat fnp_command(char *nodename, char *id, char *arg3)
{
    ipc_printf("fnp_command(\"%s\", \"%s\", \"%s\")\n", nodename, id, arg3);
    
    if (strcmp(arg3, "bootload") == 0)
    {
        ipc_printf("Received BOOTLOAD command...\n");
        
    } else if (strcmp(arg3, "accept_calls") == 0)
    {
        ipc_printf("Received ACCEPT_CALLS command...\n");
        MS_accept_calls = true;
    } else if (strcmp(arg3, "dont_accept_calls") == 0)
    {
        ipc_printf("Received DONT_ACCEPT_CALLS command...\n");
        MS_accept_calls = false;
    } else if (strncmp(arg3, "listen", 6) == 0)
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
        MS_listen [p1] = p2 != 0;
        MS_input_buffer_size [p1] = p3;
    } else if (strncmp(arg3, "change_control_string", 21) == 0)
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
        MS_csi [p1] = p2;
    } else if (strncmp(arg3, "dump_input", 10) == 0)
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
        // XXX dump the input
    } else if (strncmp(arg3, "wru", 3) == 0)
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
    } else if (strncmp(arg3, "terminal_accepted", 17) == 0)
    {
        int p1;
        int n = sscanf(arg3, "%*s %d", &p1);
        if (n != 1)
            return SCPE_ARG;
        ipc_printf("received WRU %d ...\n", p1);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: terminal_accepted p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // XXX terminal accepted ... init stuff?
    } else if (strncmp(arg3, "break_all", 8) == 0)
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
        // XXX set break all on terminal
    } else if (strncmp(arg3, "handle_quit", 11) == 0)
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
        // XXX set handle_quit on terminal
    } else if (strncmp(arg3, "full_duplex", 11) == 0)
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
        // XXX set full_duplex on terminal
    } else if (strncmp(arg3, "echoplex", 8) == 0)
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
        // XXX set echoplex on terminal
    } else if (strncmp(arg3, "crecho", 6) == 0)
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
        // XXX set crecho on terminal
    } else if (strncmp(arg3, "lfecho", 6) == 0)
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
        // XXX set lfecho on terminal
    } else if (strncmp(arg3, "tabecho", 7) == 0)
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
        // XXX set tabecho on terminal
    } else if (strncmp(arg3, "replay", 6) == 0)
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
        // XXX set replay on terminal
    } else if (strncmp(arg3, "polite", 6) == 0)
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
        // XXX set polite on terminal
    } else if (strncmp(arg3, "prefixnl", 8) == 0)
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
        // XXX set prefixnl on terminal
    } else if (strncmp(arg3, "eight_bit_out", 13) == 0)
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
        // XXX set eight_bit_out on terminal
    } else if (strncmp(arg3, "eight_bit_in", 12) == 0)
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
        // XXX set eight_bit_in on terminal
    } else if (strncmp(arg3, "odd_parity", 10) == 0)
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
        // XXX set odd_parity on terminal
    } else if (strncmp(arg3, "output_flow_control", 19) == 0)
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
        // XXX set output_flow_control on terminal
    } else if (strncmp(arg3, "input_flow_control", 18) == 0)
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
        // XXX set input_flow_control on terminal
    } else if (strncmp(arg3, "block_xfer", 10) == 0)
    {
        int p1, p2, p3;
        int n = sscanf(arg3, "%*s %d %d", &p1, &p2, &p3);
        if (n != 3)
            return SCPE_ARG;
        ipc_printf("received block_xfer %d %d ...\n", p1, p2);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: block_xfer p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // XXX set block_xfer on terminal
    } else if (strncmp(arg3, "set_delay_table", 15) == 0)
    {
        int p1, d1, d2, d3, d4, d5, d6;
        int n = sscanf(arg3, "%*s %d %d", &p1, &d1, &d2, &d3, &d4, &d5, &d6);
        if (n != 3)
            return SCPE_ARG;
        ipc_printf("received set_delay_table %d %d ...\n", p1, d1, d2, d3, d4, d5, d6);
        if (p1 < 0 && p1 >= MAX_LINES)
        {
            ipc_printf("err: set_delay_table p1 (%d) != [0..%d]\n", p1, MAX_LINES - 1);
            return SCPE_ARG;
        }
        // XXX set set_delay_table on terminal
    }
    
    return SCPE_OK;
}

