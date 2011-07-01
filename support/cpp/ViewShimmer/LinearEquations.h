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

#ifndef LINEAREQUATIONS_H
#define LINEAREQUATIONS_H

#include <QObject>
#include <QDebug>
#include <qmap.h>
#include <vector>
#include <tnt_math_utils.h>
#include <jama_qr.h>
#include <jama_svd.h>

class Equation;
class Solution;

/*!
 * This class solves systems of linear equations.
 * @author mmaroti@math.u-szeged.hu
 */
class LinearEquations : public QObject {
    Q_OBJECT
public:
    LinearEquations();
    ~LinearEquations();

    /*!
     * Get the index of a variable with this method.
     */
    unsigned int getVariable(QString name);

    bool variablesContains(QString name) { return variables.contains(name); }

    /*!
     * Returns the set of variable names in the system.
     */
    QMap<QString, unsigned int>& getVariables() { return variables; };


    /*!
     * Returns the list of equations in the system.
     */
    QList<Equation*>& getEquations() { return equations; }

    /*!
     * Returns a new empty equation
     * does not add it to the list
     */
    Equation* createEquation();

    /*!
     * Creates equations first, then add them to the system
     * using this method.
     */
    void addEquation(Equation* equation){ equations.append(equation); }

    /*!
     * Removes one equation from the set of equations. Be careful,
     * as the number of variables will not decrease and can result
     * in underdetermined system.
     * Does not delete the equation object
     */
    void removeEquation(Equation* equation){ equations.removeOne(equation); }

    /*!
     * Remove all equations and variables.
     */
    void clear();

    /*!
     * Prints some statistics on the list of equations and variables.
     */
    void printStatistics();

    /*!
     * Returns the solution where the least squares of errors
     * of the equations is the smallest.
     */
    Solution* solveLeastSquares();

    /*!
     * Returns a solution of a system of equations where the equations
     * are (almost) linearly dependent. Normalize singular values
     * that are larger than <code>alpha</code>.
     */
    Solution* solveWithSVD(double alpha);

    unsigned int getVariableCount() { return variables.size(); }
    unsigned int getVariableValue( QString name ) { return variables.value(name); }

    unsigned int getEquationsCount() { return equations.size(); }

protected:
    /*!
     * Holds a map from variable names to variable indices
     */
    QMap<QString,unsigned int> variables;

    /*!
     * Holds the list of equations.
     */
    QList<Equation*> equations;
};

TNT::Array2D<double>* transpose(const TNT::Array2D<double>& Matrix);
void printMatrix1D(const TNT::Array1D<double>&);
void printMatrix2D(const TNT::Array2D<double>&);

/*!
 * Represents one equation of the system.
 * Create the equation, set the coefficient of
 * specific elements and then set the constant.
 */
class Equation
{
protected:
    vector<double> coefficients;
    double constant;
    unsigned int getVariableIndex(QString name);
    LinearEquations* lEsystem;

public:
    Equation(LinearEquations* lEsystem);
    ~Equation();

    double getCoefficient(QString);
    double getCoefficientAt(int i) { return coefficients.at(i); }
    unsigned int getCoefficientsSize() { return coefficients.size(); }
    void setCoefficient(QString variable, double coefficient);
    void addCoefficient(QString variable, double coefficient);
    void subCoefficient(QString variable, double coefficient);

    double getConstant() { return constant; }
    void setConstant(double value) { this->constant = value; }
    void addConstant(double value) { constant += value; }
    void subConstant(double value) { constant -= value; }

    /*!
     * Returns the difference of the constant and the left hand side
     * containing the variables.
     */
    double getSignedError(const Solution*);
    double getAbsoluteError(const Solution*);

    /*!
     * Returns the value of the left hand side. This
     * value does not depend on the constant.
     */
    double getLeftHandSide(const Solution*);

    /*!
     * Multiplies each coefficient and the constant with this value.
     * The new equation will hold if and only if the original does,
     * but with the multiplied version can count with different strength
     * in a least square solutions.
     */
    void multiply(double);
};


/*!
 * This class represents a solution to the system of linear equations.
 */
class Solution
{
protected:
    LinearEquations* lEsystem;
    vector<double> values;
public:
    Solution( TNT::Array2D<double>&, LinearEquations* lEsystem );
    ~Solution();

    double getValueAt(int i) const { return this->values.at(i); }
    int getValuesSize() const { return this->values.size(); }

    /*!
     * Use this method to get the value of a variable in the solution,
     * or returns <code>NaN</code> if the variable does not occur in
     * the solution.
     */
    double getValue(QString name);

    /*!
     * Prints out the solution and some statistics on the errors.
     */
    void print();

    /*!
     * Returns the average error in the system of equations.
     */
    double getAverageError();

    /*!
     * Returns the maximum error in the system of equations.
     */
    double getMaximumError();

    const Equation* getMaximumErrorEquation();
};

#endif // LINEAREQUATIONS_H
