#ifndef CPT_FILE_PATH_H
#define CPT_FILE_PATH_H

#include <string>

class FilePath
{
public:
    FilePath();
    FilePath(const FilePath& that);
    explicit FilePath(const std::string& path);

    ~FilePath();

    bool operator==(const FilePath& that) const;
    bool operator!=(const FilePath& that) const;
    
    FilePath& operator=(const FilePath& that);

    const std::string& value() const {
        return path_;
    }

    bool empty() const {
        return path_.empty();
    }

    void clear() {
        path_.clear();
    }

    FilePath DirName() const;

    FilePath BaseName() const; 

    FilePath ReplaceExtension(const std::string& extension) const; 

    FilePath RemoveExtension() const;

    std::string Extension() const;

    static const char kPathSeparators[];
    static const char kExtensionSeparator;
    static const char kCurrentDirectory[];
    static const char kParentDirectory[];

private:
    std::string path_;
}; 

#endif /* CPT_FILE_PATH_H */