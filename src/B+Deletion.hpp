#ifndef B_DELETION_HPP_INCLUDED
#define B_DELETION_HPP_INCLUDED
#include "globals.hpp"
#include "debug.hpp"
#include "B+Tree.hpp"
#include "B+Search.hpp"

bool bpt::deleteFromBPT(long int rootPageNumber,char * key, RID rid)
{

    debugDeletion("in deleteFromBPT");

    int leafPageNumber;
    bool leafLedFlag = false; //lastElementDeletedFlag
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    leafPageNumber = searchBPT(rootPageNumber,key);
    bptLeafPage leafPage(indexHeader);
    leafPage.setLeafNodePageData(indexFileIO->getPage(leafPageNumber,leafPagePriority));
    leafPage.pageToLeafNode();

    if(!leafPage.findExistence(key))
    {
        debugDeletion("exit deleteFromBPT");
        return false;
    }
    else
    {
        deleteFromLeafPage(leafPage.getPageNumber(),key,rid);
     /*   if(leafPage.getPageNumber() != -1)
            {   leafPage.leafNodeToPage();
                indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
            }*/
        debugDeletion("exit deleteFromBPT");
        return true;

    }

    //printBPT();
}
//void bpt::deleteFromLeafPage(bptLeafPage &leafPage,char* key,RID rid)
void bpt::deleteFromLeafPage(long int leafPageNumber,char* key,RID rid)
{

        debugDeletion("in deleteFromLeafPage");
        bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
        bptUtility *bptUtil = bptUtility::getBptUtilityInstance();

        bptLeafPage leafPage(indexHeader);
        leafPage.setLeafNodePageData(indexFileIO->getPage(leafPageNumber,leafPagePriority));
        leafPage.pageToLeafNode();


        int deletePosition,replacePosition;
        bool ledFlag = false; //last element deletedflag
        RID dummyRid = {-1,-1};
        bool redistributionSuccess = false;
        deletePosition = leafPage.getDeletePosition(key,rid);
        if(deletePosition == leafPage.getnKeys()-1 ) ledFlag = true;
        //delete[] leafPage.getKey(deletePosition);

        //leafPage.setKey(deletePosition,NULL);
        leafPage.setRid(deletePosition,dummyRid);
        leafPage.shiftKeysLeft(deletePosition);
        leafPage.shiftRidsLeft(deletePosition);
        leafPage.setnKeys(leafPage.getnKeys() - 1);
        leafPage.leafNodeToPage();
        indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
        debugDeletion("deletion result in the leaf page");
        debugDeletionLeafPage(leafPage);
        if(ledFlag || leafPage.getnKeys() < indexHeader.getMinKeysInLeafPage())
        {
            debugDeletion("not just simple delete");

            bptIndexPage parentIndexPage = bptIndexPage(indexHeader);

            parentIndexPage.setIndexNodePageData(indexFileIO->getPage(leafPage.getParent(),indexPagePriority));
            parentIndexPage.pageToIndexNode();
            if(ledFlag && !(leafPage.getnKeys() < indexHeader.getMinKeysInLeafPage()))
            {
                    debugDeletion("fanout is OK but last element is deleted..so we have to replace it in the higher levels");
                    replacePosition = parentIndexPage.getReplacePosition(leafPage.getPageNumber());
                    if(replacePosition != parentIndexPage.getnKeys())
                      {
                      parentIndexPage.replaceInIndexPage(replacePosition,leafPage.getKey(leafPage.getnKeys() - 1));
                      parentIndexPage.indexNodeToPage();
                      indexFileIO->writePage(parentIndexPage.getPageNumber(),parentIndexPage.getIndexNodePageData());


                      }
                    else
                    {
                        debugDeletion("we need to update the grand parents as the last element is deleted");
                        bool successFlag;

                        successFlag = replaceInGrandParents(parentIndexPage.getPageNumber(),key,leafPage.getKey(leafPage.getnKeys() - 1));

                        if(successFlag)
                        debugDeletion("updation in grand parents is successfull");
                        if(!successFlag)
                        debugDeletion("upadation in grand parents is not success");

                    }

                   /* parentIndexPage.indexNodeToPage();
                    indexFileIO->writePage(parentIndexPage.getPageNumber(),parentIndexPage.getIndexNodePageData());*/
            }
            else
            {
              redistributionSuccess = redistributeLeafPage(parentIndexPage.getPageNumber(),leafPage.getPageNumber(),key,ledFlag);
                if(!redistributionSuccess)
                    mergeLeafPage(parentIndexPage.getPageNumber(),leafPage.getPageNumber(),key,ledFlag);

             /*   if(parentIndexPage.getPageNumber() != -1)
                {
                parentIndexPage.indexNodeToPage();
                indexFileIO->writePage(parentIndexPage.getPageNumber(),parentIndexPage.getIndexNodePageData());
                }*/
            }
        }
        debugDeletion("exit deleteFromLeafPage");
        //printBPT();
}

