#include <PDS/Player/MainPawn.h>

#include <PDS/Player/PDSPlayerController.h>

#include <Components/InputComponent.h>
#include <Components/SphereComponent.h>

DEFINE_LOG_CATEGORY_STATIC(MainPawn, Display, All)

AMainPawn::AMainPawn()
{
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMainPawn::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	// Super::SetupPlayerInputComponent(playerInputComponent);

	playerInputComponent->BindAction(TEXT("SelectOrAct"), EInputEvent::IE_Released, this, &AMainPawn::SelectOrAct);
	playerInputComponent->BindAction(TEXT("CancelOrder"), EInputEvent::IE_Released, this, &AMainPawn::CancelOrder);
	playerInputComponent->BindAction(TEXT("ReadyForNextTurn"), EInputEvent::IE_Released, this, &AMainPawn::ReadyForNextTurn);
	playerInputComponent->BindAxis(TEXT("MoveNorth"), this, &AMainPawn::MoveNorth);
	playerInputComponent->BindAxis(TEXT("MoveEast"), this, &AMainPawn::MoveEast);
	playerInputComponent->BindAction(TEXT("MoveUp"), EInputEvent::IE_Released, this, &AMainPawn::MoveUp);
	playerInputComponent->BindAction(TEXT("MoveDown"), EInputEvent::IE_Released, this, &AMainPawn::MoveDown);
}

void AMainPawn::SelectOrAct()
{
	const auto controller = Cast<APDSPlayerController>(GetController());
	auto hitResult = FHitResult{};
	const auto order = controller->GetPartialOrder();

	if (!controller->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, hitResult))
		return;

	const auto oHex = controller->GetPosUnderCursor();
	if (!oHex)
	{
		if (order == EOrders::None)
			controller->ResetSelection();
		return;
	}

	const auto hex = oHex.GetValue();
	UE_LOG(MainPawn, Display, TEXT("Clicked on hex: (%d;%d)"), hex.q(), hex.r());

	if (order == EOrders::None)
	{
		controller->SelectHex(hex);
		return;
	}

	const auto targetType = GetOrderTargetingType(controller->GetPartialOrder());

	switch (targetType)
	{
	case EOrderTargetingType::EmptyHex:
	{
		if (!controller->ValidateOrder(hex))
		{
			// Failure sound/message
			return;
		}

		controller->FinishOrder(hex);
		return;
	}
	default:
		checkNoEntry();
	}
}

void AMainPawn::CancelOrder()
{
	const auto controller = Cast<APDSPlayerController>(GetController());
	controller->CancelOrder();
}

void AMainPawn::ReadyForNextTurn()
{
	const auto controller = Cast<APDSPlayerController>(GetController());
	controller->FinishTurn();
}

void AMainPawn::MoveNorth(float delta)
{
	if (delta == 0.f)
		return;
	AddMovementInput(FVector{ 1, 0, 0, }, delta);
}

void AMainPawn::MoveEast(float delta)
{
	if (delta == 0.f)
		return;
	AddMovementInput(FVector{ 0, 1, 0, }, delta);
}

void AMainPawn::MoveUp()
{
	AddMovementInput(FVector{ 0, 0, 1, }, 200);
}

void AMainPawn::MoveDown()
{
	AddMovementInput(FVector{ 0, 0, 1, }, -200);
}
