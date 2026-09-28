# Authored Acoustic System

This is a task execution manual, not a request to implement everything at once.

For every numbered task:
1. Give the task's copy-paste prompt to the implementation AI.
2. Require it to inspect the current repository before editing.
3. Require tests and a real UE build or packaged run appropriate to the task.
4. Do not advance until every listed done criterion passes.
5. Commit only after Andrew reviews the diff and test output.

Locked naming used by this manual:
- Project/module: `DrownedFrequency`
- C++ class prefix: `TDF`
- Content root: `Content/Game`
- Source root: `Source/DrownedFrequency`
- Sound categories: Voice, Footstep, Machine, Bell/Foghorn, Water, Static, Impact

- What i did: Designed a granular authored acoustic-system implementation sequence with C++ shapes, editor assets, tests, done criteria, pitfalls, and copy-paste AI prompts for each small task.
- Files created or modified: []
- Issues encountered: []
## Non negotiable constraints
- No real microphone recording.
- No waveform analysis.
- All sounds are authored clips: clean, degraded, and contaminated variants.
- Exactly seven acoustic categories: Voice, Footstep, Machine, Bell, Static, Impact, Anomaly.
- Recorder slot count progression is 3 -> 4 -> 5.
- Every player-made noise is emitted as a first-class FAcousticStimulus.
- Deleting recordings cannot soft-lock receiver puzzles.
- UE5-stable architecture: C++ mechanics, Blueprint presentation, Data Assets for tuning.

## Implementation sequence

### A01 — Create acoustic gameplay tag taxonomy

- Id: A01
- Title: Create acoustic gameplay tag taxonomy
- Goal: Define all gameplay tags used by authored recording, anomaly playback, routing, puzzle receivers, captions, and debugging.
#### Primary files
- Config/DefaultGameplayTags.ini
- Source/<Game>/Public/Acoustics/AcousticTypes.h

- Editor assets: []
- Cpp shape: GameplayTags:
Acoustic.Category.Voice
Acoustic.Category.Footstep
Acoustic.Category.Machine
Acoustic.Category.Bell
Acoustic.Category.Static
Acoustic.Category.Impact
Acoustic.Category.Water

Acoustic.Fidelity.Clean
Acoustic.Fidelity.Degraded
Acoustic.Fidelity.Contaminated

Acoustic.Route.Direct
Acoustic.Route.Pipe
Acoustic.Route.Speaker
Acoustic.Route.PuckProjected
Acoustic.Route.Overdub

Acoustic.Puzzle.Required
Acoustic.Puzzle.Optional
Acoustic.Puzzle.Decoy

Acoustic.Debug.Visible
Acoustic.Debug.LogOnly
#### Tests
- Automation test: all required gameplay tags resolve through UGameplayTagsManager.
- Automation test: category tag list contains exactly seven categories.
- Automation test: invalid category tag is rejected by validation helper.

#### Done criteria
- All tags load in editor without warnings.
- Exactly seven category tags exist.
- Validation helper rejects unknown or non-category tags.

#### Common pitfalls
- Do not add ad-hoc category tags later; use Data Asset metadata if a subtype is needed.
- Do not encode category using enums only; gameplay tags are needed for receiver matching and data-driven authoring.
- Do not make Voice imply real mic recording.

- Copy paste ai prompt: Implement acoustic gameplay tags for UE5. Add exactly seven Acoustic.Category tags: Voice, Footstep, Machine, Bell, Static, Impact, Anomaly. Add fidelity, route, puzzle, and debug tags. Create a small C++ validation helper that checks category tags against the fixed set. Add automation tests proving all tags resolve, exactly seven categories exist, and invalid category tags are rejected. Do not implement recording yet. No real mic or waveform analysis.

### A02 — Define core acoustic C++ types

- Id: A02
- Title: Define core acoustic C++ types
- Goal: Create stable structs/enums used by recordings, stimuli, receiver matching, routing, and captions.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticTypes.h
- Source/<Game>/Private/Acoustics/AcousticTypes.cpp

- Editor assets: []
- Cpp shape: UENUM(BlueprintType)
enum class EAcousticFidelity : uint8 { Clean, Degraded, Contaminated };

UENUM(BlueprintType)
enum class ERecordingSlotUnlockTier : uint8 { Tier3 = 3, Tier4 = 4, Tier5 = 5 };

UENUM(BlueprintType)
enum class EAcousticRoute : uint8 { Direct, Pipe, Speaker, PuckProjected, Overdub };

USTRUCT(BlueprintType)
struct FAcousticCaptionLine
{
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Text;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float StartTime = 0.f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration = 2.f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag SpeakerOrSourceTag;
};

USTRUCT(BlueprintType)
struct FAcousticClipVariant
{
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USoundBase> Sound = nullptr;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) EAcousticFidelity Fidelity = EAcousticFidelity::Clean;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float Loudness = 1.f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float EffectiveRadius = 1200.f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FAcousticCaptionLine> Captions;
};
#### Tests
- Automation test: fidelity enum string conversion is stable.
- Automation test: slot unlock tier numeric values are 3, 4, and 5.
- Automation test: default clip variant has safe values and null sound is validation failure.

#### Done criteria
- Types are Blueprint-visible where presentation needs them.
- Types compile in a clean UE5 build.
- Defaults are safe and deterministic.

#### Common pitfalls
- Do not put subsystem logic into type files.
- Avoid UObject ownership inside lightweight transient structs unless references are needed.
- Keep savegame-facing fields versionable.

- Copy paste ai prompt: Create core acoustic types for UE5 in C++. Include EAcousticFidelity, ERecordingSlotUnlockTier with numeric values 3/4/5, EAcousticRoute, FAcousticCaptionLine, and FAcousticClipVariant. Make them Blueprint-friendly but mechanics-first. Add automation tests for enum stability, slot tier values, and safe defaults. No gameplay behavior yet.

### A03 — Implement Audio Anomaly Data Asset

- Id: A03
- Title: Implement Audio Anomaly Data Asset
- Goal: Represent an authored recordable anomaly with category, variants, puzzle metadata, repetition behavior, contamination behavior, captions, and editor validation.
#### Primary files
- Source/<Game>/Public/Acoustics/AudioAnomalyDataAsset.h
- Source/<Game>/Private/Acoustics/AudioAnomalyDataAsset.cpp

#### Editor assets
- Content/Audio/Anomalies/DA_AudioAnomaly_Template.uasset

- Cpp shape: UCLASS(BlueprintType)
class UAudioAnomalyDataAsset : public UPrimaryDataAsset
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Acoustic") FPrimaryAssetId StableId;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Acoustic") FGameplayTag CategoryTag;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Acoustic") TMap<EAcousticFidelity, FAcousticClipVariant> Variants;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle") TArray<FGameplayTag> ReceiverKeys;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules") bool bCanSatisfyReceiver = true;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules") int32 MaxMeaningfulRepetitions = 1;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules") float RepetitionPenaltyPerExtraPlay = 0.25f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules") bool bCanBeContaminated = true;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules") bool bCanBeOverdubbed = true;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug") FLinearColor DebugColor = FLinearColor::Cyan;

