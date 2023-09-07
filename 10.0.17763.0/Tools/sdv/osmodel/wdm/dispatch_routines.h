/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*
    This file declares the driver entry points.
*/

extern DRIVER_INITIALIZE fun_DriverEntry;

#ifdef fun_IRP_MJ_CREATE
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE; /*SAL:"IRP_MJ_CREATE"*/
#endif

#ifdef fun_IRP_MJ_CREATE_NAMED_PIPE
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE_NAMED_PIPE; /*SAL:"IRP_MJ_CREATE_NAMED_PIPE"*/
#endif

#ifdef fun_IRP_MJ_CLOSE
extern DRIVER_DISPATCH fun_IRP_MJ_CLOSE; /*SAL:"IRP_MJ_CLOSE"*/
#endif

#ifdef fun_IRP_MJ_READ
extern DRIVER_DISPATCH fun_IRP_MJ_READ; /*SAL:"IRP_MJ_READ"*/
#endif

#ifdef fun_IRP_MJ_WRITE
extern DRIVER_DISPATCH fun_IRP_MJ_WRITE; /*SAL:"IRP_MJ_WRITE"*/
#endif

#ifdef fun_IRP_MJ_QUERY_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_INFORMATION; /*SAL:"IRP_MJ_QUERY_INFORMATION"*/
#endif

#ifdef fun_IRP_MJ_SET_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_SET_INFORMATION; /*SAL:"IRP_MJ_SET_INFORMATION"*/
#endif

#ifdef fun_IRP_MJ_QUERY_EA
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_EA; /*SAL:"IRP_MJ_QUERY_EA"*/
#endif

#ifdef fun_IRP_MJ_SET_EA
extern DRIVER_DISPATCH fun_IRP_MJ_SET_EA; /*SAL:"IRP_MJ_SET_EA"*/
#endif

#ifdef fun_IRP_MJ_FLUSH_BUFFERS
extern DRIVER_DISPATCH fun_IRP_MJ_FLUSH_BUFFERS; /*SAL:"IRP_MJ_FLUSH_BUFFERS"*/
#endif

#ifdef fun_IRP_MJ_QUERY_VOLUME_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_VOLUME_INFORMATION; /*SAL:"IRP_MJ_QUERY_VOLUME_INFORMATION"*/
#endif

#ifdef fun_IRP_MJ_SET_VOLUME_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_SET_VOLUME_INFORMATION; /*SAL:"IRP_MJ_SET_VOLUME_INFORMATION"*/
#endif

#ifdef fun_IRP_MJ_DIRECTORY_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_DIRECTORY_CONTROL; /*SAL:"IRP_MJ_DIRECTORY_CONTROL"*/
#endif

#ifdef fun_IRP_MJ_FILE_SYSTEM_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_FILE_SYSTEM_CONTROL; /*SAL:"IRP_MJ_FILE_SYSTEM_CONTROL"*/
#endif

#ifdef fun_IRP_MJ_DEVICE_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_DEVICE_CONTROL; /*SAL:"IRP_MJ_DEVICE_CONTROL"*/
#endif

#ifdef fun_IRP_MJ_INTERNAL_DEVICE_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_INTERNAL_DEVICE_CONTROL; /*SAL:"IRP_MJ_INTERNAL_DEVICE_CONTROL","IRP_MJ_SCSI"*/
#endif

#ifdef fun_IRP_MJ_SHUTDOWN
extern DRIVER_DISPATCH fun_IRP_MJ_SHUTDOWN; /*SAL:"IRP_MJ_SHUTDOWN"*/
#endif

#ifdef fun_IRP_MJ_LOCK_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_LOCK_CONTROL; /*SAL:"IRP_MJ_LOCK_CONTROL"*/
#endif

#ifdef fun_IRP_MJ_CLEANUP
extern DRIVER_DISPATCH fun_IRP_MJ_CLEANUP; /*SAL:"IRP_MJ_CLEANUP"*/
#endif

#ifdef fun_IRP_MJ_CREATE_MAILSLOT
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE_MAILSLOT; /*SAL:"IRP_MJ_CREATE_MAILSLOT"*/
#endif

#ifdef fun_IRP_MJ_QUERY_SECURITY
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_SECURITY; /*SAL:"IRP_MJ_QUERY_SECURITY"*/
#endif

