#include <ilcplex/ilocplex.h>
#include <iostream>

using namespace std;

int main(int argc, char *argV[]) {
	try {

		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);


		//A1 matriz de desigualdade, A2 matriz de igualdade
		IloNumArray2 A1(env), A2(env);
		//coef vetor de coeficientes, b1 vetor de desigualdade de A1, b2 vetor de igualdade de A2
		IloNumArray coef(env), b1(env), b2(env);
		
		//lendo o arquivo
		const char* filename = "t.dat";
		ifstream file(filename);
		file >> coef >> A1 >> b1 >> A2 >> b2;
		
		int numVar = coef.getSize();
		int numIneq = A1.getSize();
		int numEq = A2.getSize();
		
		//Declaração das variáveis
		IloNumVarArray V(env, numVar, 0, IloInfinity, ILOFLOAT);
		
		//Definição da função objetiva
		IloObjective obj = IloMinimize(env, IloScalProd(V,coef));
		model.add(obj);
		
		// Restrições
		IloRangeArray r(env, numIneq+numEq);
		
		
		for (int i=0; i<numIneq; i++)
			r[i] = IloRange(IloScalProd(V, A1[i]) <= b1[i]);
		for (int i=0; i<numEq; i++)
			r[i+numIneq] = IloRange(IloScalProd(V, A2[i]) == b2[i]);
		
		// Adicionado retrições
		for (int i=0; i<numIneq+numEq; i++)
			model.add(r[i]);
		
		//Resolução
		cplex.extract(model);
		cplex.exportModel("model1.lp");
		//cplex.exportModel("modelname.dua");
		cplex.solve();
		
		
		// Output
		if ((cplex.getStatus() == IloAlgorithm::Feasible) || (cplex.getStatus() == IloAlgorithm::Optimal)) {
			cout << endl << "Função objetiva: " << cplex.getObjValue() << endl;
			IloNumArray values(env, numVar);
			cplex.getValues(values, V);
			cout << values << endl << endl;
		} else {
			cout << endl << "Feasible solution was not found. Cplex status: " << cplex.getStatus() << endl;
		}

	} catch (IloException& ex) {
		cerr << "IloException: " << ex << endl;
	} catch (...) {
		cerr << "Error" << endl;
	}
	
	return 0;
}

