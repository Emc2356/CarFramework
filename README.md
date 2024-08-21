# Car Framework
Car Framework, sometimes it works beatifuly and other times it just breaks.

a simple project born out of boardom that might be worth nothing but still a great experience

It has a backend for vulkan

# Dependencies
1. [glfw](https://www.glfw.org/) for the creation of the window and the events
2. [imgui](https://github.com/ocornut/imgui) for nice windows (currently the multiple viewports is disabled as it doesnt look and feel nice in linux)
3. [GLM](https://github.com/g-truc/glm) for the mathematical part of the framework
4. [GLAD](https://gen.glad.sh/) vulkan extension loader
5. [freetype](https://github.com/freetype/freetype) for the loading of fonts
6. [shaderc](https://github.com/google/shaderc) for converting shaders to spirv at runtime
7. [fmt](https://github.com/fmtlib/fmt) for prety printing and formating
8. [stb](https://github.com/nothings/stb) specifically stb_image
9. [spirv_cross](https://github.com/KhronosGroup/SPIRV-Cross) for analyzing spirv code 

# Building
I am not 100% sure why i decided to make my own build script, maybe out of frustration from using make and the unnecessary complexity of CMake.  
I recommend after cloning the repo to do `./build.py --help` for all of the options
```bash
git clone https://github.com/Emc2356/CarFramework --recursive
cd CarFramework
./build.py --deps
./build.py -j --verbose --shaderc
```
