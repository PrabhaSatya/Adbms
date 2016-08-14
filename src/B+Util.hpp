/*
 * B+Util.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: satya
 */

#ifndef B_UTIL_HPP_
#define B_UTIL_HPP_

#include"globals.hpp"
#include"debug.hpp"
#include "fileIO.hpp"

#include "B+IndexHeader.hpp"


using namespace std;

//singleton class
class bptUtility
{

	static bool bptUtilityInstanceFlag;
	static bptUtility *bptUtil;
	bptUtility()
	{

	}
public:

	static bptUtility* getBptUtilityInstance();
	bool checkKey(bptIndexHeader ih,char* key);
	int keyCompare(bptIndexHeader ih,char* key1,char* key2);
	template<typename pageType>
    int getInsertPosition(pageType page,char* key);
    template<typename pageType>
	int getSearchPosition(pageType page,char* key);
	void printKey(bptIndexHeader ih,char* key);
	bool isLeaf(long int pageNumber);
	void hexdump(char *tempBuffer);
	string numberToString(long number);
	long stringToNumber(string number);

	virtual ~bptUtility()
	{
		bptUtilityInstanceFlag = false;
		delete bptUtil;
	}
};

bool bptUtility::bptUtilityInstanceFlag = false;
bptUtility* bptUtility:: bptUtil = NULL;
bptUtility* bptUtility::getBptUtilityInstance()
{
	if(bptUtil == NULL)
	{
		bptUtil = new bptUtility();
		bptUtilityInstanceFlag = true;
		return bptUtil;
	}
	else return bptUtil;
}
bool bptUtility::isLeaf(long int pageNumber)
{
    char *buffer = new char[PAGESIZE];
    int offset = 0;
    int level;
    bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
    buffer = indexFileIO->getPage(pageNumber,defaultPriority);
    offset = sizeof(char) + sizeof(long int) + sizeof(long int) + sizeof (long int) + sizeof(int);
    memcpy(&level,&buffer[offset],sizeof(int));
    free(buffer);
    if(level == 0)
        return true;
    else
        return false;

}
void bptUtility::printKey(bptIndexHeader ih,char* key)
{

	int tempIntKey;
	string tempStringKey;
	long int tempLongIntKey;
	float tempFloatKey;
	double tempDoubleKey;

	switch(ih.getDataType())
	{
	case typeInt: 	        memcpy(&tempIntKey,&key[0],sizeof(int));
							cout<<tempIntKey<<"\t";
							break;
	case typeLong: 			memcpy(&tempLongIntKey,key,sizeof(long int));
							cout<<tempLongIntKey<<"\t";
							break;

	case typeFloat: 		memcpy(&tempFloatKey,key,sizeof(float));
							cout<<tempFloatKey<<"\t";
							break;

	case typeDouble: 		memcpy(&tempDoubleKey,key,sizeof(double));
							cout<<tempDoubleKey<<"\t";
							break;
	case typeString:	//	memcpy(&tempStringKey,key,sizeof(string));
							cout<<key<<"\t";
							break;

	default:        		cout<<"Error...with dataType."<<endl;
							break;

	}

}

long bptUtility::stringToNumber(string number) {
    long retNum=0;
    for(int i=number.size()-1;i>=0;i--) {
        retNum += (number[i]-48)*pow(10,((number.size()-1)-i));
    }
    return retNum;
}

