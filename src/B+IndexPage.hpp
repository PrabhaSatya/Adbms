/*
 * B+IndexNode.hpp
 *
 *  Created on: Dec 28, 2012
 *      Author: satya
 */

#ifndef B_INDEXPAGE_HPP_
#define B_INDEXPAGE_HPP_

#include"globals.hpp"
#include"debug.hpp"


#include "B+Util.hpp"


using namespace std;

class bptIndexHeader;

class bptIndexPage
{
	struct bptIndexNode
	{
		char priority;
		long int pageNumber;
		long int indexHeaderPageNumber;
		long int parent;
        int nKeys;
		int level;
	//	int keySize;
	}indexNode;
	char* indexNodePageData;

public:


	bptIndexPage()
	{}
	bptIndexPage(bptIndexHeader ih)
	{

		indexNode.priority = indexPagePriority;
		indexNode.pageNumber = -1;
		indexNode.indexHeaderPageNumber = ih.getIndexHeaderPageNumber();
		indexNode.parent = -1;
		indexNode.nKeys = 0;
		indexNode.level = 1;
	//	indexNode.keySize = ih.getKeySize();
		indexNodePageData = NULL;


	}

    int getReplacePosition(long int childPageNumber);
    bool findExistence(char* key);
    int findKeyPosition(char* key);
    void replaceInIndexPage(int replacePosition,char* key);
	void indexNodeToPage();
	void pageToIndexNode();
	void printIndexPage();
	long int getLastChildPageNumber();
	long int getFirstChildPageNumber();
	long int getRightSiblingPageNumber(bptIndexHeader ih);
	long int getLeftSiblingPageNumber(bptIndexHeader ih);
	void shiftKeysLeft(int toPosition);
	void shiftChildrenLeft(int toPosition);
	int getChildID(long int pageNumber);
	int getNumberOfChildren();
	void shiftKeysRight(int position,int nTimes);
	void shiftChildrenRight(int position,int nTimes);





	//getters and setters

    int getPriorityOffset() const
    {
        return 0;
    }
    int getPageNumberOffset() const
    {
        int offset;
        offset = getPriorityOffset()+sizeof(char);
        return offset;

    }
    int getIndexHeaderPageNumberOffset() const
    {
        int offset;
        offset = getPageNumberOffset()+sizeof(long int);
        return offset;

    }
    int getParentOffset() const
    {
        int offset;
        offset = getIndexHeaderPageNumberOffset()+sizeof(long int);
        return offset;
    }

    int getnKeysOffset() const
    {
        int offset;
        offset = getParentOffset() + sizeof(long int);
        return offset;
    }
    int getLevelOffset() const
    {
        int offset;
        offset = getnKeysOffset()+sizeof(int);
        return offset;
    }

    int getIndexNodeOffset() const
    {
        int offset;
        offset = (sizeof(char) + sizeof(long int) + sizeof(long int) + sizeof(long int) + sizeof(int) + sizeof(int));
        return offset;
    }
	long int getChild(int i) const
	{

        //bptUtility::getBptUtilityInstance()->hexdump(indexNodePageData);
        long int pageNumber;
        int offset;
        bptIndexHeader tempIndexHeader;
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
        offset = getIndexNodeOffset();
        offset = offset + ((tempIndexHeader.getFanout()-1)*tempIndexHeader.getKeySize());
        offset = offset + (i*sizeof(long int));
        memcpy(&pageNumber,&indexNodePageData[offset],sizeof(long int));
  //      cout<<"retrieving from offset"<<offset<<endl;
   //     cout<<"returning child:"<<pageNumber<<endl;
        //bptUtility::getBptUtilityInstance()->hexdump(indexNodePageData);
		return pageNumber;
	}

	void setChild(int i,long int child)
	{

        //bptUtility::getBptUtilityInstance()->hexdump(indexNodePageData);
        int offset;
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        bptIndexHeader tempIndexHeader;
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
	    offset = getIndexNodeOffset();
	    offset = offset + ((tempIndexHeader.getFanout()-1)*tempIndexHeader.getKeySize());
        offset = offset + (i*sizeof(long int));
       // cout<<"offset where the child id will be written"<<offset<<endl;
       // cout<<"child"<<child<<endl;
        memcpy(&indexNodePageData[offset],&child,sizeof(long int));
     //   long int tempChild = getChild(i);

       // cout<<"child"<<tempChild<<endl;
        //bptUtility::getBptUtilityInstance()->hexdump(indexNodePageData);





	}

