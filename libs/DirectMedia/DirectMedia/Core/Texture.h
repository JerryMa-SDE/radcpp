#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/RefCounted.h"
#include "rad/Container/Span.h"
#include "SDL2/SDL_render.h"

namespace sdl
{

class Renderer;
class Surface;

class Texture : public rad::RefCounted<Texture>
{
public:
    Texture(Renderer* renderer);
    Texture(Renderer* renderer, SDL_Texture* handle, bool managed);
    ~Texture();

    SDL_Texture* GetHandle() { return m_handle; }

    // @param format SDL_PixelFormatEnum: https://wiki.libsdl.org/SDL2/SDL_PixelFormatEnum
    // @param access SDL_TextureAccess:
    //          SDL_TEXTUREACCESS_STATIC: changes rarely, not lockable
    //          SDL_TEXTUREACCESS_STREAMING: changes frequently, lockable
    //          SDL_TEXTUREACCESS_TARGET: can be used as a render target
    bool Create(Uint32 format, int access, int w, int h);
    bool CreateFromSurface(Surface* surface);
    void Destroy();

    Uint32 GetFormat() const { return m_format; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    bool SetColorMultiplier(Uint8 r, Uint8 g, Uint8 b);
    bool GetColorMultiplier(Uint8* r, Uint8* g, Uint8* b);
    bool GetAlphaMultiplier(Uint8* alpha);
    bool SetAlphaMultiplier(Uint8 alpha);
    bool SetBlendMode(SDL_BlendMode blendMode);
    bool GetBlendMode(SDL_BlendMode* blendMode);
    bool SetScaleMode(SDL_ScaleMode scaleMode);
    bool GetScaleMode(SDL_ScaleMode* scaleMode);
    bool SetUserData(void* userData);
    bool GetUserData();

    // Update the given texture rectangle with new pixel data.
    bool Update(const SDL_Rect* rect, const void* pixels, int pitch);
    bool UpdateYUV(const SDL_Rect* rect,
        const Uint8* Yplane, int Ypitch,
        const Uint8* Uplane, int Upitch,
        const Uint8* Vplane, int Vpitch);
    bool UpdateNV(const SDL_Rect* rect,
        const Uint8* Yplane, int Ypitch,
        const Uint8* UVplane, int UVpitch);

    // Lock a portion of the texture for **write-only** pixel access.
    bool Lock(const SDL_Rect* rect, void** pixels, int* pitch);
    // Lock a portion of the texture for **write-only** pixel access,
    // and expose it as a Surface.
    rad::Ref<Surface> LockToSurface(const SDL_Rect* rect);
    // Unlock a texture, uploading the changes to video memory, if needed.
    void Unlock();

private:
    Renderer* m_renderer;
    SDL_Texture* m_handle = nullptr;
    bool m_managed = true;
    Uint32 m_format = 0;
    int m_access = 0;
    int m_width = 0;
    int m_height = 0;

}; // class Texture

} // namespace sdl
