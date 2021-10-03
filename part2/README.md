![Part II](/assets/2_title.png)

In the first part of the tutorial I have setup the tools and build the groundwork to create a game in C++.

This second part is about render objects and also I will talk about how to use the controller.


# Brief Overview

In this part 2 I will therefore cover the following topics:

1. Implement shaders
1. Using a shader
1. Add an object to the scene
1. Display the object in space
1. Integrate and use the ML Controller


# Tutorial

## 1. Implement Shaders

Like every (modern) OpenGL program, we will need shaders to display something on the “screen”. That’s not different on the ML.
For the scope of this tutorial I assume that you have a basic understanding of shaders. If not, I would highly recommend that you read some introduction articles about it like [this](https://learnopengl.com/Getting-started/Shaders).
Adding a shader program to the ML is quite easy. I personally prefer to create a folder called *shader* and put both shader files in it.
After we just have to register this folder in the ```ctest.package``` file:

```
REFS = ctest
DATAS.device= shader/ : shader/
```

Keep in mind that you don’t have to add each file in an asset folder. The package manager copies **all** of the files in the folder automatically into the application bundle. But let’s now take a look into the shader files.

The vertex shader is used to transform the attributes of vertices (points of a triangle) like position and direction from the original space to the display space. It allows the original objects to be distorted or reshaped in any manner. In our example we just transform each vertex in space by multiplying the project matrix with the vertex itself *(line 6)*.

```c
#version 330 core
in vec3 coord3D;
uniform mat4 projFrom3D;
void main(void) {
  gl_Position = projFrom3D * vec4(coord3D, 1);
}
```

This is the most simplest form of a vertex shader. But no worry, we will extend this later on during the tutorial series in many ways to create some special effects.
The second shader file we need is the fragment shader. The fragment shader takes the output from the vertex shader and associates colors, depth value of a pixel, etc.
After these operations the fragment is sent to the framebuffers for display on the screen.

```c
#version 330 core
uniform vec3 color;
out vec4 fragColor;
void main(void) {
  fragColor = vec4(color, 1);
}
```

This one is even more simpler because we just pass a color from our C++ code to it (line 6). We will also enhance this later to create some visual effects.


## 2. Using a Shader

Now we can use the shader in our program. At the end, a shader program is also just a computer program that must be compiled and linked, so it can be executed later on the [GPU](https://en.wikipedia.org/wiki/Graphics_processing_unit).

And that’s what we do now. I’ve implemented the code to load, compile and link a shader in the file ```RBSShader.cpp``` which we also have to add to our make file ```ctest.mabu``` *(line 4)*.

```
KIND = program
INCS = src/
SRCS = src/main.cpp \
src/RBShader.cpp \
DEFS.device = ML_DEVICE
USES = ml_sdk OpenGL stdc++
OPTIONS = standard-c++/11 exceptions/on warn/on
```

I don’t go into every detail of the code, but the usage should be self-explanatory.

```c++
void Shader::Load(const char * vertex_file_path, const char * fragment_file_path);
```

The most important part is done in the ```Load()``` method to which we pass the path to the vertex and fragment shader files. It loads the files into memory, compile and link them and hold a reference for later use.


## 3. Add an Object to the Scene

That’s now the point where I can actually start to show something on the screen(s), resp. in the spatial environment of the ML. We start simple by adding a primitive to the project. Unlike a Unity project we can’t just use a primitive or even a model, I have to create them by myself vertex by vertex, apply a color (and later textures). But once it’s done, those base objects can be used in many different ways.
But step by step :). As with ```RBShader.cpp``` before, I add now the file ```RBCylinder.cpp``` to the project.

```c++
#pragma once
class Shader;
class Cylinder {
public:
 Cylinder(int steps);
 ~Cylinder();
public:
  void SetPosition(float x, float y, float z) { _position = glm::vec3(x, y, z); }
  void SetRotation(float x, float y, float z) { _rotation = glm::vec3(x, y, z); }
  void SetScale(float x, float y, float z) { _scale = glm::vec3(x, y, z); }
  void SetColor(glm::vec3 color) { _color = color; }
  void ApplyShader(Shader& shader);
  void Render(glm::mat4 projectionMatrix);
private:
  int _steps = 0;
  int _verts = 0;
  GLuint _progId;
  GLuint _vaoId;
  GLuint _projId;
  GLuint _colorId;
  glm::vec3 _color;
  glm::vec3 _position;
  glm::vec3 _rotation;
  glm::vec3 _scale;
};
```

This will be the structure of every primitive I create and use in the tutorial.

*Remark: In this second part we use a simplification by not following good OO principles. Later we will refactor this class slightly to make it more universal and also use the shader more generic. But to explain at first it’s the most simplest way to do so.*

The methods from line 11–14 are self-explanatory, I just added them to manipulate the cylinder’s position, rotation, scale and color.

The interesting methods are the two in line 16 and 17 respectively.
In line 17 the shader class gets applied (which holds a reference to the compiled shader code) and apply it, so we can after use it to render.

This is done by the method ```Render()``` in *line 18*. I must pass in the projection matrix (more about this later) and so the ```Cylinder``` class will be able to calculate its position and send the vertices to the (vertex) shader. Make sense? :)

