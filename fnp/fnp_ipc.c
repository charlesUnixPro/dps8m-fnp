//
//  fnp_ipc.c
//  fnp
//
//  Created by Harry Reed on 12/2/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#ifdef VM_FNP
#include "fnp_defs.h"
#include "fnp_utils.h"
#include "fnp_mux.h"
#include "fnp_cmds.h"
#endif
#ifdef VM_DPS8
#include "sim_defs.h"
#include <stdbool.h>

char *stripquotes(char *s);
char *trim(char *s);
char *Strtok(char *, char *);
bool startsWith(const char *str, const char *pre);

void sim_printf (const char* fmt, ...);

#endif

#include "fnp_ipc.h"

t_stat set_prompt (int32 flag, char *cptr); // from scp.c:451


int32 ipc_verbose = 0;
int32 ipc_trace = 0;

char fnpName[32] = IPC_NODE;
char fnpGroup[32] = IPC_GROUP;

IPC_Peer *Peers = NULL;      // for peer name <=> id map

t_stat ipc_svc (UNIT *uptr);
t_stat ipc_reset (DEVICE *dptr);

/* IPC data structures
 
 ipc_dev      IPC device descriptor
 ipc_unit     IPC unit descriptor
 ipc_reg      IPC register list
 */

#ifdef VM_FNP
DIB ipc_dib = { MUX_INT_CLK, MUX_INT_CLK, PI_CLK, &clk };
#endif

UNIT ipc_unit = { UDATA (&ipc_svc, UNIT_DISABLE, 0) };

static t_stat ipc_start (UNIT *uptr, int32 val, char *c, void *desc);
static t_stat ipc_stop  (UNIT *uptr, int32 val, char *c, void *desc);
static t_stat ipc_test (FILE *st, UNIT *uptr, int32 val, void *desc);
static t_stat ipc_set_node  (UNIT *uptr, int32 val, char *c, void *desc);
static t_stat ipc_show_node(FILE *st, UNIT *uptr, int32 val, void *desc);
static t_stat ipc_set_grp  (UNIT *uptr, int32 val, char *c, void *desc);
static t_stat ipc_show_grp(FILE *st, UNIT *uptr, int32 val, void *desc);
static t_stat ipc_restart  (UNIT *uptr, int32 val, char *c, void *desc);
static t_stat ipc_show_peers(FILE *st, UNIT *uptr, int32 val, void *desc);
static t_stat ipc_remove_peers (UNIT *uptr, int32 val, char *c, void *desc);

MTAB ipc_mod[] = {
    { MTAB_XTD|MTAB_VDV|MTAB_NMO, 0,  "TEST", NULL, NULL, &ipc_test },              // show IPC test
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN, /* mask */
        0,                              /* match */
        (char *) "START",               /* print string */
        (char *) "START",               /* match string */
        ipc_start,                      /* validation routine */
        NULL,                           /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN, /* mask */
        0,                              /* match */
        (char *) "STOP",                /* print string */
        (char *) "STOP",                /* match string */
        ipc_stop,                       /* validation routine */
        NULL,                           /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN | MTAB_NC | MTAB_VALO | MTAB_QUOTE , /* mask */
        0,                              /* match */
        (char *) "NODE",                /* print string */
        (char *) "NODE",                /* match string */
        ipc_set_node,                   /* validation routine */
        ipc_show_node,                  /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN | MTAB_NC | MTAB_VALR | MTAB_QUOTE , /* mask */
        0,                              /* match */
        (char *) "GROUP",               /* print string */
        (char *) "GROUP",               /* match string */
        ipc_set_grp,                    /* validation routine */
        ipc_show_grp,                   /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN, /* mask */
        0,                              /* match */
        (char *) "RESTART",             /* print string */
        (char *) "RESTART",             /* match string */
        ipc_restart,                    /* validation routine */
        NULL,                           /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN, /* mask */
        0,                              /* match */
        (char *) "PEERS",               /* print string */
        (char *) "PEERS",               /* match string */
        NULL,                           /* validation routine */
        ipc_show_peers,                 /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    {
        MTAB_XTD | MTAB_VDV | MTAB_VUN, /* mask */
        0,                              /* match */
        (char *) "REMOVEPEERS",         /* print string */
        (char *) "REMOVEPEERS",         /* match string */
        ipc_remove_peers,               /* validation routine */
        NULL,                           /* display routine */
        NULL,                           /* value descriptor */
        NULL                            /* help */
    },
    { 0 }
};


