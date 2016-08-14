#include "Cache.h"
#include "Frame.h"

using namespace std;

// [Start] Singleton
bool Cache::_objFlag = false;
Cache* Cache::_obj = NULL;

Cache* Cache::getObj()
{
    if (!_objFlag)
    {
        _obj = new Cache();
        _objFlag = true;
    }
    return _obj;
}
// [Stop] Singleton

/*
Cache::Cache()
{
    this->noOfPages = -1;
    this->pageSize = -1;
    this->isCachePresent = false;
}
*/

void Cache::setNoOfPages(int noOfPages)
{
    this->noOfPages = noOfPages;
}

int Cache::getNoOfPages()
{
    return this->noOfPages;
}

void Cache::setPageSize(int pageSize)
{
    this->pageSize = pageSize;
}

int Cache::getPageSize()
{
    return this->pageSize;
}

bool Cache::ioCreateFile(string fileName)
{
    int fileFlags = O_RDWR | O_CREAT;
    int mode = 0777;
    const char* fName;

    // convert c++ string to char*
    fName = fileName.c_str();

    // use POSIX open() to open/create file.
    int fd = open (fName, fileFlags, mode);

    if ( fd < 0 ) // if file is not opened/created
    {
        return false;
    }
    else
    {
        // insert <fName,fd> mapping into the HashMap.
        this->mapFiles.insert(pair<string, int>(fileName, fd));
        return true;
    }
}

bool Cache::ioCloseFile(string fName)
{
    int fd = this->getFDFromFileName(fName);

    //this->commitFile(fName);
    if (close (fd) != -1)
    {
        //delete the fName-fd mapping.
        ////cout << "Closing file.." << fName << endl;
        this->mapFiles.erase(this->mapFiles.find(fName));
        return true;
    }
    else return false;
}

bool Cache::createCache(int noOfPages, int pageSize)
{
    this->setNoOfPages(noOfPages);
    this->setPageSize(pageSize);

    return createCache();
}

bool Cache::createCache()
{
    if (noOfPages == -1 || pageSize == -1)
    {
        //cout << "Initialize noOfPages & PageSize first." << endl;
        return false;
    }

    if (this->isCachePresent == false)
    {

        // If cache is still accidentally present, delete all the elements from the pQueue
        if ( !this->setFrames.empty() )
        {
            this->setFrames.clear();
        }

        // set the flag to true => cache is initialized.
        this->isCachePresent = true;

        return true;
    }
    else
    {
        //cout << "Cache already initialized." << endl;
        return false;
    }
}

// char* ch = getPage(...
// if ( *ch == 0 ) page returned is 0.
char* Cache::getPage(string fName, int pageNo, int priority)
{
    // get a page with pageNo from CACHE

    int fd = this->getFDFromFileName(fName);

    timeval a;
    Frame tempFrame;
    tempFrame.setFd(fd);
    tempFrame.setPageNo(pageNo);
    tempFrame.setPriority(priority);
    tempFrame.setCommitFlag(false);

    gettimeofday(&a, 0);
    tempFrame.setTimeOfAccess(a);

    set<Frame>::iterator iterSet = this->setFrames.find(tempFrame);

    if ( iterSet == this->setFrames.end() )
    {
        //cout << "Frame not in cache.." << endl;
        // if the frame is not there in the cache
        return this->readPageFromFile(fName, pageNo, priority);
    }
    else
    {
        // set the tempFrame data
        tempFrame.setPageData(iterSet->getPageData());

        // delete the page from cache
        this->setFrames.erase(iterSet);
        // insert the page with updated access time.
        this->setFrames.insert(tempFrame);

        return iterSet->getPageData();
    }
}

bool Cache::setPage(string fName, int pageNo, char *pageData, int priority)
{
    int fd = this->getFDFromFileName(fName);

    timeval a;
    Frame tempFrame;
    tempFrame.setPageSize(this->getPageSize());
    tempFrame.setFd(fd);
    tempFrame.setPageNo(pageNo);
    tempFrame.setPageData(pageData);
    tempFrame.setPriority(priority);
    tempFrame.setCommitFlag(false);

    gettimeofday(&a, 0);
    tempFrame.setTimeOfAccess(a);

    set<Frame>::iterator iterSet = this->setFrames.find(tempFrame);

    if ( iterSet == this->setFrames.end() )
    {
        // if the frame is NOT there in the cache
        // insert the frame into cache
        //this->setFrames.insert(tempFrame);

        if ( this->setFrames.size() == this->getNoOfPages() )
        {
            // cache is FULL
            bool flag = false;

            if (this->getNoOfPages() == 0)
            {
                flag = this->ioWritePage(fd, pageNo, pageData);
            }
            else
            {
                // set stores the elements in increasing order.
                // get a set iterator to the beginning.
                set<Frame>::iterator it = this->setFrames.begin();
                // write the min priority element to disk
                //cout << "fd: " << it->getFd() << endl;
                flag = this->ioWritePage(it->getFd(), it->getPageNo(), it->getPageData());
                // //cout << "Page written.." << endl;
                // delete the min priority element
                this->setFrames.erase(it);

                // insert the new frame into cache
                this->setFrames.insert(tempFrame);
            }

            return flag;
        }
        else
        {
            // cache is NOT full
            // push a new Frame into the cache
            // one case NOT HANDLED. When the same element is there in cache already.

            // insert the new frame into cache
            this->setFrames.insert(tempFrame);
        }
    }
    else
    {
        // if the frame is already in cache

        // write the about-to-be-deleted frame to disk
        //this->writePageToFile()
        // delete the frame in cache
        this->setFrames.erase(iterSet);
        // insert the frame with updated access time.
        this->setFrames.insert(tempFrame);
    }
    return true;
}