#if WITH_EDITOR
  virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
#### Tests
- Editor automation test: valid asset requires exactly one clean variant with non-null sound.
- Editor automation test: degraded and contaminated variants are optional but validated if present.
- Editor automation test: category tag must be one of the seven category tags.
- Editor automation test: receiver-satisfying asset must have at least one ReceiverKey.

#### Done criteria
- Designers can author anomaly assets without C++ changes.
- Invalid assets fail editor validation with actionable messages.
- Clean/degraded/contaminated authored variants are supported.

#### Common pitfalls
- Do not generate degraded audio procedurally from waveform analysis.
- Do not rely on asset display names for puzzle identity; use StableId or PrimaryAssetId.
- Do not let bCanSatisfyReceiver true with empty receiver keys.

- Copy paste ai prompt: Implement UAudioAnomalyDataAsset as a UPrimaryDataAsset for authored acoustic anomalies. It must store a fixed category gameplay tag, clean/degraded/contaminated authored clip variants, receiver keys, repetition settings, contamination/overdub permissions, captions through variants, and debug color. Add editor validation tests. No real microphone, no waveform analysis, no procedural audio degradation.

### A04 — Implement Audio Anomaly Actor

- Id: A04
- Title: Implement Audio Anomaly Actor
- Goal: Place recordable/playable authored anomalies in levels and emit first-class acoustic stimuli when activated.
#### Primary files
- Source/<Game>/Public/Acoustics/AudioAnomalyActor.h
- Source/<Game>/Private/Acoustics/AudioAnomalyActor.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_AudioAnomalyActor'

- Cpp shape: UCLASS()
class AAudioAnomalyActor : public AActor
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAudioAnomalyDataAsset> AnomalyData;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> StimulusOrigin;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAutoEmitOnBeginPlay = false;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float InteractionRecordRange = 250.f;

  UFUNCTION(BlueprintCallable) bool CanBeRecordedBy(AActor* RecorderOwner) const;
  UFUNCTION(BlueprintCallable) FRecordedSignal MakeRecordedSignal(EAcousticFidelity Fidelity) const;
  UFUNCTION(BlueprintCallable) void EmitWorldStimulus(EAcousticRoute Route = EAcousticRoute::Direct);
};
#### Tests
- Automation test: actor with null DataAsset cannot produce a valid recording.
- Automation test: MakeRecordedSignal preserves stable asset identity and category.
- Automation test: EmitWorldStimulus sends FAcousticStimulus to subsystem.

#### Done criteria
- Anomaly actor is placeable and designer-friendly.
- Recording and emission paths use Data Asset identity, not raw audio files.
- Null or invalid assets fail safely.

#### Common pitfalls
- Do not let actor contain duplicate tuning that should live in the Data Asset.
- Do not play local-only audio without also emitting a stimulus when gameplay requires hearing.
- Do not soft-crash on missing asset references.

- Copy paste ai prompt: Implement AAudioAnomalyActor for authored acoustic anomalies. It should reference UAudioAnomalyDataAsset, support CanBeRecordedBy, MakeRecordedSignal, and EmitWorldStimulus. Emission must go through the acoustic subsystem as a first-class FAcousticStimulus. Add tests for null data, signal identity preservation, and subsystem emission.

### A05 — Implement FRecordedSignal

- Id: A05
- Title: Implement FRecordedSignal
- Goal: Create the savegame-safe data unit representing one recorder slot entry.
#### Primary files
- Source/<Game>/Public/Acoustics/RecordedSignal.h
- Source/<Game>/Private/Acoustics/RecordedSignal.cpp

- Editor assets: []
- Cpp shape: USTRUCT(BlueprintType)
struct FRecordedSignal
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) FGuid InstanceGuid;
  UPROPERTY(BlueprintReadOnly) FPrimaryAssetId SourceAnomalyId;
  UPROPERTY(BlueprintReadOnly) FGameplayTag CategoryTag;
  UPROPERTY(BlueprintReadOnly) EAcousticFidelity Fidelity = EAcousticFidelity::Clean;
  UPROPERTY(BlueprintReadOnly) float Contamination = 0.f;
  UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
  UPROPERTY(BlueprintReadOnly) int32 PlayCount = 0;
  UPROPERTY(BlueprintReadOnly) TArray<FPrimaryAssetId> OverdubbedSourceIds;
  UPROPERTY(BlueprintReadOnly) FGameplayTagContainer ReceiverKeysSnapshot;
  UPROPERTY(BlueprintReadOnly) bool bPuzzleEligible = true;

  bool IsValid() const;
  bool CanSatisfyReceiver() const;
  FString ToDebugString() const;
};
#### Tests
- Automation test: default FRecordedSignal is invalid.
- Automation test: valid signal requires guid, source id, category, and puzzle eligibility.
- Automation test: deletion of a signal does not delete source anomaly identity from fallback puzzle state.

#### Done criteria
- Recorded signals are serializable and deterministic.
- Signal references Data Asset identity, not a copied SoundBase pointer only.
- Signal can be inspected in debug UI.

#### Common pitfalls
- Do not store transient actor pointers in FRecordedSignal.
- Do not make puzzle solving depend only on current inventory if deletion could soft-lock.
- Do not use localized caption text as identity.

- Copy paste ai prompt: Implement FRecordedSignal as the savegame-safe representation of one recorder slot. It must store instance guid, source anomaly primary asset id, fixed category tag, fidelity, contamination, generation, play count, overdub source ids, receiver key snapshot, and puzzle eligibility. Add validity helpers and tests. Do not store actor pointers.

### A06 — Implement recorder slot inventory model

- Id: A06
- Title: Implement recorder slot inventory model
- Goal: Provide deterministic slot behavior with unlock progression 3 -> 4 -> 5, overwrite, delete, and no-softlock safety hooks.
#### Primary files
- Source/<Game>/Public/Acoustics/RecordingSlots.h
- Source/<Game>/Private/Acoustics/RecordingSlots.cpp

- Editor assets: []
- Cpp shape: USTRUCT(BlueprintType)
struct FRecorderSlot
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) int32 SlotIndex = INDEX_NONE;
  UPROPERTY(BlueprintReadOnly) bool bUnlocked = false;
  UPROPERTY(BlueprintReadOnly) FRecordedSignal Signal;
};

USTRUCT(BlueprintType)
struct FRecorderSlotSet
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) TArray<FRecorderSlot> Slots;
  void InitializeToTier(ERecordingSlotUnlockTier Tier);
  bool SetUnlockedSlotCount(int32 Count);
  bool PutSignal(int32 SlotIndex, const FRecordedSignal& Signal, FRecordedSignal* OutReplaced);
  bool DeleteSignal(int32 SlotIndex, FRecordedSignal* OutDeleted);
  int32 GetUnlockedSlotCount() const;
};
#### Tests
- Automation test: Tier3 initializes exactly 3 unlocked slots.
- Automation test: Tier4 unlocks exactly 4 slots and preserves prior contents.
- Automation test: Tier5 unlocks exactly 5 slots and preserves prior contents.
- Automation test: locked slots reject writes.
- Automation test: delete returns deleted signal for no-softlock ledger update.

