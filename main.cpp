#include <iostream> // Enable standard input/output streams
#include <cpr/cpr.h> // Enable C++ web requests
#include <nlohmann/json.hpp> // Enable json parsing
#include <map> // Enable the standard map structure for data storage

// For convience
using json = nlohmann::json;

// Create a structure to store the raw data, with a member variable for each element
struct AllData {

	float consumedQuantity;
	float cost;

	std::string date;
	std::string instanceID;
	std::string meterCategory;
	std::string resourceGroup;
	std::string resourceLocation;

	std::map<std::string, std::string> tags; // Map structure passes two strings for the tag category and the entry
											 // e.g. 'app-name:' and 'Macao', 'environment:' and 'Test' etc.

	std::string unitOfMeasure;
	std::string location;
	std::string serviceName;
};

// Structures to store application and resource data.
// vector will store each application/resource as an individual piece of data that can be accessed.
struct ApplicationData {

	std::vector<std::string> list_apps;
};
struct ResourceData {

	std::vector<std::string> list_res; 
};

int main(int argc, char** argv) {

	// Output loading text while data is being processed
	std::string loadingText = "Loading...";
	std::cout << loadingText << std::endl;

	// Web requests
	cpr::Response r_raw = cpr::Get(cpr::Url{ "https://engineering-task.elancoapps.com/api/raw" });
	cpr::Response r_app = cpr::Get(cpr::Url{ "https://engineering-task.elancoapps.com/api/applications" });
	cpr::Response r_res = cpr::Get(cpr::Url{ "https://engineering-task.elancoapps.com/api/resources" });

	// Store the text from each URL in a json parser
	nlohmann::json rawData = nlohmann::json::parse(r_raw.text);
	nlohmann::json appData = nlohmann::json::parse(r_app.text);
	nlohmann::json resData = nlohmann::json::parse(r_res.text);

	// Create instances of the structures
	std::vector<AllData> dataSets;
	ApplicationData applications;
	ResourceData resources;

	try
	{
		for (auto tjsonobj : rawData)
		{
			// Temporary instance of structure for raw data
			AllData data;

			// Add the raw data from the json parser to the temporary instance, 
			// with each element corresponding to the appropriate member variable 
			data.consumedQuantity = stof(tjsonobj.at("ConsumedQuantity").get<std::string>());
			data.cost = stof(tjsonobj.at("Cost").get<std::string>());
			tjsonobj["Date"].get_to(data.date);
			tjsonobj["InstanceId"].get_to(data.instanceID);
			tjsonobj["MeterCategory"].get_to(data.meterCategory);
			tjsonobj["ResourceGroup"].get_to(data.resourceGroup);
			tjsonobj["ResourceLocation"].get_to(data.resourceLocation);
			tjsonobj.at("Tags").get_to(data.tags);
			tjsonobj["UnitOfMeasure"].get_to(data.unitOfMeasure);
			tjsonobj["Location"].get_to(data.location);
			tjsonobj["ServiceName"].get_to(data.serviceName);

			// Add the temporary instance to the vector
			dataSets.push_back(data);
		}

		// Add the data from the applications/resources json parser to the structures
		for (auto tjsonobj : appData)
		{
			applications.list_apps.push_back(tjsonobj);
		}
		for (auto tjsonobj : resData)
		{
			resources.list_res.push_back(tjsonobj);
		}

		bool isRunning = true;

		// Output a greetings and instructions message
		std::cout << "Welcome to this C++ application for data analysis!\n" << std::endl <<
			"Using one of the following keywords:" << std::endl
			<< " - applications" << std::endl << " - resources"<< std::endl << " - cost" << std::endl << " - consumed quantity" << std::endl;

		while (isRunning)
		{
			std::cout << "\nPlease enter the data you would like to view, or enter 'exit' when finished:..." << std::endl;
			
			// Enable user input 
			std::string userInput;
			std::getline(std::cin, userInput); 
			std::cout << "Entered: " << userInput << "...\n" << std::endl;
			
			if (userInput == "cost")
			{
				// Sort the raw data structure in descending order of cost
				std::sort(dataSets.begin(), dataSets.end(),
					[](const AllData a, const AllData b) {
					return a.cost > b.cost; });

				// Store the first 10 entries in a copy of the vector
				std::vector<AllData> dataSetsCopy(10);
				std::copy_n(dataSets.begin(), 10, dataSetsCopy.begin());

				std::cout << "The top 10 highest costing entries and corresponding applications:" << std::endl;

				for (auto titems : dataSetsCopy)
				{
					std::cout << titems.resourceGroup << ": " << titems.cost << std::endl;
				}
			}
			else if (userInput == "consumed quantity")
			{
				// Sort the raw data structure in descending order of consumed quantity
				std::sort(dataSets.begin(), dataSets.end(),
					[](const AllData a, const AllData b) {
					return a.consumedQuantity > b.consumedQuantity; });

				// Store the first 10 entries in a copy of the vector
				std::vector<AllData> dataSetsCopy(10);
				std::copy_n(dataSets.begin(), 10, dataSetsCopy.begin());

				std::cout << "The top 10 highest consuming entries and corresponding applications:" << std::endl;

				for (auto titems : dataSetsCopy)
				{
					std::cout << titems.resourceGroup << ": " << titems.consumedQuantity << std::endl;
				}
			}
			else if (userInput == "applications")
			{
				std::cout << "Applications list:" << std::endl;

				// Output all applications stored in the structure and the number of applications stored
				for (auto titems : applications.list_apps)
				{
					std::cout << titems << std::endl;
				}
				std::cout << "\n" << "Total number of applications: " << applications.list_apps.size() << std::endl;
			}
			else if (userInput == "resources")
			{
				// Output all resources stored in the structure and the number of resources stored
				std::cout << "Resources list:" << std::endl;

				for (auto titems : resources.list_res)
				{
					std::cout << titems << std::endl;
				}

				std::cout << "\n" << "Total number of resources: " << resources.list_res.size() << std::endl;
			}
			else if (userInput == "exit")
			{
				// Close the application
				isRunning = false;
				std::cout << "Application is now closing" << std::endl;
			}
			else
			{
				// Output error message
				std::cout << "Request not found" << std::endl;
			}
		}
	}
	catch (std::exception e)
	{
		// Output error message if application is not loaded correctly
		std::cout << e.what();
	}
	system("pause");
	return 0;
}
