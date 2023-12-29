#pragma once

#include "rad/Core/RefCounted.h"
#include "rad/Core/Span.h"
#include "SDL2/SDL.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

namespace DirectMedia {

class Codec : public rad::RefCounted<Codec>
{
public:
    Codec();
    ~Codec();

    static const AVCodec* FindDecoder(AVCodecID id);
    static std::vector<const AVCodec*> EnumerateCodecs();

private:
    AVCodec* m_codec = nullptr;

}; // class Codec

} // namespace DirectMedia
