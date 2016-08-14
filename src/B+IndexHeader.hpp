/*
 * B+IndexHeader.hpp
 *
 *  Created on: Dec 29, 2012
 *      Author: satya
 */

#ifndef B_INDEXHEADER_HPP_
#define B_INDEXHEADER_HPP_


#include "globals.hpp"
#include "fileIO.hpp"



class bptIndexHeader
{
	struct indexHeaderStruct
	{
	char priority;
	long int indexHeaderPageNumber;
	long int rootPageNumber;
	int fanout;
	int dataType;
	int keySize;
	int nLevels;
	}indexHeader;
	char* indexHeaderPageData;

public:


	bptIndexHeader()
	{
		indexHeader.priority = indexHeaderPagePriority;
		indexHeader.indexHeaderPageNumber = -1;
		indexHeader.rootPageNumber = -1;
		indexHeader.fanout = -1;
		indexHeader.dataType = -1;
		indexHeader.keySize = -1;
		indexHeader.nLevels = -1;
		indexHeaderPageData = NULL;
	}

    int getMaxKeysInLeafPage();
    int getMinKeysInLeafPage();
    int getMaxChildrenInIndexPage();
    int getMinChildrenInIndexPage();


	void indexHeaderPageToObj();
	void objToIndexHeaderPage();
	void printIndexHeader();


	//getters and setters
	int getDataType() const
	{
		return indexHeader.dataType;
	}

	void setDataType(int dataType)
	{
		this->indexHeader.dataType = dataType;
	}

	int getFanout() const
	{
		return indexHeader.fanout;
	}

	void setFanout(int fanout)
	{
		this->indexHeader.fanout = fanout;
	}

	long int getIndexHeaderPageNumber() const
	{
		return indexHeader.indexHeaderPageNumber;
	}

	void setIndexHeaderPageNumber(long int indexHeaderPageNumber)
	{
		this->indexHeader.indexHeaderPageNumber = indexHeaderPageNumber;
	}

	int getKeySize() const
	{

		return indexHeader.keySize;
	}

	void setKeySize(int keySize)
	{
		this->indexHeader.keySize = keySize;
	}

	int getnLevels() const
	{
		return indexHeader.nLevels;
	}

	void setnLevels(int levels)
	{
		indexHeader.nLevels = levels;
	}

	char getPriority() const
	{
		return indexHeader.priority;
	}

	void setPriority(char priority)
	{
		this->indexHeader.priority = priority;
	}

	long int getRootPageNumber() const
	{
		return indexHeader.rootPageNumber;
	}

	void setRootPageNumber(long int rootPageNumber)
	{
		this->indexHeader.rootPageNumber = rootPageNumber;
	}
   char* getIndexHeaderPageData() const
	{
		return indexHeaderPageData;

	}
    void setIndexHeaderPageData(char* pageData)
    {
        indexHeaderPageData = pageData;
    }
	virtual ~bptIndexHeader()
	{

//		delete[] indexHeaderPageData;
	}
};

 int bptIndexHeader::getMaxKeysInLeafPage()
    {
        return (indexHeader.fanout - 1);
    }
int bptIndexHeader::getMinKeysInLeafPage()
    {
        return ((indexHeader.fanout)/2);
    }
int bptIndexHeader::getMaxChildrenInIndexPage()
    {
           return (indexHeader.fanout);
    }
int bptIndexHeader::getMinChildrenInIndexPage()
    {
        return (indexHeader.fanout+1)/2;

    }


void bptIndexHeader::objToIndexHeaderPage()
{

	int offset = 0;

	memcpy(&indexHeaderPageData[offset],&indexHeader.priority,sizeof(char));
	offset = offset + sizeof(char);
	memcpy(&indexHeaderPageData[offset],&indexHeader.indexHeaderPageNumber,sizeof(long int));
	offset = offset + sizeof(long int);
	memcpy(&indexHeaderPageData[offset],&indexHeader.rootPageNumber,sizeof(long int));
	offset = offset + sizeof(long int);
	memcpy(&indexHeaderPageData[offset],&indexHeader.fanout,sizeof(int));
	offset = offset + sizeof(int);
	memcpy(&indexHeaderPageData[offset],&indexHeader.dataType,sizeof(int));
	offset = offset + sizeof(int);
	memcpy(&indexHeaderPageData[offset],&indexHeader.keySize,sizeof(int));
	offset = offset + sizeof(int);
	memcpy(&indexHeaderPageData[offset],&indexHeader.nLevels,sizeof(int));
	offset = offset + sizeof(int);



}

void bptIndexHeader::indexHeaderPageToObj()
{

	int offset = 0;
	if(indexHeaderPageData == NULL)
		cout<<"indexHeader is not yet initialized..it is still NULL"<<endl;
	else
	{
		memcpy(&indexHeader.priority,&indexHeaderPageData[offset],sizeof(char));
		offset = offset + sizeof(char);
		memcpy(&indexHeader.indexHeaderPageNumber,&indexHeaderPageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);
		memcpy(&indexHeader.rootPageNumber,&indexHeaderPageData[offset],sizeof(long int));
		offset = offset + sizeof(long int);
		memcpy(&indexHeader.fanout,&indexHeaderPageData[offset],sizeof(int));
		offset = offset + sizeof(int);
		memcpy(&indexHeader.dataType,&indexHeaderPageData[offset],sizeof(int));
		offset = offset + sizeof(int);
		memcpy(&indexHeader.keySize,&indexHeaderPageData[offset],sizeof(int));
		offset = offset + sizeof(int);
		memcpy(&indexHeader.nLevels,&indexHeaderPageData[offset],sizeof(int));
		offset = offset + sizeof(int);

	}
}


void bptIndexHeader::printIndexHeader()
{
	cout<<"<========Index Header========>"<<endl;
	cout<<"Index Header Page Number:"<<indexHeader.indexHeaderPageNumber<<endl;
	cout<<"Root Page Number        :"<<indexHeader.rootPageNumber<<endl;
	cout<<"Fanout                  :"<<indexHeader.fanout<<endl;
	cout<<"Data Type               :"<<indexHeader.dataType<<endl;
	cout<<"Keysize                 :"<<indexHeader.keySize<<endl;
	cout<<"number of Levels        :"<<indexHeader.nLevels<<endl;
	cout<<"<============================>"<<endl;

}
#endif /* B_INDEXHEADER_HPP_ */

