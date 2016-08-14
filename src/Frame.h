#ifndef FRAME_H
#define FRAME_H

/* IO include files*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>

#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Data Structure include files */
#include <string>
#include <queue>
#include <cstring>
#include <map>
#include <set>

class Frame
{
private:
    int fd; // file descriptor
    int pageNo;
    timeval timeOfAccess;
    int priority; // page priority
    mutable bool commitFlag;
    mutable char* pageData; // initialize the pageSize for each frame;
    int pageSize;
public:
    void setFd (int);
    int getFd () const;

    void setPageNo (int);
    int getPageNo () const;

    void setTimeOfAccess (timeval);
    timeval getTimeOfAccess () const;

    void setPriority (int);
    int getPriority() const;

    void setCommitFlag (bool);
    bool getCommitFlag () const;

    void setPageData (char*);
    char* getPageData () const;

    void setPageSize (int);
    int getPageSize() const;

    // print function
    void print() const;

    // comparision function
    bool operator<(const Frame& objOther) const;
};

#endif // FRAME_H
