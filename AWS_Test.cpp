#define _ITERATOR_DEBUG_LEVEL 0
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/UpdateItemResult.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/auth/auth.h>
#include <aws/auth/credentials.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

const std::string help =
    "1: Add Item\n"
    "2: Update Item\n"
    "3: Get Item\n"
    "4: Quit\n"
    "5: Literally just springs a memory leak because it's funny\n"
    "\n";

int addItem(Aws::DynamoDB::DynamoDBClient* dynamoClient, Aws::String* table, Aws::String* key)
{
    std::string x;
    std::cout << "What level number to do you want to update? ";
    std::cin >> x;
    const Aws::String keyVal = x;

    std::cout << "Enter the value you want to update for yes: ";
    std::cin >> x;
    const Aws::String yesVal = x;

    std::cout << "Enter the value you want to update for no: ";
    std::cin >> x;
    const Aws::String noVal = x;

    Aws::DynamoDB::Model::PutItemRequest putItemRequest;
    putItemRequest.SetTableName(*table);

    Aws::DynamoDB::Model::AttributeValue av;
    av.SetN(keyVal);
    Aws::DynamoDB::Model::AttributeValue yv;
    yv.SetN(yesVal);
    Aws::DynamoDB::Model::AttributeValue nv;
    nv.SetN(noVal);

    putItemRequest.AddItem(*key, av);
    putItemRequest.AddItem("yes", yv);
    putItemRequest.AddItem("no", nv);

    const Aws::DynamoDB::Model::PutItemOutcome result = dynamoClient->PutItem(putItemRequest);
    if (!result.IsSuccess())
    {
        std::cout << result.GetError().GetMessage() << std::endl;
        return 1;
    }
    std::cout << "Successfully added Item!" << std::endl;
    return 0;
}

int updateItem(Aws::DynamoDB::DynamoDBClient* dynamoClient, Aws::String* table, Aws::String* key) 
{
    Aws::String x;
    std::cout << "What level do you want to update? ";
    std::cin >> x;
    const Aws::String keyVal = x;

    int choice;
    std::cout << "Would you like to update yes(1) or no(anything != 1) ";
    std::cin >> choice;
    Aws::String choiceName;
    if (choice == 1) { choiceName = "yes"; }
    else { choiceName = "no"; }

    Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
    updateRequest.SetTableName(*table);

    Aws::DynamoDB::Model::AttributeValue av;
    av.SetN(keyVal);
    updateRequest.AddKey(*key, av);

    Aws::DynamoDB::Model::AttributeValue one;
    one.SetN(1);
    Aws::String incr = ":incr";

    Aws::String update_expression("SET #choiceName = #choiceName + :incr");
    updateRequest.SetUpdateExpression(update_expression);
    Aws::Map<Aws::String, Aws::String> stringMap;
    stringMap["#choiceName"] = choiceName;
    updateRequest.SetExpressionAttributeNames(stringMap);
    Aws::Map <Aws::String, Aws::DynamoDB::Model::AttributeValue> incrMap;
    incrMap[incr] = one;
    updateRequest.SetExpressionAttributeValues(incrMap);
    Aws::DynamoDB::Model::UpdateItemOutcome result = dynamoClient->UpdateItem(updateRequest);
    
    if (!result.IsSuccess())
    {
        std::cout << result.GetError().GetMessage() << std::endl;
        return 1;
    }
    std::cout << "Item was updated" << std::endl;
    return 0;
}

int getItem(Aws::DynamoDB::DynamoDBClient* dynamoClient, Aws::String* table, Aws::String* key)
{
	Aws::String x;
	std::cout << "Which level do you want to get the values for: ";
	std::cin >> x;
	const Aws::String keyval = ((Aws::String)x);

	Aws::DynamoDB::Model::GetItemRequest req;
	req.SetTableName(*table);
	Aws::DynamoDB::Model::AttributeValue kv;
	kv.SetN(keyval);
	req.AddKey(*key, kv);

	const Aws::DynamoDB::Model::GetItemOutcome& result = dynamoClient->GetItem(req);
	if (result.IsSuccess())
	{
		const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& item = result.GetResult().GetItem();
		if (item.size() > 0)
		{
			for (const auto& i : item)
				std::cout << "Values: " << i.first << ": " << i.second.GetN() << std::endl << "\n";
		}
		else
		{
			std::cout << "No item found with the key " << key << std::endl << "\n";
		}
	}
	else
	{
		std::cout << "Failed to get item: " << result.GetError().GetMessage() << "\n";
	}
    return 0;
}

void leak() // this is really funny
{
    while (true)
    {
        int* ptr = new int(20);
    }
}

/*
void setup() // This setup function requires a little bit of modification in order to work properly when compared to the rest of the script, and as a result I'm kinda just ignoring it
{
    int i = 1;
    while (i != 55)
    {
        string ii = to_string(i);
        Aws::String iStr = Aws::String(ii);
        addItem();
        i++;
        sleep_for(seconds(2));
    }
}
*/

/*
struct F {
    Aws::String a;
    Aws::Http::Scheme b;
    Aws::String c;
};
*/

int main(int argc, char** argv)
{
    std::cout << "Size of aws obj is " << sizeof(Aws::Client::ClientConfiguration) << "\n";
    std::cout << "Aws String size: " << sizeof(Aws::String) << " |  Aws String offset: " << offsetof(Aws::Client::ClientConfiguration, userAgent) << "\n";
    std::cout << "Aws http scheme size: " << sizeof(Aws::Http::Scheme) << " |  Aws http scheme offset: " << offsetof(Aws::Client::ClientConfiguration, scheme) << "\n";
    std::cout << "Aws String size: " << sizeof(Aws::String) << " |  Aws String offset: " << offsetof(Aws::Client::ClientConfiguration, region) << "\n";
    // return 0;

    Aws::SDKOptions options;
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration clientConfig;
	clientConfig.region = "eu-west-2";
	Aws::Auth::AWSCredentials credentials;
	credentials.SetAWSAccessKeyId(Aws::String("username")); // player credentials
	credentials.SetAWSSecretKey(Aws::String("password lol"));
	Aws::DynamoDB::DynamoDBClient dynamoClient(credentials, clientConfig);

	Aws::String table = ("Test");
	Aws::String key = ("Level");

    while (true)
    {
        std::cout << help;
        int command;
        std::cout << "What to do? ";
        std::cin >> command;
        switch (command)
        {
            case 1:
                addItem(&dynamoClient, &table, &key);
                break;
            case 2:
                updateItem(&dynamoClient, &table, &key);
                break;
            case 3:
                getItem(&dynamoClient, &table, &key); // figure out pointers and references again
                break;
            case 4:
                Aws::ShutdownAPI(options);
                return 0;
                break;
            case 5:
                leak ();
                break;
            default:
                std::cout << "Invalid command \n";
                break;
        }
    }
}
