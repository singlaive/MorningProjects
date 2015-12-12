

#ifndef COMMON_QUEUE_H

/* In order to make use of fifo amoung processes, we must have the assumuption that only one queue shall be supported withe the wellknown fifo name. 
 * For now this wellknown fifo name is "/tmp/ipc_queue" */
typedef enum
{
    QUEUE_CONFIGURE_invalid,
    QUEUE_CONFIGURE_ITC,
    QUEUE_CONFIGURE_IPC,
    QUEUE_CONFIGURE_lastvalue
}QUEUE_CONFIGURE;

SYSTEM_STATUS COMMON_QUEUE_createQueue(QUEUE_CONFIGURE config, COMMON_HANDLE *queue_handle);
SYSTEM_STATUS COMMON_QUEUE_postMsg(COMMON_HANDLE queue_handle,
                                   uint32_t msg_id,
                                   uint32_t payload_size,
                                   void *payload);
SYSTEM_STATUS COMMON_QUEUE_readMsg(COMMON_HANDLE queue_handle,
                                   uin32_t *msg_id,
                                   uint32_t *payload_size,
                                   void *payload);
SYSTEM_STATUS COMMON_QUEUE_destroyQueue(COMMON_HANDLE queue_handle);

#endif
