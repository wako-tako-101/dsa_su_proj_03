#include "CampusCompass.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <climits>

using namespace std;

CampusCompass::CampusCompass() = default;

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath)
{
    // return boolean based on whether parsing was successful or not
    if (edges_filepath.empty() || classes_filepath.empty())
    {
        return false;
    }

    ifstream edges_file(edges_filepath);
    if (!edges_file.is_open())
    {
        cerr << "Unsuccessful: Could not open edges file" << endl;
        return false;
    }

    ifstream classesFile(classes_filepath);
    if (!classesFile.is_open())
    {
        cerr << "Unsuccessful: Could not open classes file" << endl;
        return false;
    }

    string line;
    getline(edges_file, line); // Skips the header line

    while (getline(edges_file, line))
    {
        stringstream ss(line); // create a stringstream from the line
        string from, to, name1, name2, weight;

        getline(ss, from, ',');
        getline(ss, to, ',');

        // no need for names for edge data
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
    while (getline(classesFile, line))
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

bool CampusCompass::ParseCommand(const string &command)
{
    bool is_valid = true;
    string commandType;

    // Extract command type from the command string
    stringstream ss(command);
    getline(ss, commandType, ' ');

    if (commandType == "insert")
    {
        // parse insert command
        string name, idString, residenceString;
        int numClasses;

        size_t nameStart = command.find('"');

        if (nameStart == string::npos)
        {
            is_valid = false; // Invalid command format
            cout << "unsuccessful" << endl;
        }
        else
        {
            size_t nameEnd = command.find('"', nameStart + 1);

            if (nameEnd == string::npos)
            {
                is_valid = false; // Invalid command format
                cout << "unsuccessful" << endl;
            }

            // create a substring of the name between the quotes
            name = command.substr(nameStart + 1, nameEnd - nameStart - 1);
            string remainingCommand = command.substr(nameEnd + 1);

            // parse remaining command for id, residence_id, and number of classes
            stringstream ss(remainingCommand);
            ss >> idString >> residenceString >> numClasses;

            vector<string> classNames;
            for (int i = 0; i < numClasses; ++i)
            {
                string className;
                ss >> className;
                classNames.push_back(className);
            }

            // check if the number of classes matches the expected number
            //  if (classNames.size() != numClasses)
            //  {
            //      is_valid = false; // Mismatch in number of classes
            //      cout << "unsuccessful" << endl;
            //  }

            // try to convert idString and residenceString to integers
            int id, residence_id;
            try
            {
                id = stoi(idString);
                residence_id = stoi(residenceString);
            }
            catch (const std::exception &e)
            {
                is_valid = false; // Invalid ID or residence ID format
                cout << "unsuccessful" << endl;
            }

            // check if the student can be inserted
            if (!insertStudent(name, id, residence_id, classNames))
            {
                is_valid = false; // Insertion failed
                cout << "unsuccessful" << endl;
                return false;
            }

            cout << "successful" << endl;
        }
    }
    else if (commandType == "remove")
    {
        // parse remove command
        string idString;
        ss >> idString;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        int id;
        try
        {
            id = stoi(idString);
        }
        catch (const std::exception &e)
        {
            is_valid = false; // Invalid ID format
            cout << "unsuccessful" << endl;
            return false;
        }

        if (removeStudent(id))
        {
            cout << "successful" << endl;
        }
        else
        {
            is_valid = false;
            cout << "unsuccessful" << endl;
        }
    }
    else if (commandType == "dropClass")
    {
        // parse dropClass command
        string idString, classCode;
        ss >> idString >> classCode;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        int id = stoi(idString);
        if (dropClass(id, classCode))
        {
            cout << "successful" << endl;
        }
        else
        {
            is_valid = false;
            cout << "unsuccessful" << endl;
        }
    }
    else if (commandType == "replaceClass")
    {
        // parse replaceClass command
        string idString, oldClassCode, newClassCode;
        ss >> idString >> oldClassCode >> newClassCode;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        int id = stoi(idString);

        if (replaceClass(id, oldClassCode, newClassCode))
        {
            cout << "successful" << endl;
        }
        else
        {
            is_valid = false;
            cout << "unsuccessful" << endl;
        }
    }
    else if (commandType == "removeClass")
    {
        // parse removeClass command
        string classCode;
        ss >> classCode;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        int removedCount = removeClass(classCode);

        if (removedCount == -1)
        {
            is_valid = false;
            cout << "unsuccessful" << endl;
        }
        else
        {
            cout << removedCount << endl;
        }
    }
    else if (commandType == "toggleEdgesClosure")
    {
        // parse toggleEdgesClosure command
        int numEdges;
        ss >> numEdges;

        vector<pair<int, int>> edges;

        for (int i = 0; i < numEdges; i++)
        {
            int x, y;
            ss >> x >> y;

            if (ss.fail())
            {
                cout << "unsuccessful" << endl;
                is_valid = false;
                return false;
            }

            edges.push_back({x, y});
        }

        if (toggleEdgesClosure(edges))
        {
            cout << "successful" << endl;
        }
        else
        {
            is_valid = false;
            cout << "unsuccessful" << endl;
        }
    }
    else if (commandType == "checkEdgeStatus")
    {
        // parse checkEdgeStatus command
        int location1, location2;
        ss >> location1 >> location2;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }
        cout << checkEdgeStatus(location1, location2) << endl;
    }

    else if (commandType == "isConnected")
    {
        // parse isConnected command
        int location1, location2;
        ss >> location1 >> location2;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }
        cout << (isConnected(location1, location2) ? "successful" : "unsuccessful") << endl;
    }

    else if (commandType == "printShortestEdges")
    {
        // parse printShortestEdges command
        int id;
        ss >> id;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        printShortestEdges(id);
    }
    else if (commandType == "printStudentZone")
    {
        // parse printStudentZone command
        string idString;

        ss >> idString;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            return false;
        }

        int id;

        try
        {
            id = stoi(idString);
        }
        catch (const std::exception &e)
        {
            cout << "unsuccessful" << endl;
            return false;
        }

        // Make sure student exists
        if (students.find(id) == students.end())
        {
            cout << "unsuccessful" << endl;
            return false;
        }

        printStudentZone(id);
    }
    else if (commandType == "verifySchedule")
    {
        // parse verifySchedule command
        int id;
        ss >> id;

        if (ss.fail())
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
            return false;
        }

        if (verifySchedule(id))
        {
            cout << "successful" << endl;
        }
        else
        {
            cout << "unsuccessful" << endl;
            is_valid = false;
        }
    }
    else
    {
        is_valid = false; // invalid command type
    }

    return is_valid;
}

