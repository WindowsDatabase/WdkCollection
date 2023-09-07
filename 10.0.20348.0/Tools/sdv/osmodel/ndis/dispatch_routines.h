/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*
    This file declares the driver entry points.
*/
#include <ndis.h>

extern DRIVER_INITIALIZE fun_DriverEntry;



#ifdef fun_MINIPORT_RESET
extern MINIPORT_RESET fun_MINIPORT_RESET;
#endif

#ifdef fun_MINIPORT_PAUSE
extern MINIPORT_PAUSE fun_MINIPORT_PAUSE;
#endif

#ifdef fun_MINIPORT_RESTART
extern MINIPORT_RESTART fun_MINIPORT_RESTART;
#endif

#ifdef fun_MINIPORT_INITIALIZE
extern MINIPORT_INITIALIZE fun_MINIPORT_INITIALIZE;
#endif

#ifdef fun_MINIPORT_HALT
extern MINIPORT_HALT fun_MINIPORT_HALT;
#endif

#ifdef fun_MINIPORT_OID_REQUEST
extern MINIPORT_OID_REQUEST fun_MINIPORT_OID_REQUEST;
#endif

#ifdef fun_MINIPORT_SHUTDOWN
extern MINIPORT_SHUTDOWN fun_MINIPORT_SHUTDOWN;
#endif

#ifdef fun_MINIPORT_DEVICE_PNP_EVENT_NOTIFY
extern MINIPORT_DEVICE_PNP_EVENT_NOTIFY fun_MINIPORT_DEVICE_PNP_EVENT_NOTIFY;
#endif

#ifdef fun_MINIPORT_CANCEL_SEND
extern MINIPORT_CANCEL_SEND fun_MINIPORT_CANCEL_SEND;
#endif

#ifdef fun_MINIPORT_CHECK_FOR_HANG
extern MINIPORT_CHECK_FOR_HANG fun_MINIPORT_CHECK_FOR_HANG;
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_1
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_1; /*multiple*/
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_2
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_2; /*multiple*/
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_3
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_3; /*multiple*/
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_4
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_4; /*multiple*/
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_5
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_5; /*multiple*/
#endif

#ifdef fun_MINIPORT_CANCEL_OID_REQUEST_6
extern MINIPORT_CANCEL_OID_REQUEST fun_MINIPORT_CANCEL_OID_REQUEST_6; /*multiple*/
#endif

#ifdef fun_MINIPORT_SEND_NET_BUFFER_LISTS
extern MINIPORT_SEND_NET_BUFFER_LISTS fun_MINIPORT_SEND_NET_BUFFER_LISTS;
#endif

#ifdef fun_MINIPORT_RETURN_NET_BUFFER_LISTS
extern MINIPORT_RETURN_NET_BUFFER_LISTS fun_MINIPORT_RETURN_NET_BUFFER_LISTS;
#endif

#ifdef fun_MINIPORT_UNLOAD
extern MINIPORT_UNLOAD fun_MINIPORT_UNLOAD;
#endif

#ifdef fun_MINIPORT_SET_OPTIONS
extern MINIPORT_SET_OPTIONS fun_MINIPORT_SET_OPTIONS;
#endif

#ifdef fun_MINIPORT_PROCESS_SG_LIST
extern MINIPORT_PROCESS_SG_LIST fun_MINIPORT_PROCESS_SG_LIST;
#endif

#ifdef fun_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE
extern MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE fun_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE;
#endif



#ifdef fun_MINIPORT_ADD_DEVICE
extern MINIPORT_ADD_DEVICE fun_MINIPORT_ADD_DEVICE;
#endif

#ifdef fun_MINIPORT_START_DEVICE
extern MINIPORT_START_DEVICE fun_MINIPORT_START_DEVICE;
#endif

#ifdef fun_MINIPORT_REMOVE_DEVICE
extern MINIPORT_REMOVE_DEVICE fun_MINIPORT_REMOVE_DEVICE;
#endif

#ifdef fun_MINIPORT_FILTER_RESOURCE_REQUIREMENTS
extern MINIPORT_FILTER_RESOURCE_REQUIREMENTS fun_MINIPORT_FILTER_RESOURCE_REQUIREMENTS;
#endif




#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_1
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_1; /*multiple*/
#endif

#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_2
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_2; /*multiple*/
#endif

#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_3
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_3; /*multiple*/
#endif

#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_4
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_4; /*multiple*/
#endif

