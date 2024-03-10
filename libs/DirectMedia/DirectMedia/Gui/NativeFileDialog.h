#pragma once

#include "rad/Core/RefCounted.h"
#include "rad/Core/String.h"
#include "rad/Container/Span.h"
#include "nfd.h"

namespace sdl
{

class NativePathEnumerator : public rad::RefCounted<NativePathEnumerator>
{
public:
    NativePathEnumerator(const nfdpathset_t* pathSet);
    ~NativePathEnumerator();
    bool Begin();
    // Can only be called when Begin() returns true.
    // Returns false if reach the end.
    bool Next(std::string& path);
    // Must be called whatever Begin returns.
    void End();
private:
    const nfdpathset_t* m_pathSet = nullptr;
    nfdpathsetenum_t m_enumerator = {};
}; // class NativePathEnumerator

// Simple C++ wrapper for: https://github.com/btzy/nativefiledialog-extended
// nfdfilteritem_t filters[2] = { { "Source code", "c,cpp,cc" }, { "Headers", "h,hpp" } };
class NativeFileDialog : public rad::RefCounted<NativeFileDialog>
{
public:
    NativeFileDialog();
    ~NativeFileDialog();

    std::string OpenDialog(
        rad::Span<nfdfilteritem_t> filters, std::string_view defaultPath = {});
    // This method does array-like access on the PathSet, and is the easiest to use.
    // However, on certain platforms (Linux, and possibly Windows),
    // it takes O(N2) time in total to iterate the entire PathSet,
    // because the underlying platform-specific implementation uses a linked list.
    std::vector<std::string> OpenDialogMultiple(
        rad::Span<nfdfilteritem_t> filters, std::string_view defaultPath = {});

    // [Experimental] This method uses an enumerator object to iterate the paths in the PathSet.
    // It is guaranteed to take O(N) time in total to iterate the entire PathSet.
    // Returns nullptr if cancelled/failed.
    rad::Ref<NativePathEnumerator> OpenDialogEnumerateMultiple(
        rad::Span<nfdfilteritem_t> filters,
        std::string_view defaultPath = {});

    std::string PickFolder(std::string_view defaultPath = {});

}; // NativeFileDialog

} // namespace sdl
