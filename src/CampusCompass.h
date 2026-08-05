#pragma once
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Student {
    vector<string> classes;
    string name;
    int id, residence_hall;
};

struct Edge {
    int from, to, weight;
    bool isOpen;
};

struct Class {
    string name, start, end;
    int location_id;
};

class CampusCompass {
private:
    // Think about what member variables you need to initialize
    // perhaps some graph representation?
    unordered_map<int, Student> students;
    unordered_map<int, vector<Edge>> adjList; //adj list of graph with: Connectedness = O(V) | Adjacency = O(V) | Space = O(V + E)
    unordered_map<string, Class> classes; // maps class name to class details

public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);

    //Graph public methods
    bool insertStudent(const string &student_id, const string &class_id);

    //Print public methods

};