//bool bpt::redistributeLeafPage(bptIndexPage &parentPage,bptLeafPage &leafPage,char *key,bool ledFlag)
bool bpt::redistributeLeafPage(long int parentPageNumber,long int leafPageNumber,char *key,bool ledFlag)
{
    debugDeletion("entered redistributeLeafPage");

    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexPage parentPage(indexHeader);
    bptLeafPage leafPage(indexHeader);

    parentPage.setIndexNodePageData(indexFileIO->getPage(parentPageNumber,indexPagePriority));
    leafPage.setLeafNodePageData(indexFileIO->getPage(leafPageNumber,leafPagePriority));

    parentPage.pageToIndexNode();
    leafPage.pageToLeafNode();


    long int leftLeafPageNumber,rightLeafPageNumber;

    leftLeafPageNumber = leafPage.getLeft();
    rightLeafPageNumber = leafPage.getRight();
    int replacePosition;

    if(leftLeafPageNumber == -1 && rightLeafPageNumber == -1)
    {
         debugDeletion("no left and right siblings..is it root ??");
         debugDeletion("exit redistributeLeafPage");
         return false;
    }
    if(leftLeafPageNumber != -1)
    {
        debugDeletion("left leaf exists..trying to redistribute from it..");
        bptLeafPage leftLeafPage(indexHeader);
        leftLeafPage.setLeafNodePageData(indexFileIO->getPage(leftLeafPageNumber,leafPagePriority));
        leftLeafPage.pageToLeafNode();
        if(leftLeafPage.getnKeys() > indexHeader.getMinKeysInLeafPage())
        {
            debugDeletion("left leaf is having enough number of keys ..we will pluck one");
            leafPage.shiftKeysRight(0,1);
            leafPage.shiftRidsRight(0,1);
            leafPage.setKey(0,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
            leafPage.setnKeys(leafPage.getnKeys() + 1);
            //leftLeafPage.setKey(leftLeafPage.getnKeys() - 1,NULL);
            leftLeafPage.setRid(leftLeafPage.getnKeys() - 1,dummyRid);
            leftLeafPage.setnKeys(leftLeafPage.getnKeys() -1);
            debugDeletion("replacing in the parent");
            if(leafPage.getParent() == leftLeafPage.getParent())
            {
                replacePosition = parentPage.getReplacePosition(leftLeafPage.getPageNumber());
                parentPage.replaceInIndexPage(replacePosition,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
                parentPage.indexNodeToPage();
                indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());

            }
            else
            {
                debugDeletion("updation in grand parents is needed when plucking from left sibling");
                bool successFlag = false;
                bptIndexPage leftSiblingParent(indexHeader);

                leftSiblingParent.setIndexNodePageData(indexFileIO->getPage(leftLeafPage.getParent(),indexPagePriority));
                leftSiblingParent.pageToIndexNode();
                successFlag = replaceInGrandParents(leftSiblingParent.getPageNumber(),leafPage.getKey(0),leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
                if(successFlag)
                debugDeletion("updation in grand parents is successfull");
                if(!successFlag)
                debugDeletion("upadation in grand parents is not success");
            }
            if(ledFlag)
            {
                debugDeletion("we need to update the parent of the current leaf..as last element is deleted before redistributing");
                replacePosition = parentPage.getReplacePosition(leafPage.getPageNumber());
                if(replacePosition != parentPage.getnKeys())
                {
                parentPage.replaceInIndexPage(replacePosition,leafPage.getKey(leafPage.getnKeys() - 1));
                parentPage.indexNodeToPage();
                indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());

                }
                else
                {
                    debugDeletion("we need to update the grand parents as the last element is deleted");
                    bool successFlag;
                    successFlag = replaceInGrandParents(parentPage.getPageNumber(),key,leafPage.getKey(leafPage.getnKeys() - 1));
                    if(successFlag)
                    debugDeletion("updation in grand parents is successfull");
                    if(!successFlag)
                    debugDeletion("upadation in grand parents is not success");

                }

            }
            leafPage.leafNodeToPage();
            indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());

            debugDeletion("exit redistributeLeafPage");
            leftLeafPage.leafNodeToPage();
            indexFileIO->writePage(leftLeafPage.getPageNumber(),leftLeafPage.getLeafNodePageData());
            //printBPT();
            return true;
        }

    }
    if(rightLeafPageNumber != -1)
    {
        debugDeletion("trying to resdistribute keys from right sibling leaf");
        bptLeafPage rightLeafPage(indexHeader);
        rightLeafPage.setLeafNodePageData(indexFileIO->getPage(rightLeafPageNumber,leafPagePriority));
        rightLeafPage.pageToLeafNode();
        if(rightLeafPage.getnKeys() > indexHeader.getMinKeysInLeafPage())
        {
            debugDeletion("right sibling has enough keys..we will get the first one..");
            debugDeletion("in this case..we no need to care about the ledFlag as we need to replace the key in the parent for sure");

            leafPage.setKey(leafPage.getnKeys(),rightLeafPage.getKey(0));
            leafPage.setnKeys(leafPage.getnKeys() + 1);
            //rightLeafPage.setKey(0,NULL);
            rightLeafPage.setRid(0,dummyRid);
            rightLeafPage.shiftKeysLeft(0);
            rightLeafPage.shiftRidsLeft(0);
            rightLeafPage.setnKeys(rightLeafPage.getnKeys() - 1);
          //  parentPage.printIndexPage();
            replacePosition = parentPage.getReplacePosition(leafPage.getPageNumber());
                debugKeyDel(indexHeader,leafPage.getKey(leafPage.getnKeys() - 1));
                debugDeletion("replacePosition"<<replacePosition);
            if(replacePosition < parentPage.getnKeys())
            {
                    debugDeletion("we need to replace in the parent as the child is not its last child");

                    parentPage.replaceInIndexPage(replacePosition,leafPage.getKey(leafPage.getnKeys() - 1));
                    parentPage.indexNodeToPage();
		            indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());
		            //parentPage.printIndexPage();

            }
            else
            {
                    debugDeletion("replacement in grand parents is needed");
                    replaceInGrandParents(parentPage.getPageNumber(),key,leafPage.getKey(leafPage.getnKeys() - 1));

            }
            leafPage.leafNodeToPage();
            indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());

            rightLeafPage.leafNodeToPage();
            indexFileIO->writePage(rightLeafPage.getPageNumber(),rightLeafPage.getLeafNodePageData());
            debugDeletion("exit redistributeLeafPage");
            //printBPT();
            return true;
        }


    }
    debugDeletion("redistribution from siblings is not success.");
    debugDeletion("exit redistributeLeafPage");
    //printBPT();
    return false;
}


