![Part III](/assets/3_title.png)

In the first two parts of the tutorial I have setup the tools and built the groundwork to create a game in C++.

> In this third part we finally create a playable game!

This is quite a challenge, because most games will need much more than just a primitive to render and one of the controller buttons to interact with. But a good game should never be just about graphics or complexity but about a gameplay that’s *”Easy to understand, but hard to master!”*; which was the credo of one of the most successful game companies in history — **Atari**.

But let’s start now...


# Brief Overview

In part 3 I want cover the following topics:

1. Implement a simple game engine
2. Maze Race — The game
3. How to build a game level
4. User feedback: Display text in OpenGL
5. MazeRace — An (almost) finished game!


# Tutorial

## 1. Implement a simple game engine

Why is it important to create a game engine? In essence it’s mainly the separation of code between game and technology on one side and the abstraction from platform (*ex. ML vs. macOS*) on the other side.

![Diagram 1 — Simple game engine](/assets/3_engine.png)

What I will use in this tutorial is a very simple version of a game engine with the main intention to improve our game in each tutorial, but also to abstract things and to bring it to different platforms. I show how we can simplify the development process by test out all first on macOS.


### The Engine Class
In the last part we’ve done all from ```main.cpp```. That’s what we first will change. We initialize the game engine directly before we jump into the game loop.

```c++
...
// Part 3: Init game engine
Game game;
game.Initialize();

// The game loop
RB_LOG(Debug, “Enter game loop”);
...
```

The update and render is then done below in the code

```c++
...
// Part 3: Update and Render
game.SetProjectionMatrix(projectionMatrix);
game.Update();
game.Render();
...
```

Also you have for sure noticed that the ```while(true)``` statement for the game loop no longer exists. It’s replaced by

```c++
...
// Part 3: If engine stops, end main loop, also support of Pause
while (game.GetState() != rb::ENGINE_STOPPED) {
  if (game.GetState() != rb::ENGINE_PLAY) {
    continue;
  }
}
...
```

With this we have removed all initializing, update and rendering into the game class resp. game engine. In essence I let all platform specific code in ```main.cpp``` and move all (game-)logic to the game/engine.

The game engine is ```RBEngine.hpp``` and ```RBEngine.cpp``` and holds all the engine code but it’s (currently) quite small.

```c++
...
void Initialize();
void Update();
void Render();
void Stop();
void Pause();
void Resume();
...
```

The first group of methods we have seen above in ```main.cpp``` and are needed to control the engine; start, pause, resume, but also initialize at first and update and render it in the game loop.

```c++
...
virtual void OnCreateContent() {}
virtual void OnUpdateContent(float delta) {}
virtual void OnRenderContent(glm::mat4 projectionMatrix) {}
virtual void OnDetectCollision(rb::Actor* actor) {}
...
```

The second group of methods are virtual methods which are there to override in the effective game class. Like this it’s easier to separate general logic from game specific things as you later will see when we talk about the ```Game``` class in ```Game.cpp```. Also the next two methods are just helper methods to pass the projection matrix (the camera view based on your head pose) to the engine. And last but not least we need later the shader to render things which remains almost the same as in part 2 but it’s now encapsulated in the engine.

```c++
...
Shader& GetShader3D() { return _shader3D; }
void SetProjectionMatrix(glm::mat4 matrix) { _projectionMatrix = matrix; }
...
```

Now it’s time to talk shortly about an important concept in game programming: States and State machines. At first I use a simple implementation of it by define 3 different states:

- ```ENGINE_PLAY``` The engine is running
- ```ENGINE_PAUSED``` The engine is in pause mode
- ```ENGINE_STOPPED``` The engine has stopped and program can therefore exit (see main loop)

Don’t mix this engine states with the game states I will describe later. The second is responsible for the game logic, the first describes the *technical* state in which the engine is. Of course it’s possible to combine both. But it will be easier later if we keep them separate.

```c++
...
EngineState GetState() { return _state; }
...
```

The next ‘concept’ introduced here in part 3 are **actors** (*game objects*). Actors are an abstract way to describe any object that is part of the game like players, enemies, obstacles etc. At first (because its shorter) I use an object-oriented implementation, but I will show in the next part of the tutorial why it’s better to use a component based approach (like Unity or Unreal).

