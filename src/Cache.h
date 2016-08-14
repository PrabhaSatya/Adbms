#ifndef CACHE_H
#define CACHE_H

#include "Frame.h"

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

using namespace std;

class Cache
{
private:
    int noOfPages;
    int pageSize;

    bool isCachePresent;
    int currCacheSize; // current cache size

    map<string, int> mapFiles; // Mapping of the files fName->fd
    set<Frame> setFrames;

    int countHit;
    int countMiss;
    int countTotal;

    //[Start] Singleton
    static bool _objFlag;
    static Cache* _obj;
    Cache () // Private constructor
    {
        this->noOfPages = -1;
        this->pageSize = -1;
        this->isCachePresent = false;
    }


public:

    //Cache();

    /* Set-Get for noOfPages */
    void setNoOfPages (int);
    int getNoOfPages ();

    /* Set-Get for pageSize */
    void setPageSize (int pageSize);
    int getPageSize ();

    int getFDFromFileName (string fName);

    // create a file with the name fName
    bool ioCreateFile (string fName); // returns false if failure, takes fileName

    // close the file
    bool ioCloseFile (string fName);

    // read the char* pageData from a file (ID: fp) having page no as pNo
    // return false on failure.
    bool ioReadPage (int fp, int pageNo, char* pageData);

    // write the char* pageData to a file (ID: fp) having page no as pNo
    // return false on failure.
    bool ioWritePage (int fp, int pageNo, char* pageData);

    // get the number of pages in a file (ID: fp)
    int ioGetNoOfPagesInFile (int fp);

    // read a page (char*) to a file, location: pageNo
    char* readPageFromFile (string fName, int pageNo, int priority); // return the page data in char* (char array).

    // write a page (char*) to a file at pageNo location
    bool writePageToFile (string fName, int pageNo, char* pageData, int priority);

    // get a page from the cache. If not present in cache, get from file.
    char* getPage (string fName, int pageNo, int priority);

    // set a page to cache.
    bool setPage (string fName, int pageNo, char* pageData, int priority);

    // commit the page with pageNo if in cache
    bool commitPage (string fName, int pageNo);

    // commit all the pages in cache belonging to fName
    bool commitFile (string fName);

    // commit entire cache
    bool commitCache ();

    // create the cache
    bool createCache(); // use this if already setters for noOfPages, pageSize used.
    bool createCache (int noOfPages, int pageSize); // return false if failure, else true.

    // release the cache
    void releaseCache();

    // show cache
    void showCache ();

    // show opened files
    void showOpenedFiles ();

    void increaseHitCount();
    void increaseMissCount();
    void increaseTotalCount();

    int getHitCount();
    int getMissCount();
    int getTotalCount();

    void runCacheUI();

    // [Start] Singleton
    static Cache* getObj();
    ~Cache ()
    {
        _objFlag = false;
    }

};

#endif // CACHE_H
