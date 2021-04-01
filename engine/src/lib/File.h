#ifndef __SYS_FILE_H
#define __SYS_FILE_H

#include <string>

class File {
public:
    static char * ReadAsString(const char *path);
};
#endif // !__SYS_FILE_H