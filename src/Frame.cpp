#include "Frame.h"
#include "Globals.h"

using namespace std;

void Frame::print() const
{
    cout << "--Frame--" << endl;
    cout << setw(20) << "File Id: " << this->getFd() << endl;
    cout << setw(20) << "Page No: " << this->getPageNo() << endl;
    cout << setw(20) << "Page Data: " << this->getPageData() << endl;
    cout << setw(20) << "Priority: " << this->getPriority() << endl;
    cout << setw(20) << "Commit Flag: " << this->getCommitFlag() << endl;
    cout << setw(20) << "Time Of Access: " << this->getTimeOfAccess().tv_usec << endl;
    cout << "---------" << endl;
}

bool Frame::operator <(const Frame& right) const
{
    if ( this->getPageNo() == right.getPageNo() )
    {
        //cout << "PageNos same." << endl;
        return false;
    }
    else if ( this->getPriority() == right.getPriority() )
    {
        if ( this->getTimeOfAccess().tv_usec < right.getTimeOfAccess().tv_usec )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if ( this->getPriority() < right.getPriority() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Frame::setFd(int fd)
{
    this->fd = fd;
}

int Frame::getFd() const
{
    return this->fd;
}

void Frame::setPageNo(int pageNo)
{
    this->pageNo = pageNo;
}

int Frame::getPageNo() const
{
    return this->pageNo;
}

void Frame::setTimeOfAccess(timeval timeOfAccess)
{
    this->timeOfAccess = timeOfAccess;
}

timeval Frame::getTimeOfAccess() const
{
    return this->timeOfAccess;
}

void Frame::setPriority(int priority)
{
    this->priority = priority;
}

int Frame::getPriority() const
{
    return this->priority;
}

void Frame::setCommitFlag (bool flag)
{
    this->commitFlag = flag;
}

bool Frame::getCommitFlag () const
{
    return this->commitFlag;
}

void Frame::setPageData(char* pageD)
{
    this->pageData = pageD;

    //int size = this->getPageSize();
    //int size = sizeof (pageData);
    //this->pageData = new char[size];
    //strcpy (this->pageData, pageData); // is WRONG.
}

char* Frame::getPageData() const
{
    return this->pageData;
}

void Frame::setPageSize (int pSize)
{
    this->pageSize = pSize;
}

int Frame::getPageSize () const
{
    return this->pageSize;
}
