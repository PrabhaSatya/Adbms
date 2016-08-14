/*
 * userInterface.hpp
 *
 *  Created on: Dec 6, 2012
 *      Author: satya
 */

#ifndef USERINTERFACE_HPP_
#define USERINTERFACE_HPP_


#include "B+Tree.hpp"
#include "B+Insertion.hpp"
#include "B+Search.hpp"
#include "B+Deletion.hpp"
#include "B+Util.hpp"
#include "globals.hpp"


class userInterface
{

	public:
	int choice,typeChoice;
	int fanout;
	char* key;
	int dataType,keySize;
	long int searchResult;
	bpt *tree;
	string inputFilePath;
	string line;
	ifstream input,indexFile;
	vector<RID> searchData;
	bptUtility *bptUtil;
	bptFileIO *indexFileIO;
	bool indexCreated;
	RID rid;
	userInterface()
	{
		tree = new bpt();
		//Cache::getObj()->createCache (3, 129);
		indexFileIO = bptFileIO::getBptFileIOInstance();

		bptUtil = bptUtility::getBptUtilityInstance();
		indexCreated = false;
		rid.pageNumber = -1;
		rid.pageNumber = -1;
	}

	void start();
	void createIndex();
	void insertFileElements();
	void deleteFileElements();
	void insertElements();
	void insertIntElements();
	void insertStringElements();
	void searchElements();
	void searchIntElements();
	void searchStringElements();
	void deleteRange();
	void deleteElements();
	void deleteIntElements();
	void deleteStringElements();
	void printBPT();
	void deleteIndex();

	void printIndexHeader();
	void traverseBPT();
	void closeIndex();
	void openIndex();
	void insertRange();
	void setDebugFlags();
};
void userInterface::start()
{


		do
		    {
                cout<<"<========Enter your choice========>"<<endl;
		    	cout<<"Create Index                  : 1"<<endl;
		    	cout<<"Print Index Header            : 2"<<endl;
		    	cout<<"Open Existing Index           : 3"<<endl;
		    //	cout<<"Insert From file              : 4"<<endl;
		    	cout<<"Insert Elements               : 4"<<endl;
		    	cout<<"Insert Range of Elements      : 5"<<endl;
		    	cout<<"Search Elements               : 6"<<endl;
		    	cout<<"Delete Elements               : 7"<<endl;
		    	cout<<"Delete Range of Elements      : 8"<<endl;
		    	cout<<"Delete Index                  : 9"<<endl;
		    	cout<<"Print BPT                     : 10"<<endl;

		    	cout<<"Traverse BPT                  : 11"<<endl;
		    	cout<<"Close Index                   : 12"<<endl;
		    	cout<<"Set debug Flag                : 13"<<endl;
		    	cout<<"Quit                          : -1"<<endl;
		    	cout<<"<=================================>"<<endl;
		    	//cin.ignore(10000);
		    	cout<<"input>>>>";

		    	cin>>choice;
		    	switch(choice)
		    	{
		    	case -1:	cout<<"Exited"<<endl;
		    				break;
		    	case 1:		createIndex();
							break;
                case 2:		printIndexHeader();
		    				break;
                case 3:
                            openIndex();
                            break;
		    	/*case 4:		insertFileElements();
		    				break;*/
		    	case 4:		insertElements();
							break;
                case 5:
                            insertRange();
                            break;
		    	case 6:     searchElements();
							break;
                case 7:     deleteElements();
                            break;

		    	case 8:     deleteRange();
                            break;
		    	case 9:     deleteIndex();
		    				break;
		    	case 10:	printBPT();
							break;

		    	case 11:   	traverseBPT();
		    				break;
                case 12:    closeIndex();
                            break;

                case 13:    setDebugFlags();
                            break;
		    	default:
		    				cout<<"Improper choice,Enter valid input"<<endl;
							cout<<"input>>>>";
		    				cin>>choice;
		    				break;
		    	}
		    }while(choice != -1);
}