//bool bpt::replaceInGrandParents(bptIndexPage parentPage,char* deletionKey,char* updationKey)
bool bpt::replaceInGrandParents(long int P_parentPageNumber,char* deletionKey,char* updationKey)
{
    debugDeletion("in replaceGrandParents");
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexPage parentPage(indexHeader);
    parentPage.setIndexNodePageData(indexFileIO->getPage(P_parentPageNumber,indexPagePriority));
    parentPage.pageToIndexNode();

    long int grandParentPageNumber,parentPageNumber;
    bool successFlag = false;
    parentPageNumber = parentPage.getPageNumber();
    grandParentPageNumber = parentPage.getParent();
    if(grandParentPageNumber == -1) return false;

    do
    {
        bptIndexPage grandParentPage(indexHeader);
        grandParentPage.setIndexNodePageData(indexFileIO->getPage(grandParentPageNumber,indexPagePriority));
        grandParentPage.pageToIndexNode();

        if(grandParentPage.findExistence(deletionKey))
        {
       /*     int position,childID;
            position = grandParentPage.findKeyPosition(deletionKey);
            childID =
            if(position == -1)
            debugDeletion("exists but position is not set as proper position");
            grandParentPage.replaceInIndexPage(position,updationKey);
            grandParentPage.indexNodeToPage();
            indexFileIO->writePage(grandParentPage.getPageNumber(),grandParentPage.getIndexNodePageData());
            successFlag = true;*/
            int childID;
            childID = grandParentPage.getChildID(parentPageNumber);
            for(int i = 0; i < grandParentPage.getnKeys() ; i++ )
            {
                if(i == childID && bptUtility::getBptUtilityInstance()->keyCompare(indexHeader,deletionKey,grandParentPage.getKey(i)) == 0)
                {   grandParentPage.replaceInIndexPage(i,updationKey);
                    grandParentPage.indexNodeToPage();
                    indexFileIO->writePage(grandParentPage.getPageNumber(),grandParentPage.getIndexNodePageData());
                    successFlag = true;

                }
            }






        }
        if(grandParentPage.getLevel() == indexHeader.getnLevels() - 1)
        {   debugDeletion("exit from replaceGrandParents");
            debugDeletion("root is reached but updation is not yet done..also key is not found in the root even");
            return false;
        }

    grandParentPageNumber = grandParentPage.getParent();
    parentPageNumber = grandParentPage.getPageNumber();
    }while(!successFlag);
     debugDeletion("exit from replaceGrandParents");
     //printBPT();
    return true;


}