bool CampusCompass::insertStudent(const string &name, int id, int residence_id, const vector<string> &class_names)
{
    // Student validation checks
    string idString = to_string(id);
    if (id <= 0 || idString.size() != 8 || name.empty())
    {
        // cout << "Unsuccessful: Invalid student ID or name" << endl;
        return false;
    }

    if (students.find(id) != students.end())
    {
        // cout << "Unsuccessful: Student with this ID already exists" << endl;
        return false; // student with this ID already exists
    }

    for (const string &class_code : class_names)
    {
        if (classes.find(class_code) == classes.end())
        {
            // cout << "Unsuccessful: Class " << class_code << " does not exist" << endl;
            return false; // class does not exist
        }
    }

    if (adjList.find(residence_id) == adjList.end())
    {
        // cout << "Unsuccessful: Residence location " << residence_id << " does not exist" << endl;
        return false; // residence location does not exist in the graph
    }

    // string studentName = name;
    //  istringstream ss(studentName); // create a stringstream from the name
    //  getline(ss, studentName, '"'); // Skip the first quote
    //  getline(ss, studentName, '"'); // Get the name between quotes

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

    if (s != students.end())
    {
        students.erase(s);
        return true;
    }
    else
    {
        return false; // student with this ID does not exist
    }
}

bool CampusCompass::dropClass(int id, const string &class_code)
{
    auto student = students.find(id);
    if (student == students.end())
    {
        return false; // student with this ID does not exist
    }

    vector<string> &studentClasses = student->second.classes;
    auto classIt = find(studentClasses.begin(), studentClasses.end(), class_code);

    if (classIt != studentClasses.end())
    {
        studentClasses.erase(classIt);
    }
    else
    {
        return false; // student is not enrolled in the specified class
    }

    if (studentClasses.empty())
    {
        students.erase(student); // remove student if they have no classes left
        return true;
    }
    return true;
}

bool CampusCompass::replaceClass(int id, const string &old_class_code, const string &new_class_code)
{
    auto student = students.find(id);

    // if student does not exist
    if (student == students.end())
    {
        return false;
    }

    // if new class does not exist
    if (classes.find(new_class_code) == classes.end())
    {
        return false;
    }

    vector<string> &studentClasses = student->second.classes;
    bool oldClass = false;
    bool newClass = false;

    for (string code : studentClasses)
    {
        if (code == old_class_code)
            oldClass = true;

        if (code == new_class_code)
            newClass = true;
    }

    // if the student does not have the old class or already has the new class, return false
    if (!oldClass || newClass)
    {
        return false;
    }

    // replace the old class with the new class
    for (string &code : studentClasses)
    {
        if (code == old_class_code)
        {
            code = new_class_code;
            break;
        }
    }
    return true;
}

