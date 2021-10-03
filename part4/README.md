![Part IV](/assets/4_title.png)

What’s next? Even though I have now a simple game, it still needs many things to call it a finished game.

This part will not add many new concepts but focus on improving the game itself.


# Brief Overview

In part 4 I want cover the following topics:

1. Z-Buffering and Sorting
2. Adding a life bar
3. BONUS: A testing environment for macOS!


# Tutorial

## 1. Z-Buffering and Sorting

In computer graphics, z-buffering, also known as depth buffering, is the management of image depth coordinates in 3D graphics, usually done in hardware. It is one solution to the visibility problem, which is the problem of deciding which elements of a rendered scene are visible, and which are hidden.

The z-buffer uses the Image space method for hidden surface detection. A z-buffer can refer to a data structure or to the method used to perform operations on that structure.

In a 3d-rendering engine, when an object is projected on the screen, the depth (z-value) of a generated pixel in the projected screen image is stored in a buffer (the z-buffer or depth buffer). A Z-value is the measure of the perpendicular distance from a pixel on the projection plane to its corresponding 3d-coordinate on a polygon in world-space.

The z-buffer has the same internal data structure as an image, namely a 2d-array, with the only difference being that it stores a z-value for each screen pixel instead of pixel data. It has the same dimensions as the screen buffer, except when multiple z-buffers are used, such as in split-screen rendering. It operates in screen-space and takes as its input a projected image that originates from a projection of an object to the screen.


![Z-Buffering — Image from Wikipedia](/assets/4_zbuffer.png)

Luckily this technique is well supported by OpenGL and just needs some parameters to be defined. This happens in ```main.cpp```

```c++
...
// Part 4: Get camera position
glEnable(GL_DEPTH_TEST);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
...
```

We just have to enable depth testing (*line 1*) and also blending (*line 2–3*).


### Sorting

On most devices this almost all we have to do to support a correct Z-ordering. On the ML1 however we have an additional challenge. All of the objects rendered are kind of transparent to create the illusion of hologram like appearance. Therefore it’s also important to render all objects in their right order.That means if an object is more far away from the viewer it will be drawn at first and then the objects which are closer to the camera. How do we achieve this?

Its actually quite simple: We just **sort** the objects by their distance to the camera. But because the camera (glass) is also moving. This sorting must be done on every frame. This is done in ```OnRenderContent()``` in ```RBEngine.cpp```:

```c++
...
// Part 4: Sort back-to-front
std::sort(_actors.begin(), _actors.end(), [this](Actor* a, Actor* b {
  rb::Primitive* primitiveA = dynamic_cast<rb::Primitive*>(a);
  rb::Primitive* primitiveB = dynamic_cast<rb::Primitive*>(b);
  
  const auto dist1 = glm::distance(_cameraPosition,   primitiveA>GetPosition());
  const auto dist2 = glm::distance(_cameraPosition, primitiveB->GetPosition());
  return dist1 > dist2;
});
...
```

That’s everything we have to do, to make it look correct.


## 2. Adding a life bar

After our game scene looks now correct, the next steps are about adding more information to the game.

At first a way to tell the user how many “lives” we have. As you remember, the player can actually not touch more than 3 times the wall before he dies. This information we want show in a graphic way; with **life bars**:

![Life bars — Example from an RPG Game](/assets/4_lifebars.png)

You probably know this from RPG Games, where a small bar on top of the characters show their lives, health or any other status information.

In our game we have 3 states:

- Green: All fine, still have 3 lives
- Orange: One live already lost
- Red: Just one more life before the player dies

So our life bar will consist of 3 cubes which are showing this information and every time the player loses a life we update the color.
And because we want to see this information directly over the player we also update constantly the position.

```c++
...
void Game::_CreateLifeBar() {
  _lifeBar[0] = new rb::Primitive(rb::Primitive::Cube);
  _lifeBar[0]->SetPosition(0, 0, 0);
  _lifeBar[0]->SetTag(LIFEBAR);
  _lifeBar[0]->SetColor(COLOR_GREEN);
  _lifeBar[0]->ScaleTo(0.03, 0.01, 0.01);
  _lifeBar[1] = new rb::Primitive(rb::Primitive::Cube);
  _lifeBar[1]->SetPosition(0, 0, 0);
  _lifeBar[1]->SetTag(LIFEBAR);
  _lifeBar[1]->SetColor(COLOR_ORANGE);
  _lifeBar[1]->ScaleTo(0.03, 0.01, 0.01);
  _lifeBar[2] = new rb::Primitive(rb::Primitive::Cube);
  _lifeBar[2]->SetPosition(0, 0, 0);
  _lifeBar[2]->SetTag(LIFEBAR);
  _lifeBar[2]->SetColor(COLOR_RED);
  _lifeBar[2]->ScaleTo(0.03, 0.01, 0.01);
}

void Game::_UpdateLifeBar() {
  glm::vec3 pos = _player->GetPosition();
  _lifeBar[0]->SetPosition(pos.x-0.03, pos.y+0.1, pos.z);
  _lifeBar[1]->SetPosition(pos.x, pos.y+0.1, pos.z);
  _lifeBar[2]->SetPosition(pos.x+0.03, pos.y+0.1, pos.z);

  switch (_obstaclesHit) {
  case 0:
    _lifeBar[0]->SetColor(COLOR_GREEN);
    _lifeBar[1]->SetColor(COLOR_GREEN);
    _lifeBar[2]->SetColor(COLOR_GREEN);
    break;
  case 1:
    _lifeBar[0]->SetColor(COLOR_ORANGE);
    _lifeBar[1]->SetColor(COLOR_ORANGE);
    _lifeBar[2]->SetColor(COLOR_BLACK);
    break;
  case 2:
    _lifeBar[0]->SetColor(COLOR_RED);
    _lifeBar[1]->SetColor(COLOR_BLACK);
    _lifeBar[2]->SetColor(COLOR_BLACK);
   break;
  }
}
...
```


# 4. BONUS: A Testing Environment for macOS!

Programming for the ML1 is a lot of fun, but of course the turnaround time to *code->build->test* is not very fast. Therefore it makes sense to do the majority of the coding part on a host pc (or mac) and just do the final testing on the device. That’s also how I work personally.

I have written a simple OpenGL environment that runs on macOS and in XCode. I’ve added the basic part of this tool to this tutorial. Like that, it can even be used to play with the code if you don’t have an ML1.

![oglIDE](/assets/4_xcode.png)


# Tutorial:
- [Part 1 - Tool Setup and first step](../part1)
- [Part 2 - Render objects and use of the controller](../part2)
- [Part 3 - Create a simple game: MazeRace](../part3)
- Part 4 - Improving MazeRace and macOS Bonus Code