As you can see; when we break it down into pieces, it’s not so complicated to understand.
Let’s see now how this both methods are implemented.

```c++
void Cylinder::ApplyShader(Shader& shader) {
  _progId = shader.GetProgramID();
  glUseProgram(_progId);
  _colorId = glGetUniformLocation(_progId, “color”);
  _projId = glGetUniformLocation(_progId, “projFrom3D”);

  GLuint location = glGetAttribLocation(_progId, “coord3D”);
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);
  GLfloat cylinderVertexData[_verts * 3];
  MakeCylinder(cylinderVertexData, _steps);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
  glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderVertexData), cylinderVertexData, GL_STATIC_DRAW);
  glBindVertexArray(0);
  glUseProgram(0);

  ML_LOG_TAG(Verbose, APP_TAG, “Uniform location (%d, %d, %d), program %d”, _colorId, _projId, location, _progId);
}
```

It’s a little bit out of scope to go into every detail how OpenGL works under the hood. As mentioned there are many great resources to learn more about this. But in easy words:

- In *line 2–3* we get a reference to the shader program we will use later
- We also save the references to the shader variables to use them after in the ```Render()`` method (*line 5–7*)
- After (*line 9–20*) we create a dynamic array which holds all vertex points from the cylinder. I made it slightly more flexible by allowing to specify how many steps it uses to form the cylinder. This will help us later to decide at runtime how detailed we want it. Also I tell OpenGL to create an internal vertex buffer and bind it to the data created by the ```MakeCylinder()``` function. That’s it :)


Now we are ready to implement the ```Render()``` method:

```c++
void Cylinder::Render(glm::mat4 projectionMatrix) {
  glUseProgram(_progId);
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), _position);
  glm::mat4 scale = glm::scale(_scale);
  glm::mat4 transform = projectionMatrix * translate * scale;

  glBindVertexArray(_vaoId);
  glUniformMatrix4fv(_projId, 1, GL_FALSE, &transform[0][0]);
  glUniform3f(_colorId, _color[0], _color[1], _color[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, _verts);
  glBindVertexArray(0);
  glUseProgram(0);
}
```

To render the cylinder is now an easy thing:

- In *line 2* we tell OpenGL again which shader I want to use
- In line *4–6* I calculate the transform of the cylinder by multiplying the projection matrix with the translation matrix (position) and scale of the cylinder. (The rotation part I implement later)
- That is what it needs and I just pass this information along with the color (*line 10*) to the shader and draw the vertices in *line 11*

Ok, take a break now! The “hard” part is done and we can just use this first primitive object in an easy way for different purposes, thanks to the beauty of OO :)


## 4. Display the Object in Space

You have seen now all parts to finally visualize the cylinder. I just have to instantiate it now, apply the shader and call render. Do see as fast as possible a result, I do that for this part 2 directly in the ```main.cpp``` file. In the next part (where we actually implement the game) I will start creating a small game engine and implement it there. But for now:

```c++
...
// Part 2: Instantiate the shader
Shader shader3D = Shader(16);
shader3D.Load(“shader/standard3D.vert”, “shader/standard.frag”);

// Part 2: Instantiate the object(s) here
Cylinder cylinder = Cylinder();
cylinder.ApplyShader(_shader3D);
cylinder.SetColor(COLOR_RED);

