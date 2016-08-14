/*
 * B+Insertion.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: satya
 */

#ifndef B_INSERTION_HPP_
#define B_INSERTION_HPP_


#include"globals.hpp"
#include"debug.hpp"
#include"B+Tree.hpp"
#include"B+Util.hpp"
#include"fileIO.hpp"

using namespace std;


void bpt::H_insertIntoBPT(long int indexHeaderPageNumber,string key,RID rid)
{

    bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexHeader ih;
    long int rootPageNumber;
    int I_insertionKey;
    int dataType;
    char* C_insertionKey;
    ih.setIndexHeaderPageData(indexFileIO->getPage(indexHeaderPageNumber,indexHeaderPagePriority));
    ih.indexHeaderPageToObj();

    rootPageNumber = ih.getRootPageNumber();
    dataType = ih.getDataType();

    switch(dataType)
    {

    case typeInt:   {
                        I_insertionKey = bptUtil->stringToNumber(key);
                        C_insertionKey = new char[sizeof(int)];
                        memcpy(C_insertionKey,&I_insertionKey,sizeof(int));
                        insertToBPT(rootPageNumber,C_insertionKey,rid);
                        break;

                    }
    case typeString:
                    {
                        C_insertionKey = new char[key.size()];
                        strcpy(C_insertionKey,key.c_str());
                        insertToBPT(rootPageNumber,C_insertionKey,rid);
                        break;
                    }


    default: break;

    }
}



long int bpt::insertToBPT(long int rootPageNumber,char* key,RID rid)
{

	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptIndexPage rootPage(indexHeader);
	int insertPosition;
	long int  splitPageNumber;
	rootPage.setIndexNodePageData(indexFileIO->getPage(rootPageNumber,defaultPriority));
	rootPage.pageToIndexNode();




	if (rootPage.getLevel() == 0)
	{
		debugInsertion("node to be inserted is a leaf");
		splitPageNumber = insertToLeafPage(rootPageNumber,key,rid);
		return splitPageNumber;
	}
	else
	{
		debugInsertion("Reached an internal node..so going on with the corresponding child");

		insertPosition = bptUtil->getInsertPosition(rootPage,key);
		long int  child;
		child = rootPage.getChild(insertPosition);
		debugInsertion("child to reach"<<child);
		splitPageNumber = insertToBPT(child,key,rid);

	}

	switch ((long int) splitPageNumber)
	{
	case NOSPLIT:
		debugInsertion("there is no split in the child tree");
		return NOSPLIT;
		break;
	default:
		debugInsertion("there is a split & the internal node has to be updated");
		splitPageNumber = insertToIndexPage(rootPageNumber, splitPageNumber);
		return splitPageNumber;
		break;
	}
	return splitPageNumber;

}




long int  bpt::insertToLeafPage(long int leafPageNumber,char* key,RID rid)
{

	debugInsertion("entered insertToLeafNode" );
	int insertPosition;
	int k;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptLeafPage leafPage(indexHeader);
	leafPage.setLeafNodePageData(indexFileIO->getPage(leafPageNumber,leafPagePriority));
	leafPage.pageToLeafNode();

	if (!(leafPage.getnKeys() == indexHeader.getFanout() - 1))
	{
		debugInsertion("before insertion...");
		debugInsertionLeafPage(leafPage);
		debugInsertion("Leaf is not full..so task is simple");
		insertPosition = bptUtil->getInsertPosition(leafPage,key);
		for (k = leafPage.getnKeys() - 1; k >= insertPosition; k--)
		{
			leafPage.setKey(k + 1,leafPage.getKey(k));
			leafPage.setRid(k + 1,leafPage.getRid(k));
		}

		leafPage.setKey(insertPosition,key);
		leafPage.setRid(insertPosition,rid);
		leafPage.setnKeys(leafPage.getnKeys() + 1);

		debugInsertion("leaf is not full & hence done with insertion without a split");
		debugInsertion("Insertion Result in the node:");
		debugInsertionLeafPage(leafPage);
		leafPage.leafNodeToPage();
		indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
		return NOSPLIT;
	}
	else
	{
		debugInsertion("leaf is full..be care full splitting");
		bptLeafPage  newLeafPage(indexHeader);

		newLeafPage.setLeafNodePageData(new char[PAGESIZE]);
		memset(newLeafPage.getLeafNodePageData(),0,PAGESIZE);
		splitLeafPage(leafPage,newLeafPage, key,rid);
		leafPage.leafNodeToPage();
		newLeafPage.leafNodeToPage();
		debugInsertion("after splitting...");
		debugInsertion("leafPage");
		debugInsertionLeafPage(leafPage)
		debugInsertion("newLeafPage");
        debugInsertionLeafPage(newLeafPage);

		indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
		indexFileIO->writePage(newLeafPage.getPageNumber(),newLeafPage.getLeafNodePageData());
		debugInsertion("done with splitting of a leaf node & returning the new sibling leaf to the parent");
		return newLeafPage.getPageNumber();
	}

}


