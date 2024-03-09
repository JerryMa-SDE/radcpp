#include "Surface.h"
#include "rad/IO/Logging.h"

namespace sdl
{

Surface::Surface()
{
}

Surface::Surface(SDL_Surface* handle) :
    m_handle(handle)
{
}

Surface::~Surface()
{
    if (m_handle)
    {
        Destroy();
    }
}

rad::Ref<Surface> Surface::Create(int width, int height, int depth, Uint32 format)
{
    // the flags are unused and should be set to 0
    Uint32 flags = 0;
    SDL_Surface* handle = SDL_CreateRGBSurfaceWithFormat(flags, width, height, depth, format);
    return new Surface(handle);
}

rad::Ref<Surface> Surface::CreateFromPixels(int width, int height, int depth,
    Uint32 format, void* pixels, int pitch)
{
    SDL_Surface* handle = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels, width, height, depth, pitch, format);
    return new Surface(handle);
}

rad::Ref<Surface> Surface::CreateFromBMP(SDL_RWops* src, int freesrc)
{
    SDL_Surface* handle = SDL_LoadBMP_RW(src, freesrc);
    return new Surface(handle);
}

rad::Ref<Surface> Surface::CreateFromBMP(std::string_view fileName)
{
    SDL_Surface* handle = SDL_LoadBMP(fileName.data());
    return new Surface(handle);
}

bool Surface::SaveBMP(SDL_RWops* dst, int freedst)
{
    int res = SDL_SaveBMP_RW(m_handle, dst, freedst);
    return (res == 0);
}

bool Surface::SaveBMP(std::string_view fileName)
{
    int res = SDL_SaveBMP(m_handle, fileName.data());
    return (res == 0);
}

void Surface::Destroy()
{
    if (m_handle != nullptr)
    {
        SDL_FreeSurface(m_handle);
        m_handle = nullptr;
    }
}

bool Surface::SetSurfacePalette(SDL_Palette* palette)
{
    int res = SDL_SetSurfacePalette(m_handle, palette);
    return (res == 0);
}

bool Surface::NeedLockBeforeAccess()
{
    return SDL_MUSTLOCK(m_handle);
}

bool Surface::Lock()
{
    int res = SDL_LockSurface(m_handle);
    return (res == 0);
}

void Surface::Unlock()
{
    SDL_UnlockSurface(m_handle);
}

bool Surface::SetRLE(int flag)
{
    int res = SDL_SetSurfaceRLE(m_handle, flag);
    return (res == 0);
}

bool Surface::HasRLE()
{
    SDL_bool res = SDL_HasSurfaceRLE(m_handle);
    return (res == SDL_TRUE);
}

bool Surface::SetColorKey(int flag, Uint32 key)
{
    int res = SDL_SetColorKey(m_handle, flag, key);
    return (res == 0);
}

bool Surface::HasColorKey()
{
    SDL_bool res = SDL_HasColorKey(m_handle);
    return (res == SDL_TRUE);
}

bool Surface::GetColorKey(Uint32* key)
{
    int res = SDL_GetColorKey(m_handle, key);
    return (res == 0);
}

bool Surface::SetColorMultiplier(Uint8 r, Uint8 g, Uint8 b)
{
    int res = SDL_SetSurfaceColorMod(m_handle, r, g, b);
    return (res == 0);
}

bool Surface::GetColorMultiplier(Uint8* r, Uint8* g, Uint8* b)
{
    int res = SDL_GetSurfaceColorMod(m_handle, r, g, b);
    return (res == 0);
}

bool Surface::SetAlphaMultiplier(Uint8 alpha)
{
    int res = SDL_SetSurfaceAlphaMod(m_handle, alpha);
    return (res == 0);
}

bool Surface::GetAlphaMultiplier(Uint8* alpha)
{
    int res = SDL_GetSurfaceAlphaMod(m_handle, alpha);
    return (res == 0);
}