//bool bpt::mergeLeafPage(bptIndexPage &parentPage,bptLeafPage &leafPage,char* key,bool ledFlag)
bool bpt::mergeLeafPage(long int parentPageNumber,long int leafPageNumber,char* key,bool ledFlag)
{
    debugDeletion("entered mergeLeafPage");

    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexPage parentPage(indexHeader);
    bptLeafPage leafPage(indexHeader);

    parentPage.setIndexNodePageData(indexFileIO->getPage(parentPageNumber,indexPagePriority));
    parentPage.pageToIndexNode();

    leafPage.setLeafNodePageData(indexFileIO->getPage(leafPageNumber,leafPagePriority));
    leafPage.pageToLeafNode();



    long int leftLeafPageNumber,rightLeafPageNumber;
    bptLeafPage leftLeafPage(indexHeader),rightLeafPage(indexHeader);
    leftLeafPageNumber = leafPage.getLeft();
    rightLeafPageNumber = leafPage.getRight();


    if( leafPage.getnKeys() != indexHeader.getMinKeysInLeafPage()-1)
    {
        debugDeletion("leafPage has got short by more than one key..is it got bulk deleted??");
        return false;
    }
    if(leftLeafPageNumber == -1 && rightLeafPageNumber == -1)
    {
    debugDeletion("there is no single sibling..is it root?");
    return false;
    }


    if(leftLeafPageNumber != -1)
    {
        debugDeletion("leftLeafPage exists");
    leftLeafPage.setLeafNodePageData(indexFileIO->getPage(leftLeafPageNumber,leafPagePriority));
    leftLeafPage.pageToLeafNode();

    }
    if(rightLeafPageNumber != -1)
    {
            debugDeletion("rightLeafPage exists");
            rightLeafPage.setLeafNodePageData(indexFileIO->getPage(rightLeafPageNumber,leafPagePriority));
            rightLeafPage.pageToLeafNode();
    }

    int numKeysInLeaf = leafPage.getnKeys();
    if(rightLeafPageNumber != -1)
    {
            debugDeletion("right leaf page exists..try to merge with it");
            int numKeysInRightLeaf = rightLeafPage.getnKeys();
            int numFreeKeysInRightLeaf = indexHeader.getMaxKeysInLeafPage() - numKeysInRightLeaf;
            if(numFreeKeysInRightLeaf >= numKeysInLeaf)
            {
                debugDeletion("enough vacancy in the right leaf");
                int tempnKeys = leafPage.getnKeys();
                rightLeafPage.setLeft(leafPage.getLeft());
                leftLeafPage.setRight(leafPage.getRight());
                rightLeafPage.shiftKeysRight(0,numKeysInLeaf);
                rightLeafPage.shiftRidsRight(0,numKeysInLeaf);

                for(int i = 0;i < tempnKeys;i++)
                {
                    rightLeafPage.setKey(i,leafPage.getKey(i));
                    rightLeafPage.setRid(i,leafPage.getRid(i));
                    rightLeafPage.setnKeys(rightLeafPage.getnKeys() + 1);
                    leafPage.setnKeys(leafPage.getnKeys() - 1);
                }


                if(ledFlag && leafPage.getParent() != rightLeafPage.getParent())
                    replaceInGrandParents(parentPage.getPageNumber(),key,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
    //            if(ledFlag && leafPage.getParent() == rightLeafPage.getParent())
      //              replaceInGrandParents(parentPage,rightLeafPage.getKey(numKeysInLeaf - 1),leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));

            leftLeafPage.leafNodeToPage();
            rightLeafPage.leafNodeToPage();
            indexFileIO->writePage(leftLeafPage.getPageNumber(),leftLeafPage.getLeafNodePageData());
            indexFileIO->writePage(rightLeafPage.getPageNumber(),rightLeafPage.getLeafNodePageData());
            leafPage.leafNodeToPage();
            indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());
           // parentPage.indexNodeToPage();
           //indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());

            deleteFromIndexPage(parentPage.getPageNumber(),leafPage.getPageNumber());
            debugDeletion("success in mergeing to right sibling leaf");
            debugDeletion("exit from mergeLeafPage");
            //printBPT();
            return true;
            }
    }

    if(leftLeafPageNumber != -1)
    {
        debugDeletion("left leaf page exists and trying to merge into left leafPage completly")
        int numFreeKeysInLeftLeaf = indexHeader.getMaxKeysInLeafPage() - leftLeafPage.getnKeys();

        if(numFreeKeysInLeftLeaf >= numKeysInLeaf)
        {

            debugDeletion("merge with left sibling is possible..as it is having enough vacancies");
            int tempnKeys = leafPage.getnKeys();
            char* keyToBeReplaced = leftLeafPage.getKey(leftLeafPage.getnKeys() - 1);

            for(int i = 0; i < tempnKeys;i++)
            {
                leftLeafPage.setKey(leftLeafPage.getnKeys(),leafPage.getKey(i));
                leftLeafPage.setRid(leftLeafPage.getnKeys(),leafPage.getRid(i));
                leftLeafPage.setnKeys(leftLeafPage.getnKeys() + 1);
                leafPage.setnKeys(leafPage.getnKeys() - 1);
            }
            debugDeletion("done with the transfer of keys and rids to the left sibling");

            leftLeafPage.setRight(leafPage.getRight());
            rightLeafPage.setLeft(leafPage.getLeft());
            if(leftLeafPage.getParent() == leafPage.getParent())
            {
                    debugDeletion("ledFlag"<<ledFlag);
                    debugKeyDel(indexHeader,key);
                    debugKeyDel(indexHeader,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
                if(leafPage.getPageNumber() == parentPage.getLastChildPageNumber())
                {

                    if(ledFlag)
                    replaceInGrandParents(parentPage.getPageNumber(),key,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));


                }
                else
                {
                debugDeletion("numKeysInLeaf"<<numKeysInLeaf);

                int position = parentPage.getChildID(leftLeafPage.getPageNumber());
                parentPage.replaceInIndexPage(position,leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));
                 parentPage.indexNodeToPage();
                indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());

                debugDeletion("done with replacing in the direct parent");

                }
            }
            else
            {
                bool successFlag = false;

                bptIndexPage leftSiblingParent(indexHeader);
                leftSiblingParent.setIndexNodePageData(indexFileIO->getPage(leftLeafPage.getParent(),indexPagePriority));
                leftSiblingParent.pageToIndexNode();
                successFlag = replaceInGrandParents(leftSiblingParent.getPageNumber(),leftLeafPage.getKey(leftLeafPage.getnKeys() -numKeysInLeaf - 1),leftLeafPage.getKey(leftLeafPage.getnKeys() - 1));

                if(successFlag)
                debugDeletion("success with replacing in the grand parents");
                if(!successFlag)
                debugDeletion("failed replacing in the grand parents");
            }



        bool successFlag = false;
        leftLeafPage.leafNodeToPage();
        rightLeafPage.leafNodeToPage();
        indexFileIO->writePage(leftLeafPage.getPageNumber(),leftLeafPage.getLeafNodePageData());

        if(rightLeafPageNumber != -1)
        indexFileIO->writePage(rightLeafPage.getPageNumber(),rightLeafPage.getLeafNodePageData());

        leafPage.leafNodeToPage();
        indexFileIO->writePage(leafPage.getPageNumber(),leafPage.getLeafNodePageData());

       /* parentPage.indexNodeToPage();
        indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());*/

        successFlag = deleteFromIndexPage(parentPage.getPageNumber(),leafPage.getPageNumber());
        //printBPT();
        return true;
        }
    }
        debugDeletion("exit from mergeLeafPage");
        //printBPT();
        return false;
}