#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_5
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_5; /*multiple*/
#endif

#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_6
extern NDIS_IO_WORKITEM_FUNCTION fun_NDIS_IO_WORKITEM_FUNCTION_6; /*multiple*/
#endif


#ifdef fun_MINIPORT_ISR_1
extern MINIPORT_ISR fun_MINIPORT_ISR_1; /*multiple*/
#endif

#ifdef fun_MINIPORT_ISR_2
extern MINIPORT_ISR fun_MINIPORT_ISR_2; /*multiple*/
#endif

#ifdef fun_MINIPORT_ISR_3
extern MINIPORT_ISR fun_MINIPORT_ISR_3; /*multiple*/
#endif

#ifdef fun_MINIPORT_ISR_4
extern MINIPORT_ISR fun_MINIPORT_ISR_4; /*multiple*/
#endif

#ifdef fun_MINIPORT_ISR_5
extern MINIPORT_ISR fun_MINIPORT_ISR_5; /*multiple*/
#endif

#ifdef fun_MINIPORT_ISR_6
extern MINIPORT_ISR fun_MINIPORT_ISR_6; /*multiple*/
#endif


#ifdef fun_MINIPORT_INTERRUPT_DPC_1
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_INTERRUPT_DPC_2
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_INTERRUPT_DPC_3
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_INTERRUPT_DPC_4
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_INTERRUPT_DPC_5
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_INTERRUPT_DPC_6
extern MINIPORT_INTERRUPT_DPC fun_MINIPORT_INTERRUPT_DPC_6;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_1
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_2
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_3
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_4
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_5
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_INTERRUPT_6
extern MINIPORT_ENABLE_INTERRUPT fun_MINIPORT_ENABLE_INTERRUPT_6;  /*multiple*/
#endif


#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_1
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_2
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_3
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_4
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_5
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_6
extern MINIPORT_SYNCHRONIZE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_INTERRUPT_6;  /*multiple*/
#endif



#ifdef fun_MINIPORT_DISABLE_INTERRUPT_1
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_INTERRUPT_2
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_INTERRUPT_3
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_INTERRUPT_4
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_INTERRUPT_5
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_INTERRUPT_6
extern MINIPORT_DISABLE_INTERRUPT fun_MINIPORT_DISABLE_INTERRUPT_6;  /*multiple*/
#endif


#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_1
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_1; /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_2
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_2; /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_3
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_3; /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_4
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_4; /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_5
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_5; /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_6
extern MINIPORT_MESSAGE_INTERRUPT fun_MINIPORT_MESSAGE_INTERRUPT_6; /*multiple*/
#endif


#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_1
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_2
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_3
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_4
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_5
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_6
extern MINIPORT_MESSAGE_INTERRUPT_DPC fun_MINIPORT_MESSAGE_INTERRUPT_DPC_6;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_1
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_2
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_3
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_4
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_5
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_6
extern MINIPORT_ENABLE_MESSAGE_INTERRUPT fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_6;  /*multiple*/
#endif


#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_1
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_2
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_3
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_4
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_5
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_6
extern MINIPORT_DISABLE_MESSAGE_INTERRUPT fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_6;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_1
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_1;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_2
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_2;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_3
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_3;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_4
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_4;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_5
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_5;  /*multiple*/
#endif

#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_6
extern MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_6;  /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_1
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_1;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_2
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_2;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_3
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_3;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_4
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_4;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_5
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_5;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_6
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_6;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_7
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_7;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_8
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_8;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_9
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_9;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_10
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_10;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_11
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_11;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_12
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_12;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_13
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_13;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_14
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_14;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_15
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_15;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_16
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_16;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_17
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_17;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_18
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_18;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_19
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_19;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_20
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_20;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_21
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_21;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_22
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_22;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_23
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_23;   /*multiple*/
#endif

#ifdef fun_NDIS_TIMER_FUNCTION_24
extern NDIS_TIMER_FUNCTION fun_NDIS_TIMER_FUNCTION_24;   /*multiple*/
#endif


#ifdef fun_MINIPORT_DIRECT_OID_REQUEST
extern MINIPORT_DIRECT_OID_REQUEST fun_MINIPORT_DIRECT_OID_REQUEST;
#endif

#ifdef fun_MINIPORT_CANCEL_DIRECT_OID_REQUEST
extern MINIPORT_CANCEL_DIRECT_OID_REQUEST fun_MINIPORT_CANCEL_DIRECT_OID_REQUEST;
#endif


