![Part I](/assets/1_title.jpeg)

# Frameworks on the Magic Leap One

On the Magic Leap One ("ML" hereafter) are three level of abstractions to create games or apps. On the highest level are the game engines like Unity and Unreal. They provide a great way of abstraction and also a wide support of assets, game logic, models, sound and abstraction of the game scene, like camera and more. It's by far the easiest way to create a great experience.

One level deeper is the the Lumin Runtime framework (*No longer supported*). It provides some useful C++ classes and abstract most of the low level API's from the programmer. It also has some support for models, sounds, animations etc. and it's great for apps and as main advantage supports the creation of landscape apps. But (at least for now) it's not designed to create games (or just simple ones).

![SDK Layers](/assets/1_sdk.png)

The level we are interested in this tutorial is the lowest level. The direct access trough the C-API. We will also create an abstraction layer (aka game engine) in C++, but at least the startup and initialisation we must do in plain C accessing the platform API. There is just bare API and OpenGL support at this level, but the advantage (or burden) to implement all by yourself exactly in the way needed. 

The reason why I write a tutorial on this is because there is almost no public information about it and also it's the level where you can port your own game or even engine without limits. As I do for my Vectrex and ZX Spectrum emulator app. But of course a lot of work. We will not create a game in this tutorial series which could be put into the app store, but at least a mini game which shows all the techniques from scratch and will be a perfect fundament to create your own native games (and apps) on the ML. **Have fun!**


# Tool Setup

Before we can program in C++ for the ML, we must first install the SDK and tools.
All you have to do for this is to download the [Lab App](https://developer.magicleap.com/downloads/lab) from the Magic Leap Creator Portal. Uncompress the zip file and double click the enclosed dmg.

![macOS Issue](/assets/1_installer.png)

Unfortunately on macOS Catalina it's still not possible to install the program out of the box, but the good news is, thats easy to fix. Even that the homepage says it's not supported, all works fine after that.

- Copy the Lab Installer App at first into your home folder
- Open a terminal window
- Enter the command ```xattr -rd com.apple.quarantine $HOME/The_Lab_Installer.app```

This will solve the issue and you can install and open the program now.

![The Lab App](/assets/1_lab_1.png)

Next use the **Package Manager** button and select straight away the *Native Framework* options.

![The Lab App](/assets/1_lab_2.png)

Press the *Apply changes* button and wait until all is downloaded. From now on you don't need The Lab App anymore for our purposes.


# Create a Certificate

To build an app we also need a certificate. These keys can also be created directly on the creator portal.
Just go the Publish tab, press the *Add new* button and give the certificate a name. Use also **test** for this (as me), so it will work without any change after.

After you pressed the *Generate* button, the private and public key will be generated and a zip file containing the private key is automatically downloaded.

It's **important** that you store the **test.privkey** file which is inside of the zip file in the same folder as your **test.cert** file which can be downloaded after some seconds from the same page. Put both files into a folder folder and create an evironment variable pointing to it:

```export MLCERT=\<path to your cert file>\test.cert```

*Note: The certificate can be used for as many apps you want. It's not related to the app bundle, but to your account!*


# The SDK

For the rest of the tutorial you can use the code editor of your choice and the terminal program. That's all we need at first.
While there is a little advantage of using Visual Studio Code (VSC) because of the available plugin; it's not really needed for what we do here in this tutorial. To use the SDK and tools, Magic Leap even provides a shell script to setup.
```envsetup.sh``` is located in ```$HOME/MagicLeap/mlsdk/<version>```.

Load it by type ```source envsetup.sh``` or even better include this in your bash/zsh profile to automatically load it whenever you open a new terminal window.

To check if all is working type in ```mabu - help```. If all done well you see the help screen of the package manager and you are ready to start now with coding.


# The (most simple) Project Template

As written before, if you use VCS then even some project templates are provided. But to explain the basics and also create your own starting point I want create it manually from a basic set of files I have.

Before I describe the different files, please run at first ```./build.sh``` to see if you get no errors. You should see something like this.

![Build results](/assets/1_cli.png)

Congrats! You have already compiled and linked your first C/C++ program for the ML. Of course, not yet very impressive, but wait... step by step :)


First let me briefly explain what the meaning of the different file is:

- ```build.sh``` A simple shell script so you don't have to remind the syntax to build. Also it can be easy used to call from inside your editor. It calls the package manager and also install the app o device after
- ```ctest.mabu``` This is the equivalent of a make file and tells the toolchain what we want to create, what source files we need for it as well as some options. See more below
- ```ctest.package``` The list of resources we "pack" in the app bundle
manifest.xml - The app manifest
- ```src/main.cpp``` The Hello World for ML (Just logs the text, nothing fancy yet)


## The (meta-) make file: ctest.mabu

- ```KIND=program``` We want create a program, an ML app
- ```INCS=src/``` Where the compiler searches for the header files
- ```SRCS=src/main.cpp``` The source file(s) we compile and link together
- ```DEFS.device=ML_DEVICE``` A precompiler flag we set, so we know in the source that we compile for the ML (We will use that later)
- ```USES=ml_sdk stdc++ OpenGL``` The libraries we link
- ```OPTIONS=standard-c++/11 exceptions/on warn/on``` Some compiler options