The engine also contains a vector in which all actors are stored and can be called by the engine at any time it needs them. Logically the methods are more or less wrappers around the vector to abstract this.

```c++
...
void AddActor(Actor* actor) { _actors.push_back(actor); }
void InsertActor(Actor* actor) { _actors.insert(_actors.begin(), actor); }
Actor* GetActorAtIndex(int index) { return _actors.at(index); }
long NumberOfActors() { return _actors.size(); }
void ShowAllActors();
...
```

Last but not least we also need methods that allow us to get input from the user in a platform independent way.
I use for this the concept of virtual keys like *Fire, Left or Select*.

For the engine itself its not important if the input comes from the controller, the keyboard or even eye movement and helps also helps a lot to keep all platform code independent and flexible.

```c++
...
void PressVirtualKey(int code);
void ReleaseVirtualKey(int code);
bool IsVirtualKeyPressed(int code);
bool IsVirtualKeyContinuePressed(int code);
...
```


### The Actor Class

As written we abstract the game engine in the engine class (game handling) and the actors class (individual object behavior). Therefore the actor class contains all methods that are common for **all** game objects:

```c++
...
virtual void Update(float delta);
virtual void Render(Shader& shader, glm::mat4 projectionMatrix);
virtual bool TestCollisionAABB(Actor* actor) { return false; }
...
```

The first group of methods helps the engine to update and render each actor. Also it contains a method ```TestCollisionAABB``` which supports the collision detection between two actors.

```c++
...
void SetHidden(bool flag) { _hidden = flag; }
bool IsHidden() { return _hidden; }
void SetTag(int tag) { _tag = tag; }
int GetTag() { return _tag; }
long GetID() { return _id; }
void SetID(long id) { _id = id; }
...
```

The second group should be self describing. It’s a couple of setter/getters which allows to identify (```SetID```) and classify (```SetTag```) an actor. Also it contains a hidden flag (```SetHidden```), so an actor can be switched on and off in the game scene.


### The Primitives Class

In part 2 I have used a cylinder class to display something. The primitives class is a replacement of it and allows to render not just cylinders, but also cubes, pyramids and cones. Also it contains the same setters/getters as the cylinder class before to set the position, scale etc.
What’s new is the possibility to give the object a speed, so it’s able to move by itself without change the position from outside. It’s for now a very simple implementation which doesn’t take care of physical behavior like fraction, density, velocity etc. But it’s what we need at first to create a small simple game.


### The Clock

Before we come to the game itself there is one thing left to talk about. Now that we move an object we need a constant way to do it. Because the ML uses 1m for 1 unit, I use here also 1 m/s to define the speed.

To achieve this we need in each render/update frame the time passed since the last frame and multiply it by the speed. This is also hidden in the engine class and so worth it, to show the entire update method now:

```c++
void Engine::Update() {
  float delta = _clock.Delta();
  for (auto actor : _actors) {
    actor->Update(delta);
    OnDetectCollision(actor);
  }
  OnUpdateContent(delta);
}
```


# 2. MazeRace — the Game!

Now it’s time to talk about the game we build. It has the following requirements:

- Simple to implement (gameplay)
- Just use primitives
- Just use one controller button (bumper) as input
- Still fun to play!

Not so easy, right? I decided therefore for a game I give the name **MazeRace**. As the name says it has a maze like level. You must collect all collectables and avoid touching the wall of the maze. When you have all collected, you have won the level and it starts again, but each time more faster.

If you touch 3 times the wall, you lose the level and can try again with the same speed. While it’s easy at the beginning, it will become harder and harder the faster the player moves. Also you can just turn the player (cube) to the right by pressing the bumper button one time.

So you have to consider carefully where and how to turn to reach the goal... :)


# 3. How to build a Game Level

Before I describe how this is implemented in the game class ```Game.cpp``` I want to first talk about the creation of the level.
Of course it’s possible to code it, but it’s much more elegant to have a tool to help us do it.

I don’t go that far to create a tool here, but I use the approach to write the level in a text file, read it in the game class and create the game objects automatically. Like this I’m very flexible to define and change a level and even create new ones very easily.

This is actually more simple to do as you might think. The level is described by ASCII characters as in this example. The real level is of course bigger and it’s also not limited; you can make it as big as you like.