#ifdef fun_MINIPORT_CO_ACTIVATE_VC
extern MINIPORT_CO_ACTIVATE_VC fun_MINIPORT_CO_ACTIVATE_VC;
#endif 

#ifdef fun_MINIPORT_CO_CREATE_VC
extern MINIPORT_CO_CREATE_VC fun_MINIPORT_CO_CREATE_VC;
#endif 

#ifdef fun_MINIPORT_CO_DEACTIVATE_VC
extern MINIPORT_CO_DEACTIVATE_VC fun_MINIPORT_CO_DEACTIVATE_VC;
#endif 

#ifdef fun_MINIPORT_CO_DELETE_VC
extern MINIPORT_CO_DELETE_VC fun_MINIPORT_CO_DELETE_VC;
#endif 

#ifdef fun_MINIPORT_CO_OID_REQUEST
extern MINIPORT_CO_OID_REQUEST fun_MINIPORT_CO_OID_REQUEST;
#endif 

#ifdef fun_MINIPORT_CO_SEND_NET_BUFFER_LISTS
extern MINIPORT_CO_SEND_NET_BUFFER_LISTS fun_MINIPORT_CO_SEND_NET_BUFFER_LISTS;
#endif 


#ifdef fun_FILTER_ATTACH
extern FILTER_ATTACH fun_FILTER_ATTACH;
#endif

#ifdef fun_FILTER_CANCEL_DIRECT_OID_REQUEST
extern FILTER_CANCEL_DIRECT_OID_REQUEST fun_FILTER_CANCEL_DIRECT_OID_REQUEST;
#endif

#ifdef fun_FILTER_CANCEL_OID_REQUEST
extern FILTER_CANCEL_OID_REQUEST fun_FILTER_CANCEL_OID_REQUEST;
#endif

#ifdef fun_FILTER_CANCEL_SEND_NET_BUFFER_LISTS
extern FILTER_CANCEL_SEND_NET_BUFFER_LISTS fun_FILTER_CANCEL_SEND_NET_BUFFER_LISTS;
#endif

#ifdef fun_FILTER_DETACH
extern FILTER_DETACH fun_FILTER_DETACH;
#endif

#ifdef fun_FILTER_DEVICE_PNP_EVENT_NOTIFY
extern FILTER_DEVICE_PNP_EVENT_NOTIFY fun_FILTER_DEVICE_PNP_EVENT_NOTIFY;
#endif

#ifdef fun_FILTER_DIRECT_OID_REQUEST
extern FILTER_DIRECT_OID_REQUEST fun_FILTER_DIRECT_OID_REQUEST;
#endif

#ifdef fun_FILTER_DIRECT_OID_REQUEST_COMPLETE
extern FILTER_DIRECT_OID_REQUEST_COMPLETE fun_FILTER_DIRECT_OID_REQUEST_COMPLETE;
#endif

#ifdef fun_FILTER_DRIVER_UNLOAD
extern DRIVER_UNLOAD fun_FILTER_DRIVER_UNLOAD;
#endif

#ifdef fun_FILTER_NET_PNP_EVENT
extern FILTER_NET_PNP_EVENT fun_FILTER_NET_PNP_EVENT;
#endif

#ifdef fun_FILTER_OID_REQUEST
extern FILTER_OID_REQUEST fun_FILTER_OID_REQUEST;
#endif

#ifdef fun_FILTER_OID_REQUEST_COMPLETE
extern FILTER_OID_REQUEST_COMPLETE fun_FILTER_OID_REQUEST_COMPLETE;
#endif

#ifdef fun_FILTER_PAUSE
extern FILTER_PAUSE fun_FILTER_PAUSE;
#endif

#ifdef fun_FILTER_RECEIVE_NET_BUFFER_LISTS
extern FILTER_RECEIVE_NET_BUFFER_LISTS fun_FILTER_RECEIVE_NET_BUFFER_LISTS;
#endif

#ifdef fun_FILTER_RESTART
extern FILTER_RESTART fun_FILTER_RESTART;
#endif

#ifdef fun_FILTER_RETURN_NET_BUFFER_LISTS
extern FILTER_RETURN_NET_BUFFER_LISTS fun_FILTER_RETURN_NET_BUFFER_LISTS;
#endif
   
