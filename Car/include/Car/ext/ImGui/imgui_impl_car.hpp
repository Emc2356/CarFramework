#pragma once

#include <imgui.h>


// maybe a far fetch but as some point when 
// Car has a proper renderer it might be a 
// good idea to use that as the renderer instead
// of pure Rendering API calls so it can work
// with multiple Rendering APIs


ImGuiKey ImGui_ImplCar_KeyToImGuiKey(int key);
