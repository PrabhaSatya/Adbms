/*
 * B+LeafNode.hpp
 *
 *  Created on: Dec 28, 2012
 *      Author: satya
 */

#ifndef B_LEAFPAGE_HPP_
#define B_LEAFPAGE_HPP_


#include"globals.hpp"
#include"debug.hpp"
#include"fileIO.hpp"


#include "B+Util.hpp"

class bptIndexHeader;

class bptLeafPage
{

	struct bptleafNode
	{
		char priority;
		long int pageNumber;
		long int indexHeaderPageNumber;
		long int parent;
		int nKeys;
		int level;
		long int left;
		long int right;
	}leafNode;
	char* leafNodePageData;

public:
	bptLeafPage()
	{}

	bptLeafPage(bptIndexHeader ih)
	{

		leafNode.priority = leafPagePriority;
		leafNode.nKeys = 0;
		leafNode.indexHeaderPageNumber = ih.getIndexHeaderPageNumber();
		leafNode.parent = -1;
		leafNode.level = 0;
		leafNode.left = -1;
		leafNode.right = -1;
		leafNodePageData = NULL;

	}

	void leafNodeToPage();
	void pageToLeafNode();
	void printLeafPage();
    int getDeletePosition(char* key, RID rid);
    bool findExistence(char* key);
    void shiftKeysLeft(int position); //to fill the void created after deletion, we shift left
    void shiftRidsLeft(int position);

    void shiftKeysRight(int position,int nTimes);
    void shiftRidsRight(int position,int nTimes); //to accomodate space for an incoming elements we shift right

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

    int getLeftOffset() const
    {
        int offset;
        offset = getLevelOffset() + sizeof(int);

    }

    int getRightOffset() const
    {
        int offset;
        offset = getLevelOffset() + sizeof(long int);

    }

    int getLeafNodeOffset() const
    {
        int offset;
        offset = sizeof(char) + sizeof(long int) + sizeof(long int) + sizeof(long int) + sizeof(int) + sizeof(int) + sizeof(long int)+sizeof(long int);
        return offset;

    }


	void setLeafNodePageData(char* leafNodePageData)
	{
		this->leafNodePageData = leafNodePageData;
	}

	long int getIndexHeaderPageNumber() const
	{
		return leafNode.indexHeaderPageNumber;
	}

	void setIndexHeaderPageNumber(long int indexHeaderPageNumber)
	{
		this->leafNode.indexHeaderPageNumber = leafNode.indexHeaderPageNumber;
	}


	char* getKey(int i) const
	{
		int offset;
		bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
		bptIndexHeader tempIndexHeader;
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
		offset = getLeafNodeOffset();

		offset = offset + (i*tempIndexHeader.getKeySize());
		return &leafNodePageData[offset];

	}

	void setKey(int i, char* keys)
	{
        int offset;
	    bptIndexHeader tempIndexHeader;
	    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
        offset = getLeafNodeOffset();
		offset = offset + (i*tempIndexHeader.getKeySize());
        memcpy(&leafNodePageData[offset],keys,tempIndexHeader.getKeySize());

	}

	long int getPageNumber() const
	{
		return leafNode.pageNumber;
	}

	void setPageNumber(long int pageNumber)
	{
		this->leafNode.pageNumber = pageNumber;
	}


	char getPriority() const
	{
		return leafNode.priority;
	}

	void setPriority(char priority)
	{
		this->leafNode.priority = priority;
	}

	long int getLeft() const
	{
		return leafNode.left;
	}

	void setLeft(long int left)
	{
		this->leafNode.left = left;
	}

	int getLevel() const
	{
		return leafNode.level;
	}

	void setLevel(int level)
	{
		this->leafNode.level = level;
	}

	int getnKeys() const
	{
		return leafNode.nKeys;
	}

	void setnKeys(int nkeys)
	{
		leafNode.nKeys = nkeys;
	}

	long int getParent() const
	{
		return leafNode.parent;
	}

	void setParent(long int parent)
	{
		this->leafNode.parent = parent;
	}

	RID getRid(int i) const
	{
        RID rid;
        int offset;
        bptIndexHeader tempIndexHeader;
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
        offset = getLeafNodeOffset();
        offset = offset + ((tempIndexHeader.getFanout()-1)*tempIndexHeader.getKeySize());
        offset = offset + (i*sizeof(RID));
        memcpy(&rid,&leafNodePageData[offset],sizeof(RID));
        return rid;


	}
	void setRid(int i, RID rid)
	{

        int offset;
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        bptIndexHeader tempIndexHeader;
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();
	    offset = getLeafNodeOffset();
	    offset = offset + ((tempIndexHeader.getFanout()-1)*tempIndexHeader.getKeySize());
        offset = offset + (i*sizeof(RID));
        memcpy(&leafNodePageData[offset],&rid,sizeof(RID));


	}

	long int getRight() const
	{
		return leafNode.right;
	}

	void setRight(long int right)
	{
		this->leafNode.right = right;
	}

	char* getLeafNodePageData()
	{
		return leafNodePageData;
	}




	virtual ~bptLeafPage()
	{


	/*	for(int i = 0; i< leafNode.nKeys; i++)
			delete [] leafNode.keys[i];

		delete [] leafNode.keys;

		delete [] leafNode.rids;

        delete leafNodePageData;*/

	}
};


 void bptLeafPage::shiftKeysLeft(int position)
    {
        for(int i = position+1 ; i < getnKeys();i++)
        {

                //leafNode.keys[i-1]  = leafNode.keys[i];
                setKey(i-1,getKey(i));

        }
    }