#ifdef fun_FILTER_SEND_NET_BUFFER_LISTS
extern FILTER_SEND_NET_BUFFER_LISTS fun_FILTER_SEND_NET_BUFFER_LISTS;
#endif

#ifdef fun_FILTER_SEND_NET_BUFFER_LISTS_COMPLETE
extern FILTER_SEND_NET_BUFFER_LISTS_COMPLETE fun_FILTER_SEND_NET_BUFFER_LISTS_COMPLETE;
#endif

#ifdef fun_FILTER_SET_MODULE_OPTIONS
extern FILTER_SET_MODULE_OPTIONS fun_FILTER_SET_MODULE_OPTIONS;
#endif

#ifdef fun_FILTER_SET_OPTIONS
extern FILTER_SET_OPTIONS fun_FILTER_SET_OPTIONS;
#endif

#ifdef fun_FILTER_STATUS
extern FILTER_STATUS fun_FILTER_STATUS;
#endif


#ifdef fun_PROTOCOL_BIND_ADAPTER_EX
extern PROTOCOL_BIND_ADAPTER_EX fun_PROTOCOL_BIND_ADAPTER_EX;
#endif

#ifdef fun_PROTOCOL_CLOSE_ADAPTER_COMPLETE_EX
extern PROTOCOL_CLOSE_ADAPTER_COMPLETE_EX fun_PROTOCOL_CLOSE_ADAPTER_COMPLETE_EX;
#endif

#ifdef fun_PROTOCOL_DIRECT_OID_REQUEST_COMPLETE
extern PROTOCOL_DIRECT_OID_REQUEST_COMPLETE fun_PROTOCOL_DIRECT_OID_REQUEST_COMPLETE;
#endif

#ifdef fun_PROTOCOL_NET_PNP_EVENT
extern PROTOCOL_NET_PNP_EVENT fun_PROTOCOL_NET_PNP_EVENT;
#endif

#ifdef fun_PROTOCOL_OID_REQUEST_COMPLETE
extern PROTOCOL_OID_REQUEST_COMPLETE fun_PROTOCOL_OID_REQUEST_COMPLETE;
#endif

#ifdef fun_PROTOCOL_OPEN_ADAPTER_COMPLETE_EX
extern PROTOCOL_OPEN_ADAPTER_COMPLETE_EX fun_PROTOCOL_OPEN_ADAPTER_COMPLETE_EX;
#endif

#ifdef fun_PROTOCOL_RECEIVE_NET_BUFFER_LISTS
extern PROTOCOL_RECEIVE_NET_BUFFER_LISTS fun_PROTOCOL_RECEIVE_NET_BUFFER_LISTS;
#endif

#ifdef fun_PROTOCOL_SEND_NET_BUFFER_LISTS_COMPLETE
extern PROTOCOL_SEND_NET_BUFFER_LISTS_COMPLETE fun_PROTOCOL_SEND_NET_BUFFER_LISTS_COMPLETE;
#endif

#ifdef fun_PROTOCOL_SET_OPTIONS
extern PROTOCOL_SET_OPTIONS fun_PROTOCOL_SET_OPTIONS;
#endif

#ifdef fun_PROTOCOL_STATUS_EX
extern PROTOCOL_STATUS_EX fun_PROTOCOL_STATUS_EX;
#endif

#ifdef fun_PROTOCOL_UNBIND_ADAPTER_EX
extern PROTOCOL_UNBIND_ADAPTER_EX fun_PROTOCOL_UNBIND_ADAPTER_EX;
#endif

#ifdef fun_PROTOCOL_UNINSTALL
extern PROTOCOL_UNINSTALL fun_PROTOCOL_UNINSTALL;
#endif

#ifdef fun_PROTOCOL_UNLOAD
extern PROTOCOL_UNLOAD fun_PROTOCOL_UNLOAD;
#endif  


#ifdef fun_PROTOCOL_CL_ADD_PARTY_COMPLETE
extern PROTOCOL_CL_ADD_PARTY_COMPLETE fun_PROTOCOL_CL_ADD_PARTY_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_CALL_CONNECTED
extern PROTOCOL_CL_CALL_CONNECTED fun_PROTOCOL_CL_CALL_CONNECTED;
#endif 

