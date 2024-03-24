#define SPVGEN_STATIC_LIB
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "rad/IO/File.h"

VulkanShader::VulkanShader()
{
    if (!InitSpvGen())
    {
        LogVulkan(Error, "InitSpvGen failed");
    }
}

VulkanShader::~VulkanShader()
{
    FinalizeSpvgen();
}

rad::Ref<VulkanShader> VulkanShader::CreateFromFile(
    VkShaderStageFlagBits stage,
    std::string_view fileName,
    std::string_view entryPoint,
    rad::Span<VulkanShaderMacro> macros,
    uint32_t options)
{
    rad::Ref<VulkanShader> shader = new VulkanShader();
    if (shader->Compile(stage, fileName, rad::File::ReadAll(fileName), entryPoint, macros, options))
    {
        return shader;
    }
    else
    {
        LogVulkan(Error, "Shader compile failed: {}:\n{}",
            fileName, shader->GetLog());
        return nullptr;
    }
    return nullptr;
}

bool VulkanShader::Compile(VkShaderStageFlagBits stage, std::string_view fileName, std::string_view source,
    std::string_view entryPoint, rad::Span<VulkanShaderMacro> macros, uint32_t options)
{
    SpvGenStage spvStage = SpvGenStageInvalid;
    int sourceStringCount = 1;
    const char* stageSources[] = { source.data() };
    const char* stageFiles[] = { fileName.data() };
    const char* const* sourceList[] = { stageSources };
    const char* const* fileList[] = { stageFiles };
    const char* entryPoints[] = { entryPoint.data() };
    void* program = nullptr;
    const char* log = nullptr;
    switch (stage)
    {
    case VK_SHADER_STAGE_VERTEX_BIT:                    spvStage = SpvGenStageVertex;               break;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:      spvStage = SpvGenStageTessControl;          break;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:   spvStage = SpvGenStageTessEvaluation;       break;
    case VK_SHADER_STAGE_GEOMETRY_BIT:                  spvStage = SpvGenStageGeometry;             break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:                  spvStage = SpvGenStageFragment;             break;
    case VK_SHADER_STAGE_COMPUTE_BIT:                   spvStage = SpvGenStageCompute;              break;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:                spvStage = SpvGenStageRayTracingRayGen;     break;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:               spvStage = SpvGenStageRayTracingAnyHit;     break;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:           spvStage = SpvGenStageRayTracingClosestHit; break;
    case VK_SHADER_STAGE_MISS_BIT_KHR:                  spvStage = SpvGenStageRayTracingMiss;       break;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:          spvStage = SpvGenStageRayTracingIntersect;  break;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:              spvStage = SpvGenStageRayTracingCallable;   break;
    case VK_SHADER_STAGE_TASK_BIT_NV:                   spvStage = SpvGenStageTask;                 break;
    case VK_SHADER_STAGE_MESH_BIT_NV:                   spvStage = SpvGenStageMesh;                 break;
    }

    options |= SpvGenOptionVulkanRules;

    bool res = spvCompileAndLinkProgramEx(
        1, &spvStage,
        &sourceStringCount, sourceList, fileList, entryPoints,
        &program, &log,
        options);

    if (res)
    {
        const uint32_t* binary = nullptr;
        int binarySize = spvGetSpirvBinaryFromProgram(program, 0, &binary);
        assert((binarySize > 0) && ((binarySize % 4) == 0));
        m_binary.resize(binarySize / sizeof(uint32_t));
        memcpy(m_binary.data(), binary, binarySize);

        m_stage = stage;
        m_fileName = fileName;
        m_source = source;
        m_entryPoint = entryPoint;
        m_macros = macros;
    }
    else
    {
        m_log = log;
        LogVulkan(Error, "spvCompileAndLinkProgramEx(stage={}, fileName={}): \n{}",
            string_VkShaderStageFlagBits(stage), m_fileName, log);
    }

    spvFreeBuffer(program);
    return res;
}