// The main/game loop
while (true) {
...
```

Directly before the main loop (*line 12–13*) we must insert the new code for part 2 in ```main.cpp```:

- At first we load the shader which we will use after. This must be done just once, the shader will then be reused by every object in our game (*line 4–5*)
- After we create a cylinder and apply the shader (*line 8 to 10*)
- That’s all to create an object and render it, it’s also not much more complex.

```c++
...
glClearColor(0.0, 0.0, 0.0, 0.0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Part 2: Get the projection matrix
MLGraphicsVirtualCameraInfo &current_camera = virtual_camera_array.virtual_cameras[camera];
glm::mat4 projectionMatrix = rb_projection_matrix(current_camera) * rb_camera_matrix(current_camera);

// Part 2: Render the object
cylinder.Render(projectionMatrix);
...
```

The right place to render an object (the cylinder) is directly after cleared the frame buffer (*line 5*), what I have already done in the previous part.

So the only things remains, is to call the ```Render()``` method of the cylinder as you see in *line 12*.

But wait... from where we got the projection matrix which is needed as a parameter. The projection matrix in a 3D game represents the (virtual) camera; the view we have to the game scene. And this is where the “magic” comes in on a device like the ML.

We have here two cameras (one for each eye) and therefore two projection matrices. I must render therefore also each object **twice**.

Luckily there is an API call which delivers us this information (*line 8*) and the only thing which I must do manually is to calculate the projection matrix
(done by two helper functions I’ve added to ```main.cpp```) out of this (*line 9*)!

With that I laid out the groundwork to render an object and we can start implementing a real game in part 3.
But there is something more we should do now. We talked about displaying an object but what about user input? For a real game we need the functionality of user input to implement interaction. The easiest way to do this on the ML, is by using the controller.

And that’s how we want to end this part ...


### 5. Integrate and use the ML Controller

Luckily this is also straightforward thanks to an ML API which delivers us the information about a controller.

The following code must be inserted just after entering the main loop in ```main.cpp```:

```c++
// Part 2: Get state of the Controller
MLInputControllerState input_states[MLInput_MaxControllers];
CHECK(MLInputGetControllerState(input, input_states));

for (int k = 0; k < MLInput_MaxControllers; ++k) {
  if (input_states[k].is_connected) {
    if (input_states[k].button_state[MLInputControllerButton_Bumper]) {
       glm::vec3 pos = cylinder.GetPosition();
       cylinder.SetPosition(pos.x, pos.y+0.001, pos.z);
       cylinder.SetColor(COLOR_GREEN);
    }
  }
}
```

- At first I ask in *line 2 and 3* for the states of all connected controllers
- Then in *line 7* I check if the bumper button on the controller is pressed and if yes
- I just change for demonstration purposes the position and color of the cylinder in *line 8–12*

Of course there is much more user input we can ask for, but for that I have to explain some additional callback functions at first (to get information about the touchpad for example), but at first that’s not so bad!

That’s it for now and I hope you join again for the next part when I create a little fun game based on this groundwork we have laid out.
Have fun with the code and try to implement some additional cylinders in the meantime, let them move, scale or change color and get used of the concepts behind we already talked about in this first two parts.

*When you compile and link the sample, you will see 6 warnings in ```main.cpp```. Don’t worry about them for now.*

We will get rid of them later in the tutorial!

Also you have seen that I use [glm](https://glm.g-truc.net/0.9.9/index.html), a widely known mathematics library which is also used by the ML API itself.


# Tutorial:
- [Part 1 - Tool Setup and first step](../part1)
- Part 2 - Render objects and use of the controller
- [Part 3 - Create a simple game: MazeRace](../part3)
- [Part 4 - Improving MazeRace and macOS Bonus Code](../part4)


*Remark:*
*In the SDK is also a sample called application_framework which uses a similar approach I describe here, but focused more on apps. Unfortunately there is almost no documentation or info attached, so I believed it makes sense to start with this tutorial from scratch.*

Visit also my blog at [rogerboesch.com](http://www.rogerboesch.com) where I write about retro game development for the machines of the 80s like the Sinclair ZX Spectrum, Vectrex, Atari 2600 and many others.