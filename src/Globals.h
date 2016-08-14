#ifndef GLOBALS_H
#define GLOBALS_H

namespace glb
{
    int noOfPages = 0;
    int pageSize = 0;

    void setPageSize (int pSize)
    {
        pageSize = pSize;
    }
    void setNoOfPages (int nPages)
    {
        noOfPages = nPages;
    }
}

#endif // GLOBALS_H
