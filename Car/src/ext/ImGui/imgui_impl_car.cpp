#include "Car/ext/ImGui/imgui_impl_car.hpp"
#include "Car/Core/Core.hpp"
#include <cstdint>
#include <glad/gl.h>

#include "Car/Constants/KeyCodes.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Renderer/GraphicsContext.hpp"
#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/Renderer/VertexBuffer.hpp"

ImGuiKey ImGui_ImplCar_KeyToImGuiKey(int key) {
    switch (key) {
    case CR_KEY_TAB:
        return ImGuiKey_Tab;
    case CR_KEY_LEFT:
        return ImGuiKey_LeftArrow;
    case CR_KEY_RIGHT:
        return ImGuiKey_RightArrow;
    case CR_KEY_UP:
        return ImGuiKey_UpArrow;
    case CR_KEY_DOWN:
        return ImGuiKey_DownArrow;
    case CR_KEY_PAGE_UP:
        return ImGuiKey_PageUp;
    case CR_KEY_PAGE_DOWN:
        return ImGuiKey_PageDown;
    case CR_KEY_HOME:
        return ImGuiKey_Home;
    case CR_KEY_END:
        return ImGuiKey_End;
    case CR_KEY_INSERT:
        return ImGuiKey_Insert;
    case CR_KEY_DELETE:
        return ImGuiKey_Delete;
    case CR_KEY_BACKSPACE:
        return ImGuiKey_Backspace;
    case CR_KEY_SPACE:
        return ImGuiKey_Space;
    case CR_KEY_ENTER:
        return ImGuiKey_Enter;
    case CR_KEY_ESCAPE:
        return ImGuiKey_Escape;
    case CR_KEY_APOSTROPHE:
        return ImGuiKey_Apostrophe;
    case CR_KEY_COMMA:
        return ImGuiKey_Comma;
    case CR_KEY_MINUS:
        return ImGuiKey_Minus;
    case CR_KEY_PERIOD:
        return ImGuiKey_Period;
    case CR_KEY_SLASH:
        return ImGuiKey_Slash;
    case CR_KEY_SEMICOLON:
        return ImGuiKey_Semicolon;
    case CR_KEY_EQUAL:
        return ImGuiKey_Equal;
    case CR_KEY_LEFT_BRACKET:
        return ImGuiKey_LeftBracket;
    case CR_KEY_BACKSLASH:
        return ImGuiKey_Backslash;
    case CR_KEY_RIGHT_BRACKET:
        return ImGuiKey_RightBracket;
    case CR_KEY_GRAVE_ACCENT:
        return ImGuiKey_GraveAccent;
    case CR_KEY_CAPS_LOCK:
        return ImGuiKey_CapsLock;
    case CR_KEY_SCROLL_LOCK:
        return ImGuiKey_ScrollLock;
    case CR_KEY_NUM_LOCK:
        return ImGuiKey_NumLock;
    case CR_KEY_PRINT_SCREEN:
        return ImGuiKey_PrintScreen;
    case CR_KEY_PAUSE:
        return ImGuiKey_Pause;
    case CR_KEY_KP_0:
        return ImGuiKey_Keypad0;
    case CR_KEY_KP_1:
        return ImGuiKey_Keypad1;
    case CR_KEY_KP_2:
        return ImGuiKey_Keypad2;
    case CR_KEY_KP_3:
        return ImGuiKey_Keypad3;
    case CR_KEY_KP_4:
        return ImGuiKey_Keypad4;
    case CR_KEY_KP_5:
        return ImGuiKey_Keypad5;
    case CR_KEY_KP_6:
        return ImGuiKey_Keypad6;
    case CR_KEY_KP_7:
        return ImGuiKey_Keypad7;
    case CR_KEY_KP_8:
        return ImGuiKey_Keypad8;
    case CR_KEY_KP_9:
        return ImGuiKey_Keypad9;
    case CR_KEY_KP_DECIMAL:
        return ImGuiKey_KeypadDecimal;
    case CR_KEY_KP_DIVIDE:
        return ImGuiKey_KeypadDivide;
    case CR_KEY_KP_MULTIPLY:
        return ImGuiKey_KeypadMultiply;
    case CR_KEY_KP_SUBTRACT:
        return ImGuiKey_KeypadSubtract;
    case CR_KEY_KP_ADD:
        return ImGuiKey_KeypadAdd;
    case CR_KEY_KP_ENTER:
        return ImGuiKey_KeypadEnter;
    case CR_KEY_KP_EQUAL:
        return ImGuiKey_KeypadEqual;
    case CR_KEY_LEFT_SHIFT:
        return ImGuiKey_LeftShift;
    case CR_KEY_LEFT_CONTROL:
        return ImGuiKey_LeftCtrl;
    case CR_KEY_LEFT_ALT:
        return ImGuiKey_LeftAlt;
    case CR_KEY_LEFT_SUPER:
        return ImGuiKey_LeftSuper;
    case CR_KEY_RIGHT_SHIFT:
        return ImGuiKey_RightShift;
    case CR_KEY_RIGHT_CONTROL:
        return ImGuiKey_RightCtrl;
    case CR_KEY_RIGHT_ALT:
        return ImGuiKey_RightAlt;
    case CR_KEY_RIGHT_SUPER:
        return ImGuiKey_RightSuper;
    case CR_KEY_MENU:
        return ImGuiKey_Menu;
    case CR_KEY_0:
        return ImGuiKey_0;
    case CR_KEY_1:
        return ImGuiKey_1;
    case CR_KEY_2:
        return ImGuiKey_2;
    case CR_KEY_3:
        return ImGuiKey_3;
    case CR_KEY_4:
        return ImGuiKey_4;
    case CR_KEY_5:
        return ImGuiKey_5;
    case CR_KEY_6:
        return ImGuiKey_6;
    case CR_KEY_7:
        return ImGuiKey_7;
    case CR_KEY_8:
        return ImGuiKey_8;
    case CR_KEY_9:
        return ImGuiKey_9;
    case CR_KEY_A:
        return ImGuiKey_A;
    case CR_KEY_B:
        return ImGuiKey_B;
    case CR_KEY_C:
        return ImGuiKey_C;
    case CR_KEY_D:
        return ImGuiKey_D;
    case CR_KEY_E:
        return ImGuiKey_E;
    case CR_KEY_F:
        return ImGuiKey_F;
    case CR_KEY_G:
        return ImGuiKey_G;
    case CR_KEY_H:
        return ImGuiKey_H;
    case CR_KEY_I:
        return ImGuiKey_I;
    case CR_KEY_J:
        return ImGuiKey_J;
    case CR_KEY_K:
        return ImGuiKey_K;
    case CR_KEY_L:
        return ImGuiKey_L;
    case CR_KEY_M:
        return ImGuiKey_M;
    case CR_KEY_N:
        return ImGuiKey_N;
    case CR_KEY_O:
        return ImGuiKey_O;
    case CR_KEY_P:
        return ImGuiKey_P;
    case CR_KEY_Q:
        return ImGuiKey_Q;
    case CR_KEY_R:
        return ImGuiKey_R;
    case CR_KEY_S:
        return ImGuiKey_S;
    case CR_KEY_T:
        return ImGuiKey_T;
    case CR_KEY_U:
        return ImGuiKey_U;
    case CR_KEY_V:
        return ImGuiKey_V;
    case CR_KEY_W:
        return ImGuiKey_W;
    case CR_KEY_X:
        return ImGuiKey_X;
    case CR_KEY_Y:
        return ImGuiKey_Y;
    case CR_KEY_Z:
        return ImGuiKey_Z;
    case CR_KEY_F1:
        return ImGuiKey_F1;
    case CR_KEY_F2:
        return ImGuiKey_F2;
    case CR_KEY_F3:
        return ImGuiKey_F3;
    case CR_KEY_F4:
        return ImGuiKey_F4;
    case CR_KEY_F5:
        return ImGuiKey_F5;
    case CR_KEY_F6:
        return ImGuiKey_F6;
    case CR_KEY_F7:
        return ImGuiKey_F7;
    case CR_KEY_F8:
        return ImGuiKey_F8;
    case CR_KEY_F9:
        return ImGuiKey_F9;
    case CR_KEY_F10:
        return ImGuiKey_F10;
    case CR_KEY_F11:
        return ImGuiKey_F11;
    case CR_KEY_F12:
        return ImGuiKey_F12;
    case CR_KEY_F13:
        return ImGuiKey_F13;
    case CR_KEY_F14:
        return ImGuiKey_F14;
    case CR_KEY_F15:
        return ImGuiKey_F15;
    case CR_KEY_F16:
        return ImGuiKey_F16;
    case CR_KEY_F17:
        return ImGuiKey_F17;
    case CR_KEY_F18:
        return ImGuiKey_F18;
    case CR_KEY_F19:
        return ImGuiKey_F19;
    case CR_KEY_F20:
        return ImGuiKey_F20;
    case CR_KEY_F21:
        return ImGuiKey_F21;
    case CR_KEY_F22:
        return ImGuiKey_F22;
    case CR_KEY_F23:
        return ImGuiKey_F23;
    case CR_KEY_F24:
        return ImGuiKey_F24;
    default:
        return ImGuiKey_None;
    }
}