#### Done criteria
- Slot progression is exactly 3, then 4, then 5.
- Overwrite and delete are explicit actions.
- Deletion path returns enough information for puzzle fallback state.

#### Common pitfalls
- Do not resize slots destructively when unlocking.
- Do not allow writing into locked slot indices.
- Do not silently discard deleted signal metadata.

- Copy paste ai prompt: Implement FRecorderSlot and FRecorderSlotSet. Slots must unlock only in counts 3, 4, and 5. Preserve contents when unlocking. Reject writes to locked slots. Delete must return deleted signal metadata so puzzle systems can prevent soft-locks. Add automation tests for Tier3/Tier4/Tier5, locked writes, overwrite, and deletion.

### A07 — Implement URecorderComponent core

- Id: A07
- Title: Implement URecorderComponent core
- Goal: Attach recorder functionality to the player: record authored anomalies, manage slots, delete/overwrite safely, and expose Blueprint events.
#### Primary files
- Source/<Game>/Public/Acoustics/RecorderComponent.h
- Source/<Game>/Private/Acoustics/RecorderComponent.cpp

#### Editor assets
- Blueprint UI widget: WBP_RecorderSlots

- Cpp shape: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRecorderSlotsChanged, const FRecorderSlotSet&, Slots);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRecordedSignalChanged, int32, SlotIndex, const FRecordedSignal&, Signal);

UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class URecorderComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly) ERecordingSlotUnlockTier InitialTier = ERecordingSlotUnlockTier::Tier3;
  UPROPERTY(BlueprintAssignable) FRecorderSlotsChanged OnSlotsChanged;
  UPROPERTY(BlueprintAssignable) FRecordedSignalChanged OnSignalRecorded;

  UFUNCTION(BlueprintCallable) bool TryRecordAnomaly(AAudioAnomalyActor* Anomaly, int32 SlotIndex);
  UFUNCTION(BlueprintCallable) bool DeleteSlot(int32 SlotIndex);
  UFUNCTION(BlueprintCallable) bool UnlockTier(ERecordingSlotUnlockTier Tier);
  UFUNCTION(BlueprintCallable) bool HasValidSignal(int32 SlotIndex) const;
  const FRecorderSlotSet& GetSlots() const;
};
#### Tests
- Component automation test: BeginPlay initializes Tier3.
- Component automation test: recording valid anomaly fills requested unlocked slot.
- Component automation test: deleting slot calls no-softlock ledger hook.
- Component automation test: recording invalid anomaly fails without changing slots.
- Component automation test: Blueprint delegates fire exactly once per successful mutation.

#### Done criteria
- Player can record authored anomaly actors into unlocked slots.
- Slot changes are observable by Blueprint UI.
- Invalid operations fail without side effects.
- Deletion cannot bypass puzzle safety hook.

#### Common pitfalls
- Do not auto-select slots if UX requires explicit player choice; if auto-select is desired, make it a separate tested helper.
- Do not call audio playback from recording mutation logic.
- Do not hard-reference UI classes in component mechanics.

- Copy paste ai prompt: Implement URecorderComponent for authored acoustic recording. It must initialize with exactly 3 slots, support unlocking to 4 and 5, record AAudioAnomalyActor into a chosen slot, delete slots, expose Blueprint delegates, and call a no-softlock ledger hook when replacing or deleting puzzle-eligible signals. Add component automation tests for all mutations and failure cases.

### A08 — Implement local playback from recorder

- Id: A08
- Title: Implement local playback from recorder
- Goal: Play a recorded signal locally for player feedback and optionally emit gameplay stimuli when the playback should be heard by the world.
#### Primary files
- Source/<Game>/Public/Acoustics/RecorderPlaybackComponent.h
- Source/<Game>/Private/Acoustics/RecorderPlaybackComponent.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_RecorderPlaybackPresenter'
- SoundClass'/Game/Audio/SoundClasses/SC_RecorderPlayback'

- Cpp shape: USTRUCT(BlueprintType)
struct FRecorderPlaybackRequest
{
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SlotIndex = INDEX_NONE;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) EAcousticRoute Route = EAcousticRoute::Direct;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEmitWorldStimulus = true;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumeMultiplier = 1.f;
};

UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class URecorderPlaybackComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable) bool PlayRecordedSlot(const FRecorderPlaybackRequest& Request);
};
#### Tests
- Automation test: local playback resolves authored clip from SourceAnomalyId and Fidelity.
- Automation test: PlayCount increments after successful playback.
- Automation test: bEmitWorldStimulus false plays local sound but emits no stimulus.
- Automation test: missing asset fails gracefully with debug message.

#### Done criteria
- Playback uses authored clip variants, not recorded waveform data.
- Playback can be local-only for UI feedback.
- World-heard playback emits FAcousticStimulus.
- Play count supports repetition mechanics.

#### Common pitfalls
- Do not duplicate signal storage in playback component.
- Do not increment PlayCount on failed playback.
- Do not treat local UI preview as puzzle stimulus unless explicitly requested.

- Copy paste ai prompt: Implement recorder local playback. Add FRecorderPlaybackRequest and URecorderPlaybackComponent. Resolve the authored clip from FRecordedSignal SourceAnomalyId and fidelity, play locally, optionally emit a gameplay FAcousticStimulus, and increment PlayCount only on success. Add tests for clip resolution, local-only playback, emitted playback, and missing assets.

### A09 — Implement puck projection

- Id: A09
- Title: Implement puck projection
- Goal: Allow the player to project a recorded signal from a thrown/placed puck origin rather than the player origin.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticPuckActor.h
- Source/<Game>/Private/Acoustics/AcousticPuckActor.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticPuck'
- StaticMesh'/Game/Props/Puck/SM_AcousticPuck'

- Cpp shape: UCLASS()
class AAcousticPuckActor : public AActor
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> ProjectionOrigin;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxProjectionLifetime = 20.f;
  UFUNCTION(BlueprintCallable) bool ProjectRecordedSignal(URecorderComponent* Recorder, int32 SlotIndex);
};
#### Tests
- Automation test: puck projection emits stimulus at puck origin, not player origin.
- Automation test: invalid slot does not emit.
- Automation test: projection route is Acoustic.Route.PuckProjected.
- Functional test: receiver puzzle hears puck-projected signal behind a barrier if in radius.

#### Done criteria
- Projected audio and stimuli originate at the puck.
- Blueprint can handle throw visuals independently.
- Projection uses the same recorded signal identity and puzzle keys.

#### Common pitfalls
- Do not move receiver checks to player location.
- Do not create a separate puck-only recording type.
- Do not let destroyed puck leave active audio/stimulus state dangling.

- Copy paste ai prompt: Implement AAcousticPuckActor that can project a recorded signal from its ProjectionOrigin. It must use URecorderComponent slot data, emit FAcousticStimulus with route PuckProjected at the puck location, and fail safely for invalid slots. Add automation tests for origin, route, invalid slot, and a functional receiver test.

