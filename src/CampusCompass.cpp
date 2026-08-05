#include "CampusCompass.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    // return boolean based on whether parsing was successful or not
    if(edges_filepath.empty() || classes_filepath.empty()) {
        return false;
    }

    ifstream edges_file(edges_filepath);
    if (!edges_file.is_open()) {
        cerr << "Unsuccessful: Could not open edges file" << endl;
        return false;
    }

    ifstream classesFile(classes_filepath);
    if (!classesFile.is_open()) {
        cerr << "Unsuccessful: Could not open classes file" << endl;
        return false;
    }


    string line;
    getline(edges_file, line); // Skips the header line

    while(getline(edges_file, line))
    {
        stringstream ss(line); // create a stringstream from the line
        string from, to, name1, name2, weight; 

        getline(ss, from, ',');
        getline(ss, to, ',');

        //no need for names for edge data
        getline(ss, name1, ',');
        getline(ss, name2, ','); 

        getline(ss, weight, ',');

        // Create two edges for the undirected graph
        Edge edge1;
        edge1.to = stoi(to);
        edge1.weight = stoi(weight);
        edge1.isOpen = true;

        Edge edge2;
        edge2.to = stoi(from);
        edge2.weight = stoi(weight);
        edge2.isOpen = true;

        adjList[stoi(from)].push_back(edge1);
        adjList[stoi(to)].push_back(edge2);
    }

    edges_file.close();

    getline(classesFile, line); // Skips header
    while(getline(classesFile, line))
    {
        stringstream ss(line); // create a stringstream from the line
        ClassData classData;
        string location;

        getline(ss, classData.class_code, ',');
        getline(ss, location, ',');
        getline(ss, classData.start, ',');
        getline(ss, classData.end, ',');

        classData.location_id = stoi(location);
        classes[classData.class_code] = classData;
    }

    classesFile.close();

    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    bool is_valid = true; 
    string commandType;

    // Extract command type from the command string
    stringstream ss(command);
    getline(ss, commandType, ' ');

    if(commandType == "insert") {
        // parse insert command
        string name, idString, residenceString;
        int numClasses;

        size_t nameStart = command.find('"');

        if(nameStart == string::npos) {
            is_valid = false; // Invalid command format
            cout << "unsuccessful" << endl;
        } else {
            size_t nameStart1 = nameStart + 1;
            size_t nameEnd = command.find('"', nameStart + 1);

            if(nameEnd == string::npos) {
                is_valid = false; // Invalid command format
                cout << "unsuccessful" << endl;
            }

            name = command.substr(nameStart, nameEnd - nameStart);
            string remainingCommand = command.substr(nameEnd + 1);
            stringstream ss(remainingCommand);

            getline(ss, idString, ' ');
            getline(ss, residenceString, ' ');
            ss >> numClasses;

            vector<string> classNames;
            for(int i = 0; i < numClasses; ++i) {
                string className;
                ss >> className;
                classNames.push_back(className);
            }

            if(classNames.size() != numClasses) {
                is_valid = false; // Mismatch in number of classes
                cout << "unsuccessful" << endl;
            }

            int id, residence_id;
            try
            {             
                id = stoi(idString);
                residence_id = stoi(residenceString);
            }
            catch(const std::exception& e)
            {
                is_valid = false; // Invalid ID or residence ID format
                cout << "unsuccessful" << endl;
            }

            if(!insertStudent(name, id, residence_id, classNames)) {
                is_valid = false; // Insertion failed
                cout << "unsuccessful" << endl;
            }
            cout << "successful" << endl;
        }
    } else if(commandType == "remove") {
        // parse remove command
        string idString;
        ss >> idString;

        if(ss.fail())
        {
            cout << "unsuccessful" << endl;
            return false;
        }

        int id;
        try
        {
            id = stoi(idString);
        }
        catch(const std::exception& e)
        {
            cout << "unsuccessful" << endl;
            return false;
        }

        if(removeStudent(id))
        {
            cout << "successful" << endl;
        }
        else
        {
            cout << "unsuccessful" << endl;
        }
    } else if(commandType == "dropClass") {
        // parse dropClass command
    } else if(commandType == "replaceClass") {
        // parse replaceClass command
    } else if(commandType == "removeClass") {
        // parse removeClass command
    } else if(commandType == "toggleEdgesClosure") {
        // parse toggleEdgesClosure command
    } else if(commandType == "checkEdgeStatus") {
        // parse checkEdgeStatus command
    } else if(commandType == "printShortestEdges") {
        // parse printShortestEdges command
    } else if(commandType == "printStudentZone") {
        // parse printStudentZone command
    } else if(commandType == "verifySchedule") {
        // parse verifySchedule command
    } else {
        is_valid = false; // invalid command type
    }

    return is_valid;
}

bool CampusCompass::insertStudent(const string &name, int id, int residence_id, const vector<string> &class_names)
{
    //Student validation checks
    string idString = to_string(id);
    if(id <= 0 || idString.size() != 8 || name.empty()) {
        //cout << "Unsuccessful: Invalid student ID or name" << endl;
        return false;
    }

    if(students.find(id) != students.end()) {
        //cout << "Unsuccessful: Student with this ID already exists" << endl;
        return false; // student with this ID already exists
    }

    for(const string &class_code : class_names) {
        if(classes.find(class_code) == classes.end()) {
            //cout << "Unsuccessful: Class " << class_code << " does not exist" << endl;
            return false; // class does not exist
        }
    }

    if(adjList.find(residence_id) == adjList.end()) {
        //cout << "Unsuccessful: Residence location " << residence_id << " does not exist" << endl;
        return false; // residence location does not exist in the graph
    }

    //string studentName = name;
    // istringstream ss(studentName); // create a stringstream from the name
    // getline(ss, studentName, '"'); // Skip the first quote
    // getline(ss, studentName, '"'); // Get the name between quotes

    Student newStudent;
    newStudent.name = name;
    newStudent.id = id;
    newStudent.residence_id = residence_id;
    newStudent.classes = class_names;
    students[id] = newStudent;
    return true;
}

bool CampusCompass::removeStudent(int id)
{
    auto s = students.find(id);

    if(s != students.end()) {
        students.erase(s);
        return true;
    }
    else {
        return false; // student with this ID does not exist
    }
}