So far it's not so different from any C/C++ development on other platforms. We just us *mabu* provided by the SDK as a make tool. Of course you can use any other make tool as well. But this one is very simple to setup, use and extend. So I highly recommend.


## The Package File: ctest.package

Besides the binary mostly every app will also have some assets to load and use at runtime. Like sounds, 3D files, shaders etc.
The package file is where all this gets defined. So far it's very simple but we will extend soon:

- ```REFS = ctest``` Reference to the mabu file (project name, must be the same as the mabu file, like ctest.mabu)
- ```DATAS.device = \``` Specifies the path from where the assets get loaded and where inside the app bundle the they get stored.


## The Manifest File: ctest.manifest

```xml
<manifest
    xmlns:ml="magicleap"
    ml:package="com.rogerboesch.ctest"
    ml:version_code="1"
    ml:version_name="1.0">
    <application ml:visible_name="C TEST" ml:sdk_version="1.0" ml:min_api_level="1">
        <uses-privilege ml:name="WorldReconstruction"/>
        <uses-privilege ml:name="LowLatencyLightwear" />
        <component ml:name=".fullscreen" ml:visible_name="C TEST" ml:binary_name="bin/ctest" ml:type="Fullscreen">
            <icon ml:model_folder="Empty" ml:portal_folder="Empty" />
        </component>
    </application>
</manifest>
```

The manifest file contains some information used at runtime by the  platform. Similar to an iOS or Android app it contains the visible name (showed to the user), but also privacy related settings, among others
Important for now are:

- ```ml:package="com.rogerboesch.ctest"``` The bundle id of the app
- ```ml:name=".fullscreen"``` Defines that our app will run in fullscreen mode, which means runs immersive on the device.
- ```ml:binary_name="bin/ctest"``` The binary that gets started by the platform when the user starts an app

Still not impressed? :) I agree, just show a log message on a mixed reality device is not what blows away.
So at least let's start integrating OpenGL in this part 1 so we can focus in the next articles on the stuff that makes more fun ...
Before we can start with the fancy things, we must create some boilerplate code at first, very similar to any mobile device to use OpenGL.


# Initialize Graphics: OpenGL

This is one of the more boring things to do and also not a very well documented part. Because the entire ML platform and API is written in C (like most platforms) we use this for the startup code and have to initialize all. After we can "forget" it because we will work on a more abstract level and just in C++. The base we create here, can be used for any kind of games or apps without modification. So it's a one time job mainly.

You can find the code I will describe below in the ```main-ogl.cpp``` file instead of ```main.cpp```. We will us this one in part two then.

*To make it better readable I just describe here the code from main-ogl.cpp without showing, but you can find the full source code [here](src/main-ogl.cpp) with the corresponding numbers. So I highly suggest to open the file while you read.*

1. A basic structure to save the OpenGL related vars and assign some important functions related to them
2. Define the OpenGLES configuration and create the context. Also define the functions to activate and deactivate the context. Just use this as is.
3. Specifies the application lifecycle functions. This is not related to OpenGL but some essential callbacks from the platform to inform the app about its state. We will implement this further in the tutorial to integrate the game engine.
4. The typical main entry function you find in every C/C++ program
5. Assignment of the application lifecycle functions
6. Handover to the platform, so it will know how to communicate with the game/app
7. Set the OpenGL surface and pass it over to the platform in MLGraphicsCreateClientGL
8. Now we inform the platform that the app is ready and can receive callbacks
9. In this first part with go into an endless loop. Later we assign some controller buttons to let the program exit the loop and end the program
10. The code inside the loop is called every frame and we start it with a call to MLGraphicsInitFrameParams.
11. After the projection type, surface scale and near_clip are defined and MLGraphicsBeginFrame is called with this values
12. Now comes something a little bit different then you might know from normal OpenGL apps. Of course the ML has in reality two screens (One for each eye) and we must therefore render two different framebuffers. One for each camera/display. So we have to bind it to each of the virtual cameras and set the viewport on it
13. After we clear the framebuffer with black (which means transparent on the ML) and are ready at that point to display content. This is exactly what we do at next in part 2.
14. Synchronises the graphic client
15. Ends the frame and we are ready to display the next one.

All this should happen 60 or more times per seconds and so this part must be written very careful and efficient to reach the frame rate we want.

That's for now! I know, might be a little bit boring so far but if you understand how to setup and initialise all the basics, you will find the rest is more common to any other OpenGL development and also where the real fun starts.


# Summary

With this part 1 we have make the groundwork for the cooler things in the next part of this tutorial where we will display some objects and also 
use the controller to manipulate them.

# Tutorial:
- Part 1 - Tool Setup and first step
- [Part 2 - Render objects and use of the controller](../part2)
- [Part 3 - Create a simple game: MazeRace](../part3)
- [Part 4 - Improving MazeRace and macOS Bonus Code](../part4)


*Remark:*
*In the SDK is also a sample called application_framework which uses a similar approach I describe here, but focused more on apps. Unfortunately there is almost no documentation or info attached, so I believed it makes sense to start with this tutorial from scratch.*

Visit also my blog at [rogerboesch.com](http://www.rogerboesch.com) where I write about retro game development for the machines of the 80s like the Sinclair ZX Spectrum, Vectrex, Atari 2600 and many others.