
#include "gurobi_c++.h"
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {

    GRBVar** containers;
    GRBVar** lorries;

    const int nStores = 6;
    const int nPorts = 5;
    const int availability[nStores] = {10, 12, 20, 24, 18, 40};
    const int demand[nPorts] = {20, 15, 25, 33, 21};
    const int distances[nStores][nPorts] = {
        {290, 115, 355, 715, 810},
        {380, 340, 165, 380, 610},
        {505, 530, 285, 220, 450},
        {655, 450, 155, 240, 315},
        {1010, 840, 550, 305, 95},
        {1072, 1097, 747, 372, 333},
    };

    try {
        GRBEnv env = new GRBEnv(true);
        env.set("LogFile", "transportation.log");
        env.start();

        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "transportation");

        // Set up the containers and lorries decision variables.
        containers = new GRBVar* [nStores];
        lorries = new GRBVar* [nStores];
        for (int s = 0; s < nStores; ++s) {
            containers[s] = model.addVars(nPorts);
            lorries[s] = model.addVars(nPorts);
            for (int p = 0; p < nPorts; ++p) {
                ostringstream containerVarName, lorryVarName;
                containerVarName << "Container[" << s << "][" << p << "]";
                containers[s][p].set(GRB_StringAttr_VarName, containerVarName.str());
                containers[s][p].set(GRB_CharAttr_VType, 'I');
                containers[s][p].set(GRB_DoubleAttr_LB, 0.0);
                lorryVarName << "Lorry[" << s << "][" << p << "]";
                lorries[s][p].set(GRB_DoubleAttr_Obj, distances[s][p]);
                lorries[s][p].set(GRB_StringAttr_VarName, lorryVarName.str());
                lorries[s][p].set(GRB_CharAttr_VType, 'I');
                lorries[s][p].set(GRB_DoubleAttr_LB, 0.0);
            }
        } 

        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

        // Constraints.
        // First the availability constraints. Only as many containers can leave
        // each store as there are available.
        for (int s = 0; s < nStores; ++s) {
            GRBLinExpr total_taken = 0;
            for (int p = 0; p < nPorts; ++p) {
                total_taken += containers[s][p];
            }
            ostringstream constraint_name;
            constraint_name << "Availability[" << s << "]";
            model.addConstr(total_taken <= availability[s], constraint_name.str());
        }
        // Next add the demand constraints. Every port needs to receive as many
        // containers as they need.
        for (int p = 0; p < nPorts; ++p) {
            GRBLinExpr total_needed = 0;
            for (int s = 0; s < nStores; ++s) {
                total_needed += containers[s][p];
            }
            ostringstream constraint_name;
            constraint_name << "Demand[" << p << "]";
            model.addConstr(total_needed >= demand[p], constraint_name.str());
        }
        // Next add the containers per lorry constraint.
        for (int p = 0; p < nPorts; ++p) {
            for (int s = 0; s < nStores; ++s) {
                ostringstream constraint_name;
                constraint_name << "ContainersPerLorry[" << s << "][" << p << "]";
                model.addConstr(containers[s][p] <= 2 * lorries[s][p], constraint_name.str());
            }
        }

        model.optimize();
        model.write("transportation.lp");
        cout << "\nTOTAL COST: " << 300.0 * model.get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "SOLUTION:" << endl;
        for (int s = 0; s < nStores; ++s) {
            for (int p = 0; p < nPorts; ++p) {
                cout << lorries[s][p].get(GRB_DoubleAttr_X) << " ";
            }
            cout << endl;
        }
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Exception during optimization" << endl;
    }

    for (int s = 0; s < nStores; ++s) {
        delete[] containers[s];
        delete[] lorries[s];
    }
    delete[] containers;
    delete[] lorries;
    return 0;
}