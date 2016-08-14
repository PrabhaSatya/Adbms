/*
 * B+Tree.hpp
 *
 *  Created on: Dec 7, 2012
 *      Author: satya
 */

#ifndef B_TREE_HPP_
#define B_TREE_HPP_


#include"globals.hpp"
#include"debug.hpp"

#include "B+IndexHeader.hpp"
#include "B+IndexPage.hpp"
#include "B+LeafPage.hpp"

using namespace std;

class bpt
{

	bptIndexHeader indexHeader;
	char* propagatingElement;


public:

	bpt()
	{
        propagatingElement = NULL;

    }


    //index related  functions
    void createIndex(int fo,int dataType,int keySize);
	void openIndex();
	void closeIndex();
	void dropIndex();


    //integration functions
    int calculateFanout(int keySize);
    long int H_createIndex(int dataType,int keySize);
    void H_insertIntoBPT(long int indexHeaderPageNumber,string key,RID rid);
    vector<RID> H_searchBPT(long int indexHeaderPageNumber,string key);







	//insert functions
	long int  insertToBPT(long int rootPageNumber,char* key,RID rid);
	void splitLeafPage(bptLeafPage &leafPage,bptLeafPage &newLeafPage,char* key,RID rid);
	template <typename pageType>
	void splitIndexPage(bptIndexPage &indexPage,bptIndexPage &newIndexPage, pageType &newChildSibling,char* key);
	long int insertToLeafPage(long int leafPageNumber,char* key,RID rid);
	long int insertToIndexPage(long int rootPageNumber, long int  splitPageNumber);
	void updateRootPage(bptIndexPage &currentRootPage,bptIndexPage  &currentRootSiblingPage,char* key);
	void updateParent(long int childPageNumber,long int parentPageNumber,int parentPageLevel);


	//search functions
	vector<RID> getSearchData(long int searchResultPageNumber,char* key);
	long int searchBPT(long int rootPageNumber, char* key);


	//deletion functions
    bool deleteFromBPT(long int rootPageNumber,char* key, RID rid);
    void deleteFromLeafPage(long int leafPageNumber,char* key,RID rid);

    bool deleteFromIndexPage(long int parentPageNumber,long int  childPageNumber);
    bool mergeLeafPage(long int parentPageNumber,long int leafPageNumber,char* key,bool ledFlag);
    bool redistributeLeafPage(long int parentPageNumber,long int leafPageNumber,char* deletionKey,bool ledFlag);
    void mergeIndexPage(long int indexPageNumber);
    bool redistributeIndexPage(long int indexPageNumber);
    bool replaceInGrandParents(long int parentPageNumber,char* deletionKey,char* updationKey);



	//other functions
    void printBPT();
	void traverseBPT();




    //getters and setters

    char* getPropagatingElement() const
    {
        return propagatingElement;
    }
    void setPropagatingElement(char* pe)
    {
        propagatingElement = pe;
    }
    bptIndexHeader getIndexHeader() const
	{
		return indexHeader;
	}

	virtual ~bpt()
	{}

};