### A10 — Implement FAcousticStimulus

- Id: A10
- Title: Implement FAcousticStimulus
- Goal: Create the first-class transient gameplay event emitted by every player-made noise and authored anomaly playback.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticStimulus.h
- Source/<Game>/Private/Acoustics/AcousticStimulus.cpp

- Editor assets: []
- Cpp shape: USTRUCT(BlueprintType)
struct FAcousticStimulus
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) FGuid StimulusGuid;
  UPROPERTY(BlueprintReadOnly) FVector WorldLocation = FVector::ZeroVector;
  UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> InstigatorActor = nullptr;
  UPROPERTY(BlueprintReadOnly) FPrimaryAssetId SourceAnomalyId;
  UPROPERTY(BlueprintReadOnly) FGameplayTag CategoryTag;
  UPROPERTY(BlueprintReadOnly) FGameplayTagContainer ReceiverKeys;
  UPROPERTY(BlueprintReadOnly) EAcousticFidelity Fidelity = EAcousticFidelity::Clean;
  UPROPERTY(BlueprintReadOnly) EAcousticRoute Route = EAcousticRoute::Direct;
  UPROPERTY(BlueprintReadOnly) float Loudness = 1.f;
  UPROPERTY(BlueprintReadOnly) float Radius = 1200.f;
  UPROPERTY(BlueprintReadOnly) float Contamination = 0.f;
  UPROPERTY(BlueprintReadOnly) int32 RepetitionIndex = 0;
  UPROPERTY(BlueprintReadOnly) bool bPuzzleEligible = true;
  bool IsValid() const;
};
#### Tests
- Automation test: all player noise helpers create valid FAcousticStimulus.
- Automation test: invalid category tag makes stimulus invalid.
- Automation test: radius and loudness are clamped to safe ranges.
- Automation test: puzzle-ineligible stimuli do not satisfy receivers.

#### Done criteria
- Every gameplay-heard player noise has a stimulus path.
- Stimulus includes category, location, radius, route, fidelity, contamination, and puzzle keys.
- Stimulus validity is testable without audio playback.

#### Common pitfalls
- Do not treat sound playback as sufficient gameplay signal.
- Do not omit non-anomaly player noises like footstep, impact, and machine interactions.
- Do not make receiver logic inspect audio components directly.

- Copy paste ai prompt: Implement FAcousticStimulus as the first-class gameplay event for all player-made noise and anomaly playback. Include guid, world location, instigator, source anomaly id, category tag, receiver keys, fidelity, route, loudness, radius, contamination, repetition index, and puzzle eligibility. Add validity and clamp tests. Do not inspect real audio.

### A11 — Implement acoustic subsystem

- Id: A11
- Title: Implement acoustic subsystem
- Goal: Provide the central event bus for acoustic stimuli, spatial query, debug capture, and receiver notification.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticSubsystem.h
- Source/<Game>/Private/Acoustics/AcousticSubsystem.cpp

- Editor assets: []
- Cpp shape: DECLARE_MULTICAST_DELEGATE_OneParam(FAcousticStimulusNativeDelegate, const FAcousticStimulus&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAcousticStimulusBlueprintDelegate, const FAcousticStimulus&, Stimulus);

UCLASS()
class UAcousticSubsystem : public UWorldSubsystem
{
  GENERATED_BODY()
public:
  FAcousticStimulusNativeDelegate OnStimulusNative;
  UPROPERTY(BlueprintAssignable) FAcousticStimulusBlueprintDelegate OnStimulusBlueprint;

  UFUNCTION(BlueprintCallable) bool EmitStimulus(const FAcousticStimulus& Stimulus);
  UFUNCTION(BlueprintCallable) TArray<FAcousticStimulus> GetRecentStimuli() const;
  UFUNCTION(BlueprintCallable) void RegisterReceiver(UAcousticReceiverComponent* Receiver);
  UFUNCTION(BlueprintCallable) void UnregisterReceiver(UAcousticReceiverComponent* Receiver);
};
#### Tests
- Automation test: EmitStimulus rejects invalid stimulus.
- Automation test: valid stimulus notifies native and Blueprint delegates.
- Automation test: registered receiver receives in-radius stimulus.
- Automation test: out-of-radius receiver does not receive stimulus.
- Automation test: recent stimulus ring buffer respects configured limit.

#### Done criteria
- Subsystem is the only gameplay event bus for acoustic stimuli.
- Receivers can register/unregister safely.
- Debug tools can inspect recent stimuli.
- No audio playback dependency exists in receiver matching.

#### Common pitfalls
- Do not broadcast invalid stimuli.
- Do not keep stale receiver pointers after level unload.
- Do not run expensive global actor searches on every stimulus.

- Copy paste ai prompt: Implement UAcousticSubsystem as a UWorldSubsystem event bus. It must validate and emit FAcousticStimulus, notify native and Blueprint listeners, maintain a recent-stimulus ring buffer, and notify registered receivers by radius. Add automation tests for valid/invalid emission, delegate notification, receiver radius filtering, unregister safety, and debug buffer size.

### A12 — Convert player noises to acoustic stimuli

- Id: A12
- Title: Convert player noises to acoustic stimuli
- Goal: Ensure footsteps, impacts, machines, bells/water, static, voice-like authored barks, and anomaly playback all emit first-class stimuli.
#### Primary files
- Source/<Game>/Public/Acoustics/PlayerNoiseEmitterComponent.h
- Source/<Game>/Private/Acoustics/PlayerNoiseEmitterComponent.cpp

#### Editor assets
- DataAsset'/Game/Audio/Acoustics/DA_PlayerNoiseTuning'

- Cpp shape: UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class UPlayerNoiseEmitterComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable) bool EmitFootstep(FVector Location, float SurfaceLoudness);
  UFUNCTION(BlueprintCallable) bool EmitImpact(FVector Location, float Strength);
  UFUNCTION(BlueprintCallable) bool EmitMachine(FVector Location, FGameplayTag MachineSubtype);
  UFUNCTION(BlueprintCallable) bool EmitBell(FVector Location);
  UFUNCTION(BlueprintCallable) bool EmitStatic(FVector Location);
  UFUNCTION(BlueprintCallable) bool EmitAuthoredVoice(FVector Location, FPrimaryAssetId VoiceAssetId);
};
#### Tests
- Automation test: each of the seven categories can be emitted through supported gameplay paths.
- Automation test: every player noise path reaches UAcousticSubsystem.
- Automation test: tuning Data Asset controls radius/loudness.
- Functional test: receiver can react to a footstep stimulus.

#### Done criteria
- No player-made gameplay noise bypasses FAcousticStimulus.
- Tuning lives in Data Assets.
- Blueprint presentation can call simple emission helpers.

#### Common pitfalls
- Do not add a category outside the seven fixed categories.
- Do not hardcode all radii in animation notifies.
- Do not make machine subtype a category; use a secondary tag.

- Copy paste ai prompt: Implement UPlayerNoiseEmitterComponent to emit first-class FAcousticStimulus events for player-made Footstep, Impact, Machine, Bell, Static, authored Voice, and Anomaly playback paths. Use a Data Asset for loudness/radius tuning. Add tests proving every category path reaches UAcousticSubsystem.

