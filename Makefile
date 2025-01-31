transportation: transportation.cpp
	g++ transportation.cpp -I${GUROBI_PATH}/include/ -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi120 -o transportation

inheritance: inheritance.cpp
	g++ inheritance.cpp -I${GUROBI_PATH}/include/ -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi120 -o inheritance
