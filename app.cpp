#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

//A struct to hold information about a class
struct ClassEntry {
	//The subject and number. Ex: "ECE-211"      //could be split up into dept & courseNum
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

void cmdInput(vector<ClassEntry> classes){
	string required[10];
	int i, totalReq;
	for(i = 0; i<10;i++){
		cout<<"Please enter a course you are required to take(0 if no more required): ";
		cin >> required[i];
		if(required[i]=="0"){
			i--;
			break;
		}
	}
	totalReq = i+1;
	cout<<totalReq<< " classes entered" << endl;
	
	cout<< "You have chosen the following classes: "<<endl;
	bool classExists;
	for(int j = 0; j<totalReq;j++){ //loop through required classes
		classExists = false;
		for(int k = 0; k<classes.size();k++){ //loop through offered classes
			//Check that class number aligns with a class
			if(required[j]==classes[k].subjectNumber){
				//print class name if yes
				classExists=true;
				break;
			}
		}
		if(classExists){
			cout<< required[j] << endl;
		}
		else{
			//print error if number doesn't exist
			cout<<"A class that isn't offered"<<endl;
		}
	}
}

bool hasSameDays(string Adays,string Bdays){ //checks if classes are offered on the same day
	bool match = false;
	for(int a = 0; Adays.substr(a,1) != ""; a++){ //fill daysA w/ the chars representing each day of class
		if(Bdays.find(Adays.substr(a,1)) != -1){
//			cout << Adays.substr(a,1) << " " << Bdays.substr(a,1) << match << endl;
			match = true;
//			cout << "during function: " << match << endl;
//			break;
		}
	}
	//cout << match << endl;
	return match;
}

bool isConflict(ClassEntry courseA, ClassEntry courseB){// ISSUE: ALWAYS RETURNING FALSE
	
	string Adays = courseA.days;
	string Bdays = courseB.days;
//	cout << "After function: " << endl << hasSameDays(Adays, Bdays) << endl;
	if(hasSameDays(Adays, Bdays)){// check if courses run on any of the same days
		if(courseA.startTime <= courseB.endTime && courseA.startTime >= courseB.startTime){ //check if courseA starts during courseB
			return true;
		}
		else if(courseA.endTime <= courseB.endTime && courseA.endTime >= courseB.startTime){ //check if courseA ends during courseB
			return true;
		}
//		cout<< "inside first if: " << courseA.startTime << endl;
	}
//	else{cout << "hasSameDays() returning false" << endl;}
	return false;
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

//Read a specific column from a CSV file. First column is column 0.
vector<string> readColumn(string path, int col) {
	vector<string> data;
	ifstream file;
	file.open(path);
	string line;
	if(!file.is_open())
		return data;
	//Skip first line (names of columns) by calling getline()
	getline(file, line);
	while(getline(file, line)) {
		stringstream ss(line);
		string value;
		int count = 0;
		//Go until the correct value is found
		while(getline(ss, value, ',') && count != col) {
			count++;
		}
		data.push_back(value);
	}
	file.close();
	return data;
}

//Overload of above method. Takes a column name and returns that column. Is case sensitive.
vector<string> readColumn(string path, string columnName) {
	ifstream file;
	file.open(path);
	string line;
	getline(file, line);
	int containsAt = line.find(columnName);
	//If the columnName isn't found, just return the first column (can't return nothing)
	if(containsAt == string::npos)
		return readColumn(path, 0);
	int col = 0;
	for(int i = 0; i < line.length(); i++) {
		if(line[i] == ',')
			col++;
		if(i >= containsAt)
			break;
	}
	file.close();
	return readColumn(path, col);
}

//Read in the data from the path and return a vector of ClassEntry's holding the data
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

bool checkSched(vector<ClassEntry> checkThis){//takes a combination of class sections, returns if it is valid
	//TODO: use isConflict a lot to check if there are any conflicts TODO: TEST THIS
	int cases = 0;
	for(int a = 0; a<checkThis.size();a++){
		for(int b = a+1; b<checkThis.size();b++){
			cases++;
			if(isConflict(checkThis[a], checkThis[b])){return false;}
		}
	}
//	cout << endl << "checked " << cases << " cases" << endl;
	return true;
}

//Helper function for findSchedules()
void findSchedulesHelper(vector<vector<ClassEntry>> selectedClasses, int col, vector<ClassEntry> build, vector<vector<ClassEntry>> &out) {
	//The base case, when a schedule has been generated
	if(col == selectedClasses.size())
		return;
	//Go through all the sections for this class
	for(int row = 0; row < selectedClasses[col].size(); row++) {
		build.push_back(selectedClasses[col][row]);
		//Check if we're on the last column (which would be the end of any given schedule)
		if(col == selectedClasses.size()-1) {
			//Append the schedule to the output
			out.push_back(build);
		}
		//Move to the next class (or column in this case)
		findSchedulesHelper(selectedClasses, col + 1, build, out);
		//Remove the last element to be replaced by the next iteration of this for loop
		build.pop_back();
	}
}

//Find all possible schedules from a 2D array of classes formated like arr[class][section]
void findSchedules(vector<vector<ClassEntry>> selectedClasses, vector<vector<ClassEntry>> &out) {
	vector<ClassEntry> emptyBuild;
	findSchedulesHelper(selectedClasses, 0, emptyBuild, out);
}

int main() {
	
	//All the classes, including the same class at different times.
	vector<ClassEntry> classes = readData("schedule.csv");
	vector<string> selectedColumn = readColumn("class_Select.csv", "Select");
	vector<string> allSubjectNumbers = readColumn("class_Select.csv", "Subject-Number");
	
	
	//A 2D vector holding all the classes, including all the sections, that the user selected in class_Select.csv
	//The first dimension (selectedClasses[x]) holds all posibilities of a single class (I.E. all ECE-211 sections)
	//The second dimension (selectedClasses[x][y]) holds a single possibility for that class (I.E. ECE-211 @3:00PM)
	vector<vector<ClassEntry>> selectedClasses;
	//Populate the selectedClasses vector
	for(int i = 0; i < selectedColumn.size(); i++) {
		//If a class that is selected by the user is found, add in all copies of it.
		if(selectedColumn[i] == "x" || selectedColumn[i] == "X") {
			vector<ClassEntry> toAdd;
			for(int j = 0;  j < classes.size(); j++) {
				if(classes[j].subjectNumber == allSubjectNumbers[i]){
					toAdd.push_back(classes[j]);
				}
			}
			selectedClasses.push_back(toAdd);
		}
	}
	
	cout << endl << endl<< "What is your credit load limit? (input a number 12-25)" << endl;
	float credLimit,credTot=0;
	cin >> credLimit;
	for(int i = 0; i<selectedClasses.size();i++){
		credTot+=selectedClasses[i][0].numCredits;
	}
	if(credTot > credLimit){
		cout<< "You've chosen too many classes for that credit load!" << endl; 
		return 0;
	}
	cout << "total credits chosen: " << credTot << endl;
	//All the possible schedules, including ones that have conflicts. 
	//TODO: Filter out the conflicts
	vector<vector<ClassEntry>> allSchedules;
	vector<vector<ClassEntry>> possSchedules;
	findSchedules(selectedClasses, allSchedules);
	int count = 0;
	//cout << allSchedules[0][0].toString()<< endl;
	for(int i = 0; i < allSchedules.size(); i++) {
		if(checkSched(allSchedules[i])){
			possSchedules.push_back(allSchedules[i]);
			count++;
		}
	}
	cout << endl << endl << "Possible Schedules: " << endl;
	for(int i = 0; i < possSchedules.size(); i++) {
		for(int j = 0; j<possSchedules[i].size(); j++){
			cout << possSchedules[i][j].toString() << endl;
			
		}
		cout << endl;
	}
	cout << endl;
	cout << "Num schedules generated: " << count << endl;
	return 0;
}