int CampusCompass::removeClass(const string &class_code)
{
    //-1 if invalid class code, else return number of students that were removed from the class
    if (classes.find(class_code) == classes.end())
    {
        return -1; // class does not exist
    }

    int count = 0;
    vector<int> removeStudents;

    for (auto &s : students)
    {
        Student &student = s.second;

        auto it = find(student.classes.begin(), student.classes.end(), class_code);
        if (it != student.classes.end()) // if the student has the class, remove it
        {
            student.classes.erase(it);
            count++;

            if (student.classes.empty())
            {
                removeStudents.push_back(s.first);
            }
        }
    }

    for (int id : removeStudents)
    {
        students.erase(id);
    }

    if (count == 0)
    {
        return -1;
    }

    return count;
}

bool CampusCompass::toggleEdgesClosure(const vector<pair<int, int>> &edges)
{
    for (auto edgePair : edges)
    {
        int from = edgePair.first;
        int to = edgePair.second;
        bool found = false;

        // from -> to
        for (auto &edge : adjList[from])
        {
            if (edge.to == to) // if the edge exists, toggle its isOpen status
            {
                edge.isOpen = !edge.isOpen;
                found = true;
            }
        }

        // to -> from
        for (auto &edge : adjList[to])
        {
            if (edge.to == from) // if the edge exists, toggle its isOpen status
            {
                edge.isOpen = !edge.isOpen;
                found = true;
            }
        }

        if (!found) // if the edge was not found in the adjacency list, return false
        {
            return false;
        }
    }
    return true;
}

string CampusCompass::checkEdgeStatus(int location1, int location2)
{
    auto it = adjList.find(location1);
    if (it == adjList.end())
    {
        return "DNE";
    }

    for (const Edge &edge : it->second)
    {
        if (edge.to == location2)
        {
            return edge.isOpen ? "open" : "closed";
        }
    }

    return "DNE";
}

bool CampusCompass::isConnected(int location1, int location2)
{
    if (adjList.find(location1) == adjList.end() || adjList.find(location2) == adjList.end())
    {
        return false; // One or both locations don't exist in the graph
    }

    // BFS Traversal
    queue<int> q;
    unordered_set<int> visited;

    q.push(location1);
    visited.insert(location1);

    while (!q.empty())
    {
        int current = q.front();
        q.pop();

        if (current == location2)
        {
            return true; // Found a path to location2
        }

        for (const Edge &edge : adjList[current])
        {
            if (!edge.isOpen)
                continue; // Skip closed edges

            if (visited.find(edge.to) != visited.end())
                continue; // Skip already visited nodes

            visited.insert(edge.to);
            q.push(edge.to);
        }
    }

    return false;
}

unordered_map<int, int> CampusCompass::dijkstraWithParent(int start, unordered_map<int, int> &parent)
{
    unordered_map<int, int> distance; // map to represent the shortest distance from the starting node to each node in the graph

    // Initialize all distances
    for (auto &node : adjList)
    {
        distance[node.first] = INT_MAX; // set initial distance to "infinity"
    }

    // Distance to starting node
    distance[start] = 0;

    // pair = {distance, location}
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});

    while (!pq.empty())
    {
        int currentDist = pq.top().first;
        int currentNode = pq.top().second;
        pq.pop();

        // Ignore outdated queue entries
        if (currentDist > distance[currentNode])
        {
            continue;
        }

        for (const Edge &edge : adjList[currentNode])
        {
            // Ignore closed paths
            if (!edge.isOpen)
                continue;

            int newDist = currentDist + edge.weight;
            if (newDist < distance[edge.to]) // if the new distance is shorter than the previously recorded distance, update it
            {
                distance[edge.to] = newDist;
                parent[edge.to] = currentNode; // update the parent of the node
                pq.push({newDist, edge.to});
            }
        }
    }
    return distance;
}

vector<int> CampusCompass::getPath(int start, int end, const unordered_map<int, int> &parent)
{
    vector<int> path;
    int current = end;

    while (current != start)
    {
        path.push_back(current);
        if (parent.find(current) == parent.end())
        {
            return {}; // return empty path if there is no path from start to end
        }

        current = parent.at(current);
    }

    path.push_back(start);
    reverse(path.begin(), path.end());

    return path;
}

void CampusCompass::printShortestEdges(const int &student_id)
{
    auto studentIter = students.find(student_id);
    if (studentIter == students.end())
    {
        cout << "unsuccessful" << endl;
        return;
    }

    Student &student = studentIter->second;
    unordered_map<int, int> parent;

    cout << "Time For Shortest Edges: " << student.name << endl;
    auto distances = dijkstraWithParent(student.residence_id, parent);

    vector<string> sortedClasses = student.classes;
    sort(sortedClasses.begin(), sortedClasses.end());

    for (string classCode : sortedClasses)
    {
        int location = classes[classCode].location_id;

        if (distances[location] == INT_MAX)
        {
            cout << classCode << ": -1" << endl;
        }
        else
        {
            cout << classCode << ": " << distances[location] << endl;
        }
    }
}

