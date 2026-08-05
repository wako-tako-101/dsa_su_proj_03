#pragma once
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Student {
    vector<string> classes;
    string name;
    int id, residence_id;
};

struct Edge {
    int to, weight;
    bool isOpen = true;
};

struct ClassData {
    string class_code, start, end;
    int location_id;
};

class CampusCompass {
private:
    // Think about what member variables you need to initialize
    // perhaps some graph representation?
    unordered_map<int, Student> students;
    unordered_map<int, vector<Edge>> adjList; //adj list of graph with: Connectedness = O(V) | Adjacency = O(V) | Space = O(V + E)
    unordered_map<string, ClassData> classes; // maps class name to class details

public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);

    //Graph public methods
    bool insertStudent(const string &name, int id, int residence_id, const vector<string> &class_names);
    bool removeStudent(int id);
    bool dropClass(int id, const string &class_code);
    bool replaceClass(int id, const string &old_class_code, const string &new_class_code);
    int removeClass(const string &class_code);
    bool toggleEdgesClosure();
    string checkEdgeStatus(int location1, int location2);
    bool isConnected(int location1, int location2);

    //Print public methods
    void printShortestEdges(const int& student_id);
    void printStudentZone(const int& student_id);

    //Verification public methods
    bool verifySchedule(const int& student_id);
};
