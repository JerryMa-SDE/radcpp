#include "Renderer.h"
#include "Window.h"
#include "Surface.h"
#include "Texture.h"
#include "rad/Core/Flags.h"
#include "rad/IO/Logging.h"

namespace sdl
{

Renderer::Renderer(Window* window) :
    m_window(window)
{
}

Renderer::~Renderer()
{
    if (m_handle)
    {
        Destroy();
    }
}

std::vector<SDL_RendererInfo> Renderer::GetRendererInfos()
{
    std::vector<SDL_RendererInfo> infos;
    int count = SDL_GetNumRenderDrivers();
    if (count >= 0)
    {
        infos.resize(count);
        for (int i = 0; i < count; ++i)
        {
            if (SDL_GetRenderDriverInfo(i, &infos[i]) != 0)
            {
                LogGlobal(Error, "SDL_GetRenderDriverInfo failed: {}", SDL_GetError());
            }
        }
    }
    return infos;
}

bool Renderer::Init(int index, Uint32 flags)
{
    m_handle = SDL_CreateRenderer(m_window->GetHandle(), index, flags);
    if (m_handle)
    {
        SDL_GetRendererInfo(m_handle, &m_rendererInfo);
        LogGlobal(Info, "SDL_Renderer created successfully: {}", m_rendererInfo.name);
        return true;
    }
    else
    {
        LogGlobal(Error, "SDL_CreateRenderer failed: {}", SDL_GetError());
        return false;
    }
}

bool Renderer::Init()
{
    std::vector<SDL_RendererInfo> rendererInfos = GetRendererInfos();
    int selectedRendererIndex = -1;
    Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE;
    // prefer direct3d11 on Windows.
    const char* candidateNames[] =
    {
        "direct3d11",
        "direct3d12",
        "opengl",
    };

    for (size_t nameIndex = 0; nameIndex < std::size(candidateNames); ++nameIndex)
    {
        for (size_t rendererIndex = 0; rendererIndex < rendererInfos.size(); ++rendererIndex)
        {
            const SDL_RendererInfo& info = rendererInfos[rendererIndex];
            if (rad::StrCaseEqual(info.name, candidateNames[nameIndex]) &&
                rad::Flags32<SDL_RendererFlags>(info.flags).HasBits(flags))
            {
                selectedRendererIndex = int(rendererIndex);
                goto end;
            }
        }
    }

end:
    return Init(selectedRendererIndex, flags);
}

bool Renderer::InitSoftware(Surface* surface)
{
    m_handle = SDL_CreateSoftwareRenderer(surface->GetHandle());
    if (m_handle)
    {
        SDL_GetRendererInfo(m_handle, &m_rendererInfo);
        LogGlobal(Info, "SDL_CreateSoftwareRenderer(surface={}) successfully: {}",
            (void*)surface->GetHandle(), m_rendererInfo.name);
        return true;
    }
    else
    {
        LogGlobal(Error, "SDL_CreateSoftwareRenderer(surface={}) failed: {}",
            (void*)surface->GetHandle(), SDL_GetError());
        return false;
    }
}

void Renderer::Destroy()
{
    if (m_handle == nullptr)
    {
        SDL_DestroyRenderer(m_handle);
        m_handle = nullptr;
    }
}

bool Renderer::GetRenderInfo(SDL_RendererInfo* info)
{
    int res = SDL_GetRendererInfo(m_handle, info);
    return (res == 0);
}

bool Renderer::GetOutputSize(int* w, int* h)
{
    int res = SDL_GetRendererOutputSize(m_handle, w, h);
    return (res == 0);
}

bool Renderer::IsRenderTargetSupported() const
{
    SDL_bool res = SDL_RenderTargetSupported(m_handle);
    return (res == SDL_TRUE);
}

bool Renderer::SetRenderTarget(Texture* texture) const
{
    int res = SDL_SetRenderTarget(m_handle, texture ? texture->GetHandle() : nullptr);
    return (res == 0);
}

Texture* Renderer::GetRenderTarget()
{
    assert(!m_renderTarget || (m_renderTarget->GetHandle() == SDL_GetRenderTarget(m_handle)));
    return m_renderTarget.get();
}

bool Renderer::SetLogicalSize(int w, int h)
{
    int res = SDL_RenderSetLogicalSize(m_handle, w, h);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderSetLogicalSize(w={}, h={}) failed: {}",
            w, h, SDL_GetError());
    }
    return (res == 0);
}

