
<h1 align="center">
🌀 Vortex Game Engine 🌀
</h1>

<div align="center">
  
![Static Badge](https://img.shields.io/badge/Platform-Windows-blue?style=plastic)
![GitHub License](https://img.shields.io/github/license/dicieous/VortexGameEngine?style=plastic&color=blue)
![GitHub repo size](https://img.shields.io/github/repo-size/dicieous/VortexGameEngine?style=plastic&color=blue)

</div>

#### *"Yes, I know Unreal and Unity exist. No, I don’t want to talk about it."* 🙂

## **Overview**
- Vortex Game Engine is an in-development Game Engine, designed as a portfolio project to explore modern game Engine Development.You can clone the repository and experiment with making games using Vortex, but it is not yet at the level of a professional game engine.

<br>

## **Showcase**

![Engine](https://github.com/dicieous/VortexGameEngine/blob/6a0279746eace38afedbabbefe9fd8af9716dd3f/Readme/Engine_Showcase_Image.png)

<br>

## **Features**

### Current Features
The Engine is currently in it's initial days with some of the Basic Engine Features. I'll Keep on Updating the Table as I add new Features.

| **Feature**   |  **Description**        |
| :-------- |  :------------------------- |
| 2D Rendering Engine |  *2D Renderer Supporting Sprite Based Rendering & Shader Support.* |
| 2D Physics Engine|  *Basic 2D Physics Engine with Box2D in the Backend.* |
| Project Management System |  *Basic Project Management using YAML.* |
| Scene Management System |  *Scene Management support using YAML for smooth integration with ECS.* |
| Entity Component System (ECS)|  *ECS integration with entt as the Base.* |
| Asset Management System |  *Basic Asset Management system with Caching.* |
| GUI Editor |  *Game Engine Editor Layer which uses ImGui at backend for GUI.* |
| Event System |  *Event system API which works seamlessly with the Different Inputs in the Game Engine from the user Side too.* |
| Logger |  *Logging System Implemented using Spdlog* |

### Future Features
These are the features planned by me as of yet. Would Keep on Adding more as the list starts getting Shorter.

| **Feature**   |  **Description**        |
| :-------- |  :------------------------- |
| 3D Rendering Engine |  *3D Renderer to Supporting PBR, IBL, Shadows and other 3D Rendering Features* |
| 3D Physics Engine  |  *3D Physics Support with Physics Materials integrated with Bolt.* |
| C# Scripting |  *C# Scripting Engine support for User Side.* |
| Audio Library |  *Audio Library with both 2D and 3D Audio Support.* |
| Profiler |  *currently the Engine have a very basic Profiler. Will Implement a GUI based profiler (Most Probably Tracy)* |
| Glyphs |  *Would Implement Font Rendering using Slug Library(Most Probably).* |

<br>

## **Getting Started**

## Prerequisites

Ensure you have the following installed on your system:
- A modern C++ compiler supporting C++17 (or higher).
- [Premake](https://premake.github.io/) (v5+ recommended)
- Git

### Building the Project


1. **Clone the Repository**: Make sure you do a **recursive** clone!
```bash
  git clone --recursive https://github.com/dicieous/VortexGameEngine.git
```

2. **Navigate to the Project Directory**:

```bash
  cd VortexGameEngine
```

3. **Build the Project**: 
  - If you're in Command Prompt and in the Root Directory of the Engine below command to run the batch file to run premake to build the Project.
  ```bash
  Scripts/Win-GenProjects.bat
```
  - You can just go in the Scripts folder and click on ```Win-GenProjects.bat``` to Build the Project.

### Running the Application

- After a successful build, you can open the engine with the ```VortexEngine.sln``` and build the binaries in Debug or Release and Run the Project.
- You'll be greeted with the Vortex Editor. There you can press Play button to see physics in play, Add and Delete Components and do other stuff.
- As the Engine is still in development you cannot make a game from the Editor yet. But if you want to Play with the Engine you can set the Sandbox Project as the StartUp Project and Try stuff there in ```SandBox2D.cpp``` or you can make your own layer and Add it in the ```Sandbox Class``` in ```SandBoxApp.cpp```.

```cpp
class SandBox : public Vortex::Application
{
public:
	SandBox(Vortex::ApplicationSpecifications& specfications)
		:Vortex::Application(specfications)
	{
		//Push your Layer Here
		PushLayer(new SandBox2D());
	}

	~SandBox()
	{

	}
};

Vortex::Application* Vortex::CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecifications specs;
	specs.Name = "Vortex SandBox";
	specs.WorkingDirectory = "../VortexEditor";

	return new SandBox(specs);
}
```

<br>

## **Project Structure**

```Sandbox/```: Contains Sandbox to test Engine Features.

```VortexEditor/```: Contains the Editor related Code and Editor Resources.

```VortexEngine/```: Contains the Core Engine Source Code and it's Dependencies.

```VortexEngine.sln```: Solution file for building the project.

```.gitignore```: Specifies files and directories to be ignored by Git.

```premake5.lua```: Contains the code for Building the Project.

```LICENSE```: The project's license information.

```README.md```: This readme file.

<br>

## Dependencies

- [**OpenGL**](https://www.opengl.org/): Verify that your system supports OpenGL and that the necessary drivers are installed.

 - [**GLAD**](https://glad.dav1d.de/): Used for loading OpenGL function pointers.

- [**ImGui**](https://github.com/ocornut/imgui): Immediate Mode GUI library for debugging and UI development.

- [**spdlog**](https://github.com/gabime/spdlog): Fast C++ logging library.

- [**EnTT**](https://github.com/g-truc/glm): Entity-component-system (ECS) library.

- [**Box2D**](https://github.com/erincatto/box2d): Box2D is a 2D physics engine for games.

- [**ImGuizmo**](https://github.com/erincatto/box2d): Gizmo manipulation for ImGui.

- [**stb_image**](https://github.com/nothings/stb/tree/master): Image loading library.

- [**GLM**](https://github.com/g-truc/glm): A math library for handling matrices and vectors in graphics programming.

- [**premake5**](https://premake.github.io/): Build system configuration.

- [**Yaml-cpp**](https://github.com/jbeder/yaml-cpp): YAML parser and emitter for C++.

<br>

## **License**

This project is licensed under the [Apache-2.0](https://choosealicense.com/licenses/apache-2.0/) License. See the ```LICENSE``` file for more details.

<br>

## **Acknowledgements**

- [**The Cherno**](https://www.youtube.com/@TheCherno): For Teaching me C++ and Game Engine Related Stuff.

- [**LearnOpenGL**](https://learnopengl.com/): For Tutorials and Engaging Learning Experience.

- [**Game Engine Architecture**](https://learnopengl.com/): A very good Resource for Game Engine Development.

- [**GameMath**](https://gamemath.com/book/): A real Good Maths book for Game Dev in General.

<br>
