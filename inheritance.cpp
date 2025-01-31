#include "gurobi_c++.h"
#include <sstream>

using namespace std;

int main() {
    const int nItems = 13;
    const int items[nItems] = {25000, 5000, 20000, 40000, 12000, 12000, 12000, 3000, 6000, 10000, 15000, 10000, 13000};
    GRBEnv *env = new GRBEnv();
    try {
        GRBModel model = GRBModel(env);
        GRBVar* A = model.addVars(nItems, GRB_BINARY);
        GRBVar* B = model.addVars(nItems, GRB_BINARY);
        // Add constraints for each item going to one or the other.
        for (int i = 0; i < nItems; ++i) {
            ostringstream cname;
            cname << "partition[" << i << "]";
            model.addConstr(A[i] + B[i] == 1.0, cname.str());
        }
        GRBLinExpr A_value = 0.0;
        GRBLinExpr B_value = 0.0;
        for (int i = 0; i < nItems; ++i) {
            A_value += A[i] * items[i];
            B_value += B[i] * items[i];
        }
        // Extra variable to represent the absolute value of the difference.
        GRBVar t = model.addVar(0.0, GRB_INFINITY, 1.0, GRB_CONTINUOUS, "difference");
        model.addConstr(t >= (A_value - B_value), "absolute_value[1]");
        model.addConstr(t >= - (A_value - B_value), "absolute_value[2]");
        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
        model.write("inheritance.lp");
        model.optimize();
        cout << "Solution: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "Partition: " << endl;
        int Asum = 0;
        for (int i = 0; i < nItems; ++i) {
            if (A[i].get(GRB_DoubleAttr_X) > 0) {
                Asum += items[i];
                cout << i << " ";
            }
        }
        cout << "| ";
        int Bsum = 0;
        for (int i = 0; i < nItems; ++i) {
            if (B[i].get(GRB_DoubleAttr_X) > 0) {
                Bsum += items[i];
                cout << i << " ";
            }
        }
        cout << endl;
        cout << "A sum: " << Asum << "   B sum: " << Bsum << endl;
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Exception during optimization." << endl;
    }

    return 0;
}