bool VulkanShader::Assemble(VkShaderStageFlagBits stage, std::string_view source)
{
    bool result = false;
    unsigned int* buffer = nullptr;
    const char* log = nullptr;;
    size_t bufferSize = spvAssembleSpirv(
        source.data(), static_cast<uint32_t>(source.size()), buffer, &log);
    if (buffer && (bufferSize > 0) && ((bufferSize % 4) == 0))
    {
        m_binary.resize(bufferSize / 4);
        memcpy(m_binary.data(), buffer, bufferSize);

        m_stage = stage;
        m_source = source;

        result = true;
    }
    else
    {
        LogVulkan(Error, "VulkanShader::Assemble(stage={}): {}",
            string_VkShaderStageFlagBits(stage), log);
        m_log = log;
        result = false;
    }
    spvFreeBuffer(buffer);
    return result;
}

bool VulkanShader::LoadBinary(VkShaderStageFlagBits stage, void* binary, size_t binarySize)
{
    if (binarySize % sizeof(uint32_t) != 0)
    {
        LogVulkan(Error, "VulkanShader::LoadBinary(stage={}, binary={:p}, binarySize={}): "
            "size must be mutliple of 4.",
            string_VkShaderStageFlagBits(stage), binary, binarySize);
        return false;
    }

    m_binary.resize(binarySize / sizeof(uint32_t));
    memcpy(m_binary.data(), binary, binarySize);
    if (m_binary[0] != 0x07230203)
    {
        LogVulkan(Error, "VulkanShader::LoadBinary(stage={}, binary={:p}, binarySize={}): "
            "invalid magic number 0x{:8X}.",
            string_VkShaderStageFlagBits(stage), binary, binarySize, m_binary[0]);
    }
    m_stage = stage;
    return true;
}

bool VulkanShader::LoadBinaryFromFile(VkShaderStageFlagBits stage, const std::string_view fileName)
{
    rad::File binaryFile;
    if (binaryFile.Open(fileName, "rb"))
    {
        uint64_t fileSize = binaryFile.GetSize();
        if (fileSize % sizeof(uint32_t) != 0)
        {
            LogVulkan(Error, "VulkanShader::LoadBinaryFromFile(stage={}, fileName={}): "
                "fileSize={} is not multiple of 4.",
                string_VkShaderStageFlagBits(stage), fileName, fileSize);
            return false;
        }

        m_binary.resize(fileSize / sizeof(uint32_t));
        binaryFile.Read(reinterpret_cast<char*>(m_binary.data()), fileSize);
        if (m_binary[0] != 0x07230203)
        {
            LogVulkan(Error, "VulkanShader::LoadBinaryFromFile(stage={}, fileName={}): "
                "invalid magic number 0x{:8X}.",
                string_VkShaderStageFlagBits(stage), fileName, m_binary[0]);
        }
        m_stage = stage;
        m_fileName = fileName;
        return true;
    }
    return false;
}

std::string VulkanShader::Disassemble()
{
    std::string buffer(m_binary.size() * 120, 0);
    if (spvDisassembleSpirv(static_cast<uint32_t>(m_binary.size() * sizeof(uint32_t)), m_binary.data(),
        static_cast<uint32_t>(buffer.size()), buffer.data()))
    {
        buffer.resize(strlen(buffer.data()));
        return buffer;
    }
    else
    {
        LogVulkan(Warn, "VulkanShader::Disassemble failed: {}", buffer);
        return std::string();
    }
}

VulkanShaderModule::VulkanShaderModule(
    rad::Ref<VulkanDevice> device,
    const VkShaderModuleCreateInfo& createInfo) :
    m_device(std::move(device))
{
    VK_CHECK(m_device->GetFunctionTable()->
        vkCreateShaderModule(m_device->GetHandle(), &createInfo, nullptr, &m_handle));
}

VulkanShaderModule::~VulkanShaderModule()
{
    m_device->GetFunctionTable()->
        vkDestroyShaderModule(m_device->GetHandle(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
}
