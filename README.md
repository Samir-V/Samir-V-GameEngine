# Engine Design

Below is the list of patterns and design choices used while creating the Engine.
I will go over them, explaining where and how they have been applied.

### Patterns
- Component
- Game loop
- Update
- State
- Observer
- Event Queue
- Command
- Service Locator

### Design choices
- Ring buffer
- Scene factories
- DontDestroyOnLoad scene
- Input maps

## Game loop, Update & Component

The foundation of the engine lies in these three patterns. Game loop is set to run at a maximum of 144 fps, updating and rendering the game objects existing in the game scenes.
Update loop is exactly repsonsible for executing the changes to the game objects. The engine also has the implementation of the late update, that runs after the main update loop, 
executing the code that needs to be done after the changes have made effect on the game objects. Such as destroying the objects.
The changes in question are being applied through the work of components. Only one component of the type can exist on the game object.
The usage of components enforces designing the code in a way, that the component fully covers a certain functionality of the object, allowing therefore to reuse that functionality on different objects.
Examples of the components I made for the enigne are RectCollider2DComponent and SpritesheetComponent, providing collision functionality and animated texture visualizing.

## State 

The state pattern replaces the cacophony that might arise from the need to track the current condition of a certain object and correctly update based on that condition.
The current active state of the object defines how it should behave and what behaviour should be disabled, allowing for the object to enter the desired state only if explicitly told to do so.
I have used the State pattern for tracking the game state, enemy state and player state.

## Observer & Event Queue

The observer pattern is a great help in decoupling the receiver from the sender. In the engine any component can become an Observer and a subject, that will notify the observers when needed with an event.
I have used the Observer pattern in implementing various behaviours, like decreasing health on hitting an enemy or increasing the score when enemy is killed. The collision system also utilizes the notifications of the observer pattern, allowing to react to certain collision events.

Event queue, decoupling both receiver and the sender, was used in the engine to create the sound system. In various places in the game a request is sent to the sound system, that evaluates it without knowing who sent it, playing the requested sound.

## Command

The command pattern in the engine provides a very simple structure allowing to turn functions into objects, that can be called to execute at a certain point in time.
In my engine command pattern is heavily used for the input of the game, allowing to navigate the menu, complete game levels, and input the name to store on the score board.

## Service Locator

The mentioned before sound system in my engine is a service, that user can register and later use. Having the possibility to modify the engine, the implementation of the sound system might differ, using a different library, and hence, a different class.  
The service locator implemented in the engine exists to allow decoupling the users of a service from a specific class, giving a decision to them of which service to use.


## Ring buffer

The sound system implemented in the engine avoids usage of a queue, that is in need of reallocations when expanding. A ring buffer provides a set amount of maximum pending messages, where the current indices of active requests always travel the buffer, executing them in the process.

## Scene factories

In the engine all the scenes that can be created are tied to the scene factory, a function, that they were created with. The stored scene factory acts as a blueprint for the scene, that is used when the ActiveScene is being switched. The target scene, that becomes the new active scene, is being reconstructed anew, to look just as it was in the beginning. I use that functionality when switching in-game levels and going back to the menu.

## DontDestroyOnLoad scene

The only exception to the design of scene factories is the DontDestroyOnLoad scene. A scene that is always active, alongside the current active regular scene. I use this scene for my players and the gameplay HUD, avoiding the need to recreate and retie them every time a level is switched. 
The GameHandler component that I created to oversee the state of the game, also lives in this scene.

## Input maps

While being able to store the commands associated with a certain key or a button on the gamepad, multiple scenes create a need of a great number of commands. To avoid the same button executing multiple non-related commands, input maps were created.
The active input map is always being changed depending on the scene or gamestate, allowing me to skip the execution of HUD commands while playing and vice versa.