void bpt::createIndex(int fo,int dataType,int keySize)
{

    long int rootPageNumber,leafPageNumber;
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    debug("index creation started");
    rootPageNumber = indexFileIO->getNextPageNumber();
	leafPageNumber = indexFileIO->getNextPageNumber();

    indexHeader.setIndexHeaderPageData(new char[PAGESIZE]);
    memset(indexHeader.getIndexHeaderPageData(),0,PAGESIZE);

    debug("memory allocated for the indexHeaderPage");

    indexHeader.setRootPageNumber(rootPageNumber);
    indexHeader.setIndexHeaderPageNumber(indexFileIO->getNextPageNumber());
	indexHeader.setRootPageNumber(rootPageNumber);
	indexHeader.setFanout(fo);
	indexHeader.setDataType(dataType);
	indexHeader.setKeySize(keySize);
	indexHeader.setnLevels(2);

    debug("done with initialisation of index header");
    debug("done with page conversion");
    indexHeader.objToIndexHeaderPage();
	indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());


    bptIndexPage rootPage(indexHeader);
	bptLeafPage leafPage(indexHeader);
    rootPage.setIndexNodePageData (new char[PAGESIZE]);
    memset(rootPage.getIndexNodePageData(),0,PAGESIZE);
    leafPage.setLeafNodePageData (new char[PAGESIZE]);
    memset(leafPage.getLeafNodePageData(),0,PAGESIZE);

    debug("done with initialisation of memory to root and leaf");
	rootPage.setPageNumber(rootPageNumber);
	leafPage.setPageNumber(leafPageNumber);
	debug("rootPageNumber"<<rootPageNumber);
	debug("leafPageNumber"<<leafPageNumber);


	rootPage.setPriority(rootPagePriority);
	rootPage.setChild(0,leafPage.getPageNumber());
	leafPage.setParent(rootPage.getPageNumber());


	rootPage.setIndexHeaderPageNumber(indexHeader.getIndexHeaderPageNumber());
	leafPage.setIndexHeaderPageNumber(indexHeader.getIndexHeaderPageNumber());

    debug("done with pages intialisation");
	rootPage.indexNodeToPage();
	leafPage.leafNodeToPage();

    debug("indexHeader Page written");
	indexFileIO->writePage(rootPage.getPageNumber(),rootPage.getIndexNodePageData());
	debug("root page written");
	indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
	debug("done with leafPage written");

}


void bpt::closeIndex()
{
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    indexHeader.objToIndexHeaderPage();
    indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());
    indexFileIO->updatePageNumber();
}

void bpt::openIndex()
{
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    long int pageNumber;
    cout<<"enter index header page number"<<endl;
    cin>>pageNumber;
    indexHeader.setIndexHeaderPageData(indexFileIO->getPage(pageNumber,indexHeaderPagePriority));
    indexHeader.indexHeaderPageToObj();


}
void bpt::dropIndex()
{
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    long int pageNumber;
    cout<<"enter index header page number"<<endl;
    cin>>pageNumber;
    indexHeader.setIndexHeaderPageData(indexFileIO->getPage(pageNumber,indexHeaderPagePriority));
    indexHeader.indexHeaderPageToObj();
    indexHeader.setRootPageNumber(-1);
    indexHeader.objToIndexHeaderPage();
    indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());


}

