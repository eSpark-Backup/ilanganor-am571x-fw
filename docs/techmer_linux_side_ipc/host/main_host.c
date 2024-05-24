/* cstdlib header files */
#include <stdio.h>
#include <stdlib.h>

/* package header files */
#include <ti/ipc/Std.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/transports/TransportRpmsg.h>

#include <ti/ipc/MultiProc.h>

/* local header files */
#include "App.h"

/* private functions */
static Int Main_main(Void);
static Int Main_parseArgs(Int argc, Char *argv[]);

#define Main_USAGE "\
Usage:\n\
    app_host [options] procName\n\
\n\
Arguments:\n\
    procName      : the name of the remote processor\n\
\n\
Options:\n\
    h   : print this help message\n\
    l   : list the available remote names\n\
    d   : enable debug mode\n\
\n\
Examples:\n\
    app_host DSP\n\
    app_host -l\n\
    app_host -h\n\
    app_host DSP -d\n\
\n"

/* private data */
static String Main_remoteProcName = NULL;
static Int debugMode = 0;

/*
 *  ======== main ========
 */
Int main(Int argc, Char* argv[])
{
    Int status;

    printf("--> main:\n");

    /* configure the transport factory */
    Ipc_transportConfig(&TransportRpmsg_Factory);

    /* parse command line */
    status = Main_parseArgs(argc, argv);

    if (status < 0) {
        goto leave;
    }

    /* Ipc initialization */
    status = Ipc_start();

    if (status >= 0) {
        /* application create, exec, delete */
        status = Main_main();

        /* Ipc finalization */
        Ipc_stop();
    }
    else {
        printf("Ipc_start failed: status = %d\n", status);
        goto leave;
    }

leave:
    printf("<-- main:\n");
    status = (status >= 0 ? 0 : status);

    return (status);
}


/*
 *  ======== Main_main ========
 */
Int Main_main(Void)
{
    UInt16 remoteProcId;
    Int status = 0;

    printf("--> Main_main:\n");

    remoteProcId = MultiProc_getId(Main_remoteProcName);

    /* application create phase */
    status = App_create(remoteProcId);

    if (status < 0) {
        goto leave;
    }

    /* application execute phase */
    status = App_exec(debugMode);

    if (status < 0) {
        goto leave;
    }

    /* application delete phase */
    status = App_delete();

    if (status < 0) {
        goto leave;
    }

leave:
    printf("<-- Main_main:\n");

    status = (status >= 0 ? 0 : status);
    return (status);
}


/*
 *  ======== Main_parseArgs ========
 */
Int Main_parseArgs(Int argc, Char *argv[])
{
    Int opt;
    Int status = 0;

    /* parse the command line options */
    for (opt = 1; opt < argc; opt++) {
        if (argv[opt][0] == '-') {
            switch (argv[opt][1]) {
                case 'h':
                    printf("%s", Main_USAGE);
                    exit(0);
                    break;

                case 'l':
                    printf("Processor List\n");
                    status = Ipc_start();
                    if (status >= 0) {
                        UInt16 numProcs = MultiProc_getNumProcessors();
                        for (UInt16 i = 0; i < numProcs; i++) {
                            String name = MultiProc_getName(i);
                            printf("    procId=%d, procName=%s\n", i, name);
                        }
                        Ipc_stop();
                    }
                    else {
                        printf("Error: %s, line %d: Ipc_start failed\n", __FILE__, __LINE__);
                        goto leave;
                    }
                    exit(0);
                    break;

                case 'd':
                    debugMode = 1;
                    break;

                default:
                    printf("Error: %s, line %d: invalid option, %c\n", __FILE__, __LINE__, argv[opt][1]);
                    printf("%s", Main_USAGE);
                    status = -1;
                    goto leave;
            }
        }
        else {
            if (Main_remoteProcName == NULL) {
                Main_remoteProcName = argv[opt];
            }
            else {
                printf("Error: %s, line %d: too many arguments\n", __FILE__, __LINE__);
                printf("%s", Main_USAGE);
                status = -1;
                goto leave;
            }
        }
    }

    /* validate command line arguments */
    if (Main_remoteProcName == NULL) {
        printf("Error: missing procName argument\n");
        printf("%s", Main_USAGE);
        status = -1;
    }

leave:
    return status;
}

