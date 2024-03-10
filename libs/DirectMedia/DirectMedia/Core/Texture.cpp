#include "Texture.h"
#include "Renderer.h"
#include "Surface.h"
#include "rad/IO/Logging.h"

namespace sdl
{

Texture::Texture(Renderer* renderer) :
    m_renderer(renderer)
{
}

Texture::Texture(Renderer* renderer, SDL_Texture* handle, bool managed)
{
    m_renderer = renderer;
    m_handle = handle;
    m_managed = managed;
    int res = SDL_QueryTexture(m_handle, &m_format, &m_access, &m_width, &m_height);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_QueryTexture failed: {}", SDL_GetError());
    }
}

Texture::~Texture()
{
    if (m_managed && m_handle)
    {
        Destroy();
    }
}

bool Texture::Create(Uint32 format, int access, int w, int h)
{
    m_handle = SDL_CreateTexture(m_renderer->GetHandle(),
        format, access, w, h);
    if (m_handle != nullptr)
    {
        m_format = format;
        m_access = access;
        m_width = w;
        m_height = h;
        return true;
    }
    else
    {
        LogGlobal(Error, "SDL_CreateTexture(format={}, access={}, w={}, h={}) failed: {}",
            SDL_GetPixelFormatName(format), access, w, h, SDL_GetError());
        return false;
    }
}

bool Texture::CreateFromSurface(Surface* surface)
{
    m_handle = SDL_CreateTextureFromSurface(m_renderer->GetHandle(), surface->GetHandle());
    if (m_handle != nullptr)
    {
        int res = SDL_QueryTexture(m_handle, &m_format, &m_access, &m_width, &m_height);
        if (res != 0)
        {
            LogGlobal(Error, "SDL_QueryTexture failed: {}", SDL_GetError());
        }
        return true;
    }
    else
    {
        LogGlobal(Error, "SDL_CreateTextureFromSurface(surface={}) failed: {}",
            (void*)surface->GetHandle(), SDL_GetError());
        return false;
    }
}

void Texture::Destroy()
{
    SDL_DestroyTexture(m_handle);
    m_handle = nullptr;
}

bool Texture::SetColorMultiplier(Uint8 r, Uint8 g, Uint8 b)
{
    int res = SDL_SetTextureColorMod(m_handle, r, g, b);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_SetTextureColorMod failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::GetColorMultiplier(Uint8* r, Uint8* g, Uint8* b)
{
    int res = SDL_GetTextureColorMod(m_handle, r, g, b);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_GetTextureColorMod failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::SetAlphaMultiplier(Uint8 alpha)
{
    int res = SDL_SetTextureAlphaMod(m_handle, alpha);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_SetTextureAlphaMod failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::GetAlphaMultiplier(Uint8* alpha)
{
    int res = SDL_GetTextureAlphaMod(m_handle, alpha);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_GetTextureAlphaMod failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::SetBlendMode(SDL_BlendMode blendMode)
{
    int res = SDL_SetTextureBlendMode(m_handle, blendMode);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_SetTextureBlendMode failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::GetBlendMode(SDL_BlendMode* blendMode)
{
    int res = SDL_GetTextureBlendMode(m_handle, blendMode);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_GetTextureBlendMode failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Texture::SetScaleMode(SDL_ScaleMode scaleMode)
{
    int res = SDL_SetTextureScaleMode(m_handle, scaleMode);
    return (res == 0);
}

bool Texture::GetScaleMode(SDL_ScaleMode* scaleMode)
{
    int res = SDL_GetTextureScaleMode(m_handle, scaleMode);
    return (res == 0);
}

bool Texture::SetUserData(void* userData)
{
    int res = SDL_SetTextureUserData(m_handle, userData);
    return (res == 0);
}

bool Texture::GetUserData()
{
    return SDL_GetTextureUserData(m_handle);
}

bool Texture::Update(const SDL_Rect* rect, const void* pixels, int pitch)
{
    int res = SDL_UpdateTexture(m_handle, rect, pixels, pitch);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_UpdateTexture(rect={{ {}, {}, {}, {} }}, pixels={}, pitch={}) failed: {}",
            rect->x, rect->y, rect->w, rect->h, pixels, pitch, SDL_GetError());
    }
    return (res == 0);
}

bool Texture::UpdateYUV(const SDL_Rect* rect,
    const Uint8* Yplane, int Ypitch,
    const Uint8* Uplane, int Upitch,
    const Uint8* Vplane, int Vpitch)
{
    int res = SDL_UpdateYUVTexture(m_handle, rect,
        Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch);
    return (res == 0);
}

bool Texture::UpdateNV(const SDL_Rect* rect,
    const Uint8* Yplane, int Ypitch, const Uint8* UVplane, int UVpitch)
{
    int res = SDL_UpdateNVTexture(m_handle, rect,
        Yplane, Ypitch, UVplane, UVpitch);
    return (res == 0);
}

bool Texture::Lock(const SDL_Rect* rect, void** pixels, int* pitch)
{
    int res = SDL_LockTexture(m_handle, rect, pixels, pitch);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_LockTexture(rect={{ {}, {}, {}, {} }}) failed: {}",
            rect->x, rect->y, rect->w, rect->h, SDL_GetError());
    }
    return (res == 0);
}

rad::Ref<Surface> Texture::LockToSurface(const SDL_Rect* rect)
{
    SDL_Surface* surfaceHandle = nullptr;
    int res = SDL_LockTextureToSurface(m_handle, rect, &surfaceHandle);
    if (res == 0)
    {
        return new Surface(surfaceHandle);
    }
    else
    {
        LogGlobal(Error, "SDL_LockTextureToSurface(rect={{ {}, {}, {}, {} }}) failed: {}",
            rect->x, rect->y, rect->w, rect->h, SDL_GetError());
        return nullptr;
    }
}

void Texture::Unlock()
{
    SDL_UnlockTexture(m_handle);
}

} // namespace sdl
