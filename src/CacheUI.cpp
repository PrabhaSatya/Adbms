#include "CacheUI.h"

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

bool CacheUI::runConsoleUI (Cache* &objCache) //Cache* objCache)
{

    // strings of the selection menu (UI)
    vector <string> vstrMenu; // vector of strings Menu
    bool returnFlag = true; // set to false when exit
    int iChoice; // input choice

    vstrMenu.push_back("Show Cache");
    vstrMenu.push_back("Create/Open File");
    vstrMenu.push_back("Create Cache");
    vstrMenu.push_back("GET PAGE");
    vstrMenu.push_back("SET PAGE");
    vstrMenu.push_back("Close File");
    vstrMenu.push_back("Commit File");
    vstrMenu.push_back("Commit Cache");
    vstrMenu.push_back("Release Cache");
    vstrMenu.push_back("Delete Cache");
    vstrMenu.push_back("Show Opened Files");
    vstrMenu.push_back("Hit Rate");
    vstrMenu.push_back("Quit");

    cout << "--Cache UI--" << endl;

    for (unsigned int i=0; i<vstrMenu.size(); i++)
    {
        cout << setw(2) << i << ": " << vstrMenu[i] << endl;
    }


    cout << endl << "Enter a choice:";
    cout.flush();
    cin >> iChoice;

    //scanf ("%d", &iChoice);

    cout << endl << "--[START] of OUTPUT--" << endl;
    if (iChoice == 0)
    {
        // Show Cache
        if (objCache != NULL)
        {
            objCache->showCache();
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 1)
    {
        // Create File
        if (objCache != NULL)
        {
            string fName;
            cout << endl << "Enter the input fileName: ";
            cin >> fName;
            bool creatFlag = objCache->ioCreateFile(fName);

            creatFlag ? cout << "Success..! FILE CREATED/OPENED." << endl : cout << "Error..! FILE NOT CREATED/OPENED." << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 2)
    {
        // Create Cache
        if (objCache == NULL)
        {
            // TODO: set the noOfPages, pageSize global/get from a XML file.
            int noOfPages;
            int pageSize;

            cout << endl << "Enter cache size: ";
            cin >> noOfPages;

            cout << endl << "Enter PAGE_SIZE: ";
            cin >> pageSize;

            objCache = Cache::getObj();
            objCache->createCache(noOfPages, pageSize);
            cout << "Cache INITIALIZED!" << endl;
        }
        else
        {
            cout << "Cache Already INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 3)
    {
        // GET PAGE
        if (objCache != NULL)
        {
            string fName;
            int pageNo, priority;
            char* pageData;

            cout << endl << "Enter fileName: ";
            cin >> fName;
            cout << endl << "Enter PageNo: ";
            cin >> pageNo;
            cout << endl << "Enter Priority: ";
            cin >> priority;

            pageData = objCache->getPage(fName, pageNo, priority);

            (pageData == NULL) ? cout << "No page present." << endl : cout << "Page Data: " << pageData << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 4)
    {
        // SET PAGE
        if (objCache != NULL)
        {
            string fName;
            int pageNo, priority;
            char* pageData = new char[objCache->getPageSize()+1];
            bool returnFlag;

            cout << endl << "Enter fileName: ";
            cin >> fName;
            cout << endl << "Enter PageNo: ";
            cin >> pageNo;
            cout << endl << "Enter PageData: ";
            //cin.getline(pageData, sizeof(pageData));
            cin >> pageData;
            cout << endl << "Enter Priority: ";
            cin >> priority;

            returnFlag = objCache->setPage(fName, pageNo, pageData, priority);

            returnFlag ? cout << "SET_PAGE Success..!" << endl : cout << "SET_PAGE Failure..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 5)
    {
        // Close FILE
        if (objCache != NULL)
        {
            string fName;
            bool returnFlag;

            cout << endl << "Enter fileName to close file: ";
            cin >> fName;

            // commit the file pages in cache before closing.
            objCache->commitFile(fName);
            // close the file
            returnFlag = objCache->ioCloseFile(fName);

            returnFlag ? cout << "CLOSE_FILE Success..!" << endl : cout << "CLOSE_FILE Failure..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 6)
    {
        // Commit FILE
        if (objCache != NULL)
        {
            string fName;
            bool returnFlag;

            cout << endl << "Enter fileName to commit the file pages in CACHE: ";
            cin >> fName;

            returnFlag = objCache->commitFile(fName);

            returnFlag ? cout << "COMMIT_FILE Success..!" << endl : cout << "COMMIT_FILE Failure..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 7)
    {
        // Commit CACHE
        if (objCache != NULL)
        {
            bool returnFlag;

            returnFlag = objCache->commitCache();
            returnFlag ? cout << "COMMIT_CACHE Success..!" << endl : cout << "COMMIT_CACHE Failure..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 8)
    {
        // Release CACHE
        if (objCache != NULL)
        {
            objCache->releaseCache();
            cout << "Cache Released..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 9)
    {
        // Delete CACHE
        if (objCache != NULL)
        {
            objCache->releaseCache();
            free (objCache);
            cout << "Cache Deleted..!" << endl;
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
        //break;
    }
    else if (iChoice == 10)
    {
        // Show Opened Files
        if (objCache != NULL)
        {
            cout << "Opened Files: " << endl;
            objCache->showOpenedFiles();
        }
        else
        {
            cout << "Error! Cache NOT INITIALIZED!" << endl;
        }
    }
    else if (iChoice == 11)
    {
        // Hit Rate
    }
    else if (iChoice == 12)
    {
        // Quit
        returnFlag = false;
    }
    else
    {
        cout << "Please select only the available options." << endl;
        //break;
    }
    cout << "--[END] of OUTPUT--" << endl << endl;

    return returnFlag;
}