void Renderer::GetLogicalSize(int* w, int* h)
{
    SDL_RenderGetLogicalSize(m_handle, w, h);
}

bool Renderer::SetIntegerScaled(bool enable)
{
    int res = SDL_RenderSetIntegerScale(m_handle, enable ? SDL_TRUE : SDL_FALSE);
    return (res == 0);
}

bool Renderer::IsIntegerScaled()
{
    SDL_bool res = SDL_RenderGetIntegerScale(m_handle);
    return (res == SDL_TRUE);
}

bool Renderer::SetViewport(const SDL_Rect* rect)
{
    int res = SDL_RenderSetViewport(m_handle, rect);
    return (res == 0);
}

void Renderer::GetViewport(SDL_Rect* rect)
{
    SDL_RenderGetViewport(m_handle, rect);
}

bool Renderer::SetClipRect(const SDL_Rect* rect)
{
    int res = SDL_RenderSetClipRect(m_handle, rect);
    return (res == 0);
}

void Renderer::GetClipRect(SDL_Rect* rect)
{
    SDL_RenderGetClipRect(m_handle, rect);
}

bool Renderer::IsClipEnabled() const
{
    SDL_bool res = SDL_RenderIsClipEnabled(m_handle);
    return (res == 0);
}

bool Renderer::SetScale(float scaleX, float scaleY)
{
    int res = SDL_RenderSetScale(m_handle, scaleX, scaleY);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderSetScale(scaleX={}, scaleY={}) failed: {}",
            scaleX, scaleY, SDL_GetError());
    }
    return (res == 0);
}

void Renderer::GetScale(float* scaleX, float* scaleY)
{
    SDL_RenderGetScale(m_handle, scaleX, scaleY);
}

void Renderer::GetLogicalCoordFromWindow(int windowX, int windowY, float* logicalX, float* logicalY)
{
    SDL_RenderWindowToLogical(m_handle, windowX, windowY, logicalX, logicalY);
}

void Renderer::GetWindowCoordFromLogical(float logicalX, float logicalY, int* windowX, int* windowY)
{
    SDL_RenderLogicalToWindow(m_handle, logicalX, logicalY, windowX, windowY);
}

bool Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    int res = SDL_SetRenderDrawColor(m_handle, r, g, b, a);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_SetRenderDrawColor failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::GetDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    int res = SDL_GetRenderDrawColor(m_handle, r, g, b, a);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_GetRenderDrawColor failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::SetDrawBlendMode(SDL_BlendMode blendMode)
{
    int res = SDL_SetRenderDrawBlendMode(m_handle, blendMode);
    return (res == 0);
}

bool Renderer::GetDrawBlendMode(SDL_BlendMode* blendMode)
{
    int res = SDL_GetRenderDrawBlendMode(m_handle, blendMode);
    return (res == 0);
}

