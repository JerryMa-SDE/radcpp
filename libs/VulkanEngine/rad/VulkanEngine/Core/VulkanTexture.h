#pragma once

#include "VulkanObject.h"

namespace Vulkan {

rad::Ref<VulkanImage> CreateImage2DFromFile(VulkanDevice* device, std::string_view filePath, bool genMipmaps);

} // namespace Vulkan