DEBTAB ipc_dbg[] = {
    {"TRACE",   DBG_IPCTRACE  },
    {"VERBOSE", DBG_IPCVERBOSE},
    {0}
};


DEVICE ipc_dev = {
    "IPC",
    &ipc_unit,
    NULL,   //ipc_reg,
    ipc_mod,
    1, 0, 0, 0, 0, 0,
    NULL, NULL, &ipc_reset,
    NULL,
    NULL,   //&ipc_attach,// attach
    NULL,   //&ipc_detach,// detach
#ifdef VM_FNP
    &ipc_dib,
#else
    NULL,
#endif
    DEV_DIS | DEV_DISABLE | DEV_DEBUG,
    0,
    ipc_dbg
};


/* Unit service */

t_stat ipc_svc (UNIT *uptr)
{
    return SCPE_OK;
}

/* Reset routine */
t_stat ipc_reset (DEVICE *dptr)
{
    //    if ((dptr->flags & DEV_DIS) == 0)
    //    {
    //        if (dptr == &ipc_dev) ipc_unit.flags |= DEV_DIS;
    //    }
    
    //sim_cancel (&ipc_unit);                                 /* deactivate unit */
    return SCPE_OK;
}


/*
 * perform simple test to determine if s is a possible guid
 */
static bool isGUID(char *s)
{
    if (strlen(s) != 32)
        return false;
    
    while (*s)
    {
        if (!isxdigit(*s))
            return false;
        s += 1;
    }
    return true;
}

//  --------------------------------------------------------------------------
//  Example Zyre distributed chat application
//
//  --------------------------------------------------------------------------
//  Copyright (c) 2010-2014 iMatix Corporation and Contributors
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//  --------------------------------------------------------------------------

//  This actor will listen and publish anything received
//  on the IPC_GROUP group


static zactor_t *actor = NULL;
static zyre_t *node = 0;
static zpoller_t *poller = 0;

static char lastPeer[128];

static bool volatile terminated = false;

static void ipc_actor (zsock_t *pipe, void *args)
{
    //  Do some initialization
    char* name = (char*) args;
    
    node = zyre_new (name);
    assert(node);
    if (!node)  //  Could not create new node
    {
        sim_printf("Couldn't create IPC node ... %s\n", name);
        return;
    }
    
    ipc_printf("Starting IPC node %s ...", name);
    
    if (ipc_verbose)
        zyre_set_verbose (node);  // uncomment to watch the events
    
    zyre_start (node);
    zyre_join (node, fnpGroup);
    zsock_signal (pipe, 0);     //  Signal "ready" to caller
    
    terminated = false;
    
    poller = zpoller_new (pipe, zyre_socket (node), NULL);
    assert(poller);
    
    ipc_printf(" done\n");
    
    while (!terminated)
    {
        void *which = zpoller_wait (poller, -1); // no timeout
        if (!terminated && which == pipe)
        {
            zmsg_t *msg = zmsg_recv (which);
            if (!msg)
                break;              //  Interrupted
            char *command = zmsg_popstr (msg);
            if (streq (command, "$TERM"))
            {
                terminated = true;
            }
            else
                if (streq (command, "SHOUT"))
                {
                    char *string = zmsg_popstr (msg);
                    zyre_shouts (node, fnpGroup, "%s", string);
                }
                else if (streq (command, "WHISPER"))
                {
                    char *string = zmsg_popstr (msg);
                    zyre_whispers (node, lastPeer, "%s", string);
                } else
                {
                    sim_debug (DBG_IPCVERBOSE, &ipc_dev,"ipc_actor(): E: invalid message to actor");
                    //assert (false);
                }
            free (command);
            zmsg_destroy (&msg);
        }
        else if (!terminated && which == zyre_socket (node))
        {
            zmsg_t *msg = zmsg_recv (which);
            char *event = zmsg_popstr (msg);
            char *peer = zmsg_popstr (msg);
            char *name = zmsg_popstr (msg);
            char *group = zmsg_popstr (msg);
            char *message = zmsg_popstr (msg);  // change to zmsg_popmsg (zmsg_t *self) later
            
            if (streq (event, "ENTER"))
            {
                ipc(ipcEnter, name, peer, message, 0);
            }
            else if (streq (event, "EXIT"))
            {
                ipc(ipcExit, name, peer, 0, 0);
            }
            if (streq (event, "SHOUT"))
            {
                ipc(ipcShoutRx, name, peer, message, 0);
            }
            if (streq (event, "WHISPER"))
            {
                ipc(ipcWhisperRx, name, peer, group, 0);
            }
            
            if (ipc_verbose)
            {
                ipc_printf("Message from node\n");
                ipc_printf("event: %s peer: %s  name: %s group: %s message: %s\n", event, peer, name, group, message);
            }
            free (event);
            free (peer);
            free (name);
            free (group);
            free (message);
            zmsg_destroy (&msg);
        }
    }
}