void userInterface::createIndex()
{

/*	cout<<"<========enter the type of the data to be indexed========>"<<endl;
	do
	{
		cout<<"Integers      :"<<typeInt<<endl;
		//cout<<"Longs         :"<<typeLong<<endl;
		//cout<<"Floats        :"<<typeFloat<<endl;
		//cout<<"Doubles       :"<<typeDouble<<endl;
		cout<<"Strings       :"<<typeString<<endl;
        cout<<"<========================================================>"<<endl;

		cout<<"input>>>>";

		cin>>typeChoice;
		//cin.ignore(10000);
		if(typeChoice == typeInt)
			{
			//	int inputKey,searchKey;
				dataType = typeInt;
				keySize = sizeof(int);
			}

		else if(typeChoice == typeLong)
			{
				dataType = typeLong;
				keySize = sizeof(long int);
			//	long int inputKey,searchKey;
			}
		else if(typeChoice == typeFloat)
			{
				 dataType = typeFloat;
				 keySize = sizeof(float);
				// float inputKey,searchKey;

			}
		else if(typeChoice == typeDouble)
			{
				dataType = typeDouble;
				keySize  =  sizeof(double);
				//double inputKey,searchKey;
			}

		else if (typeChoice == typeString)
			{
				dataType = typeString;
				keySize = sizeof(string);
				//string inputKey,searchKey;
			}
		else
            {   cout<<"Invalid choice. Enter proper choice"<<endl;
                cout<<"input>>>>";
            }

	}while(typeChoice != typeInt && typeChoice != typeLong && typeChoice != typeFloat && typeChoice != typeDouble && typeChoice != typeString);
*/
	cout<<"<====enter the fanout desired====>"<<endl;
	do
	{   cout<<"input>>>>";
        cin>>fanout;
        if(fanout < 3) cout<<"Enter a valid fanout"<<endl;
	}while(fanout < 3);



	tree->createIndex(fanout,typeInt,sizeof(int));
	indexCreated = true;
	cout<<"Index has been created successfully"<<endl;




}



void userInterface::insertFileElements()
{

	int inputKey;
	if(!indexCreated)
	{	cout<<"Sorry, Index has not been created,create it first"<<endl;
        return;
	}
	else
	{
	    rid.pageNumber = 0;
        rid.slotNumber = 0;

		cout<<"enter input File Path"<<endl;
		cin>>inputFilePath;

        input.open(inputFilePath.c_str(),ios::in);
		input>>inputKey;

		do
		{
			key = new char[sizeof(int)];
			memcpy(key,&inputKey,sizeof(int));
			rid.pageNumber = rid.pageNumber + 1;
            rid.slotNumber = rid.slotNumber + 1;

			tree->insertToBPT(tree->getIndexHeader().getRootPageNumber(),key,rid);
			//bptUtil->printBPT(tree);
			input>>inputKey;

		}while(input);

		 tree->getIndexHeader().objToIndexHeaderPage();
		 indexFileIO->writePage(tree->getIndexHeader().getIndexHeaderPageNumber(),tree->getIndexHeader().getIndexHeaderPageData());

	}
}

void userInterface::insertStringElements()
{

    string inputKey;

    cout<<"<====enter the keys to be inserted====>"<<endl;
    cout<<"input>>>>";
    cin.ignore(10000,'\n');
    cin>>inputKey;

    rid.pageNumber = 0;
    rid.slotNumber = 0;
    key = new char[inputKey.size()];
   // memcpy(key,&inputKey,string.size());
  /* for(int i = 0; i < inputKey.size(); i++ )
        key[i] = inputKey[i];*/
        strcpy(key,inputKey.c_str());

    while(!bptUtil->checkKey(tree->getIndexHeader(),key))
    {
        rid.pageNumber = rid.pageNumber + 1;
        rid.slotNumber = rid.slotNumber + 1;

        tree->insertToBPT(tree->getIndexHeader().getRootPageNumber(),key,rid);
        printBPT();

        cout<<"To stop inserting enter STOP "<<endl;
        cout<<"input>>>>";
        //cin.ignore(10000,'\n');
        cin>>inputKey;

       key = new char[inputKey.size()];
        // memcpy(key,&inputKey,string.size());
    /*    for(int i = 0; i < inputKey.size(); i++ )
            key[i] = inputKey[i];*/
    strcpy(key,inputKey.c_str());
    }

}
void userInterface::insertIntElements()
{


        int inputKey;

        int stopVariable = 0;

		cout<<"<====enter the keys to be inserted====>"<<endl;
		//cin.ignore(10000,'\n');
		//cin.ignore(10000);
		//key = new char[sizeof(int)];
		//memcpy(&key[0],&inputKey,sizeof(int));

        rid.pageNumber = 0;
        rid.slotNumber = 0;

		while(stopVariable == 0)
		{
            cout<<"input>>>>";
			cin>>inputKey;
		//	cin.ignore(10000,'\n');

			key = new char[sizeof(int)];
			memcpy(key,&inputKey,sizeof(int));

		    rid.pageNumber = rid.pageNumber + 1;
            rid.slotNumber = rid.slotNumber + 1;

            bool deleted = false;

			tree->insertToBPT(tree->getIndexHeader().getRootPageNumber(),key,rid);

            cout<<"Continue Inserting : 0"<<endl;
            cout<<"Stop Inserting     : 1"<<endl;
            cout<<">>>>";
            cin>>stopVariable;

        }
}

