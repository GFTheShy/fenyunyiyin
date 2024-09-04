#ifndef QMSG_H
#define QMSG_H
#include <string.h>
struct QMSG
{
    char buf[1022];
    short type;
};

struct user
{
    user(const char * name , const char * password , bool sex) {
        strcpy(this->name,name);
        strcpy(this->password,password);
        this->sex = sex;
    }
    char name[128];
    char password[128];
    bool sex;
};

#endif // QMSG_H
