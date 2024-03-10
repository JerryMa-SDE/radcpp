#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/RefCounted.h"
#include "rad/Container/Span.h"
#include "SDL2/SDL_render.h"

namespace sdl
{

class Window;
class Surface;
class Texture;

class Renderer : public rad::RefCounted<Renderer>
{
public:
    Renderer(Window* window);
    ~Renderer();

    SDL_Renderer* GetHandle() { return m_handle; }
    // Get Renderer from Window: SDL_Renderer* SDL_GetRenderer(SDL_Window * window);
    // Get Window from Renderer: SDL_Window* SDL_RenderGetWindow(SDL_Renderer *renderer);

    static std::vector<SDL_RendererInfo> GetRendererInfos();

    // @param index the index of the rendering driver to initialize, or -1 to initialize
    //              the first one supporting the requested flags.
    // @param flags is bits of SDL_RendererFlags:
    //      SDL_RENDERER_SOFTWARE: the renderer is a software fallback.
    //      SDL_RENDERER_ACCELERATED: the renderer uses hardware acceleration.
    //      SDL_RENDERER_PRESENTVSYNC: present is synchronized with the refresh rate.
    //      SDL_RENDERER_TARGETTEXTURE: the renderer supports rendering to texture.
    bool Init(int index, Uint32 flags);
    // Select one from "direct3d11", "direct3d12" and "opengl", with most features supported:
    // flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
    bool Init();
    bool InitSoftware(Surface* surface);
    void Destroy();

    bool GetRenderInfo(SDL_RendererInfo* info);
    bool GetOutputSize(int* w, int* h);

    bool IsRenderTargetSupported() const;
    bool SetRenderTarget(Texture* texture) const;
    // return nullptr for default render target.
    Texture* GetRenderTarget();
    // Set a device independent resolution for rendering.
    // This function uses the viewport and scaling functionality to allow a
    // fixed logical resolution for rendering, regardless of the actual output resolution.
    bool SetLogicalSize(int w, int h);
    void GetLogicalSize(int* w, int* h);

    // Set whether to force integer scales for resolution-independent rendering.
    // when a resolution is between two multiples of a logical size, the viewport size
    // is rounded down to the lower multiple.
    bool SetIntegerScaled(bool enable = true);
    bool IsIntegerScaled();
    // rect can be nullptr for the entire target.
    bool SetViewport(const SDL_Rect* rect);
    void GetViewport(SDL_Rect* rect);
    bool SetClipRect(const SDL_Rect* rect);
    void GetClipRect(SDL_Rect* rect);
    bool IsClipEnabled() const;

    bool SetScale(float scaleX, float scaleY);
    void GetScale(float* scaleX, float* scaleY);

    void GetLogicalCoordFromWindow(int windowX, int windowY, float* logicalX, float* logicalY);
    void GetWindowCoordFromLogical(float logicalX, float logicalY, int* windowX, int* windowY);

    bool SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    bool GetDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a);

    bool SetDrawBlendMode(SDL_BlendMode blendMode);
    bool GetDrawBlendMode(SDL_BlendMode* blendMode);

    bool Clear();
    bool DrawPoint(int x, int y);
    bool DrawPoint(float x, float y);
    bool DrawPoints(rad::Span<SDL_Point> points);
    bool DrawPoints(rad::Span<SDL_FPoint> points);
    bool DrawLine(int x1, int y1, int x2, int y2);
    bool DrawLine(float x1, float y1, float x2, float y2);
    // Draw a series of connected lines on the current rendering target.
    bool DrawLines(rad::Span<SDL_Point> points);
    bool DrawLines(rad::Span<SDL_FPoint> points);
    bool DrawRect(const SDL_Rect* rect);
    bool DrawRect(const SDL_FRect* rect);
    bool DrawRects(rad::Span<SDL_Rect> rects);
    bool DrawRects(rad::Span<SDL_FRect> rects);
    bool FillRect(const SDL_Rect* rect);
    bool FillRect(const SDL_FRect* rect);
    bool FillRects(rad::Span<SDL_Rect> rects);
    bool FillRects(rad::Span<SDL_FRect> rects);
    // Copy a portion of the texture to the current rendering target.
    bool CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect);
    bool CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_FRect* dstRect);
    // Copy a portion of the texture to the current rendering target.
    bool CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect,
        const double angle, const SDL_Point* center, const SDL_RendererFlip flip);
    bool CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_FRect* dstRect,
        const double angle, const SDL_FPoint* center, const SDL_RendererFlip flip);
    bool RenderGeometry(Texture* texture, rad::Span<SDL_Vertex> vertices, rad::Span<int> indices);
    // @param sizeIndex 1 for byte; 2 for short; 4 for int;
    bool RenderGeometryRaw(Texture* texture,
        const float* xy, int xyStride,
        const SDL_Color* color, int colorStride,
        const float* uv, int uvStride,
        int numVertices, const void* indices,
        int numIndices, int sizeIndex);
    bool ReadPixels(const SDL_Rect* rect, Uint32 format, void* pixels, int pitch);
    void Present();
    bool Flush();

    bool SetVSync(bool vsync = true);

private:
    Window* m_window;
    SDL_Renderer* m_handle = nullptr;
    SDL_RendererInfo m_rendererInfo = {};
    rad::Ref<Texture> m_renderTarget;

}; // class Renderer

} // namespace sdl