### A13 — Implement acoustic receiver component for puzzles

- Id: A13
- Title: Implement acoustic receiver component for puzzles
- Goal: Create configurable puzzle receivers that match acoustic stimuli by category, receiver keys, route, fidelity, contamination, repetition, and radius.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticReceiverComponent.h
- Source/<Game>/Private/Acoustics/AcousticReceiverComponent.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticReceiver'

- Cpp shape: USTRUCT(BlueprintType)
struct FAcousticReceiverRule
{
  GENERATED_BODY()
  UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag RequiredCategory;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredReceiverKeys;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TSet<EAcousticRoute> AcceptedRoutes;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) EAcousticFidelity MinimumFidelity = EAcousticFidelity::Contaminated;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxContamination = 1.f;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxAcceptedRepetitionIndex = 999;
  UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bConsumeOnce = true;
};

UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class UAcousticReceiverComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FAcousticReceiverRule> Rules;
  UPROPERTY(BlueprintAssignable) FOnReceiverRuleSatisfied OnRuleSatisfied;
  bool EvaluateStimulus(const FAcousticStimulus& Stimulus);
};
#### Tests
- Automation test: receiver matches required category and receiver key.
- Automation test: receiver rejects wrong category.
- Automation test: receiver rejects excessive contamination.
- Automation test: receiver rejects disallowed route.
- Automation test: bConsumeOnce prevents duplicate satisfaction.
- Functional test: puzzle actor unlocks only after expected acoustic rule.

#### Done criteria
- Puzzle designers can configure receiver rules in editor.
- Receiver matching is deterministic and independent of audio playback.
- Rules support fidelity, repetition, contamination, pipe, speaker, and puck route constraints.

#### Common pitfalls
- Do not make receivers listen to USoundWave playback.
- Do not compare localized names.
- Do not consume failed attempts.
- Do not make all receivers accept all routes by default unless explicitly tuned.

- Copy paste ai prompt: Implement UAcousticReceiverComponent and FAcousticReceiverRule. Receivers must evaluate FAcousticStimulus by category, receiver keys, accepted routes, minimum fidelity, max contamination, max repetition index, radius via subsystem, and consume-once behavior. Add automation and functional tests for match, reject, duplicate, and puzzle unlock behavior.

### A14 — Implement no-softlock puzzle ledger

- Id: A14
- Title: Implement no-softlock puzzle ledger
- Goal: Prevent deletion or overwrite of recordings from soft-locking receiver puzzles.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticPuzzleLedgerComponent.h
- Source/<Game>/Private/Acoustics/AcousticPuzzleLedgerComponent.cpp

#### Editor assets
- DataAsset'/Game/Audio/Acoustics/DA_AcousticPuzzleSafetyRules'

- Cpp shape: UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class UAcousticPuzzleLedgerComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable) void RegisterRecordedSignal(const FRecordedSignal& Signal);
  UFUNCTION(BlueprintCallable) void RegisterDeletedOrOverwrittenSignal(const FRecordedSignal& Signal);
  UFUNCTION(BlueprintCallable) bool HasEverHeldReceiverKey(FGameplayTag RequiredKey) const;
  UFUNCTION(BlueprintCallable) bool CanPuzzleUseFallback(FGameplayTag RequiredKey) const;
private:
  UPROPERTY() FGameplayTagContainer EverHeldReceiverKeys;
  UPROPERTY() TArray<FPrimaryAssetId> EverHeldPuzzleEligibleAnomalies;
};
#### Tests
- Automation test: recording puzzle-eligible signal records receiver keys in ledger.
- Automation test: deleting signal preserves EverHeldReceiverKeys.
- Automation test: receiver can be configured to accept ledger fallback where intended.
- Automation test: non-puzzle signal does not pollute puzzle ledger.

#### Done criteria
- Deleting/overwriting cannot permanently block puzzle progression.
- Fallback policy is explicit and data-driven.
- Ledger is savegame-safe.

#### Common pitfalls
- Do not silently auto-solve puzzles just because a key was ever held unless the puzzle rule allows fallback.
- Do not store transient actor references.
- Do not let non-puzzle decoys satisfy safety ledger.

- Copy paste ai prompt: Implement an acoustic puzzle ledger that records puzzle-eligible receiver keys and anomaly ids when signals are recorded, overwritten, or deleted. It must prevent soft-locks without auto-solving every puzzle. Add tests proving deletion preserves fallback information and non-puzzle recordings do not satisfy fallback.

### A15 — Implement fidelity and repetition mechanics

- Id: A15
- Title: Implement fidelity and repetition mechanics
- Goal: Apply authored fidelity variants and repeated-play penalties to gameplay stimuli and receiver matching.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticSignalProcessingRules.h
- Source/<Game>/Private/Acoustics/AcousticSignalProcessingRules.cpp

#### Editor assets
- DataAsset'/Game/Audio/Acoustics/DA_AcousticProcessingRules'

- Cpp shape: UCLASS(BlueprintType)
class UAcousticProcessingRules : public UDataAsset
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere) int32 PlaysBeforeDegrade = 2;
  UPROPERTY(EditAnywhere) int32 PlaysBeforeContaminate = 4;
  UPROPERTY(EditAnywhere) float RepetitionRadiusPenalty = 0.15f;
  UFUNCTION(BlueprintCallable) EAcousticFidelity ComputeEffectiveFidelity(const FRecordedSignal& Signal) const;
  UFUNCTION(BlueprintCallable) float ComputeEffectiveRadiusMultiplier(const FRecordedSignal& Signal) const;
};
#### Tests
- Automation test: first play remains clean when clean signal exists.
- Automation test: repeated plays degrade according to Data Asset thresholds.
- Automation test: contaminated signal cannot become cleaner through repetition.
- Automation test: receiver rejects repeated signal beyond MaxAcceptedRepetitionIndex.

#### Done criteria
- Fidelity behavior is deterministic and data-driven.
- Repetition affects gameplay through explicit fields.
- Authored degraded/contaminated variants are selected when needed.

#### Common pitfalls
- Do not analyze waveform to determine fidelity.
- Do not mutate SourceAnomalyId when fidelity changes.
- Do not let repetition penalty make radius negative.

- Copy paste ai prompt: Implement data-driven acoustic fidelity and repetition rules. Use authored clean/degraded/contaminated variants only. Compute effective fidelity and radius multiplier from FRecordedSignal play count and contamination. Add tests for clean first play, degradation thresholds, no self-cleaning, and receiver repetition rejection.

### A16 — Implement contamination and overdub mechanics

- Id: A16
- Title: Implement contamination and overdub mechanics
- Goal: Allow recordings to become contaminated or overdubbed using authored metadata, while keeping puzzle behavior deterministic.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticOverdubComponent.h
- Source/<Game>/Private/Acoustics/AcousticOverdubComponent.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_OverdubStation'
- DataAsset'/Game/Audio/Acoustics/DA_ContaminationRules'