	char* getKey(int i) const
	{
		int offset;
		bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
		bptIndexHeader tempIndexHeader;
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
		offset = getIndexNodeOffset();

		offset = offset + (i*tempIndexHeader.getKeySize());
		return &indexNodePageData[offset];

	}

	void setKey(int i,char * key)
	{
	    int offset;
	    bptIndexHeader tempIndexHeader;
	    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
        offset = getIndexNodeOffset();

		offset = offset + (i*tempIndexHeader.getKeySize());

        memcpy(&indexNodePageData[offset],key,tempIndexHeader.getKeySize());



	}



	int getLevel() const
	{
		return indexNode.level;
	}

	void setLevel(int level)
	{
		this->indexNode.level = level;
	}

	long int getnKeys() const
	{
		return indexNode.nKeys;
	}

	void setnKeys(long int keys)
	{
		indexNode.nKeys = keys;
	}

	char* getIndexNodePageData() const
	{
		return indexNodePageData;
	}

	void setIndexNodePageData(char* indexNodePageData)
	{
		this->indexNodePageData = indexNodePageData;
	}

	long int getIndexHeaderPageNumber() const
	{
		return indexNode.indexHeaderPageNumber;
	}

	void setIndexHeaderPageNumber(long int indexHeaderPageNumber)
	{
		this->indexNode.indexHeaderPageNumber = indexHeaderPageNumber;
	}

	long int getPageNumber() const
	{
		return indexNode.pageNumber;
	}

	void setPageNumber(long int pageNumber)
	{
		this->indexNode.pageNumber = pageNumber;
	}

	long int getParent() const
	{
		return indexNode.parent;
	}

	void setParent(long int parent)
	{
		this->indexNode.parent = parent;
	}

	char getPriority() const
	{
		return indexNode.priority;
	}

	void setPriority(char priority)
	{
		this->indexNode.priority = priority;
	}





	virtual ~bptIndexPage()
	{

/*
		for(int i = 0; i < indexNode.nKeys; i++)
			delete[] indexNode.keys[i];
		delete [] indexNode.keys;
		delete[] indexNode.children;
		delete indexNodePageData;*/
	}

};

void bptIndexPage::shiftKeysRight(int position,int nTimes)
{
    int from,to;
    from = getnKeys() - 1;
    to = position;
    while(nTimes > 0)
    {
    for(int i = from; i >= to; i--)
    setKey(i+1,getKey(i));
    nTimes = nTimes - 1;
    from = from + 1;
    to = to + 1;
    }

}
void bptIndexPage::shiftChildrenRight(int position,int nTimes)
{
    int from,to;
    from = getnKeys();
    to = position;
    while(nTimes > 0)
    {
    for(int i = from; i >= to; i--)
        setChild(i+1,getChild(i));
    nTimes = nTimes - 1;
    from = from + 1;
    to = to + 1;
    }
}


int bptIndexPage::getNumberOfChildren()
{
    return indexNode.nKeys + 1;
}
void bptIndexPage::shiftKeysLeft(int toPosition)
{
    for(int i = toPosition+1 ; i < indexNode.nKeys; i++)
    setKey(i-1,getKey(i));
}
void bptIndexPage::shiftChildrenLeft(int toPosition)
{
    for(int i = toPosition+1 ; i <= indexNode.nKeys; i++)
        setChild(i - 1,getChild(i));


}

int bptIndexPage::getChildID(long int pageNumber)
{

    for(int i = 0; i <= indexNode.nKeys; i++)
    {
        if(pageNumber == getChild(i))
        return (i);
    }

    return -1;


}

long int bptIndexPage::getLeftSiblingPageNumber(bptIndexHeader ih)
{

    bptIndexPage parentPage(ih);
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    parentPage.setIndexNodePageData(indexFileIO->getPage(indexNode.parent,indexPagePriority));
    parentPage.pageToIndexNode();

    if(parentPage.getFirstChildPageNumber() == indexNode.pageNumber)
    {
        return -1;
    }
    else
    {
        for(int i = 0; i <= parentPage.getnKeys() ; i++)
        {
            if(indexNode.pageNumber == parentPage.getChild(i))
            return parentPage.getChild(i-1);
        }

    }

    return -1;
}