#ifdef fun_PROTOCOL_CL_CLOSE_AF_COMPLETE
extern PROTOCOL_CL_CLOSE_AF_COMPLETE fun_PROTOCOL_CL_CLOSE_AF_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_CLOSE_CALL_COMPLETE
extern PROTOCOL_CL_CLOSE_CALL_COMPLETE fun_PROTOCOL_CL_CLOSE_CALL_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_DEREGISTER_SAP_COMPLETE
extern PROTOCOL_CL_DEREGISTER_SAP_COMPLETE fun_PROTOCOL_CL_DEREGISTER_SAP_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_DROP_PARTY_COMPLETE
extern PROTOCOL_CL_DROP_PARTY_COMPLETE fun_PROTOCOL_CL_DROP_PARTY_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_INCOMING_CALL
extern PROTOCOL_CL_INCOMING_CALL fun_PROTOCOL_CL_INCOMING_CALL;
#endif 

#ifdef fun_PROTOCOL_CL_INCOMING_CALL_QOS_CHANGE
extern PROTOCOL_CL_INCOMING_CALL_QOS_CHANGE fun_PROTOCOL_CL_INCOMING_CALL_QOS_CHANGE;
#endif 

#ifdef fun_PROTOCOL_CL_INCOMING_CLOSE_CALL
extern PROTOCOL_CL_INCOMING_CLOSE_CALL fun_PROTOCOL_CL_INCOMING_CLOSE_CALL;
#endif 

#ifdef fun_PROTOCOL_CL_INCOMING_DROP_PARTY
extern PROTOCOL_CL_INCOMING_DROP_PARTY fun_PROTOCOL_CL_INCOMING_DROP_PARTY;
#endif 

#ifdef fun_PROTOCOL_CL_MAKE_CALL_COMPLETE
extern PROTOCOL_CL_MAKE_CALL_COMPLETE fun_PROTOCOL_CL_MAKE_CALL_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_MODIFY_CALL_QOS_COMPLETE
extern PROTOCOL_CL_MODIFY_CALL_QOS_COMPLETE fun_PROTOCOL_CL_MODIFY_CALL_QOS_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_NOTIFY_CLOSE_AF
extern PROTOCOL_CL_NOTIFY_CLOSE_AF fun_PROTOCOL_CL_NOTIFY_CLOSE_AF;
#endif 

#ifdef fun_PROTOCOL_CL_OPEN_AF_COMPLETE
extern PROTOCOL_CL_OPEN_AF_COMPLETE fun_PROTOCOL_CL_OPEN_AF_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CL_OPEN_AF_COMPLETE_EX
extern PROTOCOL_CL_OPEN_AF_COMPLETE_EX fun_PROTOCOL_CL_OPEN_AF_COMPLETE_EX;
#endif 

#ifdef fun_PROTOCOL_CL_REGISTER_SAP_COMPLETE
extern PROTOCOL_CL_REGISTER_SAP_COMPLETE fun_PROTOCOL_CL_REGISTER_SAP_COMPLETE;
#endif 


#ifdef fun_PROTOCOL_CM_ACTIVATE_VC_COMPLETE
extern PROTOCOL_CM_ACTIVATE_VC_COMPLETE fun_PROTOCOL_CM_ACTIVATE_VC_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CM_ADD_PARTY
extern PROTOCOL_CM_ADD_PARTY fun_PROTOCOL_CM_ADD_PARTY;
#endif 

#ifdef fun_PROTOCOL_CM_CLOSE_AF
extern PROTOCOL_CM_CLOSE_AF fun_PROTOCOL_CM_CLOSE_AF;
#endif 

#ifdef fun_PROTOCOL_CM_CLOSE_CALL
extern PROTOCOL_CM_CLOSE_CALL fun_PROTOCOL_CM_CLOSE_CALL;
#endif 

#ifdef fun_PROTOCOL_CM_DEACTIVATE_VC_COMPLETE
extern PROTOCOL_CM_DEACTIVATE_VC_COMPLETE fun_PROTOCOL_CM_DEACTIVATE_VC_COMPLETE;
#endif 

#ifdef fun_PROTOCOL_CM_DEREGISTER_SAP
extern PROTOCOL_CM_DEREGISTER_SAP fun_PROTOCOL_CM_DEREGISTER_SAP;
#endif 

#ifdef fun_PROTOCOL_CM_DROP_PARTY
extern PROTOCOL_CM_DROP_PARTY fun_PROTOCOL_CM_DROP_PARTY;
#endif 
 
#ifdef fun_PROTOCOL_CM_INCOMING_CALL_COMPLETE
extern PROTOCOL_CM_INCOMING_CALL_COMPLETE fun_PROTOCOL_CM_INCOMING_CALL_COMPLETE;
#endif 
 