void killIPC()
{
    terminated = true;                           // tell IPC actor to kill itself
    
    if (actor)
        zactor_destroy (&actor);
    
    if (poller)
        zpoller_destroy (&poller);
    
    // Notify peers that this peer is shutting down. Provide
    // a brief interval to ensure message is emitted.
    if (node)
    {
        zyre_stop(node);
        zclock_sleep(100);
        zyre_destroy (&node);
    }
    
    actor = 0;
    node = 0;
    poller = 0;
}

bool isIPCRunning()
{
    return actor ? true : false;
}
bool isIPCEnabled()
{
    return !(ipc_dev.flags & DEV_DIS); // IPC disabled?
}

t_stat ipc (ipc_funcs fn, char *arg1, char *arg2, char *arg3, int32 arg4)
{
    //    if (!checkIPC())
    //        return SCPE_UDIS;
    
    switch (fn)
    {
        case ipcStart:
            if (actor)
            {
                sim_printf("Info: IPC already started.\n");
                return SCPE_OK;
            }
            //    killIPC();
            
            actor = zactor_new (ipc_actor, arg1);
            assert (actor);
            deletePeers();          // remove all peers
            
            break;
            
        case ipcStop:
            if (!actor)
            {
                sim_printf("Info: IPC not started.\n");
                return SCPE_OK;
            }
            
            ipc_printf("Stopping IPC ... ");
            killIPC();
            ipc_printf("deleted %d peers ... ", deletePeers());
            ipc_printf("done\n");
            break;
            
        case ipcRestart:
            if (actor)
                ipc(ipcStop, 0, 0, 0, 0);           // kill IPC
            deletePeers();
            if (!actor)
                ipc(ipcStart, fnpName, 0, 0, 0);    // start IPC
            break;
            
        case ipcEnter:
            //sim_debug (DBG_VERBOSE, &ipc_dev, "%s/%s has entered " STR(IPC_GROUP) "\n", arg1, arg2);
            //ipc_printf("(ENTER)      %s/%s has entered %s from %s\n", arg1, arg2, fnpGroup, arg3);
            
            savePeer(arg1, arg2);
            break;
            
        case ipcExit:
            //sim_debug (DBG_VERBOSE, &ipc_dev, "%s has left " STR(IPC_GROUP) "\n", arg1);
            //ipc_printf("(EXIT)       %s/%s has left %s\n", arg1, arg2, fnpGroup);
            removePeer(arg1);
            break;
            
        case ipcShoutTx:
            zstr_sendx (actor, "SHOUT", arg1, NULL);
            break;
            
        case ipcWhisperTx:
            if (arg1)   // WHISPER msg  (use last whisper peer)
            {
                IPC_Peer *p1 = findPeer(stripquotes(trim(arg1)));    // fook for peer named 'name'
                if (p1)
                    strcpy(lastPeer, p1->peerID);   // save Peer id
                else
                {
                    bool bGuid = isGUID(stripquotes(trim(arg1)));
                    if (!bGuid)
                    {
                        sim_printf("Error: '%s' is not a GUID or a known peer\n", arg1);
                        return SCPE_ARG;
                    }
                    strcpy(lastPeer, arg1);     // Peer name not found, assume arg1 is a peerID then...
                }
            }
            
            zstr_sendx (actor, "WHISPER", arg2, NULL);
            break;
            
        case ipcShoutRx:    // when we receive a broadcast message
            //sim_debug (DBG_VERBOSE, &ipc_dev, "%s: %s\n", arg1, arg2);
            //ipc_printf("(RX SHOUT)   %s/%s:<%s>\n", arg1, arg2, arg3);
            
            /*
             * Insert code here to process any received broadcast message
             * (arg1 = nodename, arg2 = ID, arg3 = message)
             */
            break;
        case ipcWhisperRx:  // when we receive a peer-to-peer (whisper) message
            //sim_debug (DBG_VERBOSE, &ipc_dev, "%s: %s\n", arg1, arg2);
            //ipc_printf("(RX WHISPER) %s/%s:<%s>\n", arg1, arg2, arg3);
            
            /*
             * Insert code here to process any received peer-to-peer message
             * (arg1 = nodename, arg2 = ID, arg3 = message)
             */
            
            if (fnp_command(arg1, arg2, arg3) != SCPE_OK)
            {
                // TODO: process possible error
            }
            
            break;
            
        case ipcTest:
            zyre_test(true);
            break;
            
        default:
            break;
    }
    return SCPE_OK;
}

