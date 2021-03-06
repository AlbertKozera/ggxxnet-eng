//--------------------------------------------------------------------------------------
// File: Firewall.cpp
//
// Demonstrate the use of the XP/SP2 firewall API
// It requires the latest Microsoft Platform SDK to compile because it uses the Firewall
// API introduced with Windows XP SP2.
//
// Copyright (c) 2003-2004 Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------

#define _WIN32_DCOM
#include <windows.h>
#include <crtdbg.h>
#include <netfw.h>
#include <stdio.h>
#include <conio.h>




class FirewallWrapper 
{
        INetFwProfile*  m_pFwProfile;
        HRESULT         m_hrComInit;

        HRESULT InitFirewallProfile();
        FirewallWrapper();
    
    public:

        static FirewallWrapper* Create();
        ~FirewallWrapper();

        BOOL    FirewallPresent() 
            { return m_pFwProfile != NULL; } 

        BOOL    IsFirewallActive();
        HRESULT AddAuthorizedApp(
            IN const wchar_t* szFwProcessImageFileName,
            IN const wchar_t* szFwFriendlyName
            );
        HRESULT RemoveAuthorizedApp(
            IN const wchar_t* szFwProcessImageFileName
            );
        BOOL IsAppEnabled( 
            IN const wchar_t* szFwProcessImageFileName
            );
        BOOL AreExceptionsAllowed();
};


        
        
//--------------------------------------------------------------------------------------
// FirewallWrapper()
//      Initialize Com and fetch a firewall profile
//--------------------------------------------------------------------------------------
FirewallWrapper::FirewallWrapper()
    : m_pFwProfile(NULL)
{}




//--------------------------------------------------------------------------------------
// ~FirewallWrapper()
//      Release the firewall profile and uninitialize COM
//--------------------------------------------------------------------------------------
FirewallWrapper::~FirewallWrapper()
{
    // Release the firewall profile.
    if( m_pFwProfile != NULL )
    {
        m_pFwProfile->Release();
    }
}




//--------------------------------------------------------------------------------------
// FirewallWrapper::Create()
//--------------------------------------------------------------------------------------
FirewallWrapper* FirewallWrapper::Create()
{
    FirewallWrapper* pfw = new FirewallWrapper;
    if( pfw )
    {
        HRESULT hr = pfw->InitFirewallProfile();
        if( !pfw->FirewallPresent() )
        {
            hr;
            // printf( "Failed to initialize firewall profile: 0x%08lx\n", hr );
            delete pfw;
            pfw = NULL;
        }
    }
    return pfw;
}




