#include "LoadingScreenFunctionLibrary.h"
#include "ILoadingScreenModule.h"

FLoadingScreenDescription ULoadingScreenFunctionLibrary::GetStartupLoadingScreen()
{
	return GetDefault<ULoadingScreenSettings>()->StartupScreen;
}

FLoadingScreenDescription ULoadingScreenFunctionLibrary::GetDefaultLoadingScreen()
{
	return GetDefault<ULoadingScreenSettings>()->DefaultScreen;
}

void ULoadingScreenFunctionLibrary::SetLoadingScreen(FLoadingScreenDescription InDescription)
{
	GetMutableDefault<ULoadingScreenSettings>()->DefaultScreen = InDescription;
}

void ULoadingScreenFunctionLibrary::TestLoadingScreen(FLoadingScreenDescription InDescription)
{
	ILoadingScreenModule& LoadingScreenModule = ILoadingScreenModule::Get();
	LoadingScreenModule.TestLoadingScreen(InDescription);
}