// Desktop GL 2.0+ has extension and glPolygonMode() which GL ES and WebGL don't have..
// A desktop ES context can technically compile fine with our loader, so we also perform a runtime checks
#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3)
#define IMGUI_IMPL_OPENGL_HAS_EXTENSIONS        // has glGetIntegerv(GL_NUM_EXTENSIONS)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE // may have glPolygonMode()
#endif

// Desktop GL 2.1+ and GL ES 3.0+ have glBindBuffer() with GL_PIXEL_UNPACK_BUFFER target.
#if !defined(IMGUI_IMPL_OPENGL_ES2)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_BUFFER_PIXEL_UNPACK
#endif

// Desktop GL 3.1+ has GL_PRIMITIVE_RESTART state
#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3) && defined(GL_VERSION_3_1)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
#endif

// Desktop GL 3.2+ has glDrawElementsBaseVertex() which GL ES and WebGL don't have.
#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3) && defined(GL_VERSION_3_2)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
#endif

// Desktop GL 3.3+ and GL ES 3.0+ have glBindSampler()
#if !defined(IMGUI_IMPL_OPENGL_ES2) && (defined(IMGUI_IMPL_OPENGL_ES3) || defined(GL_VERSION_3_3))
#define IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
#endif

// OpenGL Data
struct ImGui_ImplCar_Data
{
    GLuint          GlVersion;               // Extracted at runtime using GL_MAJOR_VERSION, GL_MINOR_VERSION queries (e.g. 320 for GL 3.2)
    bool            GlProfileIsES2;
    bool            GlProfileIsES3;
    bool            GlProfileIsCompat;
    GLint           GlProfileMask;
    GLuint          FontTexture;
    GLuint          ShaderHandle;
    GLint           AttribLocationTex;       // Uniforms location
    GLint           AttribLocationProjMtx;
    GLuint          AttribLocationVtxPos;    // Vertex attributes location
    GLuint          AttribLocationVtxUV;
    GLuint          AttribLocationVtxColor;
    unsigned int    VboHandle, ElementsHandle;
    GLsizeiptr      VertexBufferSize;
    GLsizeiptr      IndexBufferSize;
    bool            HasPolygonMode;
    bool            HasClipOrigin;

