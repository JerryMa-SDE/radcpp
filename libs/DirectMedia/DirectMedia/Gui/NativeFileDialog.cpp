#include "NativeFileDialog.h"
#include "rad/IO/Logging.h"

namespace sdl
{

NativeFileDialog::NativeFileDialog()
{
    NFD_Init();
}

NativeFileDialog::~NativeFileDialog()
{
    NFD_Quit();
}

std::string NativeFileDialog::OpenDialog(rad::Span<nfdfilteritem_t> filters, std::string_view defaultPath)
{
    std::string path;
    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialogU8(&outPath,
        filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        defaultPath.empty() ? nullptr : defaultPath.data());
    if (result == NFD_OKAY)
    {
        path = outPath;
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialog: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialog: {}.", NFD_GetError());
    }
    return path;
}

std::vector<std::string> NativeFileDialog::OpenDialogMultiple(
    rad::Span<nfdfilteritem_t> filters,
    std::string_view defaultPath)
{
    std::vector<std::string> paths;
    const nfdpathset_t* pathSet = nullptr;
    nfdresult_t result = NFD_OpenDialogMultipleU8(
        &pathSet, filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        defaultPath.empty() ? nullptr : defaultPath.data());
    if (result == NFD_OKAY)
    {
        nfdpathsetsize_t numPaths = 0;
        NFD_PathSet_GetCount(pathSet, &numPaths);
        paths.reserve(numPaths);
        for (nfdpathsetsize_t i = 0; i < numPaths; ++i)
        {
            nfdu8char_t* outPath = nullptr;
            NFD_PathSet_GetPathU8(pathSet, i, &outPath);
            paths.push_back(outPath);
            NFD_PathSet_FreePathU8(outPath);
        }
        NFD_PathSet_Free(pathSet);
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialogMultiple: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialogMultiple: {}.", NFD_GetError());
    }
    return paths;
}

rad::Ref<NativePathEnumerator> NativeFileDialog::OpenDialogEnumerateMultiple(
    rad::Span<nfdfilteritem_t> filters,
    std::string_view defaultPath)
{
    const nfdpathset_t* pathSet = nullptr;

    nfdresult_t result = NFD_OpenDialogMultipleU8(&pathSet,
        filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        defaultPath.empty() ? nullptr : defaultPath.data());
    if (result == NFD_OKAY)
    {
        return new NativePathEnumerator(pathSet);
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialogEnumerateMultiple: cancelled.");
        return nullptr;
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialogEnumerateMultiple: {}.", NFD_GetError());
        return nullptr;
    }
}

std::string NativeFileDialog::PickFolder(std::string_view defaultPath)
{
    std::string path;
    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolderU8(&outPath,
        defaultPath.empty() ? nullptr : defaultPath.data());
    if (result == NFD_OKAY)
    {
        path = outPath;
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::PickFolder: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::PickFolder: {}.", NFD_GetError());
    }
    return path;
}

NativePathEnumerator::NativePathEnumerator(const nfdpathset_t* pathSet) :
    m_pathSet(pathSet)
{
}

NativePathEnumerator::~NativePathEnumerator()
{
    if (m_enumerator.ptr)
    {
        NFD_PathSet_FreeEnum(&m_enumerator);
    }
    if (m_pathSet)
    {
        NFD_PathSet_Free(m_pathSet);
    }
}

bool NativePathEnumerator::Begin()
{
    if (m_pathSet)
    {
        if (m_enumerator.ptr)
        {
            NFD_PathSet_FreeEnum(&m_enumerator);
            m_enumerator = {};
        }

        if (NFD_PathSet_GetEnum(m_pathSet, &m_enumerator) == NFD_OKAY)
        {
            return true;
        }
        else
        {
            LogGlobal(Error, "NativeFileDialog::EnumerateBegin: NFD_PathSet_GetEnum failed: {}.",
                NFD_GetError());
            return false;
        }
    }
    return false;
}

bool NativePathEnumerator::Next(std::string& path)
{
    assert(m_enumerator.ptr != nullptr);
    nfdu8char_t* outPath = nullptr;
    if (NFD_PathSet_EnumNextU8(&m_enumerator, &outPath) && outPath)
    {
        path = outPath;
        NFD_PathSet_FreePathU8(outPath);
        return true;
    }
    return false;
}

void NativePathEnumerator::End()
{
    // remember to free the pathset enumerator memory (before freeing the pathset)
    assert(m_pathSet && m_enumerator.ptr);
    if (m_enumerator.ptr)
    {
        NFD_PathSet_FreeEnum(&m_enumerator);
        m_enumerator = {};
    }
    if (m_pathSet)
    {
        NFD_PathSet_Free(m_pathSet);
        m_pathSet = nullptr;
    }
}

} // namespace sdl