void userInterface::insertElements()
{

    int type;
    if(!indexCreated)
	{	cout<<"Sorry, Index has not been created,create it first"<<endl;
        return;
	}
	else
	{
	    type = tree->getIndexHeader().getDataType();
        switch(type)
        {
            case typeInt: insertIntElements();
                          break;

            case typeString: insertStringElements();
                                break;


        }
	}

}



void userInterface::deleteElements()
{


        int inputKey;

        int stopVariable = 0;

		cout<<"<====enter the keys to be deleted====>"<<endl;

		//cin.ignore(10000);
		//key = new char[sizeof(int)];
		//memcpy(&key[0],&inputKey,sizeof(int));


		while(stopVariable == 0)
		{
            cout<<"input>>>>";
			cin>>inputKey;
		//	cin.ignore(10000,'\n');

			key = new char[sizeof(int)];
			memcpy(key,&inputKey,sizeof(int));

		    rid.pageNumber = rid.pageNumber + 1;
            rid.slotNumber = rid.slotNumber + 1;

            bool deleted = false;

			deleted = tree->deleteFromBPT(tree->getIndexHeader().getRootPageNumber(),key,rid);
            if(!deleted)
                cout<<"element not found in the index to delete it"<<endl;

            //tree->printBPT();

            cout<<"Continue Deleting : 0"<<endl;
            cout<<"Stop Deleting     : 1"<<endl;
            cout<<">>>>";
            cin>>stopVariable;

        }
}


void userInterface::deleteFileElements()
{
    int inputKey;
    if(!indexCreated)
	{	cout<<"Sorry, Index has not been created,create it first"<<endl;

	}
	else
	{
		cout<<"enter input File Path"<<endl;
		cin>>inputFilePath;

        input.open(inputFilePath.c_str(),ios::in);
		input>>inputKey;
		key = new char[sizeof(int)];
		do
		{
            rid.pageNumber = rid.pageNumber + 1;
            rid.slotNumber = rid.slotNumber + 1;


			memcpy(key,&inputKey,sizeof(int));
			bptUtility::getBptUtilityInstance()->printKey(tree->getIndexHeader(),key);

			tree->deleteFromBPT(tree->getIndexHeader().getRootPageNumber(),key,dummyRid);
			tree->printBPT();
			input>>inputKey;


		}while(input);
		 tree->getIndexHeader().objToIndexHeaderPage();
		 indexFileIO->writePage(tree->getIndexHeader().getIndexHeaderPageNumber(),tree->getIndexHeader().getIndexHeaderPageData());
        input.close();
	}


}