//--------------------------------------------------------------------------------------
// InitFirewallProfile()
//      Access the firewall and policy to retrieve the firewall profile.  
//          Initialize COM  
//          Retrieve the firewall profile
//--------------------------------------------------------------------------------------
HRESULT FirewallWrapper::InitFirewallProfile()
{
    HRESULT         hr = S_OK;
    INetFwMgr*      pFwMgr = NULL;
    INetFwPolicy*   pFwPolicy = NULL;

    m_pFwProfile = NULL;

    // Create an instance of the firewall settings manager.
    hr = CoCreateInstance(
            __uuidof(NetFwMgr),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwMgr),
            (void**)&pFwMgr
            );
    _ASSERT( hr != CO_E_NOTINITIALIZED && "COM has not been initialized" );
    if( FAILED(hr) )
    {
        // printf( "CoCreateInstance failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Retrieve the local firewall policy.
    hr = pFwMgr->get_LocalPolicy( &pFwPolicy );
    if( FAILED(hr) )
    {
        // printf( "get_LocalPolicy failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Retrieve the firewall profile currently in effect.
    hr = pFwPolicy->get_CurrentProfile( &m_pFwProfile );
    if( FAILED(hr) )
    {
        // printf( "get_CurrentProfile failed: 0x%08lx\n", hr );
        goto cleanup;
    }

cleanup:

    // Release the local firewall policy.
    if( pFwPolicy != NULL )
    {
        pFwPolicy->Release();
    }

    // Release the firewall settings manager.
    if( pFwMgr != NULL )
    {
        pFwMgr->Release();
    }

    return hr;
}




//--------------------------------------------------------------------------------------
// IsFirewallActive()
//      Test if the firewall is on, return as a boolean  
//--------------------------------------------------------------------------------------
BOOL FirewallWrapper::IsFirewallActive()
{
    HRESULT         hr = S_OK;
    VARIANT_BOOL    vbFwEnabled;

    // Get the current state of the firewall.
    // This requires the latest Microsoft Platform SDK to compile because 
    // it uses the Firewall API introduced with Windows XP SP2.
    hr = m_pFwProfile->get_FirewallEnabled( &vbFwEnabled );  // upgrade Platform SDK if this fails to compile
    if( FAILED(hr) || vbFwEnabled == VARIANT_FALSE )
    {
        return FALSE;
    }

    return TRUE;
}




//--------------------------------------------------------------------------------------
// FirewallWrapper::IsAppEnabled()
//      Determine if the specified application is enabled. Return the 
//      result in pbFWAppEnabled. 
//--------------------------------------------------------------------------------------
BOOL FirewallWrapper::IsAppEnabled(
            IN const wchar_t* szFwProcessImageFileName
            )
{
    HRESULT             hr = S_OK;
    BSTR                fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL        vbFwEnabled;
    INetFwAuthorizedApplication*    pFwApp = NULL;
    INetFwAuthorizedApplications*   pFwApps = NULL;
    BOOL bFwAppEnabled = FALSE;
    
    _ASSERT( szFwProcessImageFileName != NULL );

    // Retrieve the collection of authorized applications.
    hr = m_pFwProfile->get_AuthorizedApplications( &pFwApps );
    if( FAILED(hr) )
    {
        // printf( "get_AuthorizedApplications failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString( szFwProcessImageFileName );
    if( SysStringLen( fwBstrProcessImageFileName ) == 0 )
    {
        hr = E_OUTOFMEMORY;
        // printf( "SysAllocString failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Attempt to retrieve the authorized application.
    hr = pFwApps->Item( fwBstrProcessImageFileName, &pFwApp );
    if( SUCCEEDED(hr ) )
    {
        // Find out if the authorized application is enabled.
        hr = pFwApp->get_Enabled( &vbFwEnabled );
        if( FAILED(hr) )
        {
            // printf( "get_Enabled failed: 0x%08lx\n", hr );
            goto cleanup;
        }

        if( vbFwEnabled != VARIANT_FALSE )
        {
            // The authorized application is enabled.
            bFwAppEnabled = TRUE;

            // printf(
            //     "Authorized application %lS is enabled in the firewall.\n",
            //     szFwProcessImageFileName
            //     );
        }
        else
        {
            // printf(
            //     "Authorized application %lS is disabled in the firewall.\n",
            //     szFwProcessImageFileName
            //     );
        }
    }
    else
    {
        // The authorized application was not in the collection.
        hr = S_OK;

        // printf(
        //     "Authorized application %lS is disabled in the firewall.\n",
        //     szFwProcessImageFileName
        //     );
    }

cleanup:

    // Free the BSTR.
    SysFreeString( fwBstrProcessImageFileName );

    // Release the authorized application instance.
    if( pFwApp != NULL )
    {
        pFwApp->Release();
    }

    // Release the authorized application collection.
    if( pFwApps != NULL )
    {
        pFwApps->Release();
    }

    return bFwAppEnabled;
}




//--------------------------------------------------------------------------------------
// ExceptionsAllowed()
//      Find out if the system is in no-exceptions mode   
//--------------------------------------------------------------------------------------
BOOL FirewallWrapper::AreExceptionsAllowed()
{
    VARIANT_BOOL        vbNotAllowed = VARIANT_FALSE;
    HRESULT             hr = S_OK;

    hr = m_pFwProfile->get_ExceptionsNotAllowed( &vbNotAllowed );
    if( SUCCEEDED(hr) && vbNotAllowed != VARIANT_FALSE )
    {
        return FALSE;
    }
    
    return TRUE; 
}




//--------------------------------------------------------------------------------------
// AddAuthorizedApp()
//      Add an application to the exception list (aka AuthorizedApplication list)  
//--------------------------------------------------------------------------------------
HRESULT FirewallWrapper::AddAuthorizedApp(
            IN const wchar_t* szFwProcessImageFileName,
            IN const wchar_t* szFwFriendlyName
            )
{
    HRESULT             hr = S_OK;
    BSTR                fwBstrName = NULL;
    BSTR                fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication*    pFwApp = NULL;
    INetFwAuthorizedApplications*   pFwApps = NULL;

    _ASSERT( szFwProcessImageFileName != NULL );
    _ASSERT( szFwFriendlyName != NULL );

    // Retrieve the authorized application collection.
    hr = m_pFwProfile->get_AuthorizedApplications( &pFwApps );
    if( FAILED(hr) )
    {
        // printf( "get_AuthorizedApplications failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Create an instance of an authorized application.
    hr = CoCreateInstance(
            __uuidof(NetFwAuthorizedApplication),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwAuthorizedApplication),
            (void**)&pFwApp
            );
    if( FAILED(hr) )
    {
        // printf( "CoCreateInstance failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString( szFwProcessImageFileName );
    if( SysStringLen( fwBstrProcessImageFileName ) == 0 )
    {
        hr = E_OUTOFMEMORY;
        // printf( "SysAllocString failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Set the process image file name.
    hr = pFwApp->put_ProcessImageFileName( fwBstrProcessImageFileName );
    if( FAILED(hr) )
    {
        // printf( "put_ProcessImageFileName failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Allocate a BSTR for the application friendly name.
    fwBstrName = SysAllocString( szFwFriendlyName );
    if( SysStringLen( fwBstrName ) == 0 )
    {
        hr = E_OUTOFMEMORY;
        // printf( "SysAllocString failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Set the application friendly name.
    hr = pFwApp->put_Name( fwBstrName );
    if( FAILED(hr) )
    {
        // printf( "put_Name failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Add the application to the collection.
    hr = pFwApps->Add( pFwApp );
    if( FAILED(hr) )
    {
        // printf( "Add failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // printf(
    //     "Authorized application %lS is now enabled in the firewall.\n",
    //     szFwProcessImageFileName
    //     );

cleanup:

    // Free the BSTRs.
    SysFreeString( fwBstrName );
    SysFreeString( fwBstrProcessImageFileName );

    // Release the authorized application instance.
    if( pFwApp != NULL )
    {
        pFwApp->Release();
    }

    // Release the authorized application collection.
    if( pFwApps != NULL )
    {
        pFwApps->Release();
    }

    return hr;
}




//--------------------------------------------------------------------------------------
// RemoveAuthorizedApp()
//      Remove an application from the exception list (aka AuthorizedApplication list)  
//--------------------------------------------------------------------------------------
HRESULT FirewallWrapper::RemoveAuthorizedApp(
            IN const wchar_t* szFwProcessImageFileName
            )
{
    HRESULT             hr = S_OK;
    BSTR                fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplications*   pFwApps = NULL;

    _ASSERT( szFwProcessImageFileName != NULL );

    // Retrieve the authorized application collection.
    hr = m_pFwProfile->get_AuthorizedApplications( &pFwApps );
    if( FAILED(hr) )
    {
        // printf( "get_AuthorizedApplications failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString( szFwProcessImageFileName );
    if( SysStringLen( fwBstrProcessImageFileName ) == 0 )
    {
        hr = E_OUTOFMEMORY;
        // printf( "SysAllocString failed: 0x%08lx\n", hr );
        goto cleanup;
    }


    // Remove the application from the collection.
    hr = pFwApps->Remove( fwBstrProcessImageFileName );
    if( FAILED(hr) )
    {
        // printf( "Remove failed: 0x%08lx\n", hr );
        goto cleanup;
    }

    // printf(
    //     "Authorized application %lS is now removed from the firewall.\n",
    //     szFwProcessImageFileName
    //     );

cleanup:

    // Free the BSTRs.
    SysFreeString( fwBstrProcessImageFileName );

    // Release the authorized application collection.
    if( pFwApps != NULL )
    {
        pFwApps->Release();
    }

    return hr;
}




//--------------------------------------------------------------------------------------
// OnInstallApplication() 
// Add the application to the exception (aka authorized) list.
// Returns true if we've been added.
//--------------------------------------------------------------------------------------
BOOL OnInstallApplication(
            IN const wchar_t* szFwProcessImageFileName,
            IN const wchar_t* szFwFriendlyName
            )
{
    FirewallWrapper* pfw = FirewallWrapper::Create();
    if( !pfw )
        return FALSE; 

    HRESULT hr = pfw->AddAuthorizedApp(            
        szFwProcessImageFileName,
        szFwFriendlyName
        );
    return SUCCEEDED(hr);
}




//--------------------------------------------------------------------------------------
// OnUninstallApplication()
//      Remove the application from the exception list.
//--------------------------------------------------------------------------------------
BOOL OnUninstallApplication(
            IN const wchar_t* szFwProcessImageFileName
            )
{
    FirewallWrapper* pfw = FirewallWrapper::Create();
    if( !pfw )
        return FALSE; 
    
    HRESULT hr = pfw->RemoveAuthorizedApp( 
        szFwProcessImageFileName
        );
    return SUCCEEDED(hr);
}




//--------------------------------------------------------------------------------------
// CanHostMultiplayer() 
//      Check that the firewall is properly configured for the game. 
//      Returns FALSE if the firewall is configured to block us from hosting. 
//--------------------------------------------------------------------------------------
BOOL CanHostMultiplayer(
            IN const wchar_t* szFwProcessImageFileName
            )
{
    FirewallWrapper* pfw = FirewallWrapper::Create();
    if( !pfw )
        return TRUE; 
    
    if( !pfw->IsAppEnabled(szFwProcessImageFileName) )
    {
        printf( "Application is not enabled in the firewall. You will not be able host games or join a peer-to-peer game. Depending on the game, you may be able to join.\n" );
        return FALSE;
    }

    if( !pfw->AreExceptionsAllowed() )
    {
        printf( "Firewall is on with no exceptions. You will not be able host games or join a peer-to-peer game. Depending on the game, you may be able to join.\n" );
        return FALSE;
    }

    return TRUE; 
}




//--------------------------------------------------------------------------------------
// wmain()
// Run the two basic scenarios:
//          When installing, add the game to the firewall 
//          When launching multiplayer, check firewall settings 
//--------------------------------------------------------------------------------------
int __cdecl main( int argc, wchar_t* argv[] )
{

    HRESULT hrComInit = CoInitializeEx(
                 0,
                 COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
                 );
    if( hrComInit != RPC_E_CHANGED_MODE && FAILED(hrComInit) )
    {
        printf( "Failed to init COM: 0x%08lx\n", hrComInit );
        return 0; 
    }

    //--------------------------------------------------------------------------------------
    // First scenario:  During the install process
    // Add the game executable to the exception (aka authorized) list.
    // Note not to add "setup.exe" to the exception list, instead add main game executable.
    //--------------------------------------------------------------------------------------
    WCHAR strGameExeFullPath[MAX_PATH];  

    // TODO: change this and make sure this isn't strGameExeFullPath isn't "setup.exe"
    // but instead your main game executable
    GetModuleFileName( NULL, strGameExeFullPath, MAX_PATH ); 

    printf( "First scenario during install process:\n" );
    printf( "Add the application to the exception (aka authorized) list.\n" );
    printf( "Note: change the source in this sample to match your game executable name.\n" );

    WCHAR* strFriendlyAppName = L"Firewall Sample";  // TODO: Change this 
    BOOL bSuccess = OnInstallApplication( strGameExeFullPath, strFriendlyAppName );
    if( !bSuccess )
    {
        printf( "Could not add app to the exception list. "
                "If we're running on an OS older than XPSP2 this is normal.\n" );
    }
    else
    {
        printf( "Success!\n" );
    }

    //--------------------------------------------------------------------------------------
    // Second scenario:  When we launch multiplayer
    // Check firewall state to ensure the app is still ready to go 
    //--------------------------------------------------------------------------------------
    printf( "\nSecond scenario when launching multiplayer:\n" );
    printf( "Check firewall state to ensure the app is still ready to go.\n" );
    if( CanHostMultiplayer( strGameExeFullPath ) )
    {
        printf( "Success!\n" );
    }
    else
    {
        printf( "Failed!\n" );
    }


    //--------------------------------------------------------------------------------------
    // Last scenario: During the Uninstall process
    // Remove the application from the exception list.
    //--------------------------------------------------------------------------------------
    printf( "\nLast scenario during uninstall process:\n" );
    printf( "Remove the application from the exception list.\n" );
    bSuccess = OnUninstallApplication( strGameExeFullPath );
    if( !bSuccess )
    {
        printf( "Failed to remove app from to the exception list. \n" );
    }
    else
    {
        printf( "Success!\n" );
    }

    // Uninitialize COM.
    if( SUCCEEDED(hrComInit) )
    {
        CoUninitialize();
    }
 
    printf( "\nPress a key to exit\n" );
    _getch();

    return 0;
}

