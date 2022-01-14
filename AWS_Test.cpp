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

int addItem(Aws::DynamoDB::DynamoDBClient* dynamoClient)
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
    putItemRequest.SetTableName("Test");

    Aws::DynamoDB::Model::AttributeValue av;
    av.SetN(keyVal);
    Aws::DynamoDB::Model::AttributeValue yv;
    yv.SetN(yesVal);
    Aws::DynamoDB::Model::AttributeValue nv;
    nv.SetN(noVal);

    // Add all AttributeValue objects.
    putItemRequest.AddItem("Level", av);
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

int updateItem(Aws::DynamoDB::DynamoDBClient* dynamoClient) // this doesn't work and I don't know why
{
    const Aws::String key = "Level";
    const Aws::String keyVal = "1";

    char choice;
    std::cout << "Would you like to update yes or no ";
    std::cin >> choice;
    Aws::String choiceName;
    choiceName = std::tolower(choice);

    std::string x;
    std::cout << "Enter the value for the updated variable: ";
    std::cin >> x;
    const Aws::String choiceVal = (Aws::String)x;

    Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
    updateRequest.SetTableName("Test");

    Aws::DynamoDB::Model::AttributeValue av;
    av.SetN(keyVal);
    updateRequest.AddKey(key, av);

    Aws::DynamoDB::Model::AttributeValue cv;
    cv.SetN(choiceVal);

    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> expressionAttributeValues; // Construct some attribute values
    expressionAttributeValues[choiceName] = cv; // I don't know what needs to be assigned to the attribute values
    updateRequest.SetExpressionAttributeValues(expressionAttributeValues); // update request

    // ExpressionAttributeValues can only be specified when using expressions: UpdateExpression and ConditionExpression are null

    Aws::DynamoDB::Model::UpdateItemOutcome result = dynamoClient->UpdateItem(updateRequest);
    
    if (!result.IsSuccess())
    {
        std::cout << result.GetError().GetMessage() << std::endl;
        return 1;
    }
    std::cout << "Item was updated" << std::endl;
    return 0;
}

int getItem(Aws::DynamoDB::DynamoDBClient* dynamoClient)
{
	const Aws::String table = ("Test");
	const Aws::String key = ("Level");
	Aws::String x;
	std::cout << "Which level do you want to get the values for: ";
	std::cin >> x;
	const Aws::String keyval = ((Aws::String)x);

	Aws::DynamoDB::Model::GetItemRequest req;
	req.SetTableName(table);
	Aws::DynamoDB::Model::AttributeValue kv;
	kv.SetN(keyval);
	req.AddKey(key, kv);

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

int main(int argc, char** argv)
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.region = "eu-west-2";
	Aws::Auth::AWSCredentials credentials;
	credentials.SetAWSAccessKeyId(Aws::String("key")); // player credentials
	credentials.SetAWSSecretKey(Aws::String("secret key lmao"));
	Aws::DynamoDB::DynamoDBClient dynamoClient(credentials, clientConfig);

    while (true)
    {
        std::cout << help;
        int command;
        std::cout << "What to do? ";
        std::cin >> command;
        switch (command)
        {
            case 1:
                addItem(&dynamoClient);
                break;
            case 2:
                updateItem(&dynamoClient);
                break;
            case 3:
                getItem(&dynamoClient); // figure out pointers and references again
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
