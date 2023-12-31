#pragma once

#include "rad/Core/RefCounted.h"
#include "rad/Core/Span.h"
#include "rad/IO/File.h"
#include "nfd.h"

namespace DirectMedia {

// Simple C++ wrapper for: https://github.com/btzy/nativefiledialog-extended
// Filter Syntax
// nfdfilteritem_t filters[2] = { { "Source code", "c,cpp,cc" }, { "Headers", "h,hpp" } };
class NativeFileDialog : public rad::RefCounted<NativeFileDialog>
{
public:
    NativeFileDialog();
    ~NativeFileDialog();

    rad::FilePath OpenDialog(rad::Span<nfdfilteritem_t> filters,
        const rad::FilePath& defaultPath = {});
    // This method does array-like access on the PathSet, and is the easiest to use.
    // However, on certain platforms (Linux, and possibly Windows),
    // it takes O(N2) time in total to iterate the entire PathSet,
    // because the underlying platform-specific implementation uses a linked list.
    std::vector<rad::FilePath> OpenDialogMultiple(
        rad::Span<nfdfilteritem_t> filters,
        const rad::FilePath& defaultPath = {});

    // [Experimental] This method uses an enumerator object to iterate the paths in the PathSet.
    // It is guaranteed to take O(N) time in total to iterate the entire PathSet.
    bool OpenDialogEnumerateMultiple(
        rad::Span<nfdfilteritem_t> filters,
        const rad::FilePath& defaultPath = {});
    bool EnumerateBegin();
    bool EnumerateNext(rad::FilePath& path);
    // Must be called whatever EnumerateBegin returns.
    void EnumerateEnd();

    rad::FilePath PickFolder(const rad::FilePath& defaultPath = {});

private:
    const nfdpathset_t* m_pathSet = nullptr;
    nfdpathsetenum_t m_enumerator = {};

}; // NativeFileDialog

} // namespace DirectMedia