    ImGui_ImplCar_Data() { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplCar_Data* ImGui_ImplCar_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplCar_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Forward Declarations
static void ImGui_ImplCar_InitPlatformInterface();
static void ImGui_ImplCar_ShutdownPlatformInterface();


// Functions
bool    ImGui_ImplCar_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_ImplCar_Data* bd = IM_NEW(ImGui_ImplCar_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_opengl3";

    // Query for GL version (e.g. 320 for GL 3.2)
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GLES 2
    bd->GlVersion = 200;
    bd->GlProfileIsES2 = true;
#else
    // Desktop or GLES 3
    const char* gl_version_str = (const char*)glGetString(GL_VERSION);
    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major == 0 && minor == 0)
        sscanf(gl_version_str, "%d.%d", &major, &minor); // Query GL_VERSION in desktop GL 2.x, the string will start with "<major>.<minor>"
    bd->GlVersion = (GLuint)(major * 100 + minor * 10);
#if defined(GL_CONTEXT_PROFILE_MASK)
    if (bd->GlVersion >= 320)
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &bd->GlProfileMask);
    bd->GlProfileIsCompat = (bd->GlProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0;
#endif

#if defined(IMGUI_IMPL_OPENGL_ES3)
    bd->GlProfileIsES3 = true;
#else
    if (strncmp(gl_version_str, "OpenGL ES 3", 11) == 0)
        bd->GlProfileIsES3 = true;
#endif

#endif

#ifdef IMGUI_IMPL_OPENGL_DEBUG
    printf("GlVersion = %d, \"%s\"\nGlProfileIsCompat = %d\nGlProfileMask = 0x%X\nGlProfileIsES2 = %d, GlProfileIsES3 = %d\nGL_VENDOR = '%s'\nGL_RENDERER = '%s'\n", bd->GlVersion, gl_version_str, bd->GlProfileIsCompat, bd->GlProfileMask, bd->GlProfileIsES2, bd->GlProfileIsES3, (const char*)glGetString(GL_VENDOR), (const char*)glGetString(GL_RENDERER)); // [DEBUG]
#endif

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
    if (bd->GlVersion >= 320)
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
#endif
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    // Make an arbitrary GL call (we don't actually need the result)
    // IF YOU GET A CRASH HERE: it probably means the OpenGL function loader didn't do its job. Let us know!
    GLint current_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

    // Detect extensions we support
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE
    bd->HasPolygonMode = (!bd->GlProfileIsES2 && !bd->GlProfileIsES3);
#endif
    bd->HasClipOrigin = (bd->GlVersion >= 450);
#ifdef IMGUI_IMPL_OPENGL_HAS_EXTENSIONS
    GLint num_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    for (GLint i = 0; i < num_extensions; i++)
    {
        const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
        if (extension != nullptr && strcmp(extension, "GL_ARB_clip_control") == 0)
            bd->HasClipOrigin = true;
    }
#endif

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplCar_InitPlatformInterface();

    ImGui_ImplCar_CreateDeviceObjects();
        
    return true;
}

void    ImGui_ImplCar_Shutdown()
{
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplCar_ShutdownPlatformInterface();
    ImGui_ImplCar_DestroyDeviceObjects();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
    IM_DELETE(bd);
}

void    ImGui_ImplCar_NewFrame()
{
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();
    IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplCar_Init()?");

}

static void ImGui_ImplCar_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
{
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);