//template <typename pageType>

//bool bpt::deleteFromIndexPage(bptIndexPage &parentPage,pageType &page)
bool bpt::deleteFromIndexPage(long int parentPageNumber,long int childPageNumber)
{
    debugDeletion("entered deleteFromIndexPage");

    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexPage parentPage(indexHeader);


    parentPage.setIndexNodePageData(indexFileIO->getPage(parentPageNumber,indexPagePriority));
    parentPage.pageToIndexNode();

    int deletePosition;

    deletePosition = parentPage.getChildID(childPageNumber);
    parentPage.setChild(deletePosition,-1);//freeing a page
   // page.setPageNumber(-1);
    if(deletePosition != parentPage.getnKeys())
        { //  parentPage.setKey(deletePosition,NULL);
            //delete[] parentPage.getKey(deletePosition);

        }
    parentPage.shiftKeysLeft(deletePosition);
    parentPage.shiftChildrenLeft(deletePosition);

    parentPage.setnKeys(parentPage.getnKeys() - 1);



    if((parentPage.getNumberOfChildren() == 1) && (parentPage.getLevel() == (indexHeader.getnLevels()-1)))
    {
        indexHeader.setRootPageNumber(parentPage.getChild(0));
        indexHeader.setnLevels(indexHeader.getnLevels() - 1);
        indexHeader.objToIndexHeaderPage();
        indexFileIO->writePage(indexHeader.getIndexHeaderPageNumber(),indexHeader.getIndexHeaderPageData());
        bptIndexPage newRootPage(indexHeader);
        newRootPage.setIndexNodePageData(indexFileIO->getPage(parentPage.getChild(0),rootPagePriority));
        newRootPage.pageToIndexNode();
      //  newRootPage.printIndexPage();
        newRootPage.setParent(-1);
        newRootPage.setPriority(rootPagePriority);
        newRootPage.indexNodeToPage();
        indexFileIO->writePage(newRootPage.getPageNumber(),newRootPage.getIndexNodePageData());
        debugDeletion("root node got deleted");
        //printBPT();
        return true;


    }
      parentPage.indexNodeToPage();
    indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());

    if(parentPage.getNumberOfChildren() < indexHeader.getMinChildrenInIndexPage())
    {
        debugDeletion("parent page is out of fanout...either we should merge or redistribute this index page");
        bool redistributionSuccess = false;
        redistributionSuccess = redistributeIndexPage(parentPage.getPageNumber());
        if(!redistributionSuccess)
            mergeIndexPage(parentPage.getPageNumber());

    }
        debugDeletion("exit from deleteFromIndexPage");
        //printBPT();
        return true;

}

//bool bpt::redistributeIndexPage(bptIndexPage &indexPage)