void CampusCompass::printStudentZone(const int &student_id)
{
    // Get student
    auto studentIter = students.find(student_id);

    if (studentIter == students.end())
    {
        cout << "unsuccessful" << endl;
        return;
    }

    Student &student = studentIter->second;

    // Store parent relationships from Dijkstra
    unordered_map<int, int> parent;

    // Run Dijkstra from residence
    unordered_map<int, int> distances = dijkstraWithParent(student.residence_id, parent);

    // Stores all vertices that appear in shortest paths
    unordered_set<int> zoneNodes;

    // Residence is always included
    zoneNodes.insert(student.residence_id);

    // Find every shortest path from residence to classes
    for (string classCode : student.classes)
    {
        int classLocation =
            classes[classCode].location_id;

        // If class is unreachable, skip
        if (distances[classLocation] == INT_MAX || distances.find(classLocation) == distances.end())
        {
            continue;
        }

        vector<int> path = getPath(student.residence_id, classLocation, parent);

        // Add all path nodes to the zone
        for (int node : path)
        {
            zoneNodes.insert(node);
        }
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    unordered_set<int> visited;

    int totalCost = 0;

    // Start at student's residence
    pq.push({0, student.residence_id});

    while (!pq.empty())
    {
        int currentWeight = pq.top().first;
        int currentNode = pq.top().second;

        pq.pop();

        // Already included in MST
        if (visited.count(currentNode))
        {
            continue;
        }

        visited.insert(currentNode);
        totalCost += currentWeight;

        // Explore edges
        for (const Edge &edge : adjList[currentNode])
        {
            // Ignore closed edges
            if (!edge.isOpen)
            {
                continue;
            }

            // Only use nodes inside our subgraph
            if (zoneNodes.find(edge.to) == zoneNodes.end())
            {
                continue;
            }

            // Add possible MST edge
            if (!visited.count(edge.to))
            {
                pq.push({edge.weight, edge.to});
            }
        }
    }

    cout << "Student Zone Cost For " << student.name << ": " << totalCost << endl;
}

bool CampusCompass::verifySchedule(const int &student_id)
{
    if (students.find(student_id) == students.end())
    {
        return false; // student with this ID does not exist
    }

    const Student &student = students.at(student_id);

    // if a student only has 1 class, return false
    if (student.classes.size() <= 1)
    {
        return false;
    }

    // Check if all classes in the student's schedule exist
    vector<ClassData> classSchedule;
    for (const string &classCode : student.classes)
    {
        if (classes.find(classCode) == classes.end())
        {
            return false; // Class does not exist
        }
        else
        {
            classSchedule.push_back(classes[classCode]);
        }
    }

    // compare class schedules and see which one starts earlier
    for (int i = 0; i < classSchedule.size(); i++)
    {
        for (int j = i + 1; j < classSchedule.size(); j++)
        {
            if (getTime(classSchedule[i].start) > getTime(classSchedule[j].start))
            {
                swap(classSchedule[i], classSchedule[j]);
            }
        }
    }

    cout << "Schedule Check for " << student.name << ":" << endl;

    for (int i = 0; i < classSchedule.size() - 1; i++)
    {
        //get the travel time between classes
        int travelTime = getTravelTime(classSchedule[i].location_id, classSchedule[i + 1].location_id);

        //get the time between the start of the next class and the end of the current one
        int timeGap = getTime(classSchedule[i + 1].start) - getTime(classSchedule[i].end);

        cout << classSchedule[i].class_code << " - " << classSchedule[i + 1].class_code << ": ";

        //check if it is possible for the student to make it to class on time
        if (travelTime != -1 && timeGap >= travelTime)
        {
            cout << "successful";
        }
        else
        {
            cout << "unsuccessful";
        }
        cout << endl;
    }

    return true;
}

int CampusCompass::getTime(string time)
{
    int hour, minute;
    try
    {
        hour = stoi(time.substr(0, 2));
        minute = stoi(time.substr(3, 2));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // convert to minutes
    return hour * 60 + minute;
}

int CampusCompass::getTravelTime(int start, int end)
{
    unordered_map<int, int> parent;
    unordered_map<int, int> distances = dijkstraWithParent(start, parent);

    if (distances.find(end) == distances.end() || distances[end] == INT_MAX)
    {
        return -1;
    }

    return distances[end];
}
