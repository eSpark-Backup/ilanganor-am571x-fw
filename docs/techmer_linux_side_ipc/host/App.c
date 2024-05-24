#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include <ti/cmem.h>
#include <ti/ipc/Std.h>
#include <ti/ipc/MessageQ.h>
#include "HeapMem.h"
#include "SharedRegion.h"
#include "MemoryDefs.h"

#include "../shared/AppCommon.h"
#include "App.h"

#define BIG_DATA_POOL_SIZE 20000 // 20000 uint32_t values * 4 bytes each
#define ROUNDUP(size, align) \
    (UInt32)(((UInt32)(size) + ((UInt32)(align) - 1)) & ~((UInt32)(align) - 1))

typedef struct {
    MessageQ_Handle         hostQue;
    MessageQ_QueueId        slaveQue;
    UInt16                  heapId;
    UInt32                  msgSize;
} App_Module;

static App_Module Module;

Int App_create(UInt16 remoteProcId)
{
    Int status = 0;
    MessageQ_Params msgqParams;
    char msgqName[32];

    printf("--> App_create:\n");

    Module.hostQue = NULL;
    Module.slaveQue = MessageQ_INVALIDMESSAGEQ;
    Module.heapId = App_MsgHeapId;
    Module.msgSize = sizeof(App_Msg);

    MessageQ_Params_init(&msgqParams);
    Module.hostQue = MessageQ_create(App_HostMsgQueName, &msgqParams);

    if (Module.hostQue == NULL) {
        printf("App_create: Failed creating MessageQ\n");
        status = -1;
        goto leave;
    }

    sprintf(msgqName, App_SlaveMsgQueName, MultiProc_getName(remoteProcId));

    do {
        status = MessageQ_open(msgqName, &Module.slaveQue);
        sleep(1);
    } while (status == MessageQ_E_NOTFOUND);

    if (status < 0) {
        printf("App_create: Failed opening MessageQ\n");
        goto leave;
    }

    printf("App_create: Host is ready\n");

leave:
    printf("<-- App_create:\n");
    return status;
}

Int App_delete(Void)
{
    Int status;

    printf("--> App_delete:\n");

    status = MessageQ_close(&Module.slaveQue);
    if (status < 0) {
        goto leave;
    }

    status = MessageQ_delete(&Module.hostQue);
    if (status < 0) {
        goto leave;
    }

leave:
    printf("<-- App_delete:\n");
    return status;
}

