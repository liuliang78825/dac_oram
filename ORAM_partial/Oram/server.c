//
//  server.c
//  Oram
//
//  Created by Leo on 4/8/19.
//  Copyright © 2019 Liang. All rights reserved.
//

#include "server.h"

#define CONNMAX 1000
static int listenfd, clients[CONNMAX];
static void error(char *);
static void startServer(const char *);
static void respond(int);
typedef struct { char *name, *value; } header_t;
static header_t reqhdr[17] = { {"\0", "\0"} };
static int clientfd;
static char *buf;

    
