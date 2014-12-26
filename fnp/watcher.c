//
//  main.c
//  watcher
//
//  Created by Harry Reed on 12/14/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>


#define VERSION "1.0"       // version 1.0

#define IPC_GROUP           "MulticsIPC"

int vflag;  // verbose mode

int watcher (int argc, char *argv[]);

void usage()
{
    fprintf(stderr, "Usage: watcher [-v] nodeName\n");
}

int main(int argc, char * argv[])
{
    printf("Hello, World!\n");
    printf("This is watcher v" VERSION " * * *\n");
 
    char ch;
    
    vflag = 0;
    while ((ch = getopt(argc, argv, "v")) != -1)
    {
        switch (ch)
        {
            case 'v':
                vflag = 1;
                break;
            case '?':
            default:
                usage();
                return 1;
        }
    }
    argc -= optind;
    argv += optind;

    watcher(argc, argv);
    
    return 0;
}

// Derived from .....
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


#include "zyre.h"

//  This actor will listen and publish anything received
//  on the IPC_GROUP group

char last_peer[128] = "";

static void
ipc_actor (zsock_t *pipe, void *args)
{
    //  Do some initialization
    char*   name = (char*) args;
    zyre_t *node = zyre_new (name);
    if (!node)
        return;                 //  Could not create new node
    if (vflag)
        zyre_set_verbose (node);  // use -v to watch the events

    zyre_start (node);
    zyre_join (node, IPC_GROUP);
    zsock_signal (pipe, 0);     //  Signal "ready" to caller
    
    bool terminated = false;
    zpoller_t *poller = zpoller_new (pipe, zyre_socket (node), NULL);
    while (!terminated) {
        void *which = zpoller_wait (poller, -1); // no timeout
        if (which == pipe){
            zmsg_t *msg = zmsg_recv (which);
            if (!msg)
                break;              //  Interrupted
            char *command = zmsg_popstr (msg);
            if (streq (command, "$TERM")) {
                terminated = true;
            }
            else
                if (streq (command, "SHOUT")) {
                    char *string = zmsg_popstr (msg);
                    zyre_shouts (node, IPC_GROUP, "%s", string);
                } else if (streq (command, "WHISPER")) {
                    char *string = zmsg_popstr (msg);
                    zyre_whispers (node, last_peer, "%s", string);
                }
                else {
                    puts ("E: invalid message to actor");
                    assert (false);
                }
            free (command);
            zmsg_destroy (&msg);
        }
        else if (which == zyre_socket (node)) {
            zmsg_t *msg = zmsg_recv (which);
            char *event = zmsg_popstr (msg);
            char *peer = zmsg_popstr (msg); sprintf(last_peer, "%s", peer);
            char *name = zmsg_popstr (msg);
            char *group = zmsg_popstr (msg);
            char *message = zmsg_popstr (msg);
            
            if (streq (event, "ENTER")) {
                printf ("%s has joined the chat %s\n", name, peer);
            }
            else if (streq (event, "EXIT")) {
                printf ("%s has left the chat %s\n", name, peer);
            }
            if (streq (event, "SHOUT")) {
                printf ("S %s:%s:%s\n", name, peer, message);
            } else if (streq (event, "WHISPER")) {
                printf ("W %s:%s:%s\n", name, peer, group); // group here := message
            }
            
            //printf ("Message from node\n");
            //printf ("event: %s peer: %s  name: %s\n  group: %s message: %s\n", event, peer, name, group, message);
            
            free (event);
            free (peer);
            free (name);
            free (group);
            free (message);
            zmsg_destroy (&msg);
        }
    }
    zpoller_destroy (&poller);
    
    // Notify peers that this peer is shutting down. Provide
    // a brief interval to ensure message is emitted.
    zyre_stop(node);
    zclock_sleep(100);
    
    zyre_destroy (&node);
}


int
watcher (int argc, char *argv[])
{
    if (argc < 1)
    {
        usage();
        exit (0);
    }

    zactor_t *actor = zactor_new (ipc_actor, argv[0]);
    assert (actor);
    
    while (!zsys_interrupted) {
        char message [1024];
        if (!fgets( message, 1024, stdin))
            break;
        message[strlen(message)-1] = 0; // drop the trailing linefeed
        if (message[0] == '!')
            zstr_sendx (actor, "SHOUT", message+1, NULL);
        else
            zstr_sendx (actor, "WHISPER", message, NULL);
    }
    
    zactor_destroy (&actor);
    
    return 0;
}