#ifdef fun_IRP_MJ_SET_SECURITY
extern DRIVER_DISPATCH fun_IRP_MJ_SET_SECURITY; /*SAL:"IRP_MJ_SET_SECURITY"*/
#endif

#ifdef fun_IRP_MJ_POWER
extern DRIVER_DISPATCH fun_IRP_MJ_POWER; /*SAL:"IRP_MJ_POWER"*/
#endif

#ifdef fun_IRP_MJ_SYSTEM_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_SYSTEM_CONTROL; /*SAL:"IRP_MJ_SYSTEM_CONTROL"*/
#endif

#ifdef fun_IRP_MJ_DEVICE_CHANGE
extern DRIVER_DISPATCH fun_IRP_MJ_DEVICE_CHANGE; /*SAL:"IRP_MJ_DEVICE_CHANGE"*/
#endif

#ifdef fun_IRP_MJ_QUERY_QUOTA
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_QUOTA; /*SAL:"IRP_MJ_QUERY_QUOTA"*/
#endif

#ifdef fun_IRP_MJ_SET_QUOTA
extern DRIVER_DISPATCH fun_IRP_MJ_SET_QUOTA; /*SAL:"IRP_MJ_SET_QUOTA"*/
#endif

#ifdef fun_IRP_MJ_PNP
extern DRIVER_DISPATCH fun_IRP_MJ_PNP; /*SAL:"IRP_MJ_PNP","IRP_MJ_PNP_POWER"*/
#endif



#ifdef fun_DriverStartIo
extern DRIVER_STARTIO fun_DriverStartIo;
#endif

#ifdef fun_AddDevice
extern DRIVER_ADD_DEVICE fun_AddDevice;
#endif

#ifdef fun_DriverUnload
extern DRIVER_UNLOAD fun_DriverUnload;
#endif

#ifdef fun_DRIVER_CONTROL
extern DRIVER_CONTROL fun_DRIVER_CONTROL;
#endif


#ifdef fun_IO_COMPLETION_ROUTINE_1
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_2
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_3
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_4
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_5
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_6
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_7
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_8
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_9
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_10
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_11
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_11; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_12
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_12; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_1
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_2
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_3
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_4
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_5
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_6
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_6; /*multiple*/
#endif


#ifdef fun_KDEFERRED_ROUTINE_1
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_2
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_3
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_4
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_5
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_6
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_7
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_8
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_9
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_10
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_11
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_11; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_12
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_12; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_13
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_13; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_14
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_14; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_15
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_15; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_16
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_16; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_17
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_17; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_18
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_18; /*multiple*/
#endif




#ifdef fun_KSERVICE_ROUTINE_1
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_2
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_3
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_4
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_5
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_6
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_DRIVER_CANCEL_1
extern DRIVER_CANCEL fun_DRIVER_CANCEL_1; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_2
extern DRIVER_CANCEL fun_DRIVER_CANCEL_2; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_3
extern DRIVER_CANCEL fun_DRIVER_CANCEL_3; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_4
extern DRIVER_CANCEL fun_DRIVER_CANCEL_4; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_5
extern DRIVER_CANCEL fun_DRIVER_CANCEL_5; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_6
extern DRIVER_CANCEL fun_DRIVER_CANCEL_6; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_7
extern DRIVER_CANCEL fun_DRIVER_CANCEL_7; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_1
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_2
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_3
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_4
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_5
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_6
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_1
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_1; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_2
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_2; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_3
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_3; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_4
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_4; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_5
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_5; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_6
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_6; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_7
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_7; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_8
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_8; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_9
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_9; /*multiple*/
#endif

#ifdef fun_REQUEST_POWER_COMPLETE_10
extern REQUEST_POWER_COMPLETE fun_REQUEST_POWER_COMPLETE_10; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_1
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_2
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_3
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_4
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_5
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_6
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_7
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_8
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_9
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_10
extern IO_WORKITEM_ROUTINE fun_IO_WORKITEM_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_1
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_1; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_2
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_2; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_3
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_3; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_4
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_4; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_5
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_5; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_6
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_6; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_7
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_7; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_8
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_8; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_9
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_9; /*multiple*/
#endif

