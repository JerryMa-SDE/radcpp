#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/RefCounted.h"
#include "rad/Container/Span.h"
#include "SDL2/SDL_surface.h"
#include <string_view>

namespace sdl
{

class Surface : public rad::RefCounted<Surface>
{
public:
    Surface();
    Surface(SDL_Surface* handle);
    ~Surface();

    SDL_Surface* GetHandle() { return m_handle; }
    int GetWidth() const { return m_handle->w; }
    int GetHeight() const { return m_handle->h; }

    const char* GetError() const { return SDL_GetError(); }

    // Allocate a new RGBA with a specified pixel format.
    // @param depth: the depth of the surface in bits.
    static rad::Ref<Surface> Create(int width, int height, int depth, Uint32 format);
    // Allocate a new RGB surface with existing pixel data. Pixel data is not managed automatically;
    // you must free the surface before you free the pixel data.
    // @param depth: the depth of the surface in bits.
    static rad::Ref<Surface> CreateFromPixels(int width, int height, int depth,
        Uint32 format, void* pixels, int pitch);
    // Load a BMP image from a seekable SDL data stream.
    static rad::Ref<Surface> CreateFromBMP(SDL_RWops* src, int freesrc);
    static rad::Ref<Surface> CreateFromBMP(std::string_view fileName);
    bool SaveBMP(SDL_RWops* dst, int freedst);
    bool SaveBMP(std::string_view fileName);
    void Destroy();

    // Set the palette used by a surface.
    // A single palette can be shared with many surfaces.
    bool SetSurfacePalette(SDL_Palette* palette);

    bool NeedLockBeforeAccess();
    // Set up a surface for directly accessing the pixels.
    bool Lock();
    // Release a surface after directly accessing the pixels.
    void Unlock();

    // Set the RLE acceleration hint for a surface.
    // @param flag: 0 to disable, non-zero to enable RLE acceleration.
    bool SetRLE(int flag);
    bool HasRLE();
    // The color key defines a pixel value that will be treated as transparent in a blit.
    bool SetColorKey(int flag, Uint32 key);
    bool HasColorKey();
    bool GetColorKey(Uint32* key);
    // Set an additional color value multiplied into blit operations.
    bool SetColorMultiplier(Uint8 r, Uint8 g, Uint8 b);
    // Get the additional color value multiplied into blit operations.
    bool GetColorMultiplier(Uint8* r, Uint8* g, Uint8* b);
    bool SetAlphaMultiplier(Uint8 alpha);
    bool GetAlphaMultiplier(Uint8* alpha);
    bool SetBlendMode(SDL_BlendMode blendMode);
    bool GetBlendMode(SDL_BlendMode* blendMode);

    // @param rect: the SDL_Rect structure representing the clipping rectangle,
    //              or nullptr to disable clipping.
    // @returns true if the rectangle intersects the surface;
    //          otherwise false and blits will be completely clipped.
    bool SetClipRect(const SDL_Rect* rect);
    void GetClipRect(SDL_Rect* rect);

    // Creates a new surface identical to the existing surface.
    rad::Ref<Surface> Duplicate();
    // This function is used to optimize images for faster repeat blitting.
    rad::Ref<Surface> Covert(const SDL_PixelFormat* format);
    rad::Ref<Surface> Covert(Uint32 format);
    bool ConvertPixles(int width, int height,
        Uint32 srcFormat, const void* src, int srcPitch,
        Uint32 dstFormat, void* dst, int dstPitch);
    bool PremultiplyAlpha(int width, int height,
        Uint32 srcFormat, const void* src, int srcPitch,
        Uint32 dstFormat, void* dst, int dstPitch);

    // Perform a fast fill of a rectangle with a specific color.
    bool FillRect(const SDL_Rect* rect, Uint32 color);
    // Perform a fast fill of a set of rectangles with a specific color.
    bool FillRects(rad::Span<SDL_Rect> rects, Uint32 color);

    // Use this function to perform a fast surface copy to a destination surface.
    bool Blit(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // This is a semi-private blit function and it performs low-level surface blitting,
    // assuming the input rectangles have already been clipped.
    bool LowerBlit(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // Perform a fast, low quality, stretch blit between two surfaces of the same format.
    bool SoftStretch(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // Perform bilinear scaling between two surfaces of the same format, 32BPP.
    bool SoftStretchLinear(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // Use this function to perform a scaled surface copy to a destination surface.
    bool BlitScaled(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    bool LowerBlitScaled(SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);

    void SetYUVConversionMode(SDL_YUV_CONVERSION_MODE mode);
    static SDL_YUV_CONVERSION_MODE GetYUVConversionMode(void);
    static SDL_YUV_CONVERSION_MODE GetYUVConversionModeForResolution(int width, int height);

private:
    SDL_Surface* m_handle = nullptr;

}; // class Surface

} // namespace sdl
