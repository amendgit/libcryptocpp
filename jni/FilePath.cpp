#include "FilePath.h"

#include <string>

#include "Config.h"

using namespace std;

const char FilePath::kPathSeparators[] = "/";
const char FilePath::kExtensionSeparator = '.';
const char FilePath::kCurrentDirectory[] = ".";
const char FilePath::kParentDirectory[] = "..";

namespace {
    string::size_type ExtensionSeparatorPositon(const string& path)
    {
        if (path == FilePath::kCurrentDirectory || path == FilePath::kParentDirectory)
            return string::npos;

        return path.rfind(FilePath::kExtensionSeparator);
    }

    bool IsSeparator(char ch) 
    {   
        for (size_t i=0; i<arraysize(FilePath::kPathSeparators)-1; ++i) {
            if (ch == FilePath::kPathSeparators[i]) {
                return true;
            }
        }
    }
}

FilePath::FilePath() {}

FilePath::FilePath(const FilePath& that) : path_(that.path_) {}

FilePath::FilePath(const string& path) : path_(path) {}

FilePath::~FilePath() {}

FilePath& FilePath::operator=(const FilePath& that) 
{
    if (this != &that) {
        path_ = that.path_;
    }
    return *this;
}

bool FilePath::operator==(const FilePath& that) const 
{
    return path_ == that.path_;
}

bool FilePath::operator!=(const FilePath& that) const 
{
    return path_ != that.path_;
}

FilePath FilePath::DirName() const 
{
    FilePath newPath(path_);

    string::size_type lastSeparatorPos = 
        newPath.path_.find_last_of(kPathSeparators, string::npos, 
                                   arraysize(kPathSeparators)-1);

    if (lastSeparatorPos == string::npos) { 
        // path_ is in the current directory.
        newPath.path_ = ".";
    } else if (lastSeparatorPos == 0) { 
        // path is in the root directory.
        newPath.path_ = "/";
    } else if (lastSeparatorPos == 1 && IsSeparator(newPath.path_[0])) {
        // path_ is in "//". leave the double separator intact indicating 
        // alternate root.
        newPath.path_.resize(2);
    } else if (lastSeparatorPos != 0) {
        // path_ is somewhere else, trim the basename.
        newPath.path_.resize(lastSeparatorPos);
    }

    return newPath;
}

FilePath FilePath::BaseName() const 
{
    FilePath newPath(path_);
    string::size_type lastSeparatorPos = 
        newPath.path_.find_last_of(kPathSeparators, string::npos, 
                                   arraysize(kPathSeparators)-1);

    if (lastSeparatorPos != string::npos 
        && lastSeparatorPos < newPath.path_.length()-1) {
        newPath.path_.erase(0, lastSeparatorPos+1);
    }

    return newPath;
}

FilePath FilePath::ReplaceExtension(const std::string& extension) const
{
    if (path_.empty())
        return FilePath();

    string base = BaseName().value();
    if (base.empty())
        return FilePath();
    if (*(base.end()-1) == kExtensionSeparator) {
        // Special case "." and ".."
        if (base == kCurrentDirectory || base == kParentDirectory)
            return FilePath();
    }

    FilePath noExt = RemoveExtension();
    // if the new extension is "" or ".", then just remove the current extension.
    if (extension.empty() || extension == string(1, kExtensionSeparator))
        return noExt;

    string str = noExt.value();
    if (extension[0] != kExtensionSeparator)
        str.append(1, kExtensionSeparator);
    str.append(extension);
    return FilePath(str);
}

FilePath FilePath::RemoveExtension() const
{
    if (Extension().empty()) 
        return *this;

    const string::size_type dotPos = ExtensionSeparatorPositon(path_);
    if (dotPos == string::npos)
        return *this;

    return FilePath(path_.substr(0, dotPos));
}

std::string FilePath::Extension() const
{
    FilePath base(BaseName());
    const string::size_type dotPos = ExtensionSeparatorPositon(base.path_);
    if (dotPos == string::npos)
        return string();

    return base.path_.substr(dotPos, string::npos);
}