char* Cache::readPageFromFile(string fName, int pageNo, int priority)
{
    int fd;
    char* pageData;

    fd = this->getFDFromFileName(fName);

    if ( fd < 0 )
    {
        //cout << "Error opening the file: " << fName << endl;
        return NULL;
    }
    else
    {
        // read the page from file.
        pageData = new char[this->getPageSize()];
        if ( this->ioReadPage(fd, pageNo, pageData) )
        {
            // Algo Logic Here

            timeval a;
            gettimeofday(&a, 0);
            // create a new frame
            Frame tempFrame;
            tempFrame.setPageSize(this->getPageSize());
            tempFrame.setFd(fd);
            tempFrame.setPageNo(pageNo);
            tempFrame.setPageData(pageData);
            tempFrame.setPriority(priority);
            tempFrame.setCommitFlag(false); // page of frame is still not commited.
            tempFrame.setTimeOfAccess(a);

            // check if the cache is full or not.
            if ( this->setFrames.size() == this->getNoOfPages() )
            {
                // cache is FULL
                bool flag = false;

                if (this->getNoOfPages() == 0)
                {
                    //flag = this->ioWritePage(fd, pageNo, pageData);
                    return pageData;
                }
                else
                {
                    // set stores the elements in increasing order.
                    // get a set iterator to the beginning.
                    set<Frame>::iterator it = this->setFrames.begin();
                    // write the min priority element to disk
                    //cout << "fd: " << it->getFd() << endl;
                    flag = this->ioWritePage(it->getFd(), it->getPageNo(), it->getPageData());
                    // //cout << "Page written.." << endl;
                    // delete the min priority element
                    this->setFrames.erase(it);

                    // insert the new frame into cache
                    this->setFrames.insert(tempFrame);

                    if (flag == false) return NULL;
                    else return it->getPageData();
                }

            }
            else
            {
                // cache is NOT full
                // push a new Frame into the cache
                // one case NOT HANDLED. When the same element is there in cache already.

                // insert the new frame into cache
                this->setFrames.insert(tempFrame);
            }

            return pageData;
        }
        else
        {
            return NULL;
        }
    }
}

// currently waste (redundant). Use readPageFromFile.
bool Cache::writePageToFile (string fName, int pageNo, char *pageData, int priority)
{
    int fd;

    fd = this->getFDFromFileName(fName);

    if ( fd < 0 )
    {
        //cout << "Error opening the file: " << fName << endl;
        return false;
    }
    else
    {
        if ( this->ioWritePage(fd, pageNo, pageData) )
        {
            // Algo Logic Here

            timeval a;
            gettimeofday(&a, 0);
            // create a new frame
            Frame tempFrame;
            tempFrame.setPageSize(this->getPageSize());
            tempFrame.setFd(fd);
            tempFrame.setPageNo(pageNo);
            tempFrame.setPageData(pageData);
            tempFrame.setPriority(priority);
            tempFrame.setCommitFlag(false); // page of frame is still not commited.
            tempFrame.setTimeOfAccess(a);

            // check if the cache is full or not.
            if ( this->setFrames.size() == this->getNoOfPages() )
            {
                // cache is FULL

                // set stores the elements in increasing order.
                // get a set iterator to the beginning.

                // delete the min priority element
                this->setFrames.erase(this->setFrames.begin());

                // insert the new frame into cache
                this->setFrames.insert(tempFrame);
            }
            else
            {
                // cache is NOT full
                // push a new Frame into the cache
                // one case NOT HANDLED. When the same element is there in cache already.

                // insert the new frame into cache
                this->setFrames.insert(tempFrame);
            }

            return true;
        }
        else
        {
            return false;
        }
    }
}

int Cache::getFDFromFileName (string fName)
{
    int fd; // file descriptor

    map<string, int>::iterator iter;

    iter = mapFiles.find(fName);
    if (iter != mapFiles.end())
    {
        fd = iter->second;
    }

    return fd;
}

