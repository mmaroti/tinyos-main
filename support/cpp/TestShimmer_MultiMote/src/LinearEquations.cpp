/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include "LinearEquations.h"
#include "iterator"

LinearEquations::LinearEquations()
{
}

LinearEquations::~LinearEquations()
{
    clear();
}

unsigned int LinearEquations::getVariable(QString name) {
    if( variables.contains(name) )
        return variables.value(name);
    unsigned int n = variables.size();
    variables.insert(name, n);
    return n;
}

Equation* LinearEquations::createEquation() {
    Equation* retEquation = new Equation(this);
    return retEquation;
}

void LinearEquations::clear() {

    QList<Equation*>::iterator iter;
    for( iter = equations.begin(); iter != equations.end(); ++iter )
        delete *iter;

    variables.clear();
    equations.clear();
}

void LinearEquations::printStatistics() {

    qDebug() << "unknowns: "  << variables.size() << ", equations: " << equations.size();
    for( int i = 0; i < variables.size(); i++) {
        qDebug() << "Variables: " << variables.values().at(i) << variables.key(i);
    }

    for( int i = 0; i < equations.size(); i++) {
        qDebug()  << "==========================";
        qDebug() << "\n\nEquation " << i << "; Constant: " << equations.at(i)->getConstant() << "\n";
        qDebug() << "Coefficients\t" << "Variables";
        for( unsigned int j = 0; j < equations.at(i)->getCoefficientsSize(); j++) {
            qDebug() << equations.at(i)->getCoefficientAt(j) << "\t" << getVariables().key(j) ;
        }
    }

}

Solution* LinearEquations::solveLeastSquares() {
    TNT::Array2D<double> A(equations.size(), variables.size());
    TNT::Array2D<double> B(equations.size(), 1);

    for(int i = 0; i < equations.size(); ++i)
    {
            Equation* equation = equations.at(i);

            int j = variables.size();
            while( --j >= 0 )
                    A[i][j] = equation->getCoefficientAt(j);

            B[i][0] = equation->getConstant();
    }

    JAMA::QR<double> qr(A);
    TNT::Array2D<double> X = qr.solve(B);

    Solution *solToReturn = new Solution(X, this);
    return solToReturn;
}

Solution* LinearEquations::solveWithSVD(double alpha) {
    int M = equations.size();
    int N = variables.size();

    TNT::Array2D<double> A(M,N);
    TNT::Array2D<double> B(M,1);

    for(int i = 0; i < M; ++i)
    {
            Equation* equation = equations.at(i);

            int j = N;
            while( --j >= 0 )
                    A[i][j] = equation->getCoefficientAt(j);

            B[i][0] = equation->getConstant();
    }

    JAMA::SVD<double> svd(A);

    TNT::Array2D<double> V,U;
    svd.getV(V);
    svd.getU(U);

    TNT::Array1D<double> singularValues;
    svd.getSingularValues(singularValues);
    alpha *= singularValues[0];

    TNT::Array2D<double> X(N,N);
    for(int j = 0; j < N; ++j)
        for(int i = 0; i < N; ++i)
            X[i][j] = 0.0;

    for(int j = 0; j < N && singularValues[j] > alpha; ++j) {
            double a = 1.0 / singularValues[j];
            for(int i = 0; i < N; ++i)
                    X[i][j] = V[i][j] * a;
    }

    X = matmult(X, matmult(*transpose(U), B));

    Solution *solToReturn = new Solution(X, this);
    return solToReturn;
}

//===================  STATIC FUNCTIONS  ===================

TNT::Array2D<double>* transpose(const TNT::Array2D<double> &M) {
    TNT::Array2D<double>* N = new TNT::Array2D<double>(M.dim2(),M.dim1());

    for (int i = 0; i < M.dim1(); i++)
        for (int j = 0; j < M.dim2(); j++)
            (*N)[j][i] = M[i][j];

    return N;
}

void printMatrix1D(const TNT::Array1D<double>& matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        std::cout << matrix[i] <<"\n";
    }
    std::cout << "\n" << flush;
}

void printMatrix2D(const TNT::Array2D<double>& matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        for (int j = 0; j < matrix.dim2(); j++) {
            std::cout << matrix[i][j] <<"\t";
        }
        std::cout << "\n";
    }
    std::cout << flush;
}

