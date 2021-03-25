# **Spatial Audio Plugin** for Unreal Engine

## What?

This plugin adds *Vector Base Amplitude Panning* (VBAP) to *Unreal Engine*. It's intended as a proof of concept and a lot of work can still be done. Because UE restricts the number of speakers, and soundfield submixes are also generally designed to be used with ambisoncis or HRTF, there are a couple of hacks that needed to be employed to get this to work.

## Why?

I found the lack of support for larger speaker systems than 7.1 (8 channels) quite restricting. UE is a very powerful tool to create virtual worlds and it's nice to experience those on powerful speaker systems.

## How?

This plugin implements a **Spatializer** that is used to attach spatial information to the audio buffer, which will then be used to encode the audio to a VBAP-**SoundfieldPacket**. The plugin uses a **SoundfieldEndpointSubmix** to create a custom audio output to the hardware, to use an arbitrary number of speakers.

## Do I need this?

Well, you probably don't... But maybe you have a lot of speakers laying around and you want to spatialize your awesome sounds using all of them.


# Install the plugin in a custom project

Clone the repository *(or download as a `.zip` file.)*
```bash
git clone https://github.com/TobiasGrothmann/Spatial-Audio-Plugin-for-Unreal-Engine.git
```

The plugin is installed like any other UE plugin. Copy the folder `SpatialAudio` into your project's `Plugins` directory. *(If it does not exist, create a folder called **Plugins** next to your **.uproject** file.)*

## Option 1: Start project
Open the project and let *Unreal Engine* compile:

<img src="https://user-images.githubusercontent.com/28928394/112468057-1d24d380-8d68-11eb-98a1-75f3581a558f.png" alt="unreal engine asks to compile project" width="550px"/>

## Option 2: Generate the project files and compile from editor
#### Windows
Right-click the `.uproject` and select **generate project files**. This will create the Visual Studio project file. Open the Visual Studio (2019) project and select `Build > Rebuild Solution`. Afterwards, you can either start with a debugger attached by hitting `F5` or just close VS open the `.uproj` file.

#### MacOS
Create the `.xcworkspace` by running:
```bash
<path/to/engine>Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh -projectfiles -project=<path/to/project> -game -engine
```
open the xcode workspace and hit `cmd + r` to compile and run.

# Requirements

* **UE_4.25** on **MacOS** or **Windows**
* **audio device** that supports lots of speakers
* **lots of speakers**, well you really don't need a lot lot; a couple are enough, but you need to place them in a three-dimensional layout.
* **coordinates for all of your speakers**, you can either enter them manually or load them as a .csv file directly from the Spatial Audio Manager

# Usage

[![Image that links to a setup video](https://user-images.githubusercontent.com/28928394/112539588-75cc8e80-8db1-11eb-9e28-93b5bf1c2b39.png)](https://youtu.be/AEfmpIzblfE)

To open up the `Spatial Audio Setup`-tab click the icon in the toolbar:

<img src="https://user-images.githubusercontent.com/28928394/112381930-7864b080-8ceb-11eb-9a59-b9f089e2791c.png" alt="Button to open the Spatial Audio Setup window" width="120px"/>
<img src="https://user-images.githubusercontent.com/28928394/112382571-4e5fbe00-8cec-11eb-9893-f39506158214.png" alt="the Spatial Audio Setup window"/>


Make sure that the **Spatial Audio Spatializer is used as the spatializer plugin** in the project settings for your platform. Also make sure that there is **exactly one SpatialAudioManager** in the persistent level.

To spatialize sounds using VBAP, the checkbox ***spatialized* has to be checked** for the actor and the checkbox ***Endpoint* has to be checked** for the used sound asset. This makes sure that spatialization is used and set to "binaural" (not really the case, but UE uses this term to show that a spatializer plugin is used). It also makes sure that the audio asset is routed to the *SASoundfieldEndpoint* (Located in "*SpatialAudio Content* -> *Sound* -> *SASoundfieldEndpoint*"; select *ViewOptions* -> *Show Plugin Content*" first).

Select the SpatialAudioManager in the scene and change the settings in its details panel. **Select the right Audio Output Device**. As you can see, I am using Loopback Audio, because the output device needs to support as many speakers as you have set up in your speaker layout in the *Speaker Positions* array:

<img src="https://user-images.githubusercontent.com/28928394/112383285-376d9b80-8ced-11eb-9fcb-d9a6a94e7df7.png" alt="Spatial Audio Manager details panel" width="500px"/>


## Debugging

If anything doesn't work, **check the Console** or feel free to ask. Remember, this is just a proof of concept.

Select **Show Debug Speaker** and **Show Debug Speaker Amp** from the Spatial Audio Manager's details panel. This will spawn a speaker actor for every speaker you have setup. If you start a play session, the size of the speaker will visualize the currently used VBAP gain coeffient.

Make sure you selected the **selected the correct audio device** in the SpatialAudioManager.

# Example Project – Boids 3D Sound

To get started, try the example project which uses the Boids flocking algorithm to spatialize sounds around the listener.

<img src="https://user-images.githubusercontent.com/28928394/112385080-78ff4600-8cef-11eb-8992-61248dd8d0f7.png" alt="Screenshot of the example project using the boids algorithm"/>