bool bpt::redistributeIndexPage(long int indexPageNumber)
{


    debugDeletion("entered redistributeIndexPage");
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();

    bptIndexPage indexPage(indexHeader);

    indexPage.setIndexNodePageData(indexFileIO->getPage(indexPageNumber,indexPagePriority));
    indexPage.pageToIndexNode();


    long int leftSiblingPageNumber,rightSiblingPageNumber;
    bptIndexPage indexPageLeftSibling(indexHeader),indexPageRightSibling(indexHeader);
    leftSiblingPageNumber = indexPage.getLeftSiblingPageNumber(indexHeader);
    rightSiblingPageNumber = indexPage.getRightSiblingPageNumber(indexHeader);




    if(rightSiblingPageNumber == -1) debugDeletion("there is no right sibling");
    if(leftSiblingPageNumber == -1 ) debugDeletion("there is no left sibling");




    if(leftSiblingPageNumber != -1)
    {

        debugDeletion("leftSibling exists and we will check whether has enough keys to redistribute"<<leftSiblingPageNumber);
        indexPageLeftSibling.setIndexNodePageData(indexFileIO->getPage(leftSiblingPageNumber,indexPagePriority));
        indexPageLeftSibling.pageToIndexNode();

        if(indexPageLeftSibling.getNumberOfChildren() > indexHeader.getMinChildrenInIndexPage())
        {
            debugDeletion("left sibling has enough number of children..we will get the last child  to the current imdex page");
            bptIndexPage parentIndexPage(indexHeader);
            parentIndexPage.setIndexNodePageData(indexFileIO->getPage(indexPage.getParent(),indexPagePriority));
            parentIndexPage.pageToIndexNode();
            indexPage.shiftKeysRight(0,1);
            indexPage.shiftChildrenRight(0,1);
            int childID;
            childID = parentIndexPage.getChildID(indexPage.getPageNumber());
            indexPage.setKey(0,parentIndexPage.getKey(childID - 1));
            indexPage.setChild(0,indexPageLeftSibling.getChild(indexPageLeftSibling.getnKeys()));
            parentIndexPage.setKey(childID - 1,indexPageLeftSibling.getKey(indexPageLeftSibling.getnKeys()-1));
            indexPage.setnKeys(indexPage.getnKeys() + 1);
            indexPageLeftSibling.setnKeys(indexPageLeftSibling.getnKeys() - 1);
            updateParent(indexPage.getChild(0),indexPage.getPageNumber(),indexPage.getLevel());
            parentIndexPage.indexNodeToPage();
            indexFileIO->writePage(parentIndexPage.getPageNumber(),parentIndexPage.getIndexNodePageData());
            indexPageLeftSibling.indexNodeToPage();
            indexFileIO->writePage(indexPageLeftSibling.getPageNumber(),indexPageLeftSibling.getIndexNodePageData());
            indexPage.indexNodeToPage();
            indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());
            debugDeletion("done with redistribute with left sibling");
            debugDeletion("exit from redistributeIndexPage");
            //printBPT();
            return true;
        }
    }
    if(rightSiblingPageNumber != -1)
    {
        debugDeletion("right sibling exists..we try to redistribute from it");
        indexPageRightSibling.setIndexNodePageData(indexFileIO->getPage(rightSiblingPageNumber,indexPagePriority));
        indexPageRightSibling.pageToIndexNode();

        debugDeletion(indexPageRightSibling.getNumberOfChildren());

        debugDeletion(indexHeader.getMinChildrenInIndexPage());
        if(indexPageRightSibling.getNumberOfChildren() > indexHeader.getMinChildrenInIndexPage())
        {
            debugDeletion("right sibling has enough keys..we take one from it");
            bptIndexPage parentIndexPage(indexHeader);
            parentIndexPage.setIndexNodePageData(indexFileIO->getPage(indexPage.getParent(),indexPagePriority));
            parentIndexPage.pageToIndexNode();
            int childID;
            childID = parentIndexPage.getChildID(indexPage.getPageNumber());
            indexPage.setKey(indexPage.getnKeys(),parentIndexPage.getKey(childID));
            indexPage.setnKeys(indexPage.getnKeys() + 1);
            parentIndexPage.setKey(childID,indexPageRightSibling.getKey(0));
            indexPage.setChild(indexPage.getnKeys(),indexPageRightSibling.getChild(0));
            indexPageRightSibling.shiftKeysLeft(0);
            indexPageRightSibling.shiftChildrenLeft(0);
            indexPageRightSibling.setnKeys(indexPageRightSibling.getnKeys() - 1);
            updateParent(indexPage.getChild(indexPage.getnKeys()),indexPage.getPageNumber(),indexPage.getLevel());
            parentIndexPage.indexNodeToPage();
            indexFileIO->writePage(parentIndexPage.getPageNumber(),parentIndexPage.getIndexNodePageData());
            indexPageRightSibling.indexNodeToPage();
            indexFileIO->writePage(indexPageRightSibling.getPageNumber(),indexPageRightSibling.getIndexNodePageData());
            indexPage.indexNodeToPage();
            indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());

            debugDeletion("done with redistribute with right sibling");
            debugDeletion("exit from redistributeIndexPage");
            //printBPT();
            return true;

        }
    }

    debugDeletion("redistribution with index pages is not success");
    //printBPT();
    return false;
}

