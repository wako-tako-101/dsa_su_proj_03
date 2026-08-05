#include "CampusCompass.h"
#include <string>
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

    getline(classesFile, line); // Skips header
    while(getline(classesFile, line))
    {
        stringstream ss(line); // create a stringstream from the line
        Class_Data classData;
        string location;

        getline(ss, classData.class_code, ',');
        getline(ss, location, ',');
        getline(ss, classData.start, ',');
        getline(ss, classData.end, ',');

        classData.location_id = stoi(location);
        classes[classData.class_code] = classData;
    }
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
    } else if(commandType == "remove") {
        // parse remove command
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