void bpt::traverseBPT()
{

	long int pageNumber;
	int childIndex,choice;
	int currentLevel;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	currentLevel = indexHeader.getnLevels() - 1;
	pageNumber = indexHeader.getRootPageNumber();
	debug("rootPage number as retrived from indexheader"<<pageNumber);
	do
	{
		bptLeafPage tempLeafPage(indexHeader);
		bptIndexPage tempIndexPage(indexHeader);
	if(currentLevel != 0 )
		{
			if(pageNumber != -1)
			{
				tempIndexPage.setIndexNodePageData(indexFileIO->getPage(pageNumber,indexPagePriority));
                //bptUtility::getBptUtilityInstance()->hexdump(tempIndexPage.getIndexNodePageData());
				tempIndexPage.pageToIndexNode();
				//bptUtility::getBptUtilityInstance()->hexdump(tempIndexPage.getIndexNodePageData());
				tempIndexPage.printIndexPage();
				//bptUtility::getBptUtilityInstance()->hexdump(tempIndexPage.getIndexNodePageData());
			}
		}
		else
		{
			if(pageNumber != -1)
			{
				tempLeafPage.setLeafNodePageData(indexFileIO->getPage(pageNumber,leafPagePriority));
				tempLeafPage.pageToLeafNode();
				tempLeafPage.printLeafPage();
			}

		}
		cout<<"enter next page choice:"<<endl;
		cout<<"Left Page     :"<<LEFT<<endl;
		cout<<"Right Page    :"<<RIGHT<<endl;
		cout<<"Parent Page   :"<<PARENT<<endl;
		cout<<"Child Page    :"<<CHILD<<endl;
		cout<<"Exit Traverse :-1"<<endl;
		cout<<"input>>>>"<<endl;
		cin>>choice;
		//cin.ignore(10000);
		switch(choice)
		{
		case LEFT:
					if(currentLevel !=0)
					{
							pageNumber = tempIndexPage.getLeftSiblingPageNumber(indexHeader);
							if(pageNumber == -1)
								cout<<"no page in the left"<<endl;

					}
					else
					{
							pageNumber =  tempLeafPage.getLeft();
							if(pageNumber == -1)
								cout<<"no page in the left"<<endl;
					}
					break;
		case RIGHT: if(currentLevel != 0)
					{
						pageNumber = tempIndexPage.getRightSiblingPageNumber(indexHeader);
						if(pageNumber == -1)
							cout<<"no page in the right"<<endl;
					}
					else
					{
						pageNumber = tempLeafPage.getRight();
						if(pageNumber == -1)
							cout<<"no page in the right"<<endl;

					}
					break;
		case PARENT:
					 if(currentLevel != 0)
					 {
						 pageNumber = tempIndexPage.getParent();
						 if(pageNumber == -1 && currentLevel == indexHeader.getnLevels()-1)
							 cout<<"current page is the root and there is no parent"<<endl;
						 if(pageNumber == -1 && currentLevel != indexHeader.getnLevels()-1)
							 debug("Error.. parent of an index page is not set");
						 currentLevel = currentLevel + 1;
					 }
					 else
					 {
						 pageNumber = tempLeafPage.getParent();
						 if(pageNumber == -1)
							 debug("Error.. parent of a leaf page is not set")
						 currentLevel = currentLevel + 1;

					 }

					break;
		case CHILD:
					if(currentLevel == 0)
						cout<<"no child to visit"<<endl;
					else
					{
						cout<<"enter the index of the child page to be visited:"<<endl;
						cin>>childIndex;
						if(childIndex >= 0 && childIndex < indexHeader.getFanout() &&childIndex <= tempIndexPage.getnKeys())
						{
								pageNumber = tempIndexPage.getChild(childIndex);
								currentLevel = currentLevel - 1;

						}
						else
						{
						    if(childIndex >= indexHeader.getFanout())
						    {
                                    cout<<"enter proper index with in the fanout range.."<<endl;
                                    cout<<"fanout is..."<<indexHeader.getFanout()<<endl;
						    }
						    if(childIndex >= tempIndexPage.getnKeys())
						    {
                                    cout<<"enter proper index..."<<endl;
                                    cout<<"number of children existing are.."<<tempIndexPage.getnKeys();
						    }

						}
					}
					break;
		case -1: 	cout<<"Done with Traverse"<<endl;
					break;

        default :  cout<<"Enter proper choice"<<endl;
                    cout<<"input>>>>";


		}

	}while(choice != -1);


}

void bpt::printBPT()
{
	queue<long int> Q;
	long int pageNumber;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	Q.push(indexHeader.getRootPageNumber());
	Q.push(-111);

	int currentLevel = (indexHeader.getnLevels() - 1);
	cout << "level:" << currentLevel << endl;
	while (!Q.empty())
	{
        if(currentLevel == 0)
        {
            bptLeafPage currentLeafPage(indexHeader);
            currentLeafPage.setPageNumber(pageNumber);

            if(currentLeafPage.getPageNumber() != -111)
            {
                currentLeafPage.setLeafNodePageData(indexFileIO->getPage(currentLeafPage.getPageNumber(),leafPagePriority));
                currentLeafPage.pageToLeafNode();
                for (int i = 0; i < currentLeafPage.getnKeys(); i++)
                        bptUtil->printKey(indexHeader,currentLeafPage.getKey(i));
                cout<<endl;

            }
            pageNumber = Q.front();
            Q.pop();

        }
        else
        {
            bptIndexPage currentIndexPage(indexHeader);
            pageNumber = Q.front();
            Q.pop();
            currentIndexPage.setPageNumber(pageNumber);

            if (currentIndexPage.getPageNumber() == -111 && currentLevel >= 1)
            {
                currentLevel--;
                cout << "level:" << currentLevel << endl;
                pageNumber = Q.front();
                Q.pop();
                Q.push(-111);

            }
            currentIndexPage.setPageNumber(pageNumber);
            if(currentIndexPage.getPageNumber()!= -111  && currentLevel >= 1)
            {
                currentIndexPage.setIndexNodePageData(indexFileIO->getPage(currentIndexPage.getPageNumber(),indexPagePriority));
                currentIndexPage.pageToIndexNode();
                for (int i = 0; i < currentIndexPage.getnKeys(); i++)
                {
                        Q.push(currentIndexPage.getChild(i));
                        bptUtil->printKey(indexHeader,currentIndexPage.getKey(i));

                }
                    Q.push(currentIndexPage.getChild(currentIndexPage.getnKeys()));
                cout << endl;

            }

        }

	}
}