//void  bpt::mergeIndexPage(bptIndexPage &indexPage)
void  bpt::mergeIndexPage(long int indexPageNumber)
{


    debugDeletion("entered mergeIndexPage");
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    bptIndexPage indexPage(indexHeader);

    indexPage.setIndexNodePageData(indexFileIO->getPage(indexPageNumber,indexPagePriority));
    indexPage.pageToIndexNode();

    long int rightSiblingPageNumber,leftSiblingPageNumber,parentPageNumber;

    rightSiblingPageNumber = indexPage.getRightSiblingPageNumber(indexHeader);
    leftSiblingPageNumber = indexPage.getLeftSiblingPageNumber(indexHeader);
    parentPageNumber = indexPage.getParent();
    int numChildrenInIndexPage = indexPage.getNumberOfChildren();
    bptIndexPage parentPage(indexHeader);
    parentPage.setIndexNodePageData(indexFileIO->getPage(parentPageNumber,indexPagePriority));
    parentPage.pageToIndexNode();
    if(leftSiblingPageNumber != -1)
    {
        debugDeletion("left Sibling exists..we will try to merge it");
        int numFreeSlotsInLeftSibling;
        bptIndexPage indexPageLeftSibling(indexHeader);
        indexPageLeftSibling.setIndexNodePageData(indexFileIO->getPage(leftSiblingPageNumber,indexPagePriority));
        indexPageLeftSibling.pageToIndexNode();
        numFreeSlotsInLeftSibling = indexHeader.getMaxChildrenInIndexPage() - indexPageLeftSibling.getNumberOfChildren();

        if(numFreeSlotsInLeftSibling >= numChildrenInIndexPage)
        {
            debugDeletion("left sibling has enough free slots,try to merge with it");

            if(indexPage.getLevel() == 1)
            {
                bptLeafPage leftSiblingLastChildPage(indexHeader);
                leftSiblingLastChildPage.setLeafNodePageData(indexFileIO->getPage(indexPageLeftSibling.getLastChildPageNumber(),indexPagePriority));
                leftSiblingLastChildPage.pageToLeafNode();
                setPropagatingElement(new char[indexHeader.getKeySize()]);
                memset(getPropagatingElement(),0,indexHeader.getKeySize());
                memcpy(getPropagatingElement(),leftSiblingLastChildPage.getKey(leftSiblingLastChildPage.getnKeys() - 1),indexHeader.getKeySize());
               // propagatingElement = leftSiblingLastChildPage.getKey(leftSiblingLastChildPage.getnKeys() - 1);
                indexPageLeftSibling.setKey(indexPageLeftSibling.getnKeys(),propagatingElement);
              //  delete [] propagatingElement;
                indexPageLeftSibling.setnKeys(indexPageLeftSibling.getnKeys() + 1);

                int k = indexPageLeftSibling.getnKeys();
                for(int i = 0; i <= indexPage.getnKeys();i++)
                {
                        indexPageLeftSibling.setChild(k,indexPage.getChild(i));
                        updateParent(indexPage.getChild(i),indexPageLeftSibling.getPageNumber(),indexPageLeftSibling.getLevel());
                        k++;
                }
                debugDeletion("done with copy of children to the left sibling index page and also parents are updated");


                for(int i = 0; i <indexPage.getnKeys();i++)
                {
                    indexPageLeftSibling.setKey(indexPageLeftSibling.getnKeys(),indexPage.getKey(i));
                    indexPageLeftSibling.setnKeys(indexPageLeftSibling.getnKeys() + 1);

                }
                debugDeletion("done with copy of keys and also number of keys in the left sibling is updated");
            indexPageLeftSibling.indexNodeToPage();
            indexFileIO->writePage(indexPageLeftSibling.getPageNumber(),indexPageLeftSibling.getIndexNodePageData());
         /*   indexPage.indexNodeToPage();
            indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());*/

            deleteFromIndexPage(parentPage.getPageNumber(),indexPage.getPageNumber());

            }
            else
            {

                int childID = parentPage.getChildID(indexPage.getPageNumber());
                int k;
                indexPageLeftSibling.setKey(indexPageLeftSibling.getnKeys(),parentPage.getKey(childID-1));
                indexPageLeftSibling.setnKeys(indexPageLeftSibling.getnKeys() + 1);
                k = indexPageLeftSibling.getnKeys();
                for(int i = 0; i < indexPage.getnKeys(); i++)
                {
                    indexPageLeftSibling.setKey(k,indexPage.getKey(i));
                    k++;
                }
                k = indexPageLeftSibling.getnKeys();
                for(int i = 0;i <= indexPage.getnKeys();i++)
                {
                    indexPageLeftSibling.setChild(k,indexPage.getChild(i));
                    updateParent(indexPage.getChild(i),indexPageLeftSibling.getPageNumber(),indexPage.getLevel());
                    k++;

                }
                indexPageLeftSibling.setnKeys(indexPageLeftSibling.getnKeys() + indexPage.getnKeys());
                indexPageLeftSibling.indexNodeToPage();
                indexFileIO->writePage(indexPageLeftSibling.getPageNumber(),indexPageLeftSibling.getIndexNodePageData());
             /*   indexPage.indexNodeToPage();
                indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());*/

                deleteFromIndexPage(parentPage.getPageNumber(),indexPage.getPageNumber());

            }

        }
    }

     if(rightSiblingPageNumber != -1)
    {

         debugDeletion("right sibling index page exists")
         int numFreeSlotsInRightSibling;
        bptIndexPage indexPageRightSibling(indexHeader);
        indexPageRightSibling.setIndexNodePageData(indexFileIO->getPage(rightSiblingPageNumber,indexPagePriority));
        indexPageRightSibling.pageToIndexNode();
        numFreeSlotsInRightSibling = indexHeader.getMaxChildrenInIndexPage() - indexPageRightSibling.getNumberOfChildren();

        if(numFreeSlotsInRightSibling >= indexPage.getNumberOfChildren())
        {
            if(indexPage.getLevel() == 1)
            {
                debugDeletion("Level 1:right sibling has enough free slots");
                debugDeletion("right sibling before merging");
            //    indexPageRightSibling.printIndexPage();

                indexPageRightSibling.shiftKeysRight(0,indexPage.getNumberOfChildren());
                bptLeafPage indexPageLastChild(indexHeader);
                indexPageLastChild.setLeafNodePageData(indexFileIO->getPage(indexPage.getLastChildPageNumber(),indexPagePriority));
                indexPageLastChild.pageToLeafNode();
                setPropagatingElement(new char[indexHeader.getKeySize()]);
                memset(getPropagatingElement(),0,indexHeader.getKeySize());
                memcpy(getPropagatingElement(),indexPageLastChild.getKey(indexPageLastChild.getnKeys() - 1),indexHeader.getKeySize());
//                propagatingElement = indexPageLastChild.getKey(indexPageLastChild.getnKeys() - 1);
                indexPageRightSibling.shiftKeysRight(0,indexPage.getNumberOfChildren());
                indexPageRightSibling.shiftChildrenRight(0,indexPage.getNumberOfChildren());

                for(int i = 0; i < indexPage.getnKeys(); i++ )
                {
                    indexPageRightSibling.setKey(i,indexPage.getKey(i));

                }
                indexPageRightSibling.setKey(indexPage.getnKeys(),propagatingElement);
            //   delete[] propagatingElement;

                for(int i = 0; i <= indexPage.getnKeys(); i++)
                {
                    bptLeafPage tempChild(indexHeader);
                    indexPageRightSibling.setChild(i,indexPage.getChild(i));
                    tempChild.setLeafNodePageData(indexFileIO->getPage(indexPage.getChild(i),leafPagePriority));
                    tempChild.pageToLeafNode();
             //       tempChild.printLeafPage();
                    updateParent(indexPage.getChild(i),indexPageRightSibling.getPageNumber(),indexPage.getLevel());

                    tempChild.setLeafNodePageData(indexFileIO->getPage(indexPage.getChild(i),indexPagePriority));
                    tempChild.pageToLeafNode();
             //       tempChild.printLeafPage();

                }

                indexPageRightSibling.setnKeys(indexPageRightSibling.getnKeys() + indexPage.getnKeys() + 1);
                indexPage.setnKeys(0);
                indexPage.indexNodeToPage();
                indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());

                indexPageRightSibling.indexNodeToPage();

                indexFileIO->writePage(indexPageRightSibling.getPageNumber(),indexPageRightSibling.getIndexNodePageData());
//indexPageRightSibling.printIndexPage();

                debugDeletion("writeSibling after merging");
          //  indexPageRightSibling.printIndexPage();
            debugDeletion("myself after merging:")
//indexPage.printIndexPage();
            deleteFromIndexPage(parentPage.getPageNumber(),indexPage.getPageNumber());

            }
            else
            {
                debugDeletion("right sibling before merging");
          //      indexPageRightSibling.printIndexPage();
                debugDeletion("myself before merging");
//indexPage.printIndexPage();
                debugDeletion("parentPage key:");

                int childID = parentPage.getChildID(indexPage.getPageNumber());
                indexPage.setKey(indexPage.getnKeys(),parentPage.getKey(childID));
          //      bptUtility::getBptUtilityInstance()->printKey(indexHeader,parentPage.getKey(childID));
                indexPage.setnKeys(indexPage.getnKeys() + 1);
                indexPageRightSibling.shiftKeysRight(0,indexPage.getnKeys());
                indexPageRightSibling.shiftChildrenRight(0,indexPage.getnKeys());
                for(int i = 0; i < indexPage.getnKeys(); i++)
                {
                    indexPageRightSibling.setKey(i,indexPage.getKey(i));
                    indexPageRightSibling.setChild(i,indexPage.getChild(i));
                    updateParent(indexPage.getChild(i),indexPageRightSibling.getPageNumber(),indexPage.getLevel());
                }
                indexPageRightSibling.setnKeys(indexPageRightSibling.getnKeys() + indexPage.getnKeys());
                indexPage.setnKeys(0);
                indexPageRightSibling.indexNodeToPage();
                debugDeletion("Higher Levels..right sibling has enough free slots");

            indexFileIO->writePage(indexPageRightSibling.getPageNumber(),indexPageRightSibling.getIndexNodePageData());
            indexPage.indexNodeToPage();
            indexFileIO->writePage(indexPage.getPageNumber(),indexPage.getIndexNodePageData());

            debugDeletion("writeSibling after merging");
          //  indexPageRightSibling.printIndexPage();
            debugDeletion("myself after merging:")
          //  indexPage.printIndexPage();
            deleteFromIndexPage(parentPage.getPageNumber(),indexPage.getPageNumber());
            }


        }



    }

  /*  if(leftSiblingPageNumber != -1 || rightSiblingPageNumber != -1)
    {

        if(parentPage.getPageNumber() != -1)
        {
        parentPage.indexNodeToPage();
        indexFileIO->writePage(parentPage.getPageNumber(),parentPage.getIndexNodePageData());
        }


    }*/

    debugDeletion("exit mergeIndexPage");

    //printBPT();
    return;





}





#endif // B+DELETION_HPP_INCLUDED