void bptLeafPage::shiftRidsLeft(int position)
    {
        for(int i = position + 1; i < getnKeys(); i++)
        {
            //   leafNode.rids[i-1] = leafNode.rids[i];
            setRid(i-1,getRid(i));
        }
    }

void bptLeafPage::shiftKeysRight(int position,int nTimes)
{
    int from,to;
    from = getnKeys() - 1;
    to = position;
    while(nTimes > 0)
    {
    for(int i = from; i >= to; i--)
       {
         //leafNode.keys[i+1] = leafNode.keys[i];
         setKey(i+1,getKey(i));
       }
    nTimes = nTimes - 1;
    from = from + 1;
    to = to + 1;
    }

}
void bptLeafPage::shiftRidsRight(int position,int nTimes)
{
    int from,to;
    from = getnKeys() - 1;
    to = position;
    while(nTimes > 0)
    {
    for(int i = from; i >= to; i--)
       {
           setRid(i+1,getRid(i));
         //leafNode.rids[i+1] = leafNode.rids[i];
       }
    nTimes = nTimes - 1;
    from = from + 1;
    to = to + 1;
    }
}


bool bptLeafPage::findExistence(char* key)
{
    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    bptIndexHeader tempIndexHeader;
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(getIndexHeaderPageNumber(),indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
    for(int i = 0; i < getnKeys();i++)
    {   if(bptUtil->keyCompare(tempIndexHeader,getKey(i),key) == 0)
            return true;
    }

    return false;
}
int bptLeafPage::getDeletePosition(char* key, RID rid)
    {

        debugDeletion("in getDeletePosition");

        int i;

        bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        bptIndexHeader tempIndexHeader;
        tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(getIndexHeaderPageNumber(),indexHeaderPagePriority));
        tempIndexHeader.indexHeaderPageToObj();

        for(i = 0; i < getnKeys(); i++)
        {


            if(((bptUtil->keyCompare(tempIndexHeader,getKey(i),key)) == 0) && rid.pageNumber == getRid(i).pageNumber && rid.slotNumber == getRid(i).slotNumber)
                break;
        }
        debugDeletion("exit getDeletePosition");

        return i;

    }

void bptLeafPage::leafNodeToPage()
{


	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptIndexHeader tempIndexHeader;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	int offset = 0;
    tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();

	if(leafNodePageData == NULL)
	{
		leafNodePageData = indexFileIO->getPage(leafNode.pageNumber,leafPagePriority);
	}
	memcpy(&leafNodePageData[offset],&leafNode.priority,sizeof(char));
	offset = offset + sizeof(char);

	memcpy(&leafNodePageData[offset],&leafNode.pageNumber,sizeof(long int));
	offset = offset + sizeof(long int);

	memcpy(&leafNodePageData[offset],&leafNode.indexHeaderPageNumber,sizeof(long int));
	offset = offset + sizeof(long int);

	memcpy(&leafNodePageData[offset],&leafNode.parent,sizeof(long int));
	offset = offset + sizeof(long int);

	memcpy(&leafNodePageData[offset],&leafNode.nKeys,sizeof(int));
	offset = offset + sizeof(int);

	memcpy(&leafNodePageData[offset],&leafNode.level,sizeof(int));
	offset = offset + sizeof(int);

	memcpy(&leafNodePageData[offset],&leafNode.left,sizeof(long int));
	offset = offset + sizeof(long int);

	memcpy(&leafNodePageData[offset],&leafNode.right,sizeof(long int));
	offset = offset + sizeof(long int);


}

void bptLeafPage::pageToLeafNode()
{

	int offset  = 0;
    bptIndexHeader tempIndexHeader;
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();


	if(leafNodePageData == NULL)
		cout<<"Error..leaf page does't contain any data,pageData is not initialized"<<endl;
	else
	{

		memcpy(&leafNode.priority,&leafNodePageData[offset],sizeof(char));
		offset = offset + sizeof(char);

		memcpy(&leafNode.pageNumber,&leafNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&leafNode.indexHeaderPageNumber,&leafNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&leafNode.parent,&leafNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&leafNode.nKeys,&leafNodePageData[offset],sizeof(int));
		offset = offset + sizeof(int);

		memcpy(&leafNode.level,&leafNodePageData[offset],sizeof(int));
		offset = offset + sizeof(int);
		memcpy(&leafNode.left,&leafNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

		memcpy(&leafNode.right,&leafNodePageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);

	}


}

void bptLeafPage::printLeafPage()
{
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptIndexHeader tempIndexHeader;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(leafNode.indexHeaderPageNumber,indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();

	cout<<"<========LeafPage========>"<<endl;
	cout<<"priority                 :"<<leafNode.priority<<endl;
	cout<<"pageNumber               :"<<leafNode.pageNumber<<endl;
	cout<<"indexHeaderPageNumber    :"<<leafNode.indexHeaderPageNumber<<endl;
	cout<<"parent PageNumber        :"<<leafNode.parent<<endl;
	cout<<"left PageNumber          :"<<leafNode.left<<endl;
	cout<<"right PageNumber         :"<<leafNode.right<<endl;
	cout<<"Level                    :"<<leafNode.level<<endl;
	cout<<"Number of Keys           :"<<leafNode.nKeys<<endl;
	for(int i = 0; i < leafNode.nKeys; i++ )
	{   cout<<"Key                  :";
		bptUtil->printKey(tempIndexHeader,getKey(i));
		cout<<"\t";
		cout<<"rid: "<<getRid(i).pageNumber<<"\t"<<getRid(i).slotNumber<<endl;
	}
	cout<<"<=========================>"<<endl;
}

#endif /* B_LEAFNODE_HPP_ */