bool Renderer::Clear()
{
    int res = SDL_RenderClear(m_handle);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderClear failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::DrawPoint(int x, int y)
{
    int res = SDL_RenderDrawPoint(m_handle, x, y);
    return (res == 0);
}

bool Renderer::DrawPoint(float x, float y)
{
    int res = SDL_RenderDrawPointF(m_handle, x, y);
    return (res == 0);
}

bool Renderer::DrawPoints(rad::Span<SDL_Point> points)
{
    int res = SDL_RenderDrawPoints(m_handle,
        points.data(), static_cast<int>(points.size()));
    return (res == 0);
}

bool Renderer::DrawPoints(rad::Span<SDL_FPoint> points)
{
    int res = SDL_RenderDrawPointsF(m_handle,
        points.data(), static_cast<int>(points.size()));
    return (res == 0);
}

bool Renderer::DrawLine(int x1, int y1, int x2, int y2)
{
    int res = SDL_RenderDrawLine(m_handle, x1, y1, x2, y2);
    return (res == 0);
}

bool Renderer::DrawLine(float x1, float y1, float x2, float y2)
{
    int res = SDL_RenderDrawLineF(m_handle, x1, y1, x2, y2);
    return (res == 0);
}

bool Renderer::DrawLines(rad::Span<SDL_Point> points)
{
    int res = SDL_RenderDrawLines(m_handle,
        points.data(), static_cast<int>(points.size()));
    return (res == 0);
}

bool Renderer::DrawLines(rad::Span<SDL_FPoint> points)
{
    int res = SDL_RenderDrawLinesF(m_handle,
        points.data(), static_cast<int>(points.size()));
    return (res == 0);
}

bool Renderer::DrawRect(const SDL_Rect* rect)
{
    int res = SDL_RenderDrawRect(m_handle, rect);
    return (res == 0);
}

bool Renderer::DrawRect(const SDL_FRect* rect)
{
    int res = SDL_RenderDrawRectF(m_handle, rect);
    return (res == 0);
}

bool Renderer::DrawRects(rad::Span<SDL_Rect> rects)
{
    int res = SDL_RenderDrawRects(m_handle,
        rects.data(), static_cast<int>(rects.size()));
    return (res == 0);
}

bool Renderer::DrawRects(rad::Span<SDL_FRect> rects)
{
    int res = SDL_RenderDrawRectsF(m_handle,
        rects.data(), static_cast<int>(rects.size()));
    return (res == 0);
}

bool Renderer::FillRect(const SDL_Rect* rect)
{
    int res = SDL_RenderFillRect(m_handle, rect);
    return (res == 0);
}

bool Renderer::FillRect(const SDL_FRect* rect)
{
    int res = SDL_RenderFillRectF(m_handle, rect);
    return (res == 0);
}

bool Renderer::FillRects(rad::Span<SDL_Rect> rects)
{
    int res = SDL_RenderFillRects(m_handle,
        rects.data(), static_cast<int>(rects.size()));
    return (res == 0);
}

bool Renderer::FillRects(rad::Span<SDL_FRect> rects)
{
    int res = SDL_RenderFillRectsF(m_handle,
        rects.data(), static_cast<int>(rects.size()));
    return (res == 0);
}

bool Renderer::CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect)
{
    int res = SDL_RenderCopy(m_handle, texture->GetHandle(),
        srcRect, dstRect);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderCopy failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_FRect* dstRect)
{
    int res = SDL_RenderCopyF(m_handle, texture->GetHandle(),
        srcRect, dstRect);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderCopyF failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect,
    const double angle, const SDL_Point* center, const SDL_RendererFlip flip)
{
    int res = SDL_RenderCopyEx(m_handle, texture->GetHandle(), srcRect, dstRect,
        angle, center, flip);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderCopyEx failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::CopyFromTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_FRect* dstRect,
    const double angle, const SDL_FPoint* center, const SDL_RendererFlip flip)
{
    int res = SDL_RenderCopyExF(m_handle, texture->GetHandle(), srcRect, dstRect,
        angle, center, flip);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderCopyExF failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::RenderGeometry(Texture* texture, rad::Span<SDL_Vertex> vertices, rad::Span<int> indices)
{
    int res = SDL_RenderGeometry(m_handle, texture->GetHandle(),
        vertices.data(), (int)vertices.size(),
        indices.data(), (int)indices.size());
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderGeometry failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::RenderGeometryRaw(Texture* texture,
    const float* xy, int xyStride,
    const SDL_Color* color, int colorStride,
    const float* uv, int uvStride,
    int numVertices, const void* indices,
    int numIndices, int sizeIndex)
{
    int res = SDL_RenderGeometryRaw(m_handle, texture->GetHandle(),
        xy, xyStride, color, colorStride, uv, uvStride, numVertices, indices, numIndices, sizeIndex);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderGeometryRaw failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::ReadPixels(const SDL_Rect* rect, Uint32 format, void* pixels, int pitch)
{
    int res = SDL_RenderReadPixels(m_handle, rect, format, pixels, pitch);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderReadPixels failed: {}", SDL_GetError());
    }
    return (res == 0);
}

void Renderer::Present()
{
    SDL_RenderPresent(m_handle);
}

bool Renderer::Flush()
{
    int res = SDL_RenderFlush(m_handle);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderFlush failed: {}", SDL_GetError());
    }
    return (res == 0);
}

bool Renderer::SetVSync(bool vsync)
{
    int res = SDL_RenderSetVSync(m_handle, vsync ? SDL_TRUE : SDL_FALSE);
    if (res != 0)
    {
        LogGlobal(Error, "SDL_RenderSetVSync failed: {}", SDL_GetError());
    }
    return (res == 0);
}

} // namespace sdl