void userInterface::deleteRange()
{

    string inputKey;
    int lowerLimit,upperLimit;

    int step;

  /*  cout<<"<====Enter the range of elements to be inserted====>"<<endl;
    cout<<"Consider the following pattern:"<<endl;
    cout<<"enter (x,y) to insert all the elements in between x and y, excluding x and y"<<endl;
    cout<<"enter [x,y) to insert all the elements in between x and y, including x, exluding y"<<endl;
    cout<<"enter (x,y] to insert all the elements in between x and y, excluding x, including y"<<endl;
    cout<<"enter [x,y] to insert all the elements in between x and y, including x, including y"<<endl;*/

    if(!indexCreated)
    {
        cout<<"Index does't exist"<<endl;
        return;
    }

    cout<<"<====Deleting Range Of Elements====>"<<endl;
    cout<<"<====Hint: Limits Are Inclusive=====>"<<endl;
    cout<<"Enter The Lower Limit"<<endl;
    cout<<"input>>>>";
    cin>>lowerLimit;

    cout<<"Enter the Upper Limit"<<endl;
    cout<<"input>>>>";
    cin>>upperLimit;

    cout<<"Enter Step Size"<<endl;
    cout<<"input>>>>";
    cin>>step;


    for(int i = lowerLimit; i <= upperLimit ; i = i + step)
    {
        key = new char[sizeof(int)];
        memcpy(key,&i,sizeof(int));

        searchResult=tree->searchBPT(tree->getIndexHeader().getRootPageNumber(),key);
		if(searchResult!=NOTFOUND)
		{	searchData = tree->getSearchData(searchResult,key);
			if(searchData.size() > 0)
			{
				for(int k = 0;k < searchData.size(); k++)
				{
				   // bptUtil->printKey(tree->getIndexHeader(),searchData.at(k));
				 //  cout<<searchData.at(k).pageNumber<<"\t"<<searchData.at(k).slotNumber<<endl;
				 tree->deleteFromBPT(tree->getIndexHeader().getRootPageNumber(),key,searchData.at(k));

				}

			//	cout<<endl<<"Number of elements in the index with the given search key is:"<<searchData.size()<<endl;
			}
			else
			{   bptUtil->printKey(tree->getIndexHeader(),key);
				cout<<"element not found in the index"<<endl;
			
			}

        }
    }
   // printBPT();

}
void userInterface::insertRange()
{

    string inputKey;
    int lowerLimit,upperLimit;

    int step;

  /*  cout<<"<====Enter the range of elements to be inserted====>"<<endl;
    cout<<"Consider the following pattern:"<<endl;
    cout<<"enter (x,y) to insert all the elements in between x and y, excluding x and y"<<endl;
    cout<<"enter [x,y) to insert all the elements in between x and y, including x, exluding y"<<endl;
    cout<<"enter (x,y] to insert all the elements in between x and y, excluding x, including y"<<endl;
    cout<<"enter [x,y] to insert all the elements in between x and y, including x, including y"<<endl;*/

    if(!indexCreated)
    {
        cout<<"Index is not created..create it first"<<endl;
        return;
    }

    cout<<"<====Inserting Range Of Elements====>"<<endl;
    cout<<"<====Hint: Limits Are Inclusive=====>"<<endl;
    cout<<"Enter The Lower Limit"<<endl;
    cout<<"input>>>>";
    cin>>lowerLimit;

    cout<<"Enter the Upper Limit"<<endl;
    cout<<"input>>>>";
    cin>>upperLimit;

    cout<<"Enter Step Size"<<endl;
    cout<<"input>>>>";
    cin>>step;


    for(int i = lowerLimit; i <= upperLimit ; i = i + step)
    {
        key = new char[sizeof(int)];
        memcpy(key,&i,sizeof(int));
        rid.pageNumber = rid.pageNumber + 1;
        rid.slotNumber = rid.slotNumber + 1;
        tree->insertToBPT(tree->getIndexHeader().getRootPageNumber(),key,rid);
       // printBPT();


    }
   // printBPT();
}
void userInterface::searchElements()
{

    int searchKey;
    int stopVariable = 0;
	cout<<"<====Enter the element to be searched====>"<<endl;
	while(stopVariable == 0)
	{

        cout<<">>>>";
        cin>>searchKey;
        //cin.ignore(10000);
        key = new char[sizeof(int)];
        memcpy(key,&searchKey,sizeof(int));


		searchResult=tree->searchBPT(tree->getIndexHeader().getRootPageNumber(),key);
		if(searchResult!=NOTFOUND)
		{	searchData = tree->getSearchData(searchResult,key);
			if(searchData.size() > 0)
			{
				for(int k = 0;k < searchData.size(); k++)
				{
				   // bptUtil->printKey(tree->getIndexHeader(),searchData.at(k));
				   cout<<searchData.at(k).pageNumber<<"\t"<<searchData.at(k).slotNumber<<endl;

				}

				cout<<endl<<"Number of elements in the index with the given search key is:"<<searchData.size()<<endl;
			}
			else cout<<"element not found in the index"<<endl;
		}
		else debugSearch("search Result returned -1 and something wrong");
		 cout<<"Continue Searching : 0"<<endl;
         cout<<"Stop Search        : 1"<<endl;
         cout<<">>>>";
        cin>>stopVariable;
        free(key);
	}
}
void userInterface::deleteIndex()
{
	tree->dropIndex();
	cout<<"created index is deleted...if you want create index again"<<endl;
}

void userInterface::printBPT()
{
	if(!indexCreated)
	{
		cout<<"No index to print"<<endl;

	}
	else
	{
		cout<<"printing elements in BPT"<<endl;
		tree->printBPT();
	}

}

void userInterface::printIndexHeader()
{
	tree->getIndexHeader().printIndexHeader();
}

void userInterface::traverseBPT()
{

	tree->traverseBPT();

}
void userInterface::closeIndex()
{
    tree->closeIndex();
}

void userInterface::openIndex()
{
    tree->openIndex();
}


void userInterface::setDebugFlags()
{

    int temp;
    do{
        cout<<"For debugging Insertion Press 1 Else Press 0"<<endl;
        cin>>temp;
        if(temp == 1)
        debug_Insertion = true;
        if(temp == 0)
        debug_Insertion = false;
      }while (temp != 0 && temp != 1);
    do{
        cout<<"For debugging Deletion Press 1 Else Press 0"<<endl;
        cin>>temp;
        if(temp == 1)
        debug_Deletion = true;
        if(temp == 0)
        debug_Deletion = false;
      }while (temp != 0 && temp != 1);
      do{
        cout<<"For debugging Search Press 1 Else Press 0"<<endl;
        cin>>temp;
        if(temp == 1)
        debug_Insertion = true;
        if(temp == 0)
        debug_Search = false;
      }while (temp != 0 && temp != 1);


}

#endif /* USERINTERFACE_HPP_ */

