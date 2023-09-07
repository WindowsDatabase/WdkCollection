//---------------------------------------------------------------------------
// Microsoft Test Automation Sources
//
// Microsoft Confidential
// Copyright 2009 Microsoft Corporation. All Rights Reserved.
//
// Targets.h
//
// Collection: WDTF Internals - WDTFInterfaces Reference
// 
// Environment: User mode
//
// DLL: WDTFInterfaces.dll
//
// Creator: DieterA
//
// Primary Contact: WDTF Team (WDTFTeam)
//
// History:
//   [Date]        -   [Status]
//   12/15/2009    -    Created
//---------------------------------------------------------------------------
#pragma once

#include <wdtflogging.h>

//
//  CCOMERROR
//

template <class HR >
class CCOMERROR_BASE
{

private:

	BOOL m_bIsSet;

public:

	HR m_hr;
	CComBSTR m_sErrorString;

	CCOMERROR_BASE() : 
		m_hr(S_OK),
		m_sErrorString(L""),
		m_bIsSet(FALSE)
	{}

	BOOL IsSet(VOID){return m_bIsSet;}
	BOOL IsError(VOID){return FAILED(m_hr);}
	BOOL IsSuccess(VOID){return SUCCEEDED(m_hr);}
	VOID Clear(VOID){
		SetSuccess();
		m_bIsSet = FALSE;
	}

	BOOL
		Comp( CCOMERROR_BASE<HR> * pErr )
	{

		//
		//  Null cases 
		//

		if(pErr==NULL)
			return FALSE;

		if(m_sErrorString.m_str == NULL)
		{
			if(pErr->m_sErrorString.m_str == NULL)
				return TRUE;
			return FALSE;
		}

		if(pErr->m_sErrorString.m_str == NULL)
		{
			return FALSE;
		}

		//
		//  IsSet 
		//

		if(IsSet() != pErr->IsSet())
		{
			return FALSE;
		}


		// 
		//  Compare error code 
		//

		if(m_hr != pErr->m_hr)
			return FALSE;

		// 
		//  Compare error string 
		//

		if(wcscmp(m_sErrorString.m_str,pErr->m_sErrorString.m_str))
			return FALSE;

		return TRUE;
	}



	BOOL
		Move(_Inout_ CCOMERROR_BASE<HR> * Err)
	{
		m_hr = Err->m_hr;
		m_sErrorString.m_str = Err->m_sErrorString.Detach();
		m_bIsSet = TRUE;
		Err->Clear();
		return TRUE;
	}


	VOID SetFirstError(_In_ HRESULT hr){ // Will get current IErrorInfo
		if( SUCCEEDED(m_hr) && FAILED(hr))
		{
			Set(hr);
		}
	}
	VOID SetFirstError(_In_ HRESULT hr,_In_ PCWSTR psErrorString)    {
		if( SUCCEEDED(m_hr) && FAILED(hr))
		{
			Set(hr,psErrorString);
		}
	}

	VOID SetFirstError(_In_ IWDTFAction2 * pActionEx)    {
		pActionEx->GetStatus(&m_hr);
		m_sErrorString.Empty();
		pActionEx->GetStatusString(&m_sErrorString);
		m_bIsSet = TRUE;
	}

	VOID Set(_In_ HR hr){ // Will get current IErrorInfo
		HRESULT ehr;
		CComPtr<IErrorInfo> pErrInfo; 
		ehr = ::GetErrorInfo( 0, &pErrInfo );
		if(ehr == S_OK && pErrInfo != NULL)
		{

			//
			//  We have error info
			//

			m_sErrorString.Empty();
			ehr = pErrInfo->GetDescription( &m_sErrorString ); 
			if(FAILED(ehr))
			{
				m_sErrorString = SysAllocString(L"");
			}

		}
		else
		{

			//
			// No error info
			//

			m_sErrorString.Empty();
			m_sErrorString.m_str = SysAllocString(L" No Error Info Provided");

		}
		m_hr = hr;
		m_bIsSet = TRUE;
	}

	VOID Set(_In_ HR hr,_In_ PCWSTR psErrorString){
		m_sErrorString.Empty();
		m_sErrorString.m_str = SysAllocString(psErrorString);
		m_hr = hr;
		m_bIsSet = TRUE;
	}

	VOID SetFormat(_In_ HR hr,_In_ PCWSTR psErrorStringFormat,...){
		CString sTempString;
		va_list args;
		va_start(args, psErrorStringFormat);
		sTempString.FormatV(psErrorStringFormat, args);
		va_end(args);
		Set(hr,sTempString);
	}

