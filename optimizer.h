#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>

using namespace std;

class Optimizer {
public:
    static vector<string> optimize(const vector<string>& ic);

private:
    static vector<string> constantFolding(const vector<string>& ic);
    static vector<string> deadCodeElimination(const vector<string>& ic);
    static vector<string> copyPropagation(const vector<string>& ic);
    static vector<string> redundantAssignmentElimination(const vector<string>& ic);
};
#endif