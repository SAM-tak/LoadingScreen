#pragma once

#include "Modules/ModuleManager.h"

/**
 * The public interface to this module
 */
class ILoadingScreenModule : public IModuleInterface
{
public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline ILoadingScreenModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ILoadingScreenModule>("LoadingScreen");
	}

	/**
	 * Kicks off the loading screen for in game loading (not startup)
	 * NOTE : test purpose
	 */
	virtual void TestLoadingScreen(const FLoadingScreenDescription& ScreenDescription) = 0;

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "LoadingScreen" );
	}
};
