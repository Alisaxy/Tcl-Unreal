/*
*	 The MIT License (MIT)
*
*	 Copyright (c) 2015 Alisa Dolinsky
*
*	 Permission is hereby granted, free of charge, to any person obtaining a copy
*	 of this software and associated documentation files (the "Software"), to deal
*	 in the Software without restriction, including without limitation the rights
*	 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	 copies of the Software, and to permit persons to whom the Software is
*	 furnished to do so, subject to the following conditions:
*
*	 The above copyright notice and this permission notice shall be included in all
*	 copies or substantial portions of the Software.
*
*	 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	 SOFTWARE.
*/

#include "PhantomGunsDemo.h"
#include "TclUnrealEssentials.h"


UTclUnrealEssentials::UTclUnrealEssentials(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

TSubclassOf<UObject> UTclUnrealEssentials::FindClass(FString Name) {
	auto cls = FindObjectSafe<UClass>(ANY_PACKAGE, *Name);
	if(cls == nullptr || !cls->IsValidLowLevel()) { UE_LOG(LogClass, Warning, TEXT("Tcl warning: a class could not be found by the name of %s"), *Name) }
	return cls;

}
TArray<AActor*> UTclUnrealEssentials::AllActorsOf(UWorld* World, TSubclassOf<AActor> Cls) {
	TArray<AActor*> actors;
	if (!(World == nullptr || Cls == nullptr)) { UGameplayStatics::GetAllActorsOfClass(World, Cls, actors); }
	return actors;

}
Tcl_Obj* UTclUnrealEssentials::LineTraceSingleByChannel(UWorld* World, FVector Start, FVector End, int32 TraceChannelAsInt32) {
	FHitResult Result;
	auto Hit = World != nullptr && World->LineTraceSingleByChannel(Result, Start, End, TEnumAsByte<ECollisionChannel>(TraceChannelAsInt32));
	FVector ImpactPoint = Result.ImpactPoint;
	AActor* Actor = Result.Actor.Get(false);
	return UTclComponent::pack(Hit, ImpactPoint, Actor);

}
Tcl_Obj* UTclUnrealEssentials::FindComponentsOfByTag(AActor* Actor, TSubclassOf<UActorComponent> Cls, FName Tag) {
	auto Components = Actor == nullptr? TArray<UActorComponent*>() : Actor->GetComponentsByTag(Cls, Tag);
	return UTclComponent::convert(Components);

}
Tcl_Obj* UTclUnrealEssentials::Purge(AActor* Actor) {
	Tcl_Obj* obj = nullptr;
	UTclComponent* comp = nullptr;
	if(Actor != nullptr) { comp = Actor->FindComponentByClass<UTclComponent>(); } else { return obj; }
	if(comp != nullptr) { obj = comp->Purge(); }
	return obj;

}
int32 UTclUnrealEssentials::Eval(AActor* Actor, FString Filename, FString Code, Tcl_Obj* Obj) {
	UTclComponent* comp = nullptr;
	if(Actor != nullptr) { comp = Actor->FindComponentByClass<UTclComponent>(); } else { return TCL_ERROR; }
	if (comp == nullptr) { return TCL_ERROR; } else {
		comp->Fill(Obj);
		auto status = comp->Eval(Filename, Code);
		return status;
	}

}
void UTclUnrealEssentials::PrintString(FString name, int32 type) {
	switch (type) {
		case 1: UE_LOG(LogClass, Warning, TEXT("%s"), *name); break;
		case 2: UE_LOG(LogClass, Error, TEXT("%s"), *name); break;
		default: UE_LOG(LogClass, Log, TEXT("%s"), *name);
	}
}
FString UTclUnrealEssentials::TypeOf(Tcl_Obj* obj) {
	if (obj == nullptr) { return "nullptr"; }
	if (obj->typePtr == nullptr) { return "unknown"; }
	return obj->typePtr->name;

}
Tcl_Obj* UTclUnrealEssentials::SweepSingleByChannel(UWorld* World, FVector Start, FVector End, FQuat Rot, FCollisionShape Shape, int32 TraceChannelAsInt32) {
	FHitResult OutHit;
	auto Hit = World != nullptr && World->SweepSingleByChannel(OutHit, Start, End, Rot, TEnumAsByte<ECollisionChannel>(TraceChannelAsInt32), Shape, FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam);
	FVector ImpactPoint = OutHit.ImpactPoint;
	return UTclComponent::pack(Hit, ImpactPoint);

}