long int bptIndexPage::getRightSiblingPageNumber(bptIndexHeader ih)
{
    bptIndexPage parentPage(ih);
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    parentPage.setIndexNodePageData(indexFileIO->getPage(indexNode.parent,indexPagePriority));
    parentPage.pageToIndexNode();
    if(indexNode.pageNumber == parentPage.getLastChildPageNumber())
    {
        return -1;
    }
    else
    {
    for(int i = 0; i <= parentPage.getnKeys() ; i++)
        {
        if(indexNode.pageNumber == parentPage.getChild(i))
        return parentPage.getChild(i+1);
        }
    }
    return -1;

}
long int bptIndexPage::getLastChildPageNumber()
{
    return getChild(indexNode.nKeys);

}

long int bptIndexPage::getFirstChildPageNumber()
{
    return getChild(0);
}
int bptIndexPage::findKeyPosition(char* key)
{
    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    bptIndexHeader tempIndexHeader;
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
    for(int i = 0; i < getnKeys();i++)
    {   if(bptUtil->keyCompare(tempIndexHeader,getKey(i),key) == 0)
            return i;
    }

    return -1;

}
bool bptIndexPage::findExistence(char* key)
{
    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    bptIndexHeader tempIndexHeader;
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
    for(int i = 0; i < getnKeys();i++)
    {   if(bptUtil->keyCompare(tempIndexHeader,getKey(i),key) == 0)
            return true;
    }

    return false;
}

void bptIndexPage::replaceInIndexPage(int replacePosition,char* key)
{
    debugDeletion("replaced the key in the parent as a last element is deleted in the children");
    debugDeletion(replacePosition);

    setKey(replacePosition,key);
    debugDeletion("exit from replaceInIndexPage");
}


int bptIndexPage::getReplacePosition(long int childPageNumber)
{
    for(int i = 0; i < getnKeys(); i++)
    {   if(getChild(i) == childPageNumber)
            return i;
    }
    return getnKeys();
}


void bptIndexPage::indexNodeToPage()
{

	int offset = 0;
	bptIndexHeader tempIndexHeader;
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();


	memcpy(&indexNodePageData[offset],&indexNode,sizeof(char));
	offset = offset + sizeof(char);
	memcpy(&indexNodePageData[offset],&indexNode.pageNumber,sizeof(long int));
	offset = offset + sizeof(long int);
	memcpy(&indexNodePageData[offset],&indexNode.indexHeaderPageNumber,sizeof(long int));
	offset = offset + sizeof(long int);
	memcpy(&indexNodePageData[offset],&indexNode.parent,sizeof(long int));
	offset = offset + sizeof(long int);

	memcpy(&indexNodePageData[offset],&indexNode.nKeys,sizeof(int));
	offset = offset + sizeof(int);
	memcpy(&indexNodePageData[offset],&indexNode.level,sizeof(int));
	offset = offset + sizeof(int);

}

void bptIndexPage::pageToIndexNode()
{

	int offset = 0;
	bptIndexHeader tempIndexHeader;
    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

	if(indexNodePageData == NULL)
	{
		cout<<"No data in the page to create a index node,indexNodePageData is NULL"<<endl;
	}
	else
	{
		memcpy(&indexNode.priority,&indexNodePageData[offset],sizeof(char));
		offset = offset + sizeof(char);

		memcpy(&indexNode.pageNumber,&indexNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&indexNode.indexHeaderPageNumber,&indexNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&indexNode.parent,&indexNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);



		memcpy(&indexNode.nKeys,&indexNodePageData[offset],sizeof(int));
		offset = offset + sizeof(int);

		memcpy(&indexNode.level,&indexNodePageData[offset],sizeof(int));
		offset = offset + sizeof(int);

	}


}

void bptIndexPage::printIndexPage()
{


	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptIndexHeader tempIndexHeader;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(indexNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
	cout<<"<========IndexPage========>"<<endl;
	cout<<"priority                 :"<<indexNode.priority<<endl;
	cout<<"pageNumber               :"<<indexNode.pageNumber<<endl;
	cout<<"indexHeaderPageNumber    :"<<indexNode.indexHeaderPageNumber<<endl;
	cout<<"parent  PageNumber       :"<<indexNode.parent<<endl;

	cout<<"Level                    :"<<indexNode.level<<endl;
	cout<<"Number of Keys           :"<<indexNode.nKeys<<endl;

	for(int i = 0; i < indexNode.nKeys; i++)
	{

        cout<<"child PageNumber         :"<<getChild(i)<<"\t";
		cout<<"key                      :";
        bptUtil->printKey(tempIndexHeader,getKey(i));
        cout<<endl;

	}
	cout<<"child PageNumber         :"<<getChild(indexNode.nKeys)<<endl;
	cout<<"<==========================>"<<endl;

}
#endif /* B_INDEXNODE_HPP_ */