	VOID Set(_In_ IWDTFAction2 * pActionEx)    {
		pActionEx->GetStatus(&m_hr);
		m_sErrorString.Empty();
		pActionEx->GetStatusString(&m_sErrorString);
		m_bIsSet = TRUE;
	}
	VOID SetSuccess(VOID){


		//
		//  Prevent constant alloac/free of ""
		//  string when SetSucccess() is called 
		//  in a loop.
		//

		if(m_hr == S_OK && 
			m_sErrorString.m_str && 
			m_sErrorString.m_str[0] == L'\0')
		{
			m_bIsSet = TRUE;
			return;
		}

		//
		//  Set members to indecate 
		//  Success
		//

		m_hr = S_OK;
		if(m_sErrorString.m_str)
		{
			m_sErrorString.Empty();
		}
		m_sErrorString.m_str = SysAllocString(L"");
		m_bIsSet = TRUE;
	}

};

typedef CCOMERROR_BASE<HRESULT>  CCOMERROR;


//
//  CWDTFAction2Impl
//

template <class T >
class CWDTFAction2Impl : public T
{
public:

	CComQIPtr<IWDTFInternalTarget2> m_pTarget;  
	CWDTFLOGGING m_Log; 
	CCOMERROR m_LastError;

	STDMETHOD(get_Target)(_Outptr_ IWDTFTarget2** ppTarget)
	{
		if(!ppTarget)
			return E_INVALIDARG;
		CComQIPtr<IWDTFTarget2> pTarget = m_pTarget;
		HRESULT hr =  pTarget.CopyTo(ppTarget);
		return hr;
	}

	STDMETHOD(EnableObjectErrorLogging)(void)
	{
		m_Log.EnableObjectErrorLogging();
		return S_OK;
	}

	STDMETHOD(DisableObjectErrorLogging)(void)
	{
		m_Log.DisableObjectErrorLogging();
		return S_OK;
	}
	STDMETHOD(EnableObjectLogging)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(DisableObjectLogging)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(IsStatusSuccess)(_Inout_ VARIANT_BOOL* pResult)
	{
		if(!pResult)
			return E_INVALIDARG;
		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		if(m_LastError.IsSuccess())
		{
			bSuccess = VARIANT_TRUE;
		}
		*pResult = bSuccess;
		return S_OK;
	}

	STDMETHOD(GetStatus)(_Inout_ HRESULT* pResult)
	{
		if(!pResult)
			return E_INVALIDARG;

		*pResult = m_LastError.m_hr;
		return S_OK;
	}

	STDMETHOD(GetStatusString)(_Inout_ BSTR * pResult)
	{
		if(!pResult)
			return E_INVALIDARG;

		HRESULT hr = m_LastError.m_sErrorString.CopyTo(pResult);
		return hr;
	}
};

//
//  CActionsCustomeCollection template class .   Ussed to create custome action collection
//  interfaces.
//

typedef HRESULT (*PerformTaskOnSingleItemFuncPtrType)(IWDTFAction2 * pAction, PVOID pContext , VARIANT_BOOL *  pSuccess) ;


#define CE(x) \
	hr = (x);\
	if(FAILED(hr)) \
{ \
	return hr; \
}\

template <class T >
class CActionsCustomeCollection : public T
{
public:

	HRESULT
		InitActionsCustomeCollection()
	{
		return m_pActions.CoCreateInstance(__uuidof(WDTFActions2));
	}

	void
		UnInitActionsCustomeCollection()
	{
		m_pActions->Clear();
	}

	STDMETHOD(get__NewEnum)(_Outptr_ IUnknown** ppNewEnum)
	{
		return m_pActions->get__NewEnum(ppNewEnum);
	}

	STDMETHOD(get_Item)(_In_ LONG Index,_Outptr_ IWDTFAction2** ppAction)
	{
		return m_pActions->get_Item(Index,ppAction); 
	}

	STDMETHOD(get_Count)(_In_ LONG* pVal)
	{
		return m_pActions->get_Count(pVal);
	}

	STDMETHOD(Add)(_In_ IWDTFAction2* pAction)
	{
		return m_pActions->Add(pAction);
	}

	STDMETHOD(Remove)(_In_ IWDTFAction2* pAction)
	{
		return m_pActions->Remove(pAction);
	}

	STDMETHOD(Clear)(void)
	{
		return m_pActions->Clear();
	}