//===================  EQUATION  ===================

Equation::Equation(LinearEquations* equs) {
    lEsystem = equs;
    coefficients.reserve(lEsystem->getVariableCount() + 10);
}

Equation::~Equation() {

}

unsigned int Equation::getVariableIndex(QString name) {
    unsigned int n = lEsystem->getVariable(name);
    while ( coefficients.size() < lEsystem->getVariableCount() ) {  // <= n );
        coefficients.push_back(0.0);
    }
    return n;
}

double Equation::getCoefficient(QString name) {
    int n = getVariableIndex(name);
    return coefficients.at(n);
}

void Equation::setCoefficient(QString name, double value) {
    unsigned int n = getVariableIndex(name);
    this->coefficients.at(n) = value;
}

void Equation::addCoefficient(QString name, double value) {
    unsigned int n = getVariableIndex(name);
    this->coefficients.at(n) += value;
}

void Equation::subCoefficient(QString name, double value) {
    unsigned int n = getVariableIndex(name);
    this->coefficients.at(n) -= value;
}

void Equation::multiply(double value) {
    for(unsigned int i = 0; i < coefficients.size(); ++i)
            this->coefficients.at(i) *= value;

    constant *= value;
}

double Equation::getSignedError(const Solution* solution) {
    double e = constant;

    int i = coefficients.size();
    if( (int) solution->getValuesSize() < i )
            i = solution->getValuesSize();

    while( --i >= 0 )
            e -= coefficients.at(i) * solution->getValueAt(i);

    return e;
}

double Equation::getAbsoluteError(const Solution* solution) {
    return fabs(getSignedError(solution));
}

double Equation::getLeftHandSide(const Solution* solution) {
    return constant - getSignedError(solution);
}

//===================  SOLUTION  ===================

Solution::Solution( TNT::Array2D<double> &X, LinearEquations *equs ) {
    lEsystem = equs;
    for (unsigned int i = 0; i < lEsystem->getVariableCount(); i++) {
        this->values.push_back(0.0);
    }

    if( (unsigned int) X.dim2() != 1 || (unsigned int) X.dim1() != values.size() )
        qDebug() << "error";

    for(unsigned int i = 0; i < values.size(); ++i)
            values.at(i) = X[i][0];
}

Solution::~Solution() {

}

const Equation* Solution::getMaximumErrorEquation() {
    double d = -1.0;
    Equation *a = NULL;

    QList<Equation*>::iterator iter;
    for( iter = lEsystem->getEquations().begin(); iter != lEsystem->getEquations().end(); ++iter ) {
        double e = (*iter)->getAbsoluteError(this);
        if( e > d ) {
            d = e;
            a = *iter;
        }
    }

    return a;
}

double Solution::getValue(QString name) {
    if( lEsystem->variablesContains(name) )
            return values.at( lEsystem->getVariableValue(name) );

    return 0.0;
}

void Solution::print() {
    QMap<QString, unsigned int>::iterator iter;

    for( iter = lEsystem->getVariables().begin(); iter != lEsystem->getVariables().end(); ++iter) {
            QString name = iter.key();
            unsigned int index = lEsystem->getVariableValue(name);
            qDebug() << name << " = " << values.at(index);
    }
    qDebug() << "Average Error " << getAverageError();
    qDebug() << "Maximum Error " << getMaximumError();
}

double Solution::getAverageError() {
    double d = 0.0;

    QList<Equation*>::iterator iter;

    for( iter = lEsystem->getEquations().begin(); iter != lEsystem->getEquations().end(); ++iter ) {
        d += (*iter)->getAbsoluteError(this);
    }

    qDebug() << "eqsize:" << lEsystem->getEquationsCount();

    return d / lEsystem->getEquationsCount();
}

double Solution::getMaximumError() {
    double d = 0.0;

    QList<Equation*>::iterator iter;
    for( iter = lEsystem->getEquations().begin(); iter != lEsystem->getEquations().end(); ++iter ) {
        double e = (*iter)->getAbsoluteError(this);
        if( e > d )
                d = e;
    }

    return d;
}