    glDisable(GL_PRIMITIVE_RESTART);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
#if defined(GL_CLIP_ORIGIN)
    bool clip_origin_lower_left = true;
    if (bd->HasClipOrigin)
    {
        GLenum current_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&current_clip_origin);
        if (current_clip_origin == GL_UPPER_LEFT)
            clip_origin_lower_left = false;
    }
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
#if defined(GL_CLIP_ORIGIN)
    if (!clip_origin_lower_left) { float tmp = T; T = B; B = tmp; } // Swap top and bottom if origin is upper left
#endif
    const float ortho_projection[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };
    glUseProgram(bd->ShaderHandle);
    glUniform1i(bd->AttribLocationTex, 0);
    glUniformMatrix4fv(bd->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
    if (bd->GlVersion >= 330 || bd->GlProfileIsES3)
        glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 and GL ES 3.0 may set that otherwise.
#endif

    (void)vertex_array_object;

    glBindVertexArray(vertex_array_object);

    // Bind vertex/index buffers and setup attributes for ImDrawVert
    glBindBuffer(GL_ARRAY_BUFFER, bd->VboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd->ElementsHandle);
    glEnableVertexAttribArray(bd->AttribLocationVtxPos);
    glEnableVertexAttribArray(bd->AttribLocationVtxUV);
    glEnableVertexAttribArray(bd->AttribLocationVtxColor);
    glVertexAttribPointer(bd->AttribLocationVtxPos,   2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
    glVertexAttribPointer(bd->AttribLocationVtxUV,    2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
    glVertexAttribPointer(bd->AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));
}

// Car Render function.
// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly.
// This is in order to be able to run within an OpenGL engine that doesn't do so.
void    ImGui_ImplCar_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();

    // Backup GL state
    #ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE
    GLint last_polygon_mode[2]; if (bd->HasPolygonMode) { glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode); }
#endif
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
    GLboolean last_enable_primitive_restart = (bd->GlVersion >= 310) ? glIsEnabled(GL_PRIMITIVE_RESTART) : GL_FALSE;
#endif

    // Setup desired GL state
    // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
    // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
    GLuint vertex_array_object = 0;

    glGenVertexArrays(1, &vertex_array_object);

    ImGui_ImplCar_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        const GLsizeiptr vtx_buffer_size = (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const GLsizeiptr idx_buffer_size = (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        
        glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplCar_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
                glScissor((int)clip_min.x, (int)((float)fb_height - clip_max.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y));

                // Bind texture, Draw
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->GetTexID());

                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
            }
        }
    }

    // Destroy the temporary VAO
    glDeleteVertexArrays(1, &vertex_array_object);

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
    if (bd->GlVersion >= 310) { if (last_enable_primitive_restart) glEnable(GL_PRIMITIVE_RESTART); else glDisable(GL_PRIMITIVE_RESTART); }
