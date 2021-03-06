#include "LoadingScreenSettings.h"
#include "Styling/CoreStyle.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "LoadingScreen"

FLoadingScreenSlotPosition::FLoadingScreenSlotPosition()
	: Anchors(0.5f)
	, Offset(FVector::ZeroVector)
	, Alignment(FVector::ZeroVector)
{
}

FLoadingScreenSlotText::FLoadingScreenSlotText()
	: bShouldShowText(true)
	, TextJustification(ETextJustify::Center)
	, WrapAt(1000.0f)
	, TextColor(FSlateColor(FLinearColor::White))
{
	if (!IsRunningDedicatedServer())
	{
		// TODO (break Hot-reload)
		//static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		//Font = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Normal"));
	}
}

FLoadingScreenText::FLoadingScreenText()
	: SlotText(FLoadingScreenSlotText())
	, SlotPosition(FLoadingScreenSlotPosition())
{
}

FLoadingScreenThrobber::FLoadingScreenThrobber()
	: bShowThrobber(true)
	, ThrobberType(EThrobberLoadingType::TLT_Regular)
	, bFlipThrobberAnimation(false)
	, NumPiecesThrobber(6)
	, Image(*FCoreStyle::Get().GetBrush("Throbber.Chunk"))
	, SlotPosition(FLoadingScreenSlotPosition())
	, AnimateHorizontally(true)
	, AnimateVertically(true)
	, AnimateOpacity(true)
	, ThrobberPeriod(0.75f)
	, ThrobberRadius(16.0f)
	, ImageBrush(*FCoreStyle::Get().GetDefaultBrush())
	, ImageColorAndOpacity(FLinearColor::White)
{
}

FLoadingScreenTips::FLoadingScreenTips()
	: SlotText(FLoadingScreenSlotText())
	, SlotPosition(FLoadingScreenSlotPosition())
	, TimeBetweenTips(0)
{
}

FLoadingScreenDescription::FLoadingScreenDescription()
	: bShowWidget(true)
	, MinimumLoadingScreenDisplayTime(-1.0f)
	, bAutoCompleteWhenLoadingCompletes(true)
	, bMoviesAreSkippable(true)
	, bWaitForManualStop(false)	
	, PlaybackType(EMoviePlaybackType::MT_Looped)
	, bShowUiOverlay(true)
	, bShowUiAfterMovies(true)
	, Throbber(FLoadingScreenThrobber())
	, LoadingScreenText(FLoadingScreenText())
	, LoadingScreenDescription(FLoadingScreenText())
	, LoadingScreenTips(FLoadingScreenTips())
	, bShowImagesAfterMovies(true)
	, ImageStretch(EStretch::ScaleToFit)
{
	LoadingScreenText.SlotPosition.Anchors.Maximum.Y = 1.0f;
	LoadingScreenText.SlotPosition.Anchors.Minimum.Y = 1.0f;
	LoadingScreenText.SlotPosition.Offset.Y = -60.0f;
	LoadingScreenText.Text = LOCTEXT("Loading", "LOADING");
	Throbber.SlotPosition.Anchors.Maximum.Y = 1.0f;
	Throbber.SlotPosition.Anchors.Minimum.Y = 1.0f;
	Throbber.SlotPosition.Offset.Y = -40.0f;
}

ULoadingScreenSettings::ULoadingScreenSettings(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
}

#undef LOCTEXT_NAMESPACE
