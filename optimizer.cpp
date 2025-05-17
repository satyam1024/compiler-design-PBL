#include "optimizer.h"
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <regex>

using namespace std;

vector<string> Optimizer::optimize(const vector<string>& ic) {
    vector<string> result = constantFolding(ic);
    result = copyPropagation(result);
    result = redundantAssignmentElimination(result);
    result = deadCodeElimination(result);
    return result;
}

// ---------------- Constant Folding ----------------
vector<string> Optimizer::constantFolding(const vector<string>& ic) {
    vector<string> folded;
    regex const_expr(R"((\w+)\s*=\s*(\d+)\s*([+\*/-])\s*(\d+))");
    smatch match;

    for (const auto& line : ic) {
        if (regex_match(line, match, const_expr)) {
            string lhs = match[1];
            int a = stoi(match[2]);
            int b = stoi(match[4]);
            char op = match[3].str()[0];
            int result = 0;

            switch (op) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = b != 0 ? a / b : 0; break;
            }

            folded.push_back(lhs + " = " + to_string(result));
        } else {
            folded.push_back(line);
        }
    }

    return folded;
}

// ---------------- Copy Propagation ----------------
vector<string> Optimizer::copyPropagation(const vector<string>& ic) {
    unordered_map<string, string> copies;
    vector<string> optimized;

    for (const auto& line : ic) {
        istringstream iss(line);
        string lhs, eq, rhs;
        iss >> lhs >> eq >> rhs;

        if (copies.find(rhs) != copies.end()) {
            rhs = copies[rhs];
        }

        optimized.push_back(lhs + " = " + rhs);

        if (!rhs.empty() && isalpha(rhs[0])) {
            copies[lhs] = rhs;
        }
    }

    return optimized;
}

// ---------------- Redundant Assignment ----------------
vector<string> Optimizer::redundantAssignmentElimination(const vector<string>& ic) {
    unordered_map<string, string> lastAssignment;
    vector<string> optimized;

    for (const auto& line : ic) {
        istringstream iss(line);
        string lhs, eq, rhs;
        iss >> lhs >> eq >> rhs;

        if (lastAssignment[lhs] == rhs) {
            continue;
        }

        lastAssignment[lhs] = rhs;
        optimized.push_back(line);
    }

    return optimized;
}

// ---------------- Dead Code Elimination (basic) ----------------
vector<string> Optimizer::deadCodeElimination(const vector<string>& ic) {
    unordered_set<string> used;
    vector<string> reversed(ic.rbegin(), ic.rend());
    vector<string> cleaned;

    for (const auto& line : ic) {
        istringstream iss(line);
        string lhs, eq, rhs;
        iss >> lhs >> eq >> rhs;
        if (!rhs.empty() && isalpha(rhs[0])) {
            used.insert(rhs);
        }
    }

    for (const auto& line : reversed) {
        istringstream iss(line);
        string lhs, eq, rhs;
        iss >> lhs >> eq >> rhs;

        if (used.find(lhs) != used.end() || line.find("output") != string::npos) {
            cleaned.push_back(line);
            used.insert(rhs);
        }
    }

    reverse(cleaned.begin(), cleaned.end());
    return cleaned;
}