bool Cache::ioReadPage (int fp, int pageNo, char* pageData) {

    int pSize = this->getPageSize();
    int fileSeekOffset = (pSize-1) * pageNo;

    struct stat stbuf;

    // determine the file size
    if (fstat (fp, &stbuf) == -1)
    {
        //cout << "Error determining the file size." << endl;
        return false;
    }

    if ( fileSeekOffset > stbuf.st_size)
    {
        //cout << "Error! File end reached while seeking (SEEK_END:" << SEEK_END << ", fileSeekOffSet:" << fileSeekOffset << ")" << endl;
        return false;
    }

    if (( lseek (fp, fileSeekOffset, SEEK_SET) ) < 0 ) {
        //cout << "Error seeking the file (ID: " << fp << ")" << endl;
        return false;
    }

    if ( ( read (fp, pageData, pSize-1) ) < 0 ) {
        //cout << "Error reading the page (pNo: " << pageNo << ") from the file (ID: " << fp << ")" << endl;
        return false;
    }
    else {
        //cout << "Success! Read from the file (ID: " << fp << ")" << endl;
        return true;
    }

}

bool Cache::ioWritePage(int fp, int pageNo, char *pageData)
{
    int fileSeekOffset;
    int pSize = this->getPageSize();

    if (pageData == NULL)
    {
        pageData = new char[this->getPageSize()];
    }

    if ( pageNo == -1 ) {
        // Append pageData to file.
        fileSeekOffset = pSize - 1;
        if ( ( lseek (fp, fileSeekOffset, SEEK_END) ) < 0 ) {
            //cout << "Error seeking the file (ID: " << fp << ")" << endl;
            return false;
        }
    }
    else {
        // Modify the existing page.
        fileSeekOffset = (pSize - 1) * pageNo;
        if ( ( lseek (fp, fileSeekOffset, SEEK_SET) ) < 0 ) {
            //cout << "Error seeking the file (ID: " << fp << ")" << endl;
            return false;
        }
    }

    if ( ( write (fp, pageData, pSize-1) ) < 0 ) {
        //cout << "Error writing the page (pNo: " << pageNo << ") to the file (ID: " << fp << ")" << endl;
        return false;
    }
    else {
        ////cout << "Success! Written to the file (ID: " << fp << "), data: " << pageData << endl;
        return true;
    }
}

bool Cache::commitPage (string fName, int pageNo)
{
    int fd = this->getFDFromFileName(fName);
    Frame obj;
    obj.setFd(fd);
    obj.setPageNo(pageNo);

    set<Frame>::iterator it = this->setFrames.find(obj);

    if (it == this->setFrames.end())
    {
        // page not found.
        //cout << "Page not in cache." << endl;
        return false;
    }
    else
    {
        // page found and iterator returned to the location
        bool flag = this->ioWritePage(it->getFd(), it->getPageNo(), it->getPageData());
        if (flag == false)
        {
            return false;
        }
        else
        {
            //it->setCommitFlag(flag); // doesn't work!!
            // remove the constness of the iterator to set the commitFlag to true
            (const_cast<Frame&>(*it)).setCommitFlag(flag);
            return true;
        }
    }
}

bool Cache::commitFile(string fName)
{
    int fd = this->getFDFromFileName(fName);
    set<Frame>::iterator it;

    for (it=this->setFrames.begin(); it != this->setFrames.end(); it++)
    {
        // commit only the pages of specified file.
        if ( (it->getFd() == fd) && (it->getCommitFlag() == false) )
        {
            bool flag = this->ioWritePage(it->getFd(), it->getPageNo(), it->getPageData());
            if (flag == false)
            {
                return false;
            }
            else
            {
                //it->setCommitFlag(flag); // doesn't work!!
                // remove the constness of the iterator to set the commitFlag to true
                (const_cast<Frame&>(*it)).setCommitFlag(flag);
            }
        }
    }
    return true;
}

bool Cache::commitCache()
{
    set<Frame>::iterator it;

    for (it=this->setFrames.begin(); it != this->setFrames.end(); it++)
    {
        if (it->getCommitFlag() == false)
        {
            bool flag = this->ioWritePage(it->getFd(), it->getPageNo(), it->getPageData());
            if (flag == false)
            {
                return false;
            }
            else
            {
                //it->setCommitFlag(flag); // doesn't work!!
                // remove the constness of the iterator to set the commitFlag to true
                (const_cast<Frame&>(*it)).setCommitFlag(flag);
            }
        }
    }
    return true;
}

void Cache::releaseCache()
{
    this->setFrames.clear();
}

void Cache::showCache()
{
    set<Frame>::iterator it;

    for (it=setFrames.begin(); it != this->setFrames.end(); it++)
    {
        it->print();
    }
}

void Cache::showOpenedFiles()
{
    map<string, int>::iterator it;
    int width = 15;

    for (it=this->mapFiles.begin(); it != this->mapFiles.end(); it++)
    {
        //cout << setw(width) << it->first << "<==>" << it->second << endl;
    }
}