#endif

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE
    // Desktop OpenGL 3.0 and OpenGL 3.1 had separate polygon draw modes for front-facing and back-facing faces of polygons
    if (bd->HasPolygonMode) { if (bd->GlVersion <= 310 || bd->GlProfileIsCompat) { glPolygonMode(GL_FRONT, (GLenum)last_polygon_mode[0]); glPolygonMode(GL_BACK, (GLenum)last_polygon_mode[1]); } else { glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]); } }
#endif // IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE

    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    (void)bd; // Not all compilation paths use this
}

bool ImGui_ImplCar_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();

    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &bd->FontTexture);
    glBindTexture(GL_TEXTURE_2D, bd->FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

void ImGui_ImplCar_DestroyFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();
    if (bd->FontTexture)
    {
        glDeleteTextures(1, &bd->FontTexture);
        io.Fonts->SetTexID(0);
        bd->FontTexture = 0;
    }
}

bool    ImGui_ImplCar_CreateDeviceObjects()
{
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();

    // Backup GL state
    GLint last_texture, last_array_buffer;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_BUFFER_PIXEL_UNPACK
    GLint last_pixel_unpack_buffer = 0;
    if (bd->GlVersion >= 210) { glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &last_pixel_unpack_buffer); glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); }
#endif

    GLint last_vertex_array;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar* vertex_shader =
        "#version 450\n"
        "layout (location = 0) in vec2 Position;\n"
        "layout (location = 1) in vec2 UV;\n"
        "layout (location = 2) in vec4 Color;\n"
        "uniform mat4 ProjMtx;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 450\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "uniform sampler2D Texture;\n"
        "layout (location = 0) out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";


    // Create shaders
    GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_handle, 1, &vertex_shader, nullptr);
    glCompileShader(vert_handle);

    GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_handle, 1, &fragment_shader, nullptr);
    glCompileShader(frag_handle);

    // Link
    bd->ShaderHandle = glCreateProgram();
    glAttachShader(bd->ShaderHandle, vert_handle);
    glAttachShader(bd->ShaderHandle, frag_handle);
    glLinkProgram(bd->ShaderHandle);

    glDetachShader(bd->ShaderHandle, vert_handle);
    glDetachShader(bd->ShaderHandle, frag_handle);
    glDeleteShader(vert_handle);
    glDeleteShader(frag_handle);

    bd->AttribLocationTex = glGetUniformLocation(bd->ShaderHandle, "Texture");
    bd->AttribLocationProjMtx = glGetUniformLocation(bd->ShaderHandle, "ProjMtx");
    bd->AttribLocationVtxPos = (GLuint)glGetAttribLocation(bd->ShaderHandle, "Position");
    bd->AttribLocationVtxUV = (GLuint)glGetAttribLocation(bd->ShaderHandle, "UV");
    bd->AttribLocationVtxColor = (GLuint)glGetAttribLocation(bd->ShaderHandle, "Color");

    // Create buffers
    glGenBuffers(1, &bd->VboHandle);
    glGenBuffers(1, &bd->ElementsHandle);

    ImGui_ImplCar_CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_BUFFER_PIXEL_UNPACK
    if (bd->GlVersion >= 210) { glBindBuffer(GL_PIXEL_UNPACK_BUFFER, last_pixel_unpack_buffer); }
#endif

    glBindVertexArray(last_vertex_array);

    return true;
}

void    ImGui_ImplCar_DestroyDeviceObjects()
{
    ImGui_ImplCar_Data* bd = ImGui_ImplCar_GetBackendData();
    if (bd->VboHandle)      { glDeleteBuffers(1, &bd->VboHandle); bd->VboHandle = 0; }
    if (bd->ElementsHandle) { glDeleteBuffers(1, &bd->ElementsHandle); bd->ElementsHandle = 0; }
    if (bd->ShaderHandle)   { glDeleteProgram(bd->ShaderHandle); bd->ShaderHandle = 0; }
    ImGui_ImplCar_DestroyFontsTexture();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplCar_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    ImGui_ImplCar_RenderDrawData(viewport->DrawData);
}

static void ImGui_ImplCar_InitPlatformInterface()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_RenderWindow = ImGui_ImplCar_RenderWindow;
}

static void ImGui_ImplCar_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}

//-----------------------------------------------------------------------------