- Cpp shape: USTRUCT(BlueprintType)
struct FOverdubResult
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) bool bSuccess = false;
  UPROPERTY(BlueprintReadOnly) FRecordedSignal ResultSignal;
  UPROPERTY(BlueprintReadOnly) FText FailureReason;
};

UCLASS(ClassGroup=(Acoustics), meta=(BlueprintSpawnableComponent))
class UAcousticOverdubComponent : public UActorComponent
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable) FOverdubResult TryOverdub(const FRecordedSignal& Base, const FRecordedSignal& Layer);
  UFUNCTION(BlueprintCallable) FRecordedSignal ApplyContamination(const FRecordedSignal& Signal, float Amount);
};
#### Tests
- Automation test: overdub combines source ids and receiver key snapshots deterministically.
- Automation test: overdub fails when either source forbids overdubbing.
- Automation test: contamination clamps from 0 to 1.
- Automation test: contaminated signal selects contaminated authored variant when present.
- Automation test: receiver can reject overdub route or excessive contamination.

#### Done criteria
- Overdub is explicit and inspectable.
- Contamination is numeric and clamped.
- Receiver behavior remains deterministic.
- No runtime waveform mixing is required for gameplay identity.

#### Common pitfalls
- Do not physically mix arbitrary audio and then infer gameplay identity from output.
- Do not let overdub duplicate the same receiver key infinitely.
- Do not allow contamination to bypass puzzle locks accidentally.

- Copy paste ai prompt: Implement contamination and overdub mechanics for FRecordedSignal. Overdub must combine source ids and receiver key snapshots deterministically, respect bCanBeOverdubbed from source Data Assets, and mark route Overdub during playback. Contamination must clamp 0..1 and select authored contaminated variants. Add tests for combination, failure, clamping, variant choice, and receiver rejection.

### A17 — Implement pipe routing

- Id: A17
- Title: Implement pipe routing
- Goal: Route acoustic stimuli through authored pipe networks, changing origin/route without changing signal identity.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticPipeNode.h
- Source/<Game>/Private/Acoustics/AcousticPipeNode.cpp
- Source/<Game>/Public/Acoustics/AcousticPipeSubsystem.h
- Source/<Game>/Private/Acoustics/AcousticPipeSubsystem.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticPipeNode'
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticPipeExit'

- Cpp shape: UCLASS()
class AAcousticPipeNode : public AActor
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere) FName NetworkId;
  UPROPERTY(EditAnywhere) bool bIsInput = false;
  UPROPERTY(EditAnywhere) bool bIsOutput = false;
  UPROPERTY(EditAnywhere) float AttenuationMultiplier = 0.75f;
};

UCLASS()
class UAcousticPipeSubsystem : public UWorldSubsystem
{
  GENERATED_BODY()
public:
  bool TryRouteThroughPipe(const FAcousticStimulus& Input, FName NetworkId);
};
#### Tests
- Automation test: pipe input emits routed stimulus at output node location.
- Automation test: routed stimulus preserves source anomaly id and receiver keys.
- Automation test: route becomes Pipe.
- Automation test: attenuation multiplier affects radius.
- Functional test: receiver only reachable through pipe unlocks from pipe output.

#### Done criteria
- Pipe routing is authored and deterministic.
- Routed stimuli preserve identity and puzzle metadata.
- Receivers can require or reject pipe route.

#### Common pitfalls
- Do not teleport audio actor references; emit a new routed stimulus.
- Do not mutate the original stimulus in-place if debug tools need both input and output.
- Do not make pipe networks discoverable only by slow actor search every emission.

- Copy paste ai prompt: Implement authored pipe routing for acoustic stimuli. Pipe nodes have NetworkId, input/output flags, and attenuation. Routing emits a new FAcousticStimulus at output nodes with route Pipe, preserving source id, category, and receiver keys. Add tests for location, identity preservation, route, attenuation, and functional receiver unlock.

### A18 — Implement speaker routing

- Id: A18
- Title: Implement speaker routing
- Goal: Allow placed speakers to rebroadcast recorded signals or received stimuli as speaker-routed acoustic events.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticSpeakerActor.h
- Source/<Game>/Private/Acoustics/AcousticSpeakerActor.cpp

#### Editor assets
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticSpeaker'

- Cpp shape: UCLASS()
class AAcousticSpeakerActor : public AActor
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere) TObjectPtr<USceneComponent> SpeakerOrigin;
  UPROPERTY(EditAnywhere) float BroadcastRadiusMultiplier = 1.5f;
  UFUNCTION(BlueprintCallable) bool BroadcastRecordedSignal(URecorderComponent* Recorder, int32 SlotIndex);
  UFUNCTION(BlueprintCallable) bool RebroadcastStimulus(const FAcousticStimulus& Stimulus);
};
#### Tests
- Automation test: speaker broadcast emits at speaker origin.
- Automation test: route becomes Speaker.
- Automation test: speaker radius multiplier applies.
- Automation test: source identity and receiver keys are preserved.
- Functional test: receiver requiring speaker route unlocks only from speaker.

#### Done criteria
- Speakers are level-authored actors.
- Speaker routing is distinct from direct playback.
- Puzzle receivers can require speaker route.

#### Common pitfalls
- Do not make speaker broadcasts player-originated.
- Do not drop contamination/fidelity metadata.
- Do not allow infinite speaker-to-speaker feedback without loop guard.

- Copy paste ai prompt: Implement AAcousticSpeakerActor that broadcasts recorded signals or rebroadcasts stimuli from its speaker origin. Emitted stimuli must use route Speaker, preserve source identity and receiver keys, apply radius multiplier, and guard against routing loops. Add automation and functional tests.

### A19 — Implement captions

- Id: A19
- Title: Implement captions
- Goal: Display authored captions for anomaly playback, recorder playback, speaker playback, and local previews.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticCaptionSubsystem.h
- Source/<Game>/Private/Acoustics/AcousticCaptionSubsystem.cpp

#### Editor assets
- WidgetBlueprint'/Game/UI/WBP_AcousticCaptionLine'
- WidgetBlueprint'/Game/UI/WBP_AcousticCaptionStack'

- Cpp shape: USTRUCT(BlueprintType)
struct FActiveAcousticCaption
{
  GENERATED_BODY()
  UPROPERTY(BlueprintReadOnly) FText Text;
  UPROPERTY(BlueprintReadOnly) float TimeRemaining = 0.f;
  UPROPERTY(BlueprintReadOnly) FGameplayTag CategoryTag;
  UPROPERTY(BlueprintReadOnly) EAcousticRoute Route = EAcousticRoute::Direct;
};

UCLASS()
class UAcousticCaptionSubsystem : public UGameInstanceSubsystem
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable) void PushCaptionsForSignal(const FRecordedSignal& Signal, EAcousticRoute Route);
  UFUNCTION(BlueprintCallable) void PushCaptionsForStimulus(const FAcousticStimulus& Stimulus);
};
#### Tests
- Automation test: captions resolve from selected authored clip variant.
- Automation test: missing captions do not block playback.
- Automation test: local preview can show captions without emitting world stimulus.
- Automation test: captions include route/category metadata for UI styling.

