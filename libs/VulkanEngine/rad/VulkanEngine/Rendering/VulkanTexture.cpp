#include "VulkanTexture.h"
#include "rad/VulkanEngine/Core/VulkanDevice.h"
#include "rad/VulkanEngine/Core/VulkanQueue.h"
#include "rad/VulkanEngine/Core/VulkanBuffer.h"
#include "rad/VulkanEngine/Core/VulkanImage.h"
#include "rad/VulkanEngine/Core/VulkanMath.h"
#include "stb_image.h"
#include "compressonator.h"

rad::Ref<VulkanImage> CreateVulkanImage2DFromFile(VulkanDevice* device, std::string_view filePath)
{
    rad::Ref<VulkanImage> image;
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* data = stbi_load(filePath.data(),
        &width, &height, &channels, 4);
    if (data)
    {
        uint32_t mipLevels = 1;
        image = device->CreateImage2DTexture(VK_FORMAT_R8G8B8A8_UNORM, width, height, mipLevels);
        VkDeviceSize dataSize = VkDeviceSize(width) * VkDeviceSize(height) * VkDeviceSize(4);
        rad::Ref<VulkanBuffer> stagingBuffer = device->CreateStagingBuffer(dataSize);
        stagingBuffer->Write(data);
        image->CopyFromBuffer2D(stagingBuffer.get());
        stbi_image_free(data);
    }
    return image;
}

VkFormat ToVulkanFormat(CMP_FORMAT format);

rad::Ref<VulkanImage> CreateVulkanImage2DFromFile(VulkanDevice* device, std::string_view filePath, bool genMipmaps)
{
    rad::Ref<VulkanImage> image;
    VkFormat format = VK_FORMAT_UNDEFINED;
    CMP_MipSet mipSet = {};
    if (CMP_LoadTexture(filePath.data(), &mipSet) != CMP_OK)
    {
        LogVulkan(Error, "Failed to load image from file: %s", filePath.data());
        return nullptr;
    }
    // For GPU, the texture must have width and height as a multiple of 4.
    if ((mipSet.m_nWidth % 4) > 0 || (mipSet.m_nHeight % 4) > 0)
    {
        LogVulkan(Error, "Failed to load image from file: %s: width=%d; height=%d; (not multiple of 4)",
            filePath.data(), mipSet.m_nWidth, mipSet.m_nHeight);
        CMP_FreeMipSet(&mipSet);
        return nullptr;
    }
    format = ToVulkanFormat(mipSet.m_format);
    if (format != VK_FORMAT_UNDEFINED)
    {
        if (genMipmaps && (mipSet.m_nMipLevels <= 1))
        {
            CMP_INT mipLevels = CMP_CalcMinMipSize(mipSet.m_nHeight, mipSet.m_nWidth, 16);
            CMP_GenerateMIPLevels(&mipSet, mipLevels);
        }

        image = device->CreateImage2DTexture(format, mipSet.m_nWidth, mipSet.m_nHeight, mipSet.m_nMipLevels);
        VkDeviceSize stagingBufferSize = VkDeviceSize(mipSet.dwDataSize) * 2;
        rad::Ref<VulkanBuffer> stagingBuffer = device->CreateStagingBuffer(stagingBufferSize);
        void* pStaging = stagingBuffer->MapMemory(0, stagingBufferSize);
        CMP_DWORD offset = 0;
        for (CMP_INT level = 0; level < mipSet.m_nMipLevels; ++level)
        {
            CMP_MipLevel* levelData = nullptr;
            CMP_GetMipLevel(&levelData, &mipSet, level, 0);
            memcpy((uint8_t*)(pStaging)+offset,
                levelData->m_psbData, levelData->m_dwLinearSize);
            offset += levelData->m_dwLinearSize;
        }
        stagingBuffer->UnmapMemory();
        image->CopyFromBuffer2D(stagingBuffer.get(), 0,
            0, mipSet.m_nMipLevels);
        CMP_FreeMipSet(&mipSet);
    }
    return image;
}

