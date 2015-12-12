

typedef enum
{
    CNM_CLT_PROTOCOL_undefined,
	CNM_CLT_PROTOCOL_FIFO,
	CNM_CLT_PROTOCOL_UDP,
	CNM_CLT_PROTOCOL_TCP,
	CNM_CLT_PROTOCOL_HTTP,
	CNM_CLT_PROTOCOL_UPNP,
    CNM_CLT_PROTCOL_lastvalue
}CNM_CLT_PROTOCOL;

struct CNM_CLT_meta
{
    conn_prototol;
    union
    {
        fifo_fd;
    } conn;
}
typedef struct
{
    struct CNM_CLT_META conn_meta;
    void event_CB;
    CNM_CLT_PROTOCOL protocol; /* Once set cannot be changed. */
}CNM_CLT_CONTEXT;

/* Callback CNM client shall call once there is a response back from the server. */
typedef void (*CNM_CLT_event_CB)(COMMON_HANDLE client_id, RETRIEVAL_TYPE retrieval_type, void *pay_load);

/* CNM client sub funcs. */
typedef SYSTEM_STATUS (*CNM_CLT_setProtocol_func)(CNM_CLT_CONN_PROTOCOL conn_protocol);
typedef SYSTEM_STATUS (*CNM_CLT_registerEventCB)(COMMON_HADLE client_id, CNM_CLT_event_CB listener);
typedef SYSTEM_STATUS (*CNM_CLT_sendData)(CLIENT_REQUEST *p_request);

CNM_CLT_INTERFACE g_CNM_client_interfaces =
{
    CNM_CLT_setProtocol_func eLibrary_CNM_CLT_setProtocol,
	CNM_CLT_registerResponseCB_func eLibrary_CNM_CLT_registerEventCB,
  
};
