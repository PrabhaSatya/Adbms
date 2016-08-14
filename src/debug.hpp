#ifndef _debug_h
	#define _debug_h

#include "globals.hpp"
//#define DEBUG
     #ifndef DEBUG
        #define debug(x)
    #else
        #define debug(x) \
                   cout << x << endl; \

    #endif
#define DEBUGINSERTION
     #ifndef DEBUGINSERTION
        #define debugInsertion(x)
    #else
        #define debugInsertion(x) \
                    if(debug_Insertion == true) \
                        cout << x << endl; \

    #endif
#define DEBUGSEARCH
     #ifndef DEBUGSEARCH
        #define debugSearch(x)
    #else
        #define debugSearch(x) \
                    if(debug_Search == true) \
                    cout << x << endl; \

    #endif

#define INSERTIONPRINTINDEXPAGE
    #ifndef INSERTIONPRINTINDEXPAGE
        #define debugInsertionIndexPage(indexPage)
    #else
        #define debugInsertionIndexPage(indexPage) \
            if(debug_Insertion == true) \
            indexPage.printIndexPage();
	#endif

#define INSERTIONPRINTLEAFPAGE
    #ifndef INSERTIONPRINTLEAFPAGE
        #define debugInsertionLeafPage(leafPage)
    #else
        #define debugInsertionLeafPage(leafPage) \
            if(debug_Insertion == true) \
            leafPage.printLeafPage();

    #endif
#define PRINTKEY
    #ifndef PRINTKEY
        #define debugKey(ih,key)
    #else
        #define debugKey(ih,key) \
        if(debug_Insertion == true) \
        { \
         bptUtility *bptUtil = bptUtility::getBptUtilityInstance(); \
         bptUtil->printKey(ih,key); \
        }\

    #endif

#define DEBUGDELETION
    #ifndef DEBUGDELETION
        #define debugDeletion(x)
    #else
        #define debugDeletion(x) \
            if(debug_Deletion == true)\
            cout<<x<<endl;

    #endif

#define DELETIONPRINTINDEXPAGE
    #ifndef DELETIONPRINTINDEXPAGE
        #define debugDeletionIndexPage(indexPage)
    #else
        #define debugDeletionIndexPage(indexPage) \
             if(debug_Deletion == true)\
            indexPage.printIndexPage();

	#endif

#define DELETIONPRINTLEAFPAGE
    #ifndef DELETIONPRINTLEAFPAGE
        #define debugDeletionLeafPage(leafPage)
    #else
        #define debugDeletionLeafPage(leafPage) \
            if(debug_Deletion == true)\
           leafPage.printLeafPage();

    #endif

#define DELETIONPRINTKEY
    #ifndef DELETIONPRINTKEY
        #define debugKeyDel(ih,key)
    #else
        #define debugKeyDel(ih,key) \
            if(debug_Deletion == true)\
            {  \
            bptUtility *bptUtil = bptUtility::getBptUtilityInstance(); \
            bptUtil->printKey(ih,key); \
            } \

    #endif



#endif

