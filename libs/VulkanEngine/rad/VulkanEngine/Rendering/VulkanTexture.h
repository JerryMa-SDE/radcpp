#pragma once

#include "rad/VulkanEngine/Core/VulkanObject.h"

enum class VulkanTextureMapping
{
    UV,
    Planar,
    Sphere,
    Cylinder,
    Box,
};

enum class VulkanTextureBlendOp
{
    Undefined,
    Multiply,   // T = T1 * T2;
    Add,        // T = T1 + T2;
    Subtract,   // T = T1 - T2;
    Divide,     // T = T1 / T2;
    SmoothAdd,  // T = (T1 + T2) - (T1 * T2);
    SignedAdd,  // T = T1 + (T2 - 0.5);
};

enum class VulkanTextureType
{
    Undefined,

    // Geometry
    Displacement,
    Normal,

    // Porperties
    BaseColor,
    MetallicRoughness,
    Ambient,
    Emissive,

    // Special Effects
    Transmission,
    Iridescence,
    Sheen,
    Clearcoat,
};

rad::Ref<VulkanImage> CreateVulkanImage2DFromFile(VulkanDevice* device, std::string_view filePath, bool genMipmaps);