```
...

0-.-0000-------.......X------0  
0-.------------.------.------0  
0-X............@.......------0  
0----------------------------0  
100000000000000000000000000001  
```

- ```@``` Player position
- ```#``` User information position
- ```X``` Collectable
- ```0``` Wall element (Don’t touch)
- ```1``` Higher wall (Don’t touch)
- ```-``` Empty way (You can walk here)
- ```.``` Information only (Possible/Best way to follow)

Like this the level creation becomes quite easy and you just have to use a text editor and some fantasy to define a level.

*Don’t be shy about creating your very own levels!*


# 4. User Feedback: Display Text in OpenGL

One important thing (also in a game) is to have the possibility to show the user some text information. In a game these are things like the score, if you have loose or win or even some settings and information.

> In OpenGL there is no text function

You have just vertices and textures. That gives us at the end two ways to show text to the user; and both of them need work :)

- Bitmap fonts: A font where each character is a single piece on a texture and will be rendered character by character on the screen
- Vector fonts: A font where each character is drawn by lines


## Vector Fonts
Because I use vector fonts in Vec3X — A Laser Arcade Console I’m working on, I have decided to use a simple version of it in this project too.
It’s implemented in the class ```VText``` in file ```RBVText.cpp```. It contains a packed description of the alphabet and passes this information as list of vertices to the class Lines in ```RBLines.cpp```.

*The version I show here is just for static texts but we create a better version of it in the next part then.*


# 5. MazeRace — An (almost) finished Game!

I’m sure you have waited since the beginning of the tutorial on the game itself and here is it now finally!

All of the logic of MazeRace is implemented in the ```Game``` class in file ```Game.cpp``` resp. ```Game.h```.

Let’s first take a look to the header file:
The first group of methods are virtual functions. The functions where we override the game engine and building the interface between the engine and the game itself.
The second group are internal methods which helps to build the level but also controlling the player object.

```c++
...
virtual void OnCreateContent();
virtual void OnUpdateContent(float delta);
virtual void OnRenderContent(glm::mat4 projectionMatrix);
virtual void OnDetectCollision(rb::Actor* actor);
...
```

- ```OnCreateContent()``` The main thing done in this method is loading the level file level.txt and placing the game elements based on the position in the file. Besides the level itself, also the player object is created and the position saved. Like that it’s possible to reset it later when a level is win or lose.
- ```OnUpdateContent(float delta)``` The “magic” of a game happens here and in ```OnDetectCollision(rb::Actor* actor)```. Based on the state the game is in, it either shows some information to the user (like win) or process the key(s). If the right key is received, the player turns right or starts the game by changing the game state. Changing the game state results mainly in show and hide objects.
- ```OnDetectCollision(rb::Actor* actor)``` We have above discussed the game logic, the implementation happens here in this method, while two things happen:
    - Do we collide with a collectible? If yes then we check if we have collected all and if so, I change the game state to ```GAME_WIN```.
    - Do we collide with a wall? If yes then we check if this is the third time and end the level if so. The game state is changing then to ```GAME_LOOSE```

This all happens once per frame.

All the other methods in ```Game.cpp``` are just helper functions and are self explanatory I think.

```c++
...
void _CreateBlock(float x, float y, float z, glm::vec3 color, int tag);
void _SetDirection(Direction::VirtualDirection direction);
void _TurnLeft();
void _TurnRight();
void _ChangeState(GameState newState);
bool _IsKeyPressed(int code);
...
```

Huuh, that’s it. We now have a game that is really working and also (I believe) makes fun, especially when played at higher speeds.
As you will notice, the graphics look weird in some situations. This is because so far we do not take care of the Z-Ordering, the correct way of (partially) hiding objects when they are behind others. But this is a topic we solve in the next part together too.


# Tutorial:
- [Part 1 - Tool Setup and first step](../part1)
- [Part 2 - Render objects and use of the controller](../part2)
- Part 3 - Create a simple game: MazeRace
- [Part 4 - Improving MazeRace and macOS Bonus Code](../part4)


*Notes:*
*As you might noticed. Some file names start with RB while others do not.*

*RB stands for Roger Boesch (my name) of course :) but the important part is that all those files can be used in any project while the others are project specific. So if you want to create your own game use the RB files as they are and just create your own ```Game.cpp``` etc.*

*Of course you can use any of all these files as you want but if you create something cool with it, please let me know!*