void tellCPU (int cpuUnitNum, char * msg)
  {
    //ipc_printf ("tellCPU (%s)\n", msg);

#define RETRIES 2048
    int retry;
    for (retry = 0; retry < RETRIES; retry ++)
      {
        if (findPeer ("cpu-a"))
          break;
        usleep (1000);
      }
    if (retry >= RETRIES)
      ipc_printf ("CPU not found....\n");

    t_stat stat = ipc (ipcWhisperTx, "cpu-a", msg, NULL, 0);
    if (stat != SCPE_OK)
      ipc_printf ("tellCPU returned %d\n", stat);
    return;
  }

/*
 * set IPC commands
 */

static
t_stat ipc_test (FILE *st, UNIT *uptr, int32 val, void *desc)
{
    sim_printf("Test: IPC not enabled.\n");
    
    return ipc(ipcTest, 0, 0, 0, 0);
}
static t_stat ipc_start(UNIT *uptr, int32 val, char *cval, void *desc)
{
    return ipc(ipcStart, fnpName, 0, 0, 0);
}
static t_stat ipc_stop(UNIT *uptr, int32 val, char *cval, void *desc)
{
    return ipc(ipcStop, 0, 0, 0, 0);
}
static t_stat ipc_show_node (FILE *st, UNIT *uptr, int32 val, void *desc)
{
    fprintf(st, "%s", fnpName);
    return SCPE_OK;
}
static t_stat ipc_set_node (UNIT *uptr, int32 val, char *cval, void *desc)
{
    if (cval == NULL || *cval == 0)
    {
        strcpy(fnpName, IPC_NODE);
#ifdef VM_FNP
        set_prompt (0, "sim>");     // reset prompt to default
#endif
    }
    else
    {
        stripquotes(cval);
        
        if (!startsWith(cval, "fnp"))
            sim_printf("WARNING: Node name <%s> does not begin with 'fnp'\n", cval);
        
        strcpy(fnpName, cval);
#ifdef VM_FNP
        char temp[132];
        sprintf(temp, "%s>", fnpName);
        set_prompt(0, temp);
#endif
        
    }
    
    // if IPC is already running, resrart it with new node
    if (actor)
    {
        ipc(ipcStop, 0, 0, 0, 0);    // ki;; IPC
        ipc(ipcStart, fnpName, 0, 0, 0);
    }
    
    return SCPE_OK;
}
static t_stat ipc_show_grp (FILE *st, UNIT *uptr, int32 val, void *desc)
{
    sim_printf("%s", fnpGroup);
    return SCPE_OK;
}
static t_stat ipc_set_grp (UNIT *uptr, int32 val, char *cval, void *desc)
{
    stripquotes(cval);
    strcpy(fnpGroup, cval);
    
    if (actor)
    {
        ipc(ipcStop, 0, 0, 0, 0);    // kill IPC
        ipc(ipcStart, fnpName, 0, 0, 0);
    }
    return SCPE_OK;
}
static t_stat ipc_restart (UNIT *uptr, int32 val, char *cval, void *desc)
{
    if (actor)
        ipc(ipcStop, 0, 0, 0, 0);    // kill IPC
    ipc(ipcStart, fnpName, 0, 0, 0);
    
    return SCPE_OK;
}

