#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

//A struct to hold information about a class
struct ClassEntry {
	//The subject and number. Ex: "ECE-211"
	string subjectNumber;
	//The course's name
	string title;
	//The number of credit hours the course is worth
	float numCredits;
	//The time the course starts. Ex: 1430 = 2:30 PM, 830 = 8:30 AM etc
	int startTime;
	//The time the course ends. Same format as startTime
	int endTime;
	//The days during which the course happens. Haven't decided on how to format this yet, just a string for now
	string days;
	
	//Constructor for ClassEntry
	ClassEntry(string subjectNumber, string title, float numCredits, int startTime, int endTime, string days) {
		//Could have alternatively used an initializer list
		this->subjectNumber = subjectNumber;
		this->title = title;
		this->numCredits = numCredits;
		this->startTime = startTime;
		this->endTime = endTime;
		this->days = days;
	}
	
	//Returns a string representation of the class - W.I.P.
	string toString() {
		string str = to_string(numCredits);
		//Erases everything from 1 right of the first non-matching character ('0' in this case) to the end (npos)
		//This will get rid of all the extra zeros. Side effect: if an integral number x is used, get "x.", not "x.0"
		str = str.erase(str.find_last_not_of('0') + 1, string::npos);
		//This fixes that integral number case.
		if(str[str.size()-1] == '.')
			str += '0';
		
		return "Class: " + subjectNumber + ", Class Name: " + title + ", Credit Hours: " + str + 
		", Start Time: " + to_string(startTime) + ", End Time: " + to_string(endTime) + ", Days: " + days;
	}
};

//Read in the data from the path and return a vector of ClassEntry's holding the data
vector<ClassEntry> readData(string path);

int main() {
	vector<ClassEntry> classes = readData("schedule.csv");
	cout << "Printing the data for each class" << '\n';
	
	for(int i = 0; i < classes.size(); i++) {
		cout << classes[i].toString() << '\n' << '\n';
	}
	return 0;
}

//Convert a time to an int. Time passed in should be of the form "12:30 AM", "2:45 PM" etc.
int timeToInt(string str) {
	int index = str.find(":");
	if(index == string::npos)
		return -1;
	bool isPM = str.find("PM") != string::npos || str.find("pm") != string::npos;
	if(index == 1)
		return stoi(str.substr(0, 1))*100 + 1200*isPM + stoi(str.substr(2, 2));
	//Need to check the 12:XX PM case, since at 12:XX PM we don't want to add 1200. This is a problem for 12:XX AM, but we don't care.
	return stoi(str.substr(0, 2))*100 + 1200*(isPM && str.substr(0,2)!="12") + stoi(str.substr(3, 2));
}

vector<ClassEntry> readData(string path) {
	//Container for all the classes
	vector<ClassEntry> classes;
	ifstream file;
	file.open(path);
	string line;
	int count = 0;
	//If file is not found, return an empty vector
	if(!file.is_open()) {
	   cout << "Unable to find file";
	   return classes;
	}
	while(getline(file, line)) {
		//This is for the purpose of avoiding the column titles of the csv file. Might remove this if we remove the titles
		count++;
		if(count==1)
			continue;
		//Create a string stream. This is useful because getline allows you to pass a delimiter to seperate the values
		stringstream ss(line);
		string value;
		vector<string> values;
		//Use ',' as the delimiter
		while(getline(ss, value, ',')) {
			//cout << value << '\n';
			values.push_back(value);
		}
		//cout << '\n';
		//Avoid any index out of bounds exceptions, was running into these for some reason
		if(!(values.size() > 5))
			continue;
		classes.push_back(ClassEntry(values[0], values[1], stof(values[2], NULL), timeToInt(values[3]), timeToInt(values[4]), values[5]));
	}
	file.close();
	return classes;
}