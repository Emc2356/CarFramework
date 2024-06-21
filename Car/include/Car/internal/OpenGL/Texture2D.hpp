#pragma once

#include "Car/Renderer/Texture2D.hpp"


namespace Car {
    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height, uint8_t* buffer, Format format, Format internalformat, Type type);
        OpenGLTexture2D(const std::string& filepath, bool flipped, Format format, Format internalformat, Type type);

        virtual ~OpenGLTexture2D() override;

        virtual void updateData(uint8_t* buffer, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) override;
        virtual void updateData(const std::string& filepath, bool flipped=false, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) override;

        virtual void bind(uint32_t slot=0) const override;
        virtual void unbind() const override;
    
        virtual void setFilters(int min, int mag) override;
        virtual void setRepeatX(bool v) override;
        virtual void setRepeatY(bool v) override;
    private:
        uint32_t mID;
    };
}
