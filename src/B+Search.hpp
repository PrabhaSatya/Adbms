/*
 * B+Search.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: satya
 */

#ifndef B_SEARCH_HPP_
#define B_SEARCH_HPP_

#include "B+Tree.hpp"
#include "B+Util.hpp"



vector<RID> bpt::H_searchBPT(long int indexHeaderPageNumber,string key)
{

    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexHeader ih;
    long int rootPageNumber;
    int dataType;
    vector<RID> searchData;
    long int searchResult;
    ih.setIndexHeaderPageData(indexFileIO->getPage(indexHeaderPageNumber,indexHeaderPagePriority));
    ih.indexHeaderPageToObj();

    rootPageNumber = ih.getRootPageNumber();
    dataType = ih.getDataType();

    switch(dataType)
    {

    case typeInt:   {
                        int I_searchKey;
                        char* C_searchKey;

                        I_searchKey = bptUtil->stringToNumber(key);
                        C_searchKey = new char[sizeof(int)];
                        memcpy(C_searchKey,&I_searchKey,sizeof(int));
                        searchResult = searchBPT(rootPageNumber,C_searchKey);
                        searchData = getSearchData(searchResult,C_searchKey);
                        return searchData;
                        break;

                    }
    case typeString:
                    {
                        char* C_searchKey;
                        C_searchKey = new char[key.size()];
                        strcpy(C_searchKey,key.c_str());
                        searchResult = searchBPT(rootPageNumber,C_searchKey);
                        searchData = getSearchData(searchResult,C_searchKey);
                        return searchData;
                        break;
                    }


    default: break;

    }
}


vector<RID> bpt::getSearchData(long int searchResult,char* searchKey)
{

	vector<RID> searchData;
    long int nextLeafPageNumber;
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	int i = 0;
	bptLeafPage nextLeafPage(indexHeader);
	nextLeafPageNumber = searchResult;
	do
	{

        nextLeafPage.setLeafNodePageData(indexFileIO->getPage(nextLeafPageNumber,leafPagePriority));
        nextLeafPage.pageToLeafNode();

	    if(bptUtil->keyCompare(indexHeader,nextLeafPage.getKey(i),searchKey) == 0)
				searchData.push_back(nextLeafPage.getRid(i));
		i++;
		if(i == (nextLeafPage.getnKeys()))
			{
				nextLeafPageNumber = nextLeafPage.getRight();
				i = 0;
			}
		if(nextLeafPageNumber == -1)
			break;
	}while(bptUtil->keyCompare(indexHeader,nextLeafPage.getKey(i),searchKey) <= 0);
	return searchData;

}

long int  bpt::searchBPT(long int rootPageNumber, char* key)
{
	debugSearch("entered searchBPT..");
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	int index;
	if (bptUtil->isLeaf(rootPageNumber))
	{
           /* bptLeafPage tempLeafPage(indexHeader);
            tempLeafPage.setLeafNodePageData(indexFileIO->getPage(rootPageNumber));
            tempLeafPage.pageToLeafNode();
            index = bptUtil->getSearchPosition(tempLeafPage,key);
            if(index == tempLeafPage.getnKeys())
            return NOTFOUND;
            else*/
            return rootPageNumber;
	}
	else
	{
        bptIndexPage tempIndexPage(indexHeader);
        tempIndexPage.setIndexNodePageData(indexFileIO->getPage(rootPageNumber,defaultPriority));
        tempIndexPage.pageToIndexNode();
		index = bptUtil->getSearchPosition(tempIndexPage,key);
		return searchBPT((tempIndexPage.getChild(index)), key);
	}
}

#endif /* B_SEARCH_HPP_ */

