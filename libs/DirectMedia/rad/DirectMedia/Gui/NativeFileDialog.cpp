#include "NativeFileDialog.h"
#include "rad/IO/Logging.h"

namespace DirectMedia {

NativeFileDialog::NativeFileDialog()
{
    NFD_Init();
}

NativeFileDialog::~NativeFileDialog()
{
    if (m_enumerator.ptr)
    {
        NFD_PathSet_FreeEnum(&m_enumerator);
    }
    if (m_pathSet)
    {
        NFD_PathSet_Free(m_pathSet);
    }
    NFD_Quit();
}

rad::FilePath NativeFileDialog::OpenDialog(rad::Span<nfdfilteritem_t> filters, const rad::FilePath& defaultPath)
{
    rad::FilePath path;
    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialogU8(
        &outPath, filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        rad::IsDirectory(defaultPath) ? (const char*)defaultPath.u8string().c_str() : nullptr);
    if (result == NFD_OKAY)
    {
        path = (const char8_t*)outPath;
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialog: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialog: %s.", NFD_GetError());
    }
    return path;
}

std::vector<rad::FilePath> NativeFileDialog::OpenDialogMultiple(
    rad::Span<nfdfilteritem_t> filters,
    const rad::FilePath& defaultPath)
{
    std::vector<rad::FilePath> paths;
    const nfdpathset_t* pathSet = nullptr;
    nfdresult_t result = NFD_OpenDialogMultipleU8(
        &pathSet, filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        rad::IsDirectory(defaultPath) ? (const char*)defaultPath.u8string().c_str() : nullptr);
    if (result == NFD_OKAY)
    {
        nfdpathsetsize_t numPaths = 0;
        NFD_PathSet_GetCount(pathSet, &numPaths);
        paths.reserve(numPaths);
        for (nfdpathsetsize_t i = 0; i < numPaths; ++i)
        {
            nfdu8char_t* path = nullptr;
            NFD_PathSet_GetPathU8(pathSet, i, &path);
            paths.push_back((const char8_t*)path);
            NFD_PathSet_FreePathU8(path);
        }
        NFD_PathSet_Free(pathSet);
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialogMultiple: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialogMultiple: %s.", NFD_GetError());
    }
    return paths;
}

bool NativeFileDialog::OpenDialogEnumerateMultiple(
    rad::Span<nfdfilteritem_t> filters,
    const rad::FilePath& defaultPath)
{
    assert(m_pathSet == nullptr);
    assert(m_enumerator.ptr == nullptr);
    nfdresult_t result = NFD_OpenDialogMultipleU8(
        &m_pathSet, filters.data(), static_cast<nfdfiltersize_t>(filters.size()),
        rad::IsDirectory(defaultPath) ? (const char*)defaultPath.u8string().c_str() : nullptr);
    if (result == NFD_OKAY)
    {
        return true;
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::OpenDialogEnumerateMultiple: cancelled.");
        return false;
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::OpenDialogEnumerateMultiple: %s.", NFD_GetError());
        return false;
    }
}

bool NativeFileDialog::EnumerateBegin()
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
            LogGlobal(Error, "NativeFileDialog::EnumerateBegin: NFD_PathSet_GetEnum failed: %s.",
                NFD_GetError());
            return false;
        }
    }
    return false;
}

bool NativeFileDialog::EnumerateNext(rad::FilePath& path)
{
    assert(m_enumerator.ptr != nullptr);
    nfdu8char_t* outPath = nullptr;
    if (NFD_PathSet_EnumNextU8(&m_enumerator, &outPath) && outPath)
    {
        path = (const char8_t*)outPath;
        NFD_PathSet_FreePathU8(outPath);
        return true;
    }
    return false;
}

void NativeFileDialog::EnumerateEnd()
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

rad::FilePath NativeFileDialog::PickFolder(const rad::FilePath& defaultPath)
{
    rad::FilePath path;
    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolderU8(&outPath,
        rad::IsDirectory(defaultPath) ? (const char*)defaultPath.u8string().c_str() : nullptr);
    if (result == NFD_OKAY)
    {
        path = (const char8_t*)outPath;
    }
    else if (result == NFD_CANCEL)
    {
        LogGlobal(Info, "NativeFileDialog::PickFolder: cancelled.");
    }
    else
    {
        LogGlobal(Error, "NativeFileDialog::PickFolder: %s.", NFD_GetError());
    }
    return path;
}

} // namespace DirectMedia