VkFormat ToVulkanFormat(CMP_FORMAT format)
{
    switch (format)
    {
        // Channel Component formats --------------------------------------------------------------------------------
        // Byte Format 0x0nn0
    case CMP_FORMAT_RGBA_8888_S: return VK_FORMAT_R8G8B8A8_SNORM; // RGBA format with signed 8-bit fixed channels.
    case CMP_FORMAT_ARGB_8888_S: return VK_FORMAT_UNDEFINED; // ARGB format with signed 8-bit fixed channels.
    case CMP_FORMAT_ARGB_8888: return VK_FORMAT_UNDEFINED; // ARGB format with 8-bit fixed channels.
    case CMP_FORMAT_ABGR_8888: return VK_FORMAT_UNDEFINED; // ABGR format with 8-bit fixed channels.
    case CMP_FORMAT_RGBA_8888: return VK_FORMAT_R8G8B8A8_UNORM; // RGBA format with 8-bit fixed channels.
    case CMP_FORMAT_BGRA_8888: return VK_FORMAT_B8G8R8A8_UNORM; // BGRA format with 8-bit fixed channels.
    case CMP_FORMAT_RGB_888: return VK_FORMAT_UNDEFINED; // RGB format with 8-bit fixed channels.
    case CMP_FORMAT_RGB_888_S: return VK_FORMAT_UNDEFINED; // RGB format with 8-bit fixed channels.
    case CMP_FORMAT_BGR_888: return VK_FORMAT_UNDEFINED; // BGR format with 8-bit fixed channels.
    case CMP_FORMAT_RG_8_S: return VK_FORMAT_UNDEFINED; // Two component format with signed 8-bit fixed channels.
    case CMP_FORMAT_RG_8: return VK_FORMAT_UNDEFINED; // Two component format with 8-bit fixed channels.
    case CMP_FORMAT_R_8_S: return VK_FORMAT_UNDEFINED; // Single component format with signed 8-bit fixed channel.
    case CMP_FORMAT_R_8: return VK_FORMAT_UNDEFINED; // Single component format with 8-bit fixed channel.
    case CMP_FORMAT_ARGB_2101010: return VK_FORMAT_A2R10G10B10_UNORM_PACK32; // ARGB format with 10-bit fixed channels for color & a 2-bit fixed channel for alpha.
    case CMP_FORMAT_RGBA_1010102: return VK_FORMAT_UNDEFINED; // RGBA format with 10-bit fixed channels for color & a 2-bit fixed channel for alpha.
    case CMP_FORMAT_ARGB_16: return VK_FORMAT_UNDEFINED; // ARGB format with 16-bit fixed channels.
    case CMP_FORMAT_ABGR_16: return VK_FORMAT_UNDEFINED; // ABGR format with 16-bit fixed channels.
    case CMP_FORMAT_RGBA_16: return VK_FORMAT_R16G16B16A16_UNORM; // RGBA format with 16-bit fixed channels.
    case CMP_FORMAT_BGRA_16: return VK_FORMAT_UNDEFINED; // BGRA format with 16-bit fixed channels.
    case CMP_FORMAT_RG_16: return VK_FORMAT_R16G16_UNORM; // Two component format with 16-bit fixed channels.
    case CMP_FORMAT_R_16: return VK_FORMAT_R16_UNORM; // Single component format with 16-bit fixed channels.

        // Float Format 0x1nn0
    case CMP_FORMAT_RGBE_32F: return VK_FORMAT_UNDEFINED; // RGB format with 9-bit floating point each channel and shared 5 bit exponent
    case CMP_FORMAT_ARGB_16F: return VK_FORMAT_UNDEFINED; // ARGB format with 16-bit floating-point channels.
    case CMP_FORMAT_ABGR_16F: return VK_FORMAT_UNDEFINED; // ABGR format with 16-bit floating-point channels.
    case CMP_FORMAT_RGBA_16F: return VK_FORMAT_R16G16B16A16_SFLOAT; // RGBA format with 16-bit floating-point channels.
    case CMP_FORMAT_BGRA_16F: return VK_FORMAT_UNDEFINED; // BGRA format with 16-bit floating-point channels.
    case CMP_FORMAT_RG_16F: return VK_FORMAT_R16G16_SFLOAT; // Two component format with 16-bit floating-point channels.
    case CMP_FORMAT_R_16F: return VK_FORMAT_R16_SFLOAT; // Single component with 16-bit floating-point channels.
    case CMP_FORMAT_ARGB_32F: return VK_FORMAT_UNDEFINED; // ARGB format with 32-bit floating-point channels.
    case CMP_FORMAT_ABGR_32F: return VK_FORMAT_UNDEFINED; // ABGR format with 32-bit floating-point channels.
    case CMP_FORMAT_RGBA_32F: return VK_FORMAT_R32G32B32A32_SFLOAT; // RGBA format with 32-bit floating-point channels.
    case CMP_FORMAT_BGRA_32F: return VK_FORMAT_UNDEFINED; // BGRA format with 32-bit floating-point channels.
    case CMP_FORMAT_RGB_32F: return VK_FORMAT_R32G32B32_SFLOAT; // RGB format with 32-bit floating-point channels.
    case CMP_FORMAT_BGR_32F: return VK_FORMAT_UNDEFINED; // BGR format with 32-bit floating-point channels.
    case CMP_FORMAT_RG_32F: return VK_FORMAT_R32G32_SFLOAT; // Two component format with 32-bit floating-point channels.
    case CMP_FORMAT_R_32F: return VK_FORMAT_R32_SFLOAT; // Single component with 32-bit floating-point channels.

        // Lossless Based Compression Formats --------------------------------------------------------------------------------
        // Format 0x2nn0
    case CMP_FORMAT_BROTLIG: return VK_FORMAT_UNDEFINED; //< Lossless CMP format compression : Prototyping


        // Compression formats ------------ GPU Mapping DirectX, Vulkan and OpenGL formats and comments --------
        // Compressed Format 0xSnn1..0xSnnF   (Keys 0x00Bv..0x00Bv) S =1 is signed, 0 = unsigned, B =Block Compressors 1..7 (BC1..BC7) and v > 1 is a variant like signed or swizzle
    case CMP_FORMAT_BC1: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_BC1_UNORM GL_COMPRESSED_RGBA_S3TC_DXT1_EXT A four component opaque (or 1-bit alpha) compressed texture format for Microsoft DirectX10. Identical to DXT1.  Four bits per pixel.
    case CMP_FORMAT_BC2: return VK_FORMAT_BC2_UNORM_BLOCK; // DXGI_FORMAT_BC2_UNORM VK_FORMAT_BC2_UNORM_BLOCK GL_COMPRESSED_RGBA_S3TC_DXT3_EXT A four component compressed texture format with explicit alpha for Microsoft DirectX10. Identical to DXT3. Eight bits per pixel.
    case CMP_FORMAT_BC3: return VK_FORMAT_BC3_UNORM_BLOCK; // DXGI_FORMAT_BC3_UNORM VK_FORMAT_BC3_UNORM_BLOCK GL_COMPRESSED_RGBA_S3TC_DXT5_EXT A four component compressed texture format with interpolated alpha for Microsoft DirectX10. Identical to DXT5. Eight bits per pixel.
    case CMP_FORMAT_BC4: return VK_FORMAT_BC4_UNORM_BLOCK; // DXGI_FORMAT_BC4_UNORM VK_FORMAT_BC4_UNORM_BLOCK GL_COMPRESSED_RED_RGTC1 A single component compressed texture format for Microsoft DirectX10. Identical to ATI1N. Four bits per pixel.
    case CMP_FORMAT_BC4_S: return VK_FORMAT_BC4_SNORM_BLOCK; // DXGI_FORMAT_BC4_SNORM VK_FORMAT_BC4_SNORM_BLOCK GL_COMPRESSED_SIGNED_RED_RGTC1 A single component compressed texture format for Microsoft DirectX10. Identical to ATI1N. Four bits per pixel.
    case CMP_FORMAT_BC5: return VK_FORMAT_BC5_UNORM_BLOCK; // DXGI_FORMAT_BC5_UNORM VK_FORMAT_BC5_UNORM_BLOCK GL_COMPRESSED_RG_RGTC2 A two component compressed texture format for Microsoft DirectX10. Identical to ATI2N_XY. Eight bits per pixel.
    case CMP_FORMAT_BC5_S: return VK_FORMAT_BC5_SNORM_BLOCK; // DXGI_FORMAT_BC5_SNORM VK_FORMAT_BC5_SNORM_BLOCK GL_COMPRESSED_RGBA_BPTC_UNORM A two component compressed texture format for Microsoft DirectX10. Identical to ATI2N_XY. Eight bits per pixel.
    case CMP_FORMAT_BC6H: return VK_FORMAT_BC6H_UFLOAT_BLOCK; // DXGI_FORMAT_BC6H_UF16 VK_FORMAT_BC6H_UFLOAT_BLOCK GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT BC6H compressed texture format (UF)
    case CMP_FORMAT_BC6H_SF: return VK_FORMAT_BC6H_SFLOAT_BLOCK; // DXGI_FORMAT_BC6H_SF16 VK_FORMAT_BC6H_SFLOAT_BLOCK GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT   BC6H compressed texture format (SF)
    case CMP_FORMAT_BC7: return VK_FORMAT_BC7_UNORM_BLOCK; // DXGI_FORMAT_BC7_UNORM VK_FORMAT_BC7_UNORM_BLOCK GL_COMPRESSED_RGBA_BPTC_UNORM BC7  compressed texture format

    case CMP_FORMAT_ATI1N: return VK_FORMAT_BC4_UNORM_BLOCK; // DXGI_FORMAT_BC4_UNORM VK_FORMAT_BC4_UNORM_BLOCK GL_COMPRESSED_RED_RGTC1 Single component compression format using the same technique as DXT5 alpha. Four bits per pixel.
    case CMP_FORMAT_ATI2N: return VK_FORMAT_BC5_UNORM_BLOCK; // DXGI_FORMAT_BC5_UNORM VK_FORMAT_BC5_UNORM_BLOCK GL_COMPRESSED_RG_RGTC2 Two component compression format using the same technique as DXT5 alpha. Designed for compression of tangent space normal maps. Eight bits per pixel.
    case CMP_FORMAT_ATI2N_XY: return VK_FORMAT_BC5_UNORM_BLOCK; // DXGI_FORMAT_BC5_UNORM VK_FORMAT_BC5_UNORM_BLOCK GL_COMPRESSED_RG_RGTC2 Two component compression format using the same technique as DXT5 alpha. The same as ATI2N but with the channels swizzled. Eight bits per pixel.
    case CMP_FORMAT_ATI2N_DXT5: return VK_FORMAT_BC5_UNORM_BLOCK; // DXGI_FORMAT_BC5_UNORM VK_FORMAT_BC5_UNORM_BLOCK GL_COMPRESSED_RG_RGTC2 ATI2N like format using DXT5. Intended for use on GPUs that do not natively support ATI2N. Eight bits per pixel.

    case CMP_FORMAT_DXT1: return VK_FORMAT_BC1_RGB_UNORM_BLOCK; // DXGI_FORMAT_BC1_UNORM VK_FORMAT_BC1_RGB_UNORM_BLOCK GL_COMPRESSED_RGBA_S3TC_DXT1_EXT An DXTC compressed texture matopaque (or 1-bit alpha). Four bits per pixel.
    case CMP_FORMAT_DXT3: return VK_FORMAT_BC2_UNORM_BLOCK; // DXGI_FORMAT_BC2_UNORM VK_FORMAT_BC2_UNORM_BLOCK GL_COMPRESSED_RGBA_S3TC_DXT3_EXT    DXTC compressed texture format with explicit alpha. Eight bits per pixel.

    case CMP_FORMAT_DXT5: return VK_FORMAT_BC3_UNORM_BLOCK; // DXGI_FORMAT_BC3_UNORM VK_FORMAT_BC3_UNORM_BLOCK GL_COMPRESSED_RGBA_S3TC_DXT5_EXT    DXTC compressed texture format with interpolated alpha. Eight bits per pixel.
    case CMP_FORMAT_DXT5_xGBR: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_UNKNOWN DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.
    case CMP_FORMAT_DXT5_RxBG: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_UNKNOWN swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel.
    case CMP_FORMAT_DXT5_RBxG: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_UNKNOWN swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel.
    case CMP_FORMAT_DXT5_xRBG: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_UNKNOWN swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel.
    case CMP_FORMAT_DXT5_RGxB: return VK_FORMAT_UNDEFINED; // DXGI_FORMAT_UNKNOWN swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.
    case CMP_FORMAT_DXT5_xGxR: return VK_FORMAT_UNDEFINED; // two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel.

    case CMP_FORMAT_ATC_RGB: return VK_FORMAT_UNDEFINED; // CMP - a compressed RGB format.
    case CMP_FORMAT_ATC_RGBA_Explicit: return VK_FORMAT_UNDEFINED; // CMP - a compressed ARGB format with explicit alpha.
    case CMP_FORMAT_ATC_RGBA_Interpolated: return VK_FORMAT_UNDEFINED; // CMP - a compressed ARGB format with interpolated alpha.

    case CMP_FORMAT_ASTC: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK; // DXGI_FORMAT_UNKNOWN   VK_FORMAT_ASTC_4x4_UNORM_BLOCK to VK_FORMAT_ASTC_12x12_UNORM_BLOCK
    case CMP_FORMAT_APC: return VK_FORMAT_UNDEFINED; // APC Texture Compressor
    case CMP_FORMAT_PVRTC: return VK_FORMAT_UNDEFINED; //

    case CMP_FORMAT_ETC_RGB: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK GL_COMPRESSED_RGB8_ETC2  backward compatible
    case CMP_FORMAT_ETC2_RGB: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK GL_COMPRESSED_RGB8_ETC2
    case CMP_FORMAT_ETC2_SRGB: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK GL_COMPRESSED_SRGB8_ETC2
    case CMP_FORMAT_ETC2_RGBA: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK GL_COMPRESSED_RGBA8_ETC2_EAC
    case CMP_FORMAT_ETC2_RGBA1: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    case CMP_FORMAT_ETC2_SRGBA: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    case CMP_FORMAT_ETC2_SRGBA1: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK; // DXGI_FORMAT_UNKNOWN VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2


        // New Compression Formats -------------------------------------------------------------
    case CMP_FORMAT_BINARY: return VK_FORMAT_UNDEFINED; //< Binary/Raw Data Format
    case CMP_FORMAT_GTC: return VK_FORMAT_UNDEFINED; //< GTC   Fast Gradient Texture Compressor
    case CMP_FORMAT_BASIS: return VK_FORMAT_UNDEFINED; //< BASIS compression
    }

    return VK_FORMAT_UNDEFINED;
}