static t_stat ipc_show_peers(FILE *st, UNIT *uptr, int32 val, void *desc)
{
    int n = HASH_COUNT(Peers);
    if (n == 0)
    {
        sim_printf("There are no peers to list.\n");
        return SCPE_OK;
    }
    
    IPC_Peer *s, *tmp;
    
    n = 1;
    HASH_ITER(hh, Peers, s, tmp)
    {
        sim_printf("Peer %d:\n", n++);
        sim_printf("    Name: %s\n", s->peerName);
        sim_printf("    ID: %s\n", s->peerID);
    }
    return SCPE_OK;
}
static t_stat ipc_remove_peers (UNIT *uptr, int32 val, char *c, void *desc)
{
    int n = deletePeers();
    sim_printf("deleted %d peers\n", n);
    return SCPE_OK;
}

/*
 * custom commands ...
 */
t_stat ipc_shout (int32 arg, char *buf)
{
    if (!isIPCEnabled())
    {
        sim_printf("Shout: IPC not enabled.\n");
        return SCPE_OK;
    }
    if (!isIPCRunning())
    {
        sim_printf("Shout: IPC not running.\n");
        return SCPE_OK;
    }
    return ipc(ipcShoutTx, stripquotes(trim(buf)), 0, 0, 0);
}

t_stat ipc_whisper (int32 arg, char *buf)
{
    if (!isIPCEnabled())
    {
        sim_printf("Whisper: IPC not enabled.\n");
        return SCPE_OK;
    }
    if (!isIPCRunning())
    {
        sim_printf("Whisper: IPC not running.\n");
        return SCPE_OK;
    }
    
    // format peer, message
    char *a1 = Strtok(buf, ",");
    char *a2 = Strtok(0, ",");
    
    char *peer;
    char *msg;
    
    if (a1 && a2)           // peer, message
    {
        peer = stripquotes(trim(a1));
        msg = stripquotes(trim(a2));
    } else if (a1 && !a2)   // message
    {
        peer = NULL;
        msg = stripquotes(trim(a1));
    } else
    {
        sim_printf("Usage: whisper [peer,] message\n");
        return SCPE_ARG;
    }
    
    return ipc(ipcWhisperTx, peer, msg, 0, 0);
}

/*
 * IPC peer mappings ...
 */

#define FREE(t)     \
    if (t) { free(t); t = 0; }

// enter peer into list (if possible)
IPC_Peer *savePeer(char *name, char *id)
{
    IPC_Peer *s = findPeer(name);
    if (s == NULL)
    {
        IPC_Peer *s = (IPC_Peer*)calloc(1, sizeof(IPC_Peer));
        assert(s);
        
        s->peerName = strdup(name);
        s->peerID = strdup(id);
        
        HASH_ADD_KEYPTR( hh, Peers, s->peerName, strlen(s->peerName), s );
        
        sim_printf("saved peer %s/%s\n", name, id);
    }
    
    return s;
}

IPC_Peer *findPeer(char *name)
{
    IPC_Peer *s;
    
    HASH_FIND_STR(Peers, name, s);
    return s;
}

bool removePeer(char *name)
{
    IPC_Peer *s = findPeer(name);
    if (!s)
        return false;   // peer not found
    
    HASH_DEL(Peers, s);  /* user: pointer to deletee */
    FREE(s->peerID);
    FREE(s->peerName);
    FREE(s);
    
    sim_printf("removed peer %s\n", name);
    
    return true;
}

int deletePeers()
{
    int n = 0;
    IPC_Peer *current, *tmp;
    
    HASH_ITER(hh, Peers, current, tmp)
    {
        HASH_DEL(Peers, current);  /* delete it (current advances to next) */
        FREE(current->peerID);
        FREE(current->peerName);
        FREE(current);            /* free it */
        
        n += 1;
    }
    Peers = 0;
    
    strcpy(lastPeer, "");   // no last peer
    
    return n;
}
