#include "sdk.h"

__forceinline unsigned long __stdcall cheat_thread( _In_ LPVOID reserved ) {
	g_cl.init( );

	while ( !( GetAsyncKeyState( VK_DELETE ) ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	g_cl.unload( );

	FreeLibraryAndExitThread( static_cast< HMODULE >( reserved ), 0 );
}

BOOL WINAPI DllMain( _In_ HMODULE hmodule, _In_ DWORD reason, _In_ LPVOID reserved ) {
	if ( reason == DLL_PROCESS_ATTACH ) {
		DisableThreadLibraryCalls( hmodule );
		if ( auto* const thread_handle = CreateThread( nullptr, 0, cheat_thread, hmodule, 0, nullptr ) )
			CloseHandle( thread_handle );
	}

	return TRUE;
}