long int bpt::insertToIndexPage(long int indexPageNumber, long int  newChildPageNumber)
{
	debugInsertion("Entered insertToIndexPage ");
	debugInsertion("indexPageNumber"<<indexPageNumber);
	debugInsertion("newChildPageNumber"<<newChildPageNumber);
	int insertPosition;
	char* key = new char[indexHeader.getKeySize()];
	int k;
	bool rootSplitted = false;
	bptIndexPage indexPage(indexHeader);
	bptLeafPage newChildLeafPage(indexHeader);
	bptIndexPage newChildIndexPage(indexHeader);
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

	indexPage.setIndexNodePageData(indexFileIO->getPage(indexPageNumber,indexPagePriority));
	indexPage.pageToIndexNode();
	debugInsertionIndexPage(indexPage);


	if(indexPage.getLevel() == 1)
	{
        debugInsertion("propagation from a leaf");
        newChildLeafPage.setLeafNodePageData(indexFileIO->getPage(newChildPageNumber,leafPagePriority));
        newChildLeafPage.pageToLeafNode();
        memcpy(key,getPropagatingElement(),indexHeader.getKeySize());
        //key = propagatingElement;
		//newChildLeafPage.setKey(indexHeader.getFanout() - 1,NULL);

	}
	else
	{
	    debugInsertion("propagation from an indexPage");
        newChildIndexPage.setIndexNodePageData(indexFileIO->getPage(newChildPageNumber,indexPagePriority));
        newChildIndexPage.pageToIndexNode();
        memcpy(key,getPropagatingElement(),indexHeader.getKeySize());
        //key = propagatingElement;
      //  newChildIndexPage.setKey(indexHeader.getFanout() - 1, NULL);
	}


    if (indexPage.getnKeys() != indexHeader.getFanout()-1)
    {
        debugInsertion("current index page is not full & hence no need to split");
        insertPosition = bptUtil->getInsertPosition(indexPage,key);
        indexPage.setChild(indexPage.getnKeys() + 1,indexPage.getChild(indexPage.getnKeys()));
        for (k = indexPage.getnKeys() - 1; k >= insertPosition; k--)
        {
            indexPage.setKey(k + 1,indexPage.getKey(k));
            indexPage.setChild(k + 1,indexPage.getChild(k));
        }
        indexPage.setKey(insertPosition,key);
        indexPage.setChild(insertPosition + 1,newChildPageNumber);
        indexPage.setnKeys(indexPage.getnKeys() + 1) ;
        if (indexPage.getLevel() == 1)
           {
                newChildLeafPage.setParent(indexPageNumber);
                newChildLeafPage.leafNodeToPage();
                indexFileIO->writePage(newChildLeafPage.getPageNumber(),newChildLeafPage.getLeafNodePageData());
                debugInsertionLeafPage(newChildLeafPage);

           }
        else
            {
                newChildIndexPage.setParent(indexPageNumber);
                newChildIndexPage.indexNodeToPage();
                indexFileIO->writePage(newChildIndexPage.getPageNumber(),newChildIndexPage.getIndexNodePageData());
                debugInsertionIndexPage(newChildIndexPage);
            }
        debugInsertion("new child page has been written to the file");
        indexPage.indexNodeToPage();
        indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());
        debugInsertionIndexPage(indexPage);
        debugInsertion("index page written to the file");
        return NOSPLIT;
    }
    else
    {
        debugInsertion("current index page is full & we need to split");
        bptIndexPage newIndexPage(indexHeader);
        if (indexPage.getLevel() == 1)
            splitIndexPage(indexPage, newIndexPage, newChildLeafPage, key);
        else
            splitIndexPage(indexPage, newIndexPage, newChildIndexPage, key);
        if (indexPage.getLevel() == indexHeader.getnLevels() - 1)
        {
            debugInsertion("Current internal node is the current root...it has got split.. so we have to update with a newRoot");
            debugInsertion("currentRoot:");
            debugInsertionIndexPage(indexPage);
            debugInsertion("currentRootSibling:");
            debugInsertionIndexPage(newIndexPage);
            memcpy(key,getPropagatingElement(),indexHeader.getKeySize());
            //key = propagatingElement;
            updateRootPage(indexPage, newIndexPage, key);
            rootSplitted = true;
        }
        indexPage.indexNodeToPage();
        newIndexPage.indexNodeToPage();
        indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());
        indexFileIO->writePage(newIndexPage.getPageNumber(),newIndexPage.getIndexNodePageData());

        if (indexPage.getLevel() == 1)
        {
            newChildLeafPage.leafNodeToPage();
            indexFileIO->writePage(newChildLeafPage.getPageNumber(),newChildLeafPage.getLeafNodePageData());
        }
        else
        {
            newChildIndexPage.indexNodeToPage();
            indexFileIO->writePage(newChildIndexPage.getPageNumber(),newChildIndexPage.getIndexNodePageData());
        }

        if(rootSplitted == true)
        return NOSPLIT; //terminates the propagation of insertion to the higher levels
        else
        return newIndexPage.getPageNumber();
    }


}
void bpt::splitLeafPage(bptLeafPage &leafPage, bptLeafPage &newLeafPage,char* key,RID rid)
{
	debugInsertion("Now..task is to split a leaf and copy into a newLeaf accordingly");
	bptUtility *bptUtil = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();


	int insertPosition;
	int tempnKeys;
	int i, k;
	RID dummyRid = {-1,-1};

	newLeafPage.setPageNumber(indexFileIO->getNextPageNumber());
	newLeafPage.setLeafNodePageData(new char[PAGESIZE]);
	memset(newLeafPage.getLeafNodePageData(),0,PAGESIZE);
	insertPosition = bptUtil->getInsertPosition(leafPage,key);


	newLeafPage.setLeft(leafPage.getPageNumber());
	newLeafPage.setRight(leafPage.getRight());
	if(leafPage.getRight() != -1)
	{
        bptLeafPage leafPageRightSibling(indexHeader);
        leafPageRightSibling.setLeafNodePageData(indexFileIO->getPage(leafPage.getRight(),leafPagePriority));
        leafPageRightSibling.pageToLeafNode();
        leafPageRightSibling.setLeft(newLeafPage.getPageNumber());
        leafPageRightSibling.leafNodeToPage();
        indexFileIO->writePage(leafPageRightSibling.getPageNumber(),leafPageRightSibling.getLeafNodePageData());
	}

	leafPage.setRight(newLeafPage.getPageNumber());


	debugInsertion("insertPosition:");
	debugInsertion(insertPosition);
	debugInsertion("middleElementPosition:");
	debugInsertion(middleElementPosition);
	tempnKeys = leafPage.getnKeys();
	if (insertPosition <= middleElementPosition)
	{
		debugInsertion("new key has to be inserted in the first half i.e in the existing leaf");
		k = 0;
		for (i = (middleElementPosition); i < tempnKeys; i++)
		{
			newLeafPage.setKey(k,leafPage.getKey(i));
			//leafPage.setKey(i,NULL);
			newLeafPage.setRid(k,leafPage.getRid(i));
			leafPage.setRid(i,dummyRid);
			k++;
			leafPage.setnKeys(leafPage.getnKeys() - 1);
			newLeafPage.setnKeys(newLeafPage.getnKeys() + 1) ;
			}
		newLeafPage.setRid(k,leafPage.getRid(i));
		leafPage.setRid(i,dummyRid);
		debugInsertion("after copying each element");
        debugInsertion("Leaf:");
        debugInsertionLeafPage(leafPage);
        debugInsertion("newLeaf:");
        debugInsertionLeafPage(newLeafPage);
        for (k = leafPage.getnKeys() - 1; k >= insertPosition; k--)
		{
			leafPage.setKey(k + 1,leafPage.getKey(k));
			leafPage.setRid(k + 1,leafPage.getRid(k));
		}

		leafPage.setKey(insertPosition,key);
		leafPage.setRid(insertPosition,rid);
		leafPage.setnKeys(leafPage.getnKeys() + 1);
		debugInsertion("new key is inserted in the first half i.e first leaf");
		debugInsertion("done with splitting & copying..");
		debugInsertion("printing the split nodes");
		debugInsertion("Leaf:");
		debugInsertionLeafPage(leafPage);
		debugInsertion("newLeaf:");
		debugInsertionLeafPage(newLeafPage);
	}
	else
	{
		k = 0;
		debugInsertion( "new key has to be inserted in the second half i.e new leaf");

		for (i = (middleElementPosition + 1); i < tempnKeys; i++)
		{
            debugInsertion("child copied is:");
			newLeafPage.setKey(k,leafPage.getKey(i));
			//leafPage.setKey(i,NULL);
			newLeafPage.setRid(k,leafPage.getRid(i));
			leafPage.setRid(i,dummyRid);
			k++;
			leafPage.setnKeys(leafPage.getnKeys()-1);
			newLeafPage.setnKeys(newLeafPage.getnKeys()+1);

		}
		newLeafPage.setRid(k,leafPage.getRid(i));
		leafPage.setRid(i,dummyRid);
        insertPosition = bptUtil->getInsertPosition(newLeafPage,key);
		for (k = newLeafPage.getnKeys() - 1; k >= insertPosition; k--)
		{
			newLeafPage.setKey(k + 1,newLeafPage.getKey(k));
			newLeafPage.setRid(k + 1,newLeafPage.getRid(k));
		}


		newLeafPage.setKey(insertPosition,key);
		newLeafPage.setRid(insertPosition,rid);
		newLeafPage.setnKeys(newLeafPage.getnKeys() + 1);

		debugInsertion("new key has been inserted in the new Leaf node");
		debugInsertion("done with spitting & copying");
		debugInsertion("printing the split nodes");
		debugInsertion("Leaf:");
		debugInsertionLeafPage(leafPage);
		debugInsertion("newLeaf:");
		debugInsertionLeafPage(newLeafPage);
	}
	setPropagatingElement(new char[indexHeader.getKeySize()]);
    memcpy(getPropagatingElement(),leafPage.getKey(leafPage.getnKeys()-1),indexHeader.getKeySize());
	//propagatingElement = leafPage.getKey(leafPage.getnKeys()-1);



}
template <typename pageType>
void bpt::splitIndexPage(bptIndexPage &indexPage,bptIndexPage &newIndexPage, pageType &newChildSibling,char* key)
{
	debugInsertion("Entered splitIndexPage");
	debugInsertion("IndexPage before splitting");
	debugInsertionIndexPage(indexPage);
	int insertPosition;
	int tempnKeys, k,i = 0;
    bptUtility *bptUtil  = bptUtility::getBptUtilityInstance();
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

	newIndexPage.setPageNumber(indexFileIO->getNextPageNumber());
	newIndexPage.setIndexNodePageData(new char[PAGESIZE]);
	memset(newIndexPage.getIndexNodePageData(),0,PAGESIZE);
	insertPosition = bptUtil->getInsertPosition(indexPage,key);
	debugInsertion("element to be inserted :");

	debugInsertion("element is to be inserted in the position:");
	debugInsertion(insertPosition);
	tempnKeys = indexPage.getnKeys();
	newIndexPage.setLevel(indexPage.getLevel());
	debugInsertion("done with new internal node initialization");
	if (insertPosition <= middleElementPosition)
	{
		debugInsertion("incoming has to be inserted in the first half i.e existing internal node");
		if (insertPosition == middleElementPosition)
		{
			setPropagatingElement(new char[indexHeader.getKeySize()]);
			memcpy(getPropagatingElement(),key,indexHeader.getKeySize());
			//propagatingElement = key;
			debugInsertion("incoming element has to be propagated");
			debugInsertion("element being propagated is:");
			debugKey(indexHeader,propagatingElement);

			k = 0;
			newIndexPage.setChild(0,newChildSibling.getPageNumber());
			newChildSibling.setParent(newIndexPage.getPageNumber());
			for(i = middleElementPosition; i < tempnKeys; i++)
			{
				newIndexPage.setKey(k,(indexPage.getKey(i)));
				newIndexPage.setChild(k+1,indexPage.getChild(i+1));
				updateParent(newIndexPage.getChild(k+1),newIndexPage.getPageNumber(),indexPage.getLevel());
				//indexPage.setKey(i,NULL);
				indexPage.setChild(i+1,-1);
				indexPage.setnKeys(indexPage.getnKeys() - 1);
				newIndexPage.setnKeys(newIndexPage.getnKeys() + 1);
				k++;
			}
		}
		else
		{
			//propagatingElement = indexPage.getKey(middleElementPosition - 1);
			setPropagatingElement(new char[indexHeader.getKeySize()]);
			memcpy(getPropagatingElement(),indexPage.getKey(middleElementPosition - 1),indexHeader.getKeySize());

			debugInsertion("element being propagated is:");
			debugKey(indexHeader,propagatingElement);
			//indexPage.setKey(middleElementPosition-1,NULL);
			debugInsertion("incoming element has to be inserted within the middle element in the existing internal node");

			k = 0;
			for (i = (middleElementPosition); i < tempnKeys; i++)
			{

				newIndexPage.setKey(k,indexPage.getKey(i));
                debugInsertion("child copied is:");
				debugKey(indexHeader,indexPage.getKey(i));


				//indexPage.setKey(i,NULL);
				newIndexPage.setChild(k,indexPage.getChild(i));
				updateParent(newIndexPage.getChild(k),newIndexPage.getPageNumber(),indexPage.getLevel());
				debugInsertion("child node moved from existing internal node to the new internal node");
				debug(indexPage.getChild(i));
				indexPage.setChild(i,-1);
				k++;
				indexPage.setnKeys(indexPage.getnKeys() - 1);
				newIndexPage.setnKeys(newIndexPage.getnKeys() + 1);
			}
			newIndexPage.setChild(k,indexPage.getChild(i));
			updateParent(newIndexPage.getChild(k),newIndexPage.getPageNumber(),indexPage.getLevel());

			indexPage.setChild(i,-1);
			debugInsertion("inserting with in the appropriate position with in the existing internal node");
			int k;
			for (k = indexPage.getnKeys() - 1; k >= insertPosition; k--)
			{
					indexPage.setKey(k + 1,indexPage.getKey(k));
					indexPage.setChild(k + 1,indexPage.getChild(k));
			}
			indexPage.setKey(insertPosition,key);
			indexPage.setChild(insertPosition + 1,newChildSibling.getPageNumber());
			newChildSibling.setParent(indexPage.getPageNumber());

		}



	}
	else
	{
		debugInsertion("insertion in the new internal node");
        debugKey(indexHeader,key);
		debugInsertionIndexPage(indexPage);
		setPropagatingElement(new char[indexHeader.getKeySize()]);
		memcpy(getPropagatingElement(),indexPage.getKey(middleElementPosition),indexHeader.getKeySize());
		//propagatingElement = indexPage.getKey(middleElementPosition);
		//indexPage.setKey(middleElementPosition,NULL);
		indexPage.setnKeys(indexPage.getnKeys() - 1);
		debugInsertion("middleElementPosition:");
		debugInsertion(middleElementPosition);
		k = 0;
		for (i = (middleElementPosition + 1); i < tempnKeys; i++)
		{
			debugInsertion("element being copied from existing internal node to the new internal node");
			debugInsertion(indexPage.getKey(i));
			newIndexPage.setKey(k,indexPage.getKey(i));
			//indexPage.setKey(i,NULL);
			debugInsertion("child node moved from existing internal node to the new internal node");
			newIndexPage.setChild(k,indexPage.getChild(i));
			updateParent(newIndexPage.getChild(k),newIndexPage.getPageNumber(),indexPage.getLevel());
			indexPage.setChild(i,-1);
			k++;
			indexPage.setnKeys(indexPage.getnKeys() - 1);
			newIndexPage.setnKeys(newIndexPage.getnKeys() + 1);
		}

		newIndexPage.setChild(k,indexPage.getChild(i));
		updateParent(newIndexPage.getChild(k),newIndexPage.getPageNumber(),indexPage.getLevel());
		indexPage.setChild(i,-1);
		insertPosition = bptUtil->getInsertPosition(newIndexPage,key);
		for (k = newIndexPage.getnKeys() - 1; k >= insertPosition; k--)
		{
			newIndexPage.setKey(k + 1,newIndexPage.getKey(k));
			newIndexPage.setChild(k + 2,newIndexPage.getChild(k+1));
		}
		newIndexPage.setKey(insertPosition,key);
		newIndexPage.setnKeys(newIndexPage.getnKeys() + 1);
		debugInsertion("element at insert position:");
		debugInsertion(newIndexPage.getKey(insertPosition));
		debugInsertion(newIndexPage.getnKeys());
		newIndexPage.setChild(insertPosition + 1,newChildSibling.getPageNumber());
	//	updateParent(newChildSibling.getPageNumber(),newIndexPage.getPageNumber(),indexPage.getLevel());
		newChildSibling.setParent(newIndexPage.getPageNumber());
		indexPage.setChild(middleElementPosition+1,-1);
	}
	debugInsertion("After splitting:");
	debugInsertion("index Page:");
	debugInsertionIndexPage(indexPage);
	debugInsertion("newIndexPage:");
	debugInsertionIndexPage(newIndexPage);


}