#ifdef fun_IO_WORKITEM_ROUTINE_EX_10
extern IO_WORKITEM_ROUTINE_EX fun_IO_WORKITEM_ROUTINE_EX_10; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_1
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_2
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_3
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_4
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_5
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_6
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_7
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_8
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_9
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_10
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_11
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_11; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_12
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_12; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_13
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_13; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_14
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_14; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_15
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_15; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_16
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_16; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_17
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_17; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_18
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_18; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_19
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_19; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_20
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_20; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_21
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_21; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_22
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_22; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_23
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_23; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_24
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_24; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_25
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_25; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_26
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_26; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_27
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_27; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_28
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_28; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_29
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_29; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_30
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_30; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_31
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_31; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_32
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_32; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_33
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_33; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_34
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_34; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_35
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_35; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_36
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_36; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_37
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_37; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_38
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_38; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_39
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_39; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_40
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_40; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_41
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_41; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_42
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_42; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_43
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_43; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_44
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_44; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_45
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_45; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_46
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_46; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_47
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_47; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_48
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_48; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_49
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_49; /*multiple*/
#endif

#ifdef fun_KSYNCHRONIZE_ROUTINE_50
extern KSYNCHRONIZE_ROUTINE fun_KSYNCHRONIZE_ROUTINE_50; /*multiple*/
#endif



















#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_1
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_2
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_3
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_4
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_5
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_6
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_7
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_8
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_9
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_10
extern DRIVER_NOTIFICATION_CALLBACK_ROUTINE fun_DRIVER_NOTIFICATION_CALLBACK_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_1
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_2
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_3
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_4
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_5
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_6
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_7
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_8
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_9
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_IO_TIMER_ROUTINE_10
extern IO_TIMER_ROUTINE fun_IO_TIMER_ROUTINE_10; /*multiple*/
#endif

#ifdef fun_RTL_QUERY_REGISTRY_ROUTINE
extern RTL_QUERY_REGISTRY_ROUTINE fun_RTL_QUERY_REGISTRY_ROUTINE;
#endif

#ifdef fun_ALLOCATE_FUNCTION
extern ALLOCATE_FUNCTION fun_ALLOCATE_FUNCTION;
#endif

#ifdef fun_FREE_FUNCTION
extern FREE_FUNCTION fun_FREE_FUNCTION;
#endif

#ifdef fun_ALLOCATE_FUNCTION_EX
extern ALLOCATE_FUNCTION_EX fun_ALLOCATE_FUNCTION_EX;
#endif

#ifdef fun_FREE_FUNCTION_EX
extern FREE_FUNCTION_EX fun_FREE_FUNCTION_EX;
#endif

#ifdef fun_KSTART_ROUTINE
extern KSTART_ROUTINE fun_KSTART_ROUTINE;
#endif

#ifdef fun_KMESSAGE_SERVICE_ROUTINE
extern KMESSAGE_SERVICE_ROUTINE fun_KMESSAGE_SERVICE_ROUTINE;
#endif

#ifdef fun_KBUGCHECK_REASON_CALLBACK_ROUTINE
extern KBUGCHECK_REASON_CALLBACK_ROUTINE fun_KBUGCHECK_REASON_CALLBACK_ROUTINE;
#endif

#ifdef fun_KBUGCHECK_CALLBACK_ROUTINE  
extern KBUGCHECK_CALLBACK_ROUTINE fun_KBUGCHECK_CALLBACK_ROUTINE;
#endif


#ifdef fun_NMI_CALLBACK
extern NMI_CALLBACK fun_NMI_CALLBACK;
#endif

#ifdef fun_PROCESSOR_CALLBACK_FUNCTION
extern PROCESSOR_CALLBACK_FUNCTION fun_PROCESSOR_CALLBACK_FUNCTION;
#endif

#ifdef fun_CALLBACK_FUNCTION 
extern CALLBACK_FUNCTION  fun_CALLBACK_FUNCTION;
#endif

#ifdef fun_EX_CALLBACK_FUNCTION
extern EX_CALLBACK_FUNCTION  fun_EX_CALLBACK_FUNCTION;
#endif

#ifdef fun_WMI_NOTIFICATION_CALLBACK
extern WMI_NOTIFICATION_CALLBACK  fun_WMI_NOTIFICATION_CALLBACK;
#endif

#ifdef fun_DEVICE_CHANGE_COMPLETE_CALLBACK
extern DEVICE_CHANGE_COMPLETE_CALLBACK  fun_DEVICE_CHANGE_COMPLETE_CALLBACK;
#endif