#ifdef fun_PROTOCOL_CM_MAKE_CALL
extern PROTOCOL_CM_MAKE_CALL fun_PROTOCOL_CM_MAKE_CALL;
#endif 
  
#ifdef fun_PROTOCOL_CM_MODIFY_QOS_CALL
extern PROTOCOL_CM_MODIFY_QOS_CALL fun_PROTOCOL_CM_MODIFY_QOS_CALL;
#endif 
  
#ifdef fun_PROTOCOL_CM_NOTIFY_CLOSE_AF_COMPLETE
extern PROTOCOL_CM_NOTIFY_CLOSE_AF_COMPLETE fun_PROTOCOL_CM_NOTIFY_CLOSE_AF_COMPLETE;
#endif 
 
#ifdef fun_PROTOCOL_CM_OPEN_AF
extern PROTOCOL_CM_OPEN_AF fun_PROTOCOL_CM_OPEN_AF;
#endif 
 
#ifdef fun_PROTOCOL_CM_REG_SAP
extern PROTOCOL_CM_REG_SAP fun_PROTOCOL_CM_REG_SAP;
#endif 
 

#ifdef fun_PROTOCOL_CO_AF_REGISTER_NOTIFY
extern PROTOCOL_CO_AF_REGISTER_NOTIFY fun_PROTOCOL_CO_AF_REGISTER_NOTIFY;
#endif 
 
#ifdef fun_PROTOCOL_CO_CREATE_VC
extern PROTOCOL_CO_CREATE_VC fun_PROTOCOL_CO_CREATE_VC;
#endif 
 
#ifdef fun_PROTOCOL_CO_DELETE_VC
extern PROTOCOL_CO_DELETE_VC fun_PROTOCOL_CO_DELETE_VC;
#endif 

#ifdef fun_PROTOCOL_CO_OID_REQUEST
extern PROTOCOL_CO_OID_REQUEST fun_PROTOCOL_CO_OID_REQUEST;
#endif 
 
#ifdef fun_PROTOCOL_CO_OID_REQUEST_COMPLETE
extern PROTOCOL_CO_OID_REQUEST_COMPLETE fun_PROTOCOL_CO_OID_REQUEST_COMPLETE;
#endif 
 
#ifdef fun_PROTOCOL_CO_RECEIVE_NET_BUFFER_LISTS
extern PROTOCOL_CO_RECEIVE_NET_BUFFER_LISTS fun_PROTOCOL_CO_RECEIVE_NET_BUFFER_LISTS;
#endif 
#ifdef fun_PROTOCOL_CO_SEND_NET_BUFFER_LISTS_COMPLETE
extern PROTOCOL_CO_SEND_NET_BUFFER_LISTS_COMPLETE fun_PROTOCOL_CO_SEND_NET_BUFFER_LISTS_COMPLETE;
#endif 
 
#ifdef fun_PROTOCOL_CO_STATUS_EX
extern PROTOCOL_CO_STATUS_EX fun_PROTOCOL_CO_STATUS_EX;
#endif 


#ifdef fun_IF_QUERY_OBJECT_1
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_1;  /*multiple*/
#endif

#ifdef fun_IF_QUERY_OBJECT_2
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_2;  /*multiple*/
#endif

#ifdef fun_IF_QUERY_OBJECT_3
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_3;  /*multiple*/
#endif

#ifdef fun_IF_QUERY_OBJECT_4
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_4;  /*multiple*/
#endif

#ifdef fun_IF_QUERY_OBJECT_5
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_5;  /*multiple*/
#endif

#ifdef fun_IF_QUERY_OBJECT_6
extern IF_QUERY_OBJECT fun_IF_QUERY_OBJECT_6;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_1
extern IF_SET_OBJECT fun_IF_SET_OBJECT_1;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_2
extern IF_SET_OBJECT fun_IF_SET_OBJECT_2;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_3
extern IF_SET_OBJECT fun_IF_SET_OBJECT_3;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_4
extern IF_SET_OBJECT fun_IF_SET_OBJECT_4;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_5
extern IF_SET_OBJECT fun_IF_SET_OBJECT_5;  /*multiple*/
#endif

#ifdef fun_IF_SET_OBJECT_6
extern IF_SET_OBJECT fun_IF_SET_OBJECT_6;  /*multiple*/
#endif