void bpt::updateRootPage(bptIndexPage &currentRootPage,bptIndexPage  &currentRootSiblingPage,char* key)
{
	debugInsertion("Entered a function to update the existing the root");
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

	bptIndexPage newRootPage(indexHeader);
	newRootPage.setIndexNodePageData(new char[PAGESIZE]);
	memset(newRootPage.getIndexNodePageData(),0,PAGESIZE);
	newRootPage.setPageNumber(indexFileIO->getNextPageNumber());
	//newRootPage.setIndexNodePageData(indexFileIO->getPage(newRootPage.getPageNumber(),rootPagePriority));
	newRootPage.setChild(0,currentRootPage.getPageNumber());
	newRootPage.setChild(1,currentRootSiblingPage.getPageNumber());
	newRootPage.setLevel(currentRootPage.getLevel() + 1);
	currentRootPage.setParent(newRootPage.getPageNumber());
	currentRootSiblingPage.setParent(newRootPage.getPageNumber());
	currentRootPage.setPriority(indexPagePriority);

	newRootPage.setKey(0,key);
	newRootPage.setnKeys(1);
	newRootPage.setPriority(rootPagePriority);
	indexHeader.setnLevels(indexHeader.getnLevels() + 1);
	indexHeader.setRootPageNumber(newRootPage.getPageNumber());
	indexHeader.objToIndexHeaderPage();
	indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());
	newRootPage.indexNodeToPage();
	indexFileIO->writePage(newRootPage.getPageNumber(),newRootPage.getIndexNodePageData());

	return;
}

void bpt::updateParent(long int childPageNumber,long int parentPageNumber,int parentPageLevel)
{
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	bptIndexPage tempIndexPage(indexHeader);
	bptLeafPage tempLeafPage(indexHeader);
	if(parentPageLevel == 1)
	{

		tempLeafPage.setLeafNodePageData(indexFileIO->getPage(childPageNumber,leafPagePriority));
		tempLeafPage.pageToLeafNode();
		tempLeafPage.setParent(parentPageNumber);
		tempLeafPage.leafNodeToPage();
		indexFileIO->writePage(childPageNumber,tempLeafPage.getLeafNodePageData());

	}
	else
	{

		tempIndexPage.setIndexNodePageData(indexFileIO->getPage(childPageNumber,indexPagePriority));
		tempIndexPage.pageToIndexNode();
		tempIndexPage.setParent(parentPageNumber);
		tempIndexPage.indexNodeToPage();
		indexFileIO->writePage(childPageNumber,tempIndexPage.getIndexNodePageData());

	}

}


#endif /* B_INSERTION_HPP_ */