void printFirstAndLastTen(const UInt32 *data, int size, const char *label) {
    printf("%s: First 10 elements: ", label);
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    printf("%s: Last 10 elements: ", label);
    for (int i = size - 10; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

Int App_exec(Int debugMode)
{
    Int status;
    App_Msg *msg;
    UInt32 *bigDataLocalPtr;
    UInt16 regionId = 1;
    SharedRegion_Entry *pSrEntry;
    void *sharedRegionAllocPtr = NULL;
    CMEM_AllocParams cmemAttrs;
    HeapMem_Handle sr1Heap;
    UInt32 errorCount = 0;
    bigDataLocalDesc_t bigDataLocalDesc;
    SharedRegion_Config sharedRegionConfig;
    SharedRegion_Entry srEntry;
    HeapMem_Config HeapMemConfig;
    HeapMem_Params heapMem_params;
    Memory_Stats stats;
    HeapMem_ExtendedStats extStats;

    printf("--> App_exec:\n");

    status = CMEM_init();
    if (status < 0) {
        printf("CMEM_init failed\n");
        goto leave;
    }
    printf("CMEM_init success\n");

    int pool_id = CMEM_getPool(BIG_DATA_POOL_SIZE);
    if (pool_id < 0) {
        printf("CMEM_getPool failed\n");
        goto leave;
    }
    printf("CMEM_getPool success\n");

    cmemAttrs.type = CMEM_HEAP;
    cmemAttrs.flags = CMEM_CACHED;
    cmemAttrs.alignment = 0;
    sharedRegionAllocPtr = CMEM_allocPool(pool_id, &cmemAttrs);
    if (sharedRegionAllocPtr == NULL) {
        printf("CMEM_allocPool failed\n");
        goto leave;
    }
    printf("CMEM_allocPool success: Allocated buffer %p\n", sharedRegionAllocPtr);

    SharedRegion_getConfig(&sharedRegionConfig);
    status = SharedRegion_setup(&sharedRegionConfig);
    if (status < 0) {
        printf("SharedRegion_setup failed\n");
        goto leave;
    }
    printf("SharedRegion_setup success\n");

    srEntry.base = sharedRegionAllocPtr;
    srEntry.len = BIG_DATA_POOL_SIZE;
    srEntry.ownerProcId = MultiProc_self();
    srEntry.isValid = TRUE;
    srEntry.cacheEnable = TRUE;
    srEntry.createHeap = FALSE;
    srEntry.cacheLineSize = 128;
    srEntry.name = "SR1";

    status = SharedRegion_setEntry(regionId, &srEntry);
    pSrEntry = SharedRegion_getEntryPtr(regionId);
    printf("App_exec: SR_1, base 0x%x, len=%x\n", (UInt32)pSrEntry->base, pSrEntry->len);

    HeapMem_getConfig(&HeapMemConfig);
    status = HeapMem_setup(&HeapMemConfig);
    if (status < 0) {
        printf("HeapMem_setup failed\n");
        goto leave;
    }
    printf("HeapMem_setup success\n");

    HeapMem_Params_init(&heapMem_params);
    heapMem_params.name = "sr1HeapMem";
    heapMem_params.sharedAddr = pSrEntry->base;
    heapMem_params.sharedBufSize = ROUNDUP(pSrEntry->len, pSrEntry->cacheLineSize);
    heapMem_params.gate = NULL;
    sr1Heap = HeapMem_create(&heapMem_params);
    if (!sr1Heap) {
        printf("sr1Heap creation failed\n");
        status = -1;
        goto leave;
    }
    printf("HeapMem_create success\n");
    HeapMem_getStats((HeapMem_Handle)sr1Heap, &stats);
    printf("App_exec: SR_1 heap, totalSize=%d,totalFreeSize=%d,largestFreeSize=%d\n", stats.totalSize, stats.totalFreeSize, stats.largestFreeSize);
    HeapMem_getExtendedStats((HeapMem_Handle)sr1Heap, &extStats);
    printf("App_exec: SR_1 heap, buf=0x%p,size=%d\n", extStats.buf, extStats.size);

    printf("App_exec: Sending message\n");

    msg = (App_Msg *)MessageQ_alloc(Module.heapId, Module.msgSize);
    if (msg == NULL) {
        status = -1;
        goto leave;
    }

    MessageQ_setReplyQueue(Module.hostQue, (MessageQ_Msg)msg);

    msg->cmd = App_CMD_BIGDATA;
    msg->id = 1;

    bigDataLocalPtr = (UInt32 *)(HeapMem_alloc(sr1Heap, BIG_DATA_POOL_SIZE, 128));
    if (!bigDataLocalPtr) {
        status = -1;
        printf("HeapMem_alloc failed\n");
        goto leave;
    }

    for (int j = 0; j < BIG_DATA_POOL_SIZE / sizeof(uint32_t); j++) {
        bigDataLocalPtr[j] = j + 1;
    }

    if (debugMode) {
        printf("Debug: Sending a message with %d uint32_t values (total %d bytes).\n", BIG_DATA_POOL_SIZE / sizeof(uint32_t), BIG_DATA_POOL_SIZE);
        printFirstAndLastTen(bigDataLocalPtr, BIG_DATA_POOL_SIZE / sizeof(uint32_t), "Sending Data");
    }

    bigDataLocalDesc.localPtr = (Ptr)bigDataLocalPtr;
    bigDataLocalDesc.size = BIG_DATA_POOL_SIZE;

    int retVal = bigDataXlatetoGlobalAndSync(regionId, &bigDataLocalDesc, &msg->u.bigDataSharedDesc);
    if (retVal) {
        status = -1;
        printf("bigDataXlatetoGlobalAndSync failed\n");
        goto leave;
    }
    msg->regionId = regionId;

    MessageQ_put(Module.slaveQue, (MessageQ_Msg)msg);

    printf("App_exec: Waiting for response message\n");

    status = MessageQ_get(Module.hostQue, (MessageQ_Msg *)&msg, MessageQ_FOREVER);
    if (status < 0) {
        goto leave;
    }

    if (msg->cmd == App_CMD_BIGDATA) {
        retVal = bigDataXlatetoLocalAndSync(msg->regionId, &msg->u.bigDataSharedDesc, &bigDataLocalDesc);
        if (retVal) {
            status = -1;
            goto leave;
        }
        bigDataLocalPtr = (UInt32 *)bigDataLocalDesc.localPtr;

        if (debugMode) {
            printf("Debug: Received a message with %d uint32_t values (total %d bytes).\n", bigDataLocalDesc.size / sizeof(uint32_t), bigDataLocalDesc.size);
            printFirstAndLastTen(bigDataLocalPtr, bigDataLocalDesc.size / sizeof(uint32_t), "Received Data");
        }

        for (int j = 0; j < bigDataLocalDesc.size / sizeof(uint32_t); j++) {
            if (bigDataLocalPtr[j] != (j + 1)) {
                errorCount++;
            }
        }

        HeapMem_free(sr1Heap, bigDataLocalPtr, bigDataLocalDesc.size);
    }

    printf("App_exec: message received\n");

    MessageQ_free((MessageQ_Msg)msg);

leave:
    if (sr1Heap) {
        HeapMem_delete(&sr1Heap);
    }
    if (sharedRegionAllocPtr) {
        CMEM_free(sharedRegionAllocPtr, &cmemAttrs);
    }
    if (errorCount) {
        printf("App_exec: Error Count %d\n", errorCount);
        status = -1;
    } else {
        printf("App_exec: Data check clean\n");
    }

    printf("<-- App_exec: %d\n", status);
    return status;
}