#ifdef fun_POWER_SETTING_CALLBACK
extern POWER_SETTING_CALLBACK  fun_POWER_SETTING_CALLBACK;
#endif

#ifdef fun_IO_SESSION_NOTIFICATION_FUNCTION
extern IO_SESSION_NOTIFICATION_FUNCTION   fun_IO_SESSION_NOTIFICATION_FUNCTION;
#endif

#ifdef fun_PCW_CALLBACK
extern PCW_CALLBACK   fun_PCW_CALLBACK;
#endif

#ifdef fun_IO_CSQ_INSERT_IRP
extern IO_CSQ_INSERT_IRP   fun_IO_CSQ_INSERT_IRP;
#endif

#ifdef fun_IO_CSQ_INSERT_IRP_EX 
extern IO_CSQ_INSERT_IRP_EX    fun_IO_CSQ_INSERT_IRP_EX;
#endif

#ifdef fun_IO_CSQ_REMOVE_IRP 
extern IO_CSQ_REMOVE_IRP fun_IO_CSQ_REMOVE_IRP;
#endif

#ifdef fun_IO_CSQ_PEEK_NEXT_IRP 
extern IO_CSQ_PEEK_NEXT_IRP fun_IO_CSQ_PEEK_NEXT_IRP;
#endif

#ifdef fun_IO_CSQ_ACQUIRE_LOCK 
extern IO_CSQ_ACQUIRE_LOCK fun_IO_CSQ_ACQUIRE_LOCK;
#endif

#ifdef fun_IO_CSQ_RELEASE_LOCK 
extern IO_CSQ_RELEASE_LOCK   fun_IO_CSQ_RELEASE_LOCK;
#endif

#ifdef fun_IO_CSQ_COMPLETE_CANCELED_IRP 
extern IO_CSQ_COMPLETE_CANCELED_IRP   fun_IO_CSQ_COMPLETE_CANCELED_IRP;
#endif

#ifdef fun_FAST_IO_CHECK_IF_POSSIBLE 
extern FAST_IO_CHECK_IF_POSSIBLE   fun_FAST_IO_CHECK_IF_POSSIBLE;
#endif

#ifdef fun_FAST_IO_READ 
extern FAST_IO_READ   fun_FAST_IO_READ;
#endif

#ifdef fun_FAST_IO_WRITE 
extern FAST_IO_WRITE   fun_FAST_IO_WRITE;
#endif

#ifdef fun_FAST_IO_QUERY_BASIC_INFO 
extern FAST_IO_QUERY_BASIC_INFO   fun_FAST_IO_QUERY_BASIC_INFO;
#endif

#ifdef fun_FAST_IO_QUERY_STANDARD_INFO 
extern FAST_IO_QUERY_STANDARD_INFO   fun_FAST_IO_QUERY_STANDARD_INFO;
#endif

#ifdef fun_FAST_IO_LOCK 
extern FAST_IO_LOCK   fun_fun_FAST_IO_LOCK;
#endif

#ifdef fun_FAST_IO_UNLOCK_SINGLE 
extern FAST_IO_UNLOCK_SINGLE   fun_FAST_IO_UNLOCK_SINGLE;
#endif

#ifdef fun_FAST_IO_UNLOCK_ALL 
extern FAST_IO_UNLOCK_ALL fun_FAST_IO_UNLOCK_ALL ;
#endif

#ifdef fun_FAST_IO_UNLOCK_ALL_BY_KEY 
extern FAST_IO_UNLOCK_ALL_BY_KEY   fun_FAST_IO_UNLOCK_ALL_BY_KEY;
#endif

#ifdef fun_FAST_IO_DEVICE_CONTROL 
extern FAST_IO_DEVICE_CONTROL   fun_FAST_IO_DEVICE_CONTROL;
#endif

#ifdef fun_FAST_IO_ACQUIRE_FILE 
extern FAST_IO_ACQUIRE_FILE   fun_FAST_IO_ACQUIRE_FILE;
#endif

#ifdef fun_FAST_IO_RELEASE_FILE 
extern FAST_IO_RELEASE_FILE   fun_FAST_IO_RELEASE_FILE;
#endif