bool Surface::SetBlendMode(SDL_BlendMode blendMode)
{
    int res = SDL_SetSurfaceBlendMode(m_handle, blendMode);
    return (res == 0);
}

bool Surface::GetBlendMode(SDL_BlendMode* blendMode)
{
    int res = SDL_GetSurfaceBlendMode(m_handle, blendMode);
    return (res == 0);
}

bool Surface::SetClipRect(const SDL_Rect* rect)
{
    SDL_bool res = SDL_SetClipRect(m_handle, rect);
    return (res == SDL_TRUE);
}

void Surface::GetClipRect(SDL_Rect* rect)
{
    SDL_GetClipRect(m_handle, rect);
}

rad::Ref<Surface> Surface::Duplicate()
{
    SDL_Surface* newHandle = SDL_DuplicateSurface(m_handle);
    return new Surface(newHandle);
}

rad::Ref<Surface> Surface::Covert(const SDL_PixelFormat* format)
{
    // the flags are unused and should be set to 0; this is a leftover from SDL 1.2's API.
    Uint32 flags = 0;
    SDL_Surface* newHandle = SDL_ConvertSurface(m_handle, format, flags);
    if (newHandle != nullptr)
    {
        return new Surface(newHandle);
    }
    else
    {
        LogGlobal(Error, "SDL_ConvertSurface failed: %s", SDL_GetError());
        return nullptr;
    }
}

rad::Ref<Surface> Surface::Covert(Uint32 format)
{
    // the flags are unused and should be set to 0; this is a leftover from SDL 1.2's API.
    Uint32 flags = 0;
    SDL_Surface* newHandle = SDL_ConvertSurfaceFormat(m_handle, format, flags);
    if (newHandle != nullptr)
    {
        return new Surface(newHandle);
    }
    else
    {
        LogGlobal(Error, "SDL_ConvertSurfaceFormat failed: %s", SDL_GetError());
        return nullptr;
    }
}

bool Surface::ConvertPixles(int width, int height,
    Uint32 srcFormat, const void* src, int srcPitch,
    Uint32 dstFormat, void* dst, int dstPitch)
{
    int res = SDL_ConvertPixels(width, height, srcFormat, src, srcPitch,
        dstFormat, dst, dstPitch);
    return (res == 0);
}

bool Surface::PremultiplyAlpha(int width, int height,
    Uint32 srcFormat, const void* src, int srcPitch,
    Uint32 dstFormat, void* dst, int dstPitch)
{
    int res = SDL_PremultiplyAlpha(width, height, srcFormat, src, srcPitch,
        dstFormat, dst, dstPitch);
    return (res == 0);
}

bool Surface::FillRect(const SDL_Rect* rect, Uint32 color)
{
    int res = SDL_FillRect(m_handle, rect, color);
    return (res == 0);
}

bool Surface::FillRects(rad::Span<SDL_Rect> rects, Uint32 color)
{
    int res = SDL_FillRects(m_handle, rects.data(), static_cast<int>(rects.size()), color);
    return (res == 0);
}

bool Surface::Blit(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_BlitSurface(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

bool Surface::LowerBlit(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_LowerBlit(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

bool Surface::SoftStretch(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_SoftStretch(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

bool Surface::SoftStretchLinear(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_SoftStretchLinear(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

bool Surface::BlitScaled(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_BlitScaled(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

bool Surface::LowerBlitScaled(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    int res = SDL_LowerBlitScaled(m_handle, srcRect, dst->GetHandle(), dstRect);
    return (res == 0);
}

void Surface::SetYUVConversionMode(SDL_YUV_CONVERSION_MODE mode)
{
    SDL_SetYUVConversionMode(mode);
}

SDL_YUV_CONVERSION_MODE Surface::GetYUVConversionMode(void)
{
    return SDL_GetYUVConversionMode();
}

SDL_YUV_CONVERSION_MODE Surface::GetYUVConversionModeForResolution(int width, int height)
{
    return SDL_GetYUVConversionModeForResolution(width, height);
}

} // namespace sdl