#### Done criteria
- Captions are authored in Data Assets or clip variants.
- Blueprint UI owns presentation.
- Caption system does not infer text from audio.

#### Common pitfalls
- Do not run speech-to-text.
- Do not store captions only on SoundWave assets if designers need per-fidelity variants.
- Do not make captions required for non-voice categories.

- Copy paste ai prompt: Implement an acoustic caption subsystem for authored captions. It must resolve captions from the selected clip variant for FRecordedSignal or FAcousticStimulus, support local preview captions without world stimulus, expose active captions to Blueprint UI, and include category/route metadata. Add tests for resolution, missing captions, local preview, and UI metadata.

### A20 — Implement Blueprint presentation layer

- Id: A20
- Title: Implement Blueprint presentation layer
- Goal: Expose recorder UI, slot state, playback buttons, anomaly interact prompts, receiver feedback, and debug visuals without moving mechanics out of C++.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticBlueprintLibrary.h
- Source/<Game>/Private/Acoustics/AcousticBlueprintLibrary.cpp

#### Editor assets
- WidgetBlueprint'/Game/UI/WBP_RecorderSlots'
- WidgetBlueprint'/Game/UI/WBP_RecorderSlot'
- WidgetBlueprint'/Game/UI/WBP_AcousticInteractPrompt'
- Blueprint'/Game/Blueprints/Acoustics/BP_AcousticDebugVisualizer'

- Cpp shape: UCLASS()
class UAcousticBlueprintLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure) static FText GetSignalDisplayName(const FRecordedSignal& Signal);
  UFUNCTION(BlueprintPure) static FSlateColor GetCategoryColor(FGameplayTag CategoryTag);
  UFUNCTION(BlueprintPure) static bool IsSignalPuzzleEligible(const FRecordedSignal& Signal);
};
#### Tests
- Automation test: Blueprint helper returns fallback display text for missing asset.
- Automation test: category colors exist for exactly seven categories.
- Manual QA checklist: record, overwrite, delete, play, project puck, route speaker, route pipe.

#### Done criteria
- Blueprints handle presentation only.
- UI updates from delegates, not polling every frame.
- Missing data has graceful fallback text.

#### Common pitfalls
- Do not implement puzzle matching in Widget Blueprints.
- Do not bind UI every tick when delegates exist.
- Do not hide invalid asset errors behind generic labels only.

- Copy paste ai prompt: Implement Blueprint-facing presentation helpers for the acoustic system. Add UAcousticBlueprintLibrary for display name, category color, and puzzle eligibility. Create/prepare UI-facing delegates so recorder widgets update from events. Keep mechanics in C++; Blueprints should only present slot state, prompts, captions, and debug visuals. Add tests for fallback display text and exactly seven category colors.

### A21 — Implement debug tools

- Id: A21
- Title: Implement debug tools
- Goal: Provide console commands, on-screen overlays, logging, and recent-stimulus inspection for designers and QA.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticDebug.h
- Source/<Game>/Private/Acoustics/AcousticDebug.cpp

#### Editor assets
- WidgetBlueprint'/Game/UI/Debug/WBP_AcousticDebugPanel'

- Cpp shape: Console commands:
acoustic.Debug.DrawStimuli 0|1
acoustic.Debug.ListRecent
acoustic.Debug.EmitTest <Category> <Radius>
acoustic.Debug.ValidateAssets
acoustic.Debug.DumpReceivers

Log categories:
LogAcoustics
LogAcousticReceiver
LogRecorder
#### Tests
- Automation test: debug emit test creates valid stimulus for each of seven categories.
- Automation test: asset validation command reports invalid Data Assets.
- Automation test: recent stimulus dump includes route, category, fidelity, contamination, location.

#### Done criteria
- QA can see why a receiver did or did not trigger.
- Designers can validate anomaly assets in-editor.
- Debug output includes enough data to diagnose routing and repetition.

#### Common pitfalls
- Do not ship noisy debug logs enabled by default.
- Do not let debug emit bypass category validation.
- Do not expose destructive debug commands in shipping builds.

- Copy paste ai prompt: Implement acoustic debug tools. Add log categories, console commands to draw/list recent stimuli, emit validated test stimuli, validate anomaly assets, and dump receivers. Include route, category, fidelity, contamination, repetition, source id, and location in debug output. Add tests for debug emit categories, validation command, and recent-stimulus dump fields.

### A22 — Implement save/load integration

- Id: A22
- Title: Implement save/load integration
- Goal: Persist recorder slots, signal metadata, unlock tier, play counts, contamination, overdub identity, and no-softlock ledger.
#### Primary files
- Source/<Game>/Public/Acoustics/AcousticSaveTypes.h
- Source/<Game>/Private/Acoustics/AcousticSaveTypes.cpp

- Editor assets: []
- Cpp shape: USTRUCT(BlueprintType)
struct FAcousticSaveData
{
  GENERATED_BODY()
  UPROPERTY() int32 Version = 1;
  UPROPERTY() ERecordingSlotUnlockTier UnlockTier = ERecordingSlotUnlockTier::Tier3;
  UPROPERTY() TArray<FRecordedSignal> SlotSignals;
  UPROPERTY() FGameplayTagContainer EverHeldReceiverKeys;
  UPROPERTY() TArray<FPrimaryAssetId> EverHeldPuzzleEligibleAnomalies;
};
#### Tests
- Automation test: save/load round-trips empty Tier3 recorder.
- Automation test: save/load round-trips Tier5 with contaminated overdubbed signals.
- Automation test: deleted signal ledger survives load.
- Automation test: unknown asset id fails gracefully after load.

#### Done criteria
- All gameplay-relevant acoustic state persists.
- Save version is explicit.
- Missing assets after load degrade gracefully.

#### Common pitfalls
- Do not persist raw actor pointers or audio component references.
- Do not forget play counts and contamination.
- Do not reset ledger on load.

- Copy paste ai prompt: Implement save/load structs for acoustic recorder state. Persist version, unlock tier, slot signals, play counts, contamination, overdub source ids, and no-softlock ledger. Add round-trip tests for empty Tier3, Tier5 with contaminated overdubbed signals, deleted-signal ledger, and missing asset ids.

### A23 — Create editor asset templates and validation pass

- Id: A23
- Title: Create editor asset templates and validation pass
- Goal: Provide concrete designer-facing Data Asset templates for anomalies, player noise tuning, processing rules, contamination, and puzzle safety.
#### Primary files
- Content/Audio/Acoustics/*
- Content/Audio/Anomalies/*

#### Editor assets
- DA_AudioAnomaly_Template
- DA_PlayerNoiseTuning
- DA_AcousticProcessingRules
- DA_ContaminationRules
- DA_AcousticPuzzleSafetyRules
- BP_AudioAnomalyActor
- BP_AcousticReceiver
- BP_AcousticPuck
- BP_AcousticSpeaker

- Cpp shape: No new mechanics. Use existing Data Asset classes and Blueprint wrappers.
#### Tests
- Editor validation command passes on all template assets.
- Asset audit: all anomaly templates have clean variant.
- Asset audit: any puzzle-eligible anomaly has receiver keys.
- Asset audit: all seven category colors/tuning entries exist.

#### Done criteria
- A designer can create a new anomaly by duplicating a template.
- All templates pass validation.
- Template folder names and asset names are documented.

#### Common pitfalls
- Do not create one-off Blueprint variables duplicating C++ Data Asset fields.
- Do not leave placeholder SoundBase references in valid assets.
- Do not mark decoys as puzzle-eligible unless intended.

- Copy paste ai prompt: Create designer-facing editor asset templates for the authored acoustic system using the implemented C++ classes. Provide anomaly, player noise tuning, processing rules, contamination rules, puzzle safety rules, anomaly actor, receiver, puck, and speaker templates. Run the acoustic asset validation command and fix all validation errors. Do not add new mechanics.

### A24 — Build vertical functional test map

- Id: A24
- Title: Build vertical functional test map
- Goal: Create one small map that proves the complete loop: record authored anomaly, play locally, project puck, route through pipe/speaker, satisfy receiver, delete without soft-lock.
#### Primary files
- Content/Maps/Test/AcousticSystem_Functional.umap
- Source/<Game>/Private/Tests/AcousticSystemFunctionalTests.cpp

#### Editor assets
- FunctionalTest'/Game/Tests/FT_Acoustic_RecordPlaybackReceiver'
- FunctionalTest'/Game/Tests/FT_Acoustic_DeleteNoSoftlock'
- FunctionalTest'/Game/Tests/FT_Acoustic_PipeSpeakerPuckRoutes'

- Cpp shape: Functional scenarios:
1. Player records clean anomaly into slot 0.
2. Player plays slot locally; caption appears.
3. Player emits world playback; receiver hears direct route.
4. Player projects slot from puck; receiver requiring puck route hears it.
5. Pipe input routes to hidden receiver.
6. Speaker broadcasts to speaker-only receiver.
7. Player deletes recording; ledger fallback prevents soft-lock where configured.
#### Tests
- Functional test: record/play/direct receiver.
- Functional test: puck projection receiver.
- Functional test: pipe-routed receiver.
- Functional test: speaker-routed receiver.
- Functional test: delete and fallback puzzle safety.

#### Done criteria
- Functional tests can run unattended.
- Each major mechanic is represented in one vertical slice.
- Failures identify the mechanic that broke.

#### Common pitfalls
- Do not rely on human timing for functional tests.
- Do not assert on perceived audio output; assert on stimuli, receiver state, captions, and delegates.
- Do not make one giant opaque test with no intermediate diagnostics.

- Copy paste ai prompt: Create a vertical functional test map for the authored acoustic system. It must test recording an authored anomaly, local playback captions, direct receiver satisfaction, puck projection, pipe routing, speaker routing, and deletion no-softlock fallback. Assertions should inspect stimuli, receiver state, delegates, captions, and ledger state, not perceived audio output.

### A25 — Run final test and validation suite

- Id: A25
- Title: Run final test and validation suite
- Goal: Verify the full authored acoustic system meets acceptance criteria.
#### Primary files
- Source/<Game>/Private/Tests/*Acoustic*
- Config/DefaultGameplayTags.ini
- Content/Audio/Acoustics/*
- Content/Audio/Anomalies/*

- Editor assets: []
- Cpp shape: Suggested commands:
UnrealEditor-Cmd.exe <Project>.uproject -ExecCmds="Automation RunTests Acoustic; Quit" -TestExit="Automation Test Queue Empty" -unattended -nop4
UnrealEditor-Cmd.exe <Project>.uproject -run=DataValidation -unattended -nop4
#### Tests
- Gameplay tag automation tests.
- Data Asset validation tests.
- Recorded signal tests.
- Recorder component tests.
- Playback tests.
- Stimulus subsystem tests.
- Receiver tests.
- Fidelity/repetition tests.
- Contamination/overdub tests.
- Pipe/speaker/puck routing tests.
- Caption tests.
- Save/load tests.
- Functional test map suite.

#### Done criteria
- All acoustic automation tests pass.
- All acoustic functional tests pass unattended.
- Data validation passes for all acoustic assets.
- No acoustic log errors appear during test run.
- Shipping build excludes destructive debug commands.

#### Common pitfalls
- Do not accept manual audio playback as final verification.
- Do not skip Data Validation command.
- Do not ignore warnings about missing PrimaryAssetIds.

- Copy paste ai prompt: Run the final acoustic system verification suite. Execute automation tests filtered to Acoustic, run functional acoustic tests, run Data Validation, and confirm no acoustic log errors. Fix failures using strict TDD: write or update a failing test first, verify red, implement minimal fix, verify green, then rerun the full acoustic suite.


## Recommended test order
- Gameplay tags resolve and fixed category count is enforced.
- Core structs validate defaults and enum stability.
- Data Asset validation catches invalid authoring.
- FRecordedSignal identity and validity work.
- Slot model enforces 3 -> 4 -> 5 progression.
- Recorder records, overwrites, deletes, and fires delegates.
- Playback resolves authored variants and emits optional stimuli.
- FAcousticStimulus validates all gameplay-relevant fields.
- Subsystem dispatches and receivers evaluate deterministically.
- Fidelity, repetition, contamination, and overdub rules are deterministic.
- Puck, pipe, and speaker routing preserve signal identity.
- Captions resolve from authored metadata.
- Save/load round-trips recorder and ledger state.
- Functional map proves the end-to-end loop.

## Global done criteria
- No real mic input exists anywhere in the implementation.
- No waveform analysis exists anywhere in the implementation.
- All player-made gameplay noise emits FAcousticStimulus.
- Exactly seven category tags are valid.
- Recorder starts with 3 slots and can unlock to 4 then 5.
- Recorded signals reference authored assets and variants.
- Clean/degraded/contaminated clips are authored, not generated from analysis.
- Receivers match stimuli through deterministic metadata, not perceived audio.
- Deleting or overwriting recordings cannot soft-lock configured puzzles.
- Puck, pipe, speaker, overdub, repetition, contamination, and fidelity are covered by tests.
- Blueprints handle presentation; C++ owns mechanics; Data Assets own tuning.
- Automation, functional tests, and data validation pass.

## Global common pitfalls
- Treating audio playback as the gameplay event instead of emitting FAcousticStimulus.
- Letting category creep beyond the seven locked categories.
- Using runtime audio analysis to determine fidelity or puzzle identity.
- Storing actor pointers or SoundBase-only references in savegame data.
- Allowing deletion or overwrite to erase the only path to puzzle completion.
- Putting puzzle logic in widgets or presentation Blueprints.
- Making pipe/speaker/puck routing mutate the original stimulus instead of emitting traceable routed stimuli.
- Skipping editor Data Validation because C++ tests pass.
- Writing tests that assert on subjective audio output instead of deterministic metadata and state.

- Accomplished: No files were written. The result is an implementation-ready, task-by-task plan for the authored acoustic system with C++ interfaces, assets, tests, done criteria, pitfalls, and prompts.
