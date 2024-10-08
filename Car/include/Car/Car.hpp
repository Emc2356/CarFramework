#pragma once

/////////////////////////////////////////
////////////// APPLICATION //////////////
/////////////////////////////////////////
#include "Car/Application.hpp"
#include <imgui.h>
#include "Car/Time.hpp"
#include "Car/Core/Timestep.hpp"
#include "Car/Core/Log.hpp"
#include "Car/ResourceManager.hpp"
#include "Car/Random.hpp"

/////////////////////////////////////////
//////////// POLLING SYSTEM /////////////
/////////////////////////////////////////
#include "Car/Input.hpp"

/////////////////////////////////////////
/////////// GENERAL UTILITIES ///////////
/////////////////////////////////////////
#include "Car/Utils.hpp"

/////////////////////////////////////////
/////////// RENDERING ITEMS /////////////
/////////////////////////////////////////
#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Renderer2D.hpp"
#include "Car/Renderer/Font.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/SSBO.hpp"

/////////////////////////////////////////
///////////// LAYER SYSTEM //////////////
/////////////////////////////////////////
#include "Car/Layers/Layer.hpp"
#include "Car/Layers/ImGuiLayer.hpp"

/////////////////////////////////////////
///////////// EVENT SYSTEM //////////////
/////////////////////////////////////////
#include "Car/Events/Event.hpp"
#include "Car/Events/WindowEvent.hpp"
#include "Car/Events/MouseEvent.hpp"
#include "Car/Events/KeyEvent.hpp"

/////////////////////////////////////////
/////////////// CONSTATS ////////////////
/////////////////////////////////////////
#include "Car/Constants/GeneralCodes.hpp"
#include "Car/Constants/MouseCodes.hpp"
#include "Car/Constants/KeyCodes.hpp"

/////////////////////////////////////////
///////////// ENTRY POINT ///////////////
/////////////////////////////////////////
#ifdef CR_ENTRY
#include "Car/EntryPoint.hpp"
#endif