#ifdef fun_FAST_IO_DETACH_DEVICE 
extern FAST_IO_DETACH_DEVICE   fun_FAST_IO_DETACH_DEVICE;
#endif

#ifdef fun_FAST_IO_QUERY_NETWORK_OPEN_INFO 
extern FAST_IO_QUERY_NETWORK_OPEN_INFO   fun_FAST_IO_QUERY_NETWORK_OPEN_INFO;
#endif

#ifdef fun_FAST_IO_MDL_READ 
extern FAST_IO_MDL_READ   fun_FAST_IO_MDL_READ;
#endif

#ifdef fun_FAST_IO_MDL_READ_COMPLETE 
extern FAST_IO_MDL_READ_COMPLETE   fun_FAST_IO_MDL_READ_COMPLETE;
#endif

#ifdef fun_FAST_IO_PREPARE_MDL_WRITE 
extern FAST_IO_PREPARE_MDL_WRITE   fun_FAST_IO_PREPARE_MDL_WRITE;
#endif

#ifdef fun_FAST_IO_MDL_WRITE_COMPLETE 
extern FAST_IO_MDL_WRITE_COMPLETE   fun_FAST_IO_MDL_WRITE_COMPLETE;
#endif

#ifdef fun_FAST_IO_ACQUIRE_FOR_MOD_WRITE 
extern FAST_IO_ACQUIRE_FOR_MOD_WRITE   fun_FAST_IO_ACQUIRE_FOR_MOD_WRITE;
#endif

#ifdef fun_FAST_IO_RELEASE_FOR_MOD_WRITE 
extern FAST_IO_RELEASE_FOR_MOD_WRITE   fun_FAST_IO_RELEASE_FOR_MOD_WRITE;
#endif

#ifdef fun_FAST_IO_ACQUIRE_FOR_CCFLUSH 
extern FAST_IO_ACQUIRE_FOR_CCFLUSH    fun_FAST_IO_ACQUIRE_FOR_CCFLUSH;
#endif

#ifdef fun_FAST_IO_RELEASE_FOR_CCFLUSH 
extern FAST_IO_RELEASE_FOR_CCFLUSH   fun_FAST_IO_RELEASE_FOR_CCFLUSH;
#endif

#ifdef fun_FAST_IO_READ_COMPRESSED 
extern FAST_IO_READ_COMPRESSED   fun_FAST_IO_READ_COMPRESSED;
#endif

#ifdef fun_FAST_IO_WRITE_COMPRESSED 
extern FAST_IO_WRITE_COMPRESSED   fun_FAST_IO_WRITE_COMPRESSED;
#endif

#ifdef fun_FAST_IO_MDL_READ_COMPLETE_COMPRESSED 
extern FAST_IO_MDL_READ_COMPLETE_COMPRESSED   fun_FAST_IO_MDL_READ_COMPLETE_COMPRESSED;
#endif

#ifdef fun_FAST_IO_MDL_WRITE_COMPLETE_COMPRESSED 
extern FAST_IO_MDL_WRITE_COMPLETE_COMPRESSED   fun_FAST_IO_MDL_WRITE_COMPLETE_COMPRESSED;
#endif

#ifdef fun_FAST_IO_QUERY_OPEN 
extern FAST_IO_QUERY_OPEN   fun_FAST_IO_QUERY_OPEN;
#endif


#ifdef fun_PO_FX_COMPONENT_IDLE_STATE_CALLBACK
extern PO_FX_COMPONENT_IDLE_STATE_CALLBACK   fun_PO_FX_COMPONENT_IDLE_STATE_CALLBACK;
#endif

#ifdef fun_PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK
extern PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK   fun_PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK;
#endif

#ifdef fun_PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK
extern PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK   fun_PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK;
#endif

#ifdef fun_PO_FX_DEVICE_POWER_REQUIRED_CALLBACK
extern PO_FX_DEVICE_POWER_REQUIRED_CALLBACK   fun_PO_FX_DEVICE_POWER_REQUIRED_CALLBACK;
#endif


#ifdef fun_PO_FX_POWER_CONTROL_CALLBACK
extern PO_FX_POWER_CONTROL_CALLBACK   fun_PO_FX_POWER_CONTROL_CALLBACK;
#endif


#ifdef fun_PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK
extern PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK   fun_PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK;
#endif

#ifdef fun_PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK
extern PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK   fun_PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK;
#endif




