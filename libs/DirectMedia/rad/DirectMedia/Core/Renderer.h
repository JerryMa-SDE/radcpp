#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/RefCounted.h"
#include "rad/Core/Span.h"
#include "SDL2/SDL_render.h"

namespace DirectMedia {

class Window;
class Texture;

class Renderer : public rad::RefCounted<Renderer>
{
public:
    Renderer(Window* window);
    ~Renderer();

    SDL_Renderer* GetHandle() { return m_renderer; }

    static std::vector<SDL_RendererInfo> GetRendererInfos();

    // @param index - the index of the rendering driver to initialize,
    // or -1 to initialize the first one supporting the requested flags.
    // @param flags is bits of SDL_RendererFlags
    // SDL_RENDERER_SOFTWARE: the renderer is a software fallback
    // SDL_RENDERER_ACCELERATED: the renderer uses hardware acceleration
    // SDL_RENDERER_PRESENTVSYNC: present is synchronized with the refresh rate
    // SDL_RENDERER_TARGETTEXTURE: the renderer supports rendering to texture
    bool Init(int index, Uint32 flags);
    // Select one from "direct3d11", "direct3d12" and "opengl", with most features supported:
    // flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
    bool Init();
    void Destroy();

    int Clear();
    void Present();

    int SetLogicalSize(int w, int h);

    int SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    int RenderGeometry(
        SDL_Texture* texture,
        rad::Span<SDL_Vertex> vertices,
        rad::Span<int> indices);

    // Copy a portion of the texture to the current rendering target.
    int CopyTexture(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect);
    // Copy a portion of the texture to the current rendering target.
    int CopyTextureEx(Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect,
        const double angle, const SDL_Point* center, const SDL_RendererFlip flip);

    int SetScale(float scaleX, float scaleY);

private:
    Window* m_window;
    SDL_Renderer* m_renderer = nullptr;
    SDL_RendererInfo m_rendererInfo = {};

}; // class Renderer

} // namespace DirectMedia
