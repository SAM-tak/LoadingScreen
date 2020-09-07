#include "ILoadingScreenModule.h"
#include "LoadingScreenSettings.h"
#include "SSimpleLoadingScreen.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

#include "Runtime/Core/Public/Logging/LogMacros.h"

#define LOCTEXT_NAMESPACE "LoadingScreen"

DECLARE_LOG_CATEGORY_EXTERN(LogLoadingScreen, Verbose, All);
DEFINE_LOG_CATEGORY(LogLoadingScreen);

class FLoadingScreenModule : public ILoadingScreenModule
{
public:
	FLoadingScreenModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void TestLoadingScreen(const FLoadingScreenDescription& ScreenDescription) override;

private:
	void HandlePrepareLoadingScreen();

	void HandleMovieClipFinished(const FString& FinishedClip);

	void BeginLoadingScreen(const FLoadingScreenDescription& ScreenDescription);

	TSharedPtr<class SSimpleLoadingScreen> WidgetLoadingScreen;
};

IMPLEMENT_MODULE(FLoadingScreenModule, LoadingScreen)

FLoadingScreenModule::FLoadingScreenModule()
{
}

void FLoadingScreenModule::StartupModule()
{
	if ( !IsRunningDedicatedServer() && FSlateApplication::IsInitialized() )
	{
		// Load for cooker reference
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();
		for ( const FStringAssetReference& Ref : Settings->StartupScreen.Images )
		{
			Ref.TryLoad();
		}
		for ( const FStringAssetReference& Ref : Settings->DefaultScreen.Images )
		{
			Ref.TryLoad();
		}

		if ( IsMoviePlayerEnabled() )
		{
			// Binds the delegate to auto fire the loading screen code when a level changes and when a movie finishes
			GetMoviePlayer()->OnPrepareLoadingScreen().AddRaw(this, &FLoadingScreenModule::HandlePrepareLoadingScreen);
		}

		// Prepare the startup screen, the PrepareLoadingScreen callback won't be called
		// if we've already explictly setup the loading screen.
		BeginLoadingScreen(Settings->StartupScreen);
	}
}

void FLoadingScreenModule::ShutdownModule()
{
	if ( !IsRunningDedicatedServer() )
	{
		if (WidgetLoadingScreen)
		{
			WidgetLoadingScreen.Reset();
		}
		GetMoviePlayer()->OnPrepareLoadingScreen().RemoveAll(this);
	}
}


void FLoadingScreenModule::TestLoadingScreen(const FLoadingScreenDescription& ScreenDescription)
{
	UE_LOG(LogLoadingScreen, Verbose, TEXT("TestLoadingScreen"));
	if (FSlateApplication::IsInitialized())
	{
		auto widgetLoadingScreen = SNew(SSimpleLoadingScreen, ScreenDescription)
								  .bShowThrobber(ScreenDescription.Throbber.bShowThrobber)
								  .ThrobberType(ScreenDescription.Throbber.ThrobberType);
		widgetLoadingScreen->HandleMoviesFinishedPlaying();
		FSlateApplication::Get().GetGameViewport()->SetContent(widgetLoadingScreen);
	}
}

void FLoadingScreenModule::HandlePrepareLoadingScreen()
{
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();
	BeginLoadingScreen(Settings->DefaultScreen);
}

void FLoadingScreenModule::HandleMovieClipFinished(const FString & FinishedClip)
{
	// If its not the last movie then try keep waiting
	if (!GetMoviePlayer()->IsLastMovieInPlaylist())
	{
		return;
	}

	// Unbind the delegate so we're not firing this multiple times
	GetMoviePlayer()->OnMovieClipFinished().RemoveAll(this);
	
	// Show the loading screen widget	
	if (WidgetLoadingScreen)
	{
		WidgetLoadingScreen->HandleMoviesFinishedPlaying();
	}
}

void FLoadingScreenModule::BeginLoadingScreen(const FLoadingScreenDescription& ScreenDescription)
{
	if (WidgetLoadingScreen)
	{
		WidgetLoadingScreen.Reset();
	}

	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.MinimumLoadingScreenDisplayTime = ScreenDescription.MinimumLoadingScreenDisplayTime;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = ScreenDescription.bAutoCompleteWhenLoadingCompletes;
	LoadingScreen.bMoviesAreSkippable = ScreenDescription.bMoviesAreSkippable;
	LoadingScreen.bWaitForManualStop = ScreenDescription.bWaitForManualStop;
	LoadingScreen.MoviePaths = ScreenDescription.MoviePaths;
	LoadingScreen.PlaybackType = ScreenDescription.PlaybackType;


	if (ScreenDescription.bShowWidget)
	{
		// Create and store widget
		WidgetLoadingScreen = SNew(SSimpleLoadingScreen, ScreenDescription)
			.bShowThrobber(ScreenDescription.Throbber.bShowThrobber)
			.ThrobberType(ScreenDescription.Throbber.ThrobberType)
			;
		LoadingScreen.WidgetLoadingScreen = WidgetLoadingScreen;
	}

	const bool IsPlayingValidMovies = LoadingScreen.MoviePaths.Num() != 0;
	// Incase we have no movie paths, this will force it to show the loading screen anyway
	if (!IsPlayingValidMovies)
	{
		// Forces the movie player to create a movie streamer to actually show the widget and such
		LoadingScreen.MoviePaths.Add("");			
	}
	// If we have movies to show, then setup what happens if we're supposed to show ui otherwise skip this
	else
	{
		// Edgecase
		GetMoviePlayer()->OnMovieClipFinished().RemoveAll(this);
		
		GetMoviePlayer()->OnMovieClipFinished().AddRaw(this, &FLoadingScreenModule::HandleMovieClipFinished);
	}

	// This happens last after everything has been prepared ahead of time
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

	if (!IsPlayingValidMovies)
	{
		if (WidgetLoadingScreen)
		{
			WidgetLoadingScreen->HandleMoviesFinishedPlaying();
		}
	}
}

#undef LOCTEXT_NAMESPACE