int bpt::calculateFanout(int keySize)
{
    int offset,N,D,fanout;
    bptIndexPage tempIndexPage;
    offset = tempIndexPage.getIndexNodeOffset();
    N = 0;
    N = PAGESIZE - offset + keySize;
    D = sizeof(long int) + keySize;
    fanout = N/D;
    return fanout;

}


long int bpt::H_createIndex(int dataType,int keySize)
{

    long int rootPageNumber,leafPageNumber;
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    int fanout;


    fanout = calculateFanout(keySize);

    debug("index creation started");
    rootPageNumber = indexFileIO->getNextPageNumber();
	leafPageNumber = indexFileIO->getNextPageNumber();

    indexHeader.setIndexHeaderPageData(new char[PAGESIZE]);
    memset(indexHeader.getIndexHeaderPageData(),0,PAGESIZE);

    debug("memory allocated for the indexHeaderPage");

    indexHeader.setRootPageNumber(rootPageNumber);
    indexHeader.setIndexHeaderPageNumber(indexFileIO->getNextPageNumber());
	indexHeader.setRootPageNumber(rootPageNumber);
	indexHeader.setFanout(fanout);
	indexHeader.setDataType(dataType);
	indexHeader.setKeySize(keySize);
	indexHeader.setnLevels(2);

    debug("done with initialisation of index header");
    debug("done with page conversion");
    indexHeader.objToIndexHeaderPage();
	indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());


    bptIndexPage rootPage(indexHeader);
	bptLeafPage leafPage(indexHeader);
    rootPage.setIndexNodePageData (new char[PAGESIZE]);
    memset(rootPage.getIndexNodePageData(),0,PAGESIZE);
    leafPage.setLeafNodePageData (new char[PAGESIZE]);
    memset(leafPage.getLeafNodePageData(),0,PAGESIZE);

    debug("done with initialisation of memory to root and leaf");
	rootPage.setPageNumber(rootPageNumber);
	leafPage.setPageNumber(leafPageNumber);
	debug("rootPageNumber"<<rootPageNumber);
	debug("leafPageNumber"<<leafPageNumber);


	rootPage.setPriority(rootPagePriority);
	rootPage.setChild(0,leafPage.getPageNumber());
	leafPage.setParent(rootPage.getPageNumber());


	rootPage.setIndexHeaderPageNumber(indexHeader.getIndexHeaderPageNumber());
	leafPage.setIndexHeaderPageNumber(indexHeader.getIndexHeaderPageNumber());

    debug("done with pages intialisation");
	rootPage.indexNodeToPage();
	leafPage.leafNodeToPage();

    debug("indexHeader Page written");
	indexFileIO->writePage(rootPage.getPageNumber(),rootPage.getIndexNodePageData());
	debug("root page written");
	indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
	debug("done with leafPage written");

    return indexHeader.getIndexHeaderPageNumber();


}

#endif /* B_TREE_HPP_ */