	STDMETHOD (IsStatusSuccess)(_Inout_ VARIANT_BOOL* pResult)
	{
		VARIANT_BOOL bSuccess = VARIANT_FALSE;

		if(m_Error.IsSuccess())
		{
			bSuccess = VARIANT_TRUE;
		}

		*pResult = bSuccess;
		return S_OK;
	}

	STDMETHOD(GetStatus)(_Inout_ HRESULT* pResult)
	{
		if(pResult)
			return E_POINTER;

		return S_OK;
	}

	STDMETHOD(GetStatusString)(_Inout_ BSTR * pResult)
	{
		return m_Error.m_sErrorString.CopyTo(pResult);
	}

	STDMETHOD(EnableObjectErrorLogging)(void)
	{
		LONG nCount;
		HRESULT hr;
		CE( m_pActions->get_Count(&nCount) );
		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction;
			CE( m_pActions->get_Item(i,&pAction) );
			CE( pAction->EnableObjectErrorLogging() );
		}
		return S_OK;
	}

	STDMETHOD(DisableObjectErrorLogging)(void)
	{
		LONG nCount;
		HRESULT hr;
		CE( m_pActions->get_Count(&nCount) );
		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction;
			CE( m_pActions->get_Item(i,&pAction) );
			CE( pAction->DisableObjectErrorLogging() );
		}
		return S_OK;
	}

	STDMETHOD(EnableObjectLogging)(void)
	{
		LONG nCount;
		HRESULT hr;
		CE( m_pActions->get_Count(&nCount) );
		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction;
			CE( m_pActions->get_Item(i,&pAction) );
			CE( pAction->EnableObjectLogging() );
		}
		return S_OK;
	}

	STDMETHOD(DisableObjectLogging)(void)
	{
		LONG nCount;
		HRESULT hr;
		CE( m_pActions->get_Count(&nCount) );
		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction;
			CE( m_pActions->get_Item(i,&pAction) );
			CE( pAction->DisableObjectLogging() );
		}
		return S_OK;
	}


	STDMETHOD(PerformTaskOnAllActionItems)(_In_ IWDTFActions2 * pActions,_In_ PerformTaskOnSingleItemFuncPtrType pPerformTaskOnSingleItemFunc,_In_opt_ PVOID pContext,_In_ VARIANT_BOOL bStopOnFirstError,_Inout_ VARIANT_BOOL *  pResult,_In_ CWDTFLOGGING* pLog)
	{
        /*++

        Routine Description:

        This handles calling all the IO, Pnp , Fuzz , Power actions for each target device(s).

        1. Retrieves the underlying action, config , WDTF object etc
        2. Calls pPerformTaskOnSingleItemFunc for each action on a specific target

        Arguments:

        pActions                     - Pointer to the the actions to be used.
        pPerformTaskOnSingleItemFunc - Function Pointer to the action runner
        pContext                     - Pointer to context
        bStopOnFirstError            - BOOL to indicate if the function should stop on the first error
        pResult                      - BOOL to indicate overall result
        pLog                         - Logging Object pointer handle
       

        Return Value:

        Hresult value indicating the result of the function.

        1. S_OK means all IO, Pnp , Fuzz , Power actions for each target device(s) resulted in SUCCESS
        2. Otherwise failed
        
        --*/

		//
		//  Locals
		//

		HRESULT hr        = S_OK;
        HRESULT hr_inner           = S_OK;
		VARIANT_BOOL  TotalSuccess = VARIANT_TRUE;
		VARIANT_BOOL  ItemSuccess  = VARIANT_FALSE;

		//
		//  Clear error and get collection item count.
		//

		m_Error.Clear();
		LONG nCount=0;
		CE( pActions->get_Count(&nCount) );

		//
		//  Enum threw action collection
		//

		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction = NULL;
            CComPtr<IWDTFTarget2> pTarget = NULL;
			CComPtr<IWDTF2> pWDTF2 = NULL;
			CComPtr<IWDTFCONFIG2> pConfig2 = NULL;
            CCOMERROR Error;
            CCOMERROR Error_Inner;
            // protect against IO action unload by harness such as TAEF
            hr_inner=pActions->get_Item(i,&pAction);
            Error_Inner.Set(hr_inner);
            if (pAction == NULL)
            {
                if (pLog != NULL)
                {
                    pLog->OutputInfo(L"pAction is NULL after calling get_Item inside PerformTaskOnAllActionItems Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                }
                else
                {
                    wprintf(L"pAction is NULL after calling get_Item inside PerformTaskOnAllActionItems hr_inner:=(%lx\n).\n", hr_inner);
                }
                return hr_inner;
            }
            else if (FAILED(hr_inner))
            {
                if (pLog != NULL)
                {
                    pLog->OutputInfo(L"The call to get_Item inside PerformTaskOnAllActionItems failed with Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                }
                else
                {
                    wprintf(L"The call to get_Item inside PerformTaskOnAllActionItems failed with hr_inner:=(%lx).\n", hr_inner);
                }
                return hr_inner;
            }

			//
			//  Perform operation on current action
			//
            // protect against IO action unload by harness such as TAEF
            hr = pPerformTaskOnSingleItemFunc(pAction,pContext,&ItemSuccess);
			if (FAILED(hr))
            {
				Error.Set(hr);
                if (pAction == NULL)
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"pAction is NULL after calling pPerformTaskOnSingleItemFunc inside PerformTaskOnAllActionItems Error.m_sErrorString:=(%s).\n", Error.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"pAction is NULL after calling pPerformTaskOnSingleItemFunc inside PerformTaskOnAllActionItems hr_inner:=(%lx).\n",hr);
                    }
                    return hr;
                    
                }
                // protect against pTarget unload by harness such as TAEF
                hr_inner=pAction->get_Target(&pTarget);
                Error_Inner.Set(hr_inner);
                if (pTarget == NULL)
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"pTarget is NULL after calling get_Target inside PerformTaskOnAllActionItems Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"pTarget is NULL after calling get_Target inside PerformTaskOnAllActionItems hr_inner:=(%lx).\n", hr_inner);
                    }
                    return hr;
                }
                else if (FAILED(hr_inner))
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"The call to get_Target inside PerformTaskOnAllActionItems failed with Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"The call to get_Target inside PerformTaskOnAllActionItems failed with hr_inner:=(%lx).\n", hr_inner);
                    }
                    return hr_inner;
                }
                // protect against WDTF unload by harness such as TAEF
                hr_inner=pTarget->get_WDTF(&pWDTF2);
                Error_Inner.Set(hr_inner);
                if (pWDTF2 == NULL)
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"pWDTF2 is NULL after calling get_WDTF inside PerformTaskOnAllActionItems Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"pWDTF2 is NULL after calling get_WDTF inside PerformTaskOnAllActionItems hr_inner:=(%lx).\n", hr_inner);
                    }
                    return hr;
                }
                else if (FAILED(hr_inner))
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"The call to get_WDTF inside PerformTaskOnAllActionItems failed with Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"The call to get_WDTF inside PerformTaskOnAllActionItems failed with hr_inner:=(%lx\n).\n", hr_inner);
                    }
                    return hr_inner;
                }
                // protect against WDTF CONFIG unload by harness such as TAEF
                hr_inner = pWDTF2->get_Config(&pConfig2);
                Error_Inner.Set(hr_inner);
                if (pConfig2 == NULL)
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"pConfig2 is NULL after calling get_Config inside PerformTaskOnAllActionItems Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"pConfig2 is NULL after calling get_Config inside PerformTaskOnAllActionItems hr_inner:=(%lx).\n", hr_inner);
                    }
                    return hr;
                }
                else if (FAILED(hr_inner))
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputInfo(L"The call to get_Config inside PerformTaskOnAllActionItems failed with Error_Inner.m_sErrorString:=(%s).\n", Error_Inner.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"The call to get_Config inside PerformTaskOnAllActionItems failed with hr_inner:=(%lx).\n", hr_inner);
                    }
                    return hr_inner;
                }

				VARIANT_BOOL vbErrLogEnabled = VARIANT_FALSE;
				CE(pConfig2->IsObjectLoggingEnabled(&vbErrLogEnabled));

				//
				// If object logging is enabled we log the critical failure and continue, otherwise we bail out of the function 
				// and return the critical error upstream
				//

				if (vbErrLogEnabled==VARIANT_TRUE)
                {
                    if (pLog != NULL)
                    {
                        pLog->OutputError(L"PerformTaskOnAllActionItems returning Error.m_sErrorString:=(%s).\n", Error.m_sErrorString);
                    }
                    else
                    {
                        wprintf(L"PerformTaskOnAllActionItems returning hr:=(%lx).\n", hr);
                    }
				}
                else
                {
					return Error.m_hr;
				}
			} 

			if(ItemSuccess == VARIANT_FALSE)
			{

				m_Error.SetFirstError(pAction);

				TotalSuccess = VARIANT_FALSE;

				if(bStopOnFirstError == VARIANT_TRUE)
				{
					break;
				}
			}
		}



		if(pResult)
		{
			*pResult = TotalSuccess;
		}

		return S_OK;
	}


	STDMETHOD(PerformTaskOnAllItems)(_In_ PerformTaskOnSingleItemFuncPtrType pPerformTaskOnSingleItemFunc,_In_opt_ PVOID pContext,_In_ VARIANT_BOOL bStopOnFirstError,_Inout_ VARIANT_BOOL *  pResult,_In_ CWDTFLOGGING* pLog)
	{
		return PerformTaskOnAllActionItems(m_pActions,pPerformTaskOnSingleItemFunc,pContext,bStopOnFirstError,pResult, pLog);
	}

	STDMETHOD(CreateParentsOnlyItemsActionsFromActions)(_In_ IWDTFActions2 * pSrcActions,_Outptr_ IWDTFActions2 ** ppActions)
	{


		HRESULT hr = S_FALSE;

		//
		//  Create Actions collection 
		//

		CComPtr<IWDTFActions2> pActions;
		CE( pActions.CoCreateInstance(__uuidof(WDTFActions2)));

		//
		//  Fill in action collection with 
		//  only parent devices.
		//

#ifdef WDTFPAFPOA
		wprintf(L"\nProcess action for parent only actions PAFPOA\n");
#endif

		VARIANT_BOOL bAdd =  VARIANT_FALSE;

		LONG nCount = 0;
		CE( pSrcActions->get_Count(&nCount) );
		for(LONG i=0;i<nCount;i++)
		{
			CComPtr<IWDTFAction2> pAction;
			CE( pSrcActions->get_Item(i,&pAction) );

			if(nCount == 1)
			{
#ifdef WDTFPAFPOA
				wprintf(L"PAFPOA: Single case\n");
#endif
				pActions->Add(pAction);
				*ppActions = pActions.Detach();
				return S_OK;
			}

			CComPtr<IWDTFTarget2> pDevice;
			CE( pAction->get_Target(&pDevice) );

#ifdef WDTFPAFPOA
			wprintf(L"PAFPOA: Pass\n");
#endif

			bAdd =  VARIANT_TRUE;

			for(LONG n=0;n<nCount;n++)
			{

				if(i==n)
					continue;

				CComPtr<IWDTFAction2> pActionComp;
				CE(  pSrcActions->get_Item(n,&pActionComp) );
				CComPtr<IWDTFTarget2> pDeviceComp;
				CE( pActionComp->get_Target(&pDeviceComp) );


				//
				// Get devnode #
				//
				LONG DevInst = 0;
				CE( pDevice->GetValueNumber(  
					CComBSTR(L"DevInst"),
					&DevInst) );

				//
				//  See if Actions[n] is a child of Actions[i]
				//

				CString StringIsChild;
				StringIsChild.Format(L"below-or-self/DevInst=%lu",DevInst);
				CComBSTR SdelIsChild = StringIsChild;
				VARIANT_BOOL bIsChild =  VARIANT_FALSE;
				CE( pDeviceComp->Eval(SdelIsChild,&bIsChild) );
				if(bIsChild == VARIANT_TRUE)
				{

					bAdd = VARIANT_FALSE;

#ifdef WDTFPAFPOA
					wprintf(L"PAFPOA: Comp %lu %lu (Child) \n",i,n);
#endif
				}
				else
				{
#ifdef WDTFPAFPOA
					wprintf(L"PAFPOA: Comp %lu %lu (Parent) \n",i,n);
#endif
				}
			}

			if(bAdd == VARIANT_TRUE)
			{
				pActions->Add(pAction);
			}

		}

		//
		//  Return Actions
		//

#ifdef WDTFPAFPOA
		nCount = 0;
		CE( pActions->get_Count(&nCount) );
		wprintf(L"PAFPOA result count # %lu\n",nCount);
#endif

		*ppActions = pActions.Detach();
		return S_OK;
	}


	STDMETHOD(CreateParentsOnlyItemsActions)(_Outptr_ IWDTFActions2 ** ppActions)
	{
		return CreateParentsOnlyItemsActionsFromActions(m_pActions,ppActions);
	}



	CComPtr<IWDTFActions2> m_pActions;
	CCOMERROR m_Error;

};


//
//  WDTF Action Interface specific error codes.
//

#define E_WDTF_ACTION_DEVICE_NOT_CONFIGURED_PROPERLY_FOR_PLUGIN     MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x200)
