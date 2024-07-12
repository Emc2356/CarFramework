#include "Car/Renderer/GraphicsContext.hpp"


namespace Car {
    static Ref<GraphicsContext> sContext = nullptr;

    void GraphicsContext::Set(Ref<GraphicsContext> context) {
        sContext = context;
    }
    
    Ref<GraphicsContext> GraphicsContext::Get() {
        // CR_ASSERT((void*)sContext, "Interal Error: Tried to get the graphics context when it hasnt been created yet");
        return sContext;
    }
}
