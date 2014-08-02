#include "FileUtil.h"

#include <string>

#include "Debug.h"
#include "Error.h"

using namespace std;

errno_t RenameFile(const std::string& from, const std::string& to)
{
    LOGPOS();
    if (rename(from.c_str(), to.c_str()) == 0) 
        return CPT_OK;
    return CPT_ERROR; 
}