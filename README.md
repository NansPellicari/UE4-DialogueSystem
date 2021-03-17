# UE4 Dialog System

This plugin offers nodes to use in [Behavior Trees](https://docs.unrealengine.com/en-US/InteractiveExperiences/ArtificialIntelligence/BehaviorTrees/index.html). With them we don't need a new dedicated interface to use in Unreal Engine's Editor, you can add your dialogues next to other basics AI actions. Plus, it works with the [Gameplay Ability System](https://docs.unrealengine.com/en-US/InteractiveExperiences/GameplayAbilitySystem/index.html) and provides a basic [Dialog ability](./Source/NansDialogSystem/Public/Aiblity/GA_BaseDialog.h) you can tweak.

> :warning: For now, some very useful features missing to use it in a production games, like serialization of the Dialogues History (see below) to allow saving data across levels or make a savegame, but keep in touch!

|                                                                                                       <a href="https://www.buymeacoffee.com/NansUE4" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-green.png" alt="Buy Me A Coffee" height="51" width="217"></a>                                                                                                       |
| :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| I've decided to make all the code I developed for my games free to use and open source.<br> I am a true believer in the mindset that sharing and collaborating makes the world a better place.<br> The thing is: I'm fulltime dedicated to my project and these open source plugins, for coding I need a looooot of coffee, so please, help me to get my drug :stuck_out_tongue_closed_eyes: !! |

<!-- TOC -->

-   [Requirements](#requirements)
    -   [UE4 Plugins](#ue4-plugins)
-   [Features](#features)
    -   [Components](#components)
        -   [AIDialogComponent](#aidialogcomponent)
        -   [PlayerDialogComponent](#playerdialogcomponent)
    -   [Dialogue History](#dialogue-history)
    -   [Behavior Tree's nodes](#behavior-trees-nodes)
    -   [UI](#ui)
    -   [Dialogue Ability](#dialogue-ability)
-   [Contributing and Supporting](#contributing-and-supporting)

<!-- /TOC -->

<a id="markdown-requirements" name="requirements"></a>

## Requirements

<a id="markdown-ue4-plugins" name="ue4-plugins"></a>

### UE4 Plugins

-   [Gameplay Ability System](https://docs.unrealengine.com/en-US/InteractiveExperiences/GameplayAbilitySystem/index.html) (native UE4 plugins)
-   [NansUE4TestsHelpers](https://github.com/NansPellicari/UE4-TestsHelpers) (free UE4 plugins)
-   [NansCoreHelpers](https://github.com/NansPellicari/UE4-CoreHelpers) (free UE4 plugins)
-   [NansUE4Utilities](https://github.com/NansPellicari/UE4-Utilities) (free UE4 plugins)
-   [NansUMGExtent](https://github.com/NansPellicari/UE4-UMGExtent) (free UE4 plugins)
-   [NansBehaviorSteps](https://github.com/NansPellicari/UE4-BehaviorSteps) (free UE4 plugins)

<a id="markdown-features" name="features"></a>

## Features

<a id="markdown-components" name="components"></a>

### Components

<a id="markdown-aidialogcomponent" name="aidialogcomponent"></a>

#### AIDialogComponent

This is a really basics component for now (more will comes for it), its purpose is to clean up the Behavior Tree its NPC owner owns when a Dialog aborts.

> see [AIDialogComponent.h](./Source/NansDialogSystem/Public/Component/AIDialogComponent.h)

<a id="markdown-playerdialogcomponent" name="playerdialogcomponent"></a>

#### PlayerDialogComponent

Its purpose is to save all the Dialog results, communicate with the Dialogue Ability and make some searches in the last Dialogue results.

> see [PlayerDialogComponent.h](./Source/NansDialogSystem/Public/Component/PlayerDialogComponent.h)

<a id="markdown-dialogue-history" name="dialogue-history"></a>

### Dialogue History

Soon

> [More detailed docs](./Docs/Core/DialogHistory.md)

<a id="markdown-behavior-trees-nodes" name="behavior-trees-nodes"></a>

### Behavior Tree's nodes

| Node                                 | Node Type | Usefull for/when |
| ------------------------------------ | --------- | ---------------- |
| ![Step](./Docs/images/task-step.png) | Composite |                  |

<a id="markdown-ui" name="ui"></a>

### UI

Soon

> [More detailed docs](./Docs/UIs.md)

<a id="markdown-dialogue-ability" name="dialogue-ability"></a>

### Dialogue Ability

Soon

## Plugin Parameters

Soon

<a id="markdown-contributing-and-supporting" name="contributing-and-supporting"></a>

## Contributing and Supporting

I've decided to make all the code I developed for my games free to use and open source.  
I am a true believer in the mindset that sharing and collaborating makes the world a better place.  
I'll be very glad if you decided to help me to follow my dream.

| How?                                                                                                                                                                               |                                                                                         With                                                                                         |
| :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| **Donating**<br> Because I'm an independant developer/creator and for now I don't have<br> any income, I need money to support my daily needs (coffeeeeee).                        | <a href="https://www.buymeacoffee.com/NansUE4" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-green.png" alt="Buy Me A Coffee" height="51" width="217" ></a> |
| **Contributing**<br> You are very welcome if you want to contribute. I explain [here](./CONTRIBUTING.md) in details what<br> is the most comfortable way to me you can contribute. |                                                                         [CONTRIBUTING.md](./CONTRIBUTING.md)                                                                         |
