# Car Engine
Car game engine, sometimes it works beatifuly and other times it just breaks.

Ok it is an insult to actual engines to call this an engine, more like a framework

a simple project born out of boardom that might be worth nothing but still a great experience

for now it only supports OpenGL but i try to limit OpenGL specific calls to only a couple of files so it is easier to add Vulkan to it later if i have the motivation 

# Dependencies
1. [glfw](https://www.glfw.org/) for the creation of the window and the events
2. [imgui](https://github.com/ocornut/imgui) for nice windows (currently the multiple viewports is disabled as it doesnt look and feel nice in linux)
3. [GLM](https://github.com/g-truc/glm) for the mathematical part of the engine
4. [GLAD](https://gen.glad.sh/) as a OpenGL loader and in the future for vulkan extension loader

# Building
I am not 100% sure why i decided to make my own build script, maybe out of frustration from using make and the unnecessary complexity of CMake
```bash
./build.py 
```
