#include "Codec.h"

namespace DirectMedia {

Codec::Codec()
{
}

Codec::~Codec()
{
}

const AVCodec* Codec::FindDecoder(AVCodecID id)
{
    return avcodec_find_decoder(id);
}

std::vector<const AVCodec*> Codec::EnumerateCodecs()
{
    std::vector<const AVCodec*> codecs;
    const AVCodec* codec = NULL;
    void* iter = NULL;
    while (codec = av_codec_iterate(&iter))
    {
        codecs.push_back(codec);
    }
    return codecs;
}

} // namespace DirectMedia