string bptUtility::numberToString(long number) {
    string retNum;
    string tempStr;
    char tempChar;
    while(number!=0) {
        int tempNum = 0;
        tempNum=number%10;
        number=number/10;
        tempChar = tempNum+48;
        tempStr.append(1,tempChar);
    }
    for(int i=tempStr.size()-1;i>=0;i--) {
        tempChar = tempStr[i];
        retNum.append(1,tempChar);
    }
    return retNum;
}
void bptUtility::hexdump(char *tempBuffer) {
    cout << endl;
    unsigned char readChar;
    int iterator = 0, j = 0;
 //   char * tempBuffer = (char *)malloc(pageSize);
   // tempBuffer = getPage(pageNumber);
    for(int i=0;i<PAGESIZE;i++) {
        if(iterator==0) {
            printf("%04d : ", j);
        }
        readChar = tempBuffer[i];
        iterator++;
        j++;
        printf("%02x ", readChar);//%02x
        if (iterator == 16) {
            iterator = 0;
            cout<<endl;
        }
    }
}
bool bptUtility::checkKey(bptIndexHeader ih,char* key)
{

    int type;


    type = ih.getDataType();

    switch(type)
    {
        case typeInt:
                        {
                        char *buffer = new char[sizeof(int)];
                        int temp = -1;
                        memcpy(buffer,&temp,sizeof(int));
                        if(strcmp(key,"-1") == 0)
                        {
                            delete[] buffer;
                            return true;
                        }
                        else
                        {
                            delete[] buffer;
                            return false;
                        }
                        break;
                        }


        case typeString:
                        {
                        if(strcmp(key,"STOP") == 0)
                        {

                            return true;
                        }
                        else
                        {

                            return false;
                        }

                        }

                        break;
        default: break;

    }



}

int bptUtility::keyCompare(bptIndexHeader ih,char* key1,char* key2)
{

    int diff;
	switch(ih.getDataType())
	{
	case typeInt:
                            int tempIntKey1,tempIntKey2;
                            memcpy(&tempIntKey1,&key1[0],sizeof(int));
                            memcpy(&tempIntKey2,&key2[0],sizeof(int));
                            diff = tempIntKey1 - tempIntKey2;
                            break;



	case typeString:


                            //memcpy(&tempStringKey1,&key1[0],sizeof(string));
                           // memcpy(&tempStringKey2,&key2[0],sizeof(string));
                            diff = strcmp(key1,key2);
                            break;

	default:        		cout<<"Error...with dataType."<<endl;
							break;


    }
    return diff;

}
template<typename pageType>
int bptUtility::getInsertPosition(pageType page,char* key)
{
	debugInsertion("entered getInsertPosition");
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	bptIndexHeader tempIndexHeader;
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(page.getIndexHeaderPageNumber(),indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
	int i;
	for (i = 0; i < page.getnKeys(); i++)
	{
    if(keyCompare(tempIndexHeader,page.getKey(i),key) > 0)
        break;

	}

	debugInsertion("to be inserted at: "<< i);
	debugInsertion("exit from getInsertPosition");
	return i;

}

template<typename pageType>
int bptUtility::getSearchPosition(pageType page,char* key)
{
	debugSearch("entered getSearchPosition");
	bptIndexHeader tempIndexHeader;
	bptFileIO *indexFileIO = bptFileIO::getBptFileIOInstance();
	tempIndexHeader.setIndexHeaderPageData(indexFileIO->getPage(page.getIndexHeaderPageNumber(),indexHeaderPagePriority));
	tempIndexHeader.indexHeaderPageToObj();
	int i;
	for (i = 0; i < page.getnKeys(); i++)
	{
		if (keyCompare(tempIndexHeader,page.getKey(i),key) >= 0)
			break;
	}

	 return i;

}




/*
void bptUtility::roundTraverseBPT(bpt *tree)
{
	bptNodePtr initial;
	bptNodePtr currentNode;
	bptNodePtr temp;
	initial = tree->getRoot();
	currentNode = tree->getRoot();
	cout<<"moving in downward direction"<<endl;
	while(true)
	{

		for (int i = 0; i < currentNode->getnKeys(); i++)
			bptUtility::printKey(tree,currentNode->getKeys(i));
		cout<<endl;
		if(currentNode->getLevel() == 0)
			break;

		currentNode = currentNode->getChildren(0);
	}
	cout<<"moving horizontally...over leaves"<<endl;
	while(currentNode!= NULL)
	{
		for (int i = 0; i < currentNode->getnKeys(); i++)
			bptUtility::printKey(tree,currentNode->getKeys(i));
		cout<<endl;
		temp = currentNode;
		currentNode = currentNode->getRight();
	}
	cout<<"going in up direction"<<endl;
	currentNode = temp;
	while(currentNode != initial)
	{
		currentNode = currentNode->getParent();
		for (int i = 0; i < currentNode->getnKeys(); i++)
			bptUtility::printKey(tree,currentNode->getKeys(i));
		cout<<endl;
	}

}*/
#endif /* B_UTIL_HPP_ */

