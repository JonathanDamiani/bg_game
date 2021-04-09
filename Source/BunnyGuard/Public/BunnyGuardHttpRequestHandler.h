// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "PlayerInfo.h"
#include "Characters/Player/PlayerBase.h"
#include "BGGameInstance.h"
#include "Characters/Player/PlayerStateBase.h"
#include "EngineUtils.h"

class BUNNYGUARD_API BunnyGuardHttpRequestHandler
{
public:
	static FString GetRequestURL()
	{
		FString RequestUrl = FString("https://bunnyguard-288422.uc.r.appspot.com/api");

		#if WITH_EDITOR
		RequestUrl = FString("localhost:3002/api");
		#endif

		return RequestUrl;
	}

	static TSharedRef<IHttpRequest> CreateRequest(FString Type, FString Path, FString Data, std::function<void(FHttpRequestPtr, FHttpResponsePtr, bool)> CallBack)
	{
		TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb(Type);
		HttpRequest->SetHeader("Content-Type", "application/json");
		FString RequestUrl = GetRequestURL().Append(Path);
		HttpRequest->SetURL(RequestUrl);
		HttpRequest->SetContentAsString(Data);
		HttpRequest->OnProcessRequestComplete().BindLambda(CallBack);
		HttpRequest->ProcessRequest();

		return HttpRequest;
	}

	static void SendGameStart(int32& TelemetryId, FAllServeInfo& ServerInfo)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetStringField(TEXT("name"), ServerInfo.ServerName);

		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		CreateRequest("POST", "/telemetry/session/create", OutputString,
			[&TelemetryId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				UE_LOG(LogTemp, Warning, TEXT("Posted successfully"));
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

				FJsonSerializer::Deserialize(JsonReader, JsonObject);
				TelemetryId = JsonObject->GetIntegerField("id");
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Coudldn't send to server"));
			}
		});
	}

	static void SendPlayerCreate(int32& TelemetryId, int32 SessionId, FText NetPlayerName, int8 ClassSelected)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetNumberField(TEXT("id"), SessionId);
		JsonObject->SetStringField(TEXT("playerId"), NetPlayerName.ToString());
		JsonObject->SetNumberField(TEXT("playerClass"), static_cast<uint8>(ClassSelected));

		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		CreateRequest("POST", "/telemetry/player/create", OutputString,
			[&TelemetryId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				UE_LOG(LogTemp, Warning, TEXT("Posted successfully"));
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

				FJsonSerializer::Deserialize(JsonReader, JsonObject);
				TelemetryId = JsonObject->GetIntegerField("id");
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Coudldn't send to server"));
			}
		});
	}

	static void SendGameEnd(int32 TelemetryId, const bool bIsVictory)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetNumberField(TEXT("id"), TelemetryId);
		JsonObject->SetBoolField(TEXT("wasVictorious"), bIsVictory);
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		CreateRequest("POST", "/telemetry/session/finalize", OutputString, [](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {});
	}

	static void SendCombatStart(int32 TelemetryId)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetNumberField(TEXT("id"), TelemetryId);
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		CreateRequest("POST", "/telemetry/combat/start", OutputString, [](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {});
	}

	static void SendCombatEnd(int32 TelemetryId, float DamageTaken)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetNumberField(TEXT("id"), TelemetryId);
		JsonObject->SetNumberField(TEXT("damageTaken"), DamageTaken);

		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		CreateRequest("POST", "/telemetry/combat/end", OutputString, [](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {});
	}

	static void NotifyAbilityUse(int32 TelemetryId, EAbilityType UsedAbility)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetNumberField(TEXT("id"), TelemetryId);
		JsonObject->SetNumberField(TEXT("abilityType"), static_cast<int8>(UsedAbility));

		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
		CreateRequest("POST", "/telemetry/abilities/", OutputString, [](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {});
	}
};
