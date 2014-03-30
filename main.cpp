#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <string>
#include <cstring>
#include <cstdlib>
#include <memory.h>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <sstream>

using namespace std;

const int POPULATION_SIZE = 8;
const int MAXGENERATIONS = 50000;
const int MAXITER = 64;
const int TOURNIR_CONST = 4;
const int MUTATION_PROBOBILITY = 20;
const int CUR_GEN_MUTATION_PROBOBILITY = 70;
const int MAX_ROOT = 100;
const double EPS = 1e-8;
const double INF = 1e5;
int global_answer1 = 100;
int global_answer2 = 100;

int mabs(int arg)
{
    if (arg < 0) return -arg;
    return arg;
}

double mabs(double arg)
{
    if (arg < 0) return -arg;
    return arg;
}

/**
 * Class Gen is used to store current values of alleles of the individuals
 * x and y are the values of the alleles, result1 is the answer for the first
 * formula, delta1 is the deviation of the result1 from the answer for the first
 * formula. The same is true for formula2 and for result2 and delta2
 * fitness is the survival of the individual and likelihood is the survival
 * normalized over the all individuals
 * x - x-value in the formula
 * y - y-value in the formula
 * result1 - f1(x, y)
 * result2 - f2(x, y)
 * delta1 - |anwer1 - result1|
 * delta2 - |answer2 - result2|
 * fitness - survival
 * likelihood - normalized survival
 * x_to_str - x transfered into string
 * y_to_str - y transfered into string
 */
class Gen
{
public:
    int x, y;
    int result1, result2, delta1, delta2;
    double fitness;
    double likelihood;

    string x_to_str, y_to_str;
    /**
     * Transfering int to string
     */
    string IntToString(int arg)
    {
        string ans;
        while (arg != 0)
            ans.push_back((arg % 10) + 48), arg /= 10;
        if (!ans.length())
            ans.push_back(arg + 48);
        reverse(ans.begin(), ans.end());
        return ans;
    }
    Gen()
    {
        x = rand() % (global_answer1 + 1);
        y = rand() % (global_answer1 + 1);
        x_to_str = IntToString(x);
        y_to_str = IntToString(y);
    }
    Gen(int a, int b)
    {
        x = a;
        y = b;
        x_to_str = IntToString(x);
        y_to_str = IntToString(y);
    }
    bool operator== (Gen r)
    {
        return x == y;
    }
    void Print()
    {
        cout << "X = " << x << " Y = " << y << " RESULT1 = " << result1 << " RESULT 2 " << result2 << endl;
    }
};

bool operator< (pair< double, Gen > p1, pair< double, Gen > p2)
{
    return p1.first < p2.first;
}

/**
 * Class ExpressionParser is used to calculate formulae
 * It has two fields: input and answer, where input is a left part of
 * the formula (the formula before '=') and answer is the constant number
 * after '=' Expression parser is used to calculate forumlae with diffrent
 * variables, where variables are set by the user
 * Syntax of the formulae:
 * The left part of the formula is the correct expression with +, -, * and functions pow[x,y],
 * cos[x], log[x], sinh[x], cosh[x], sqrt[x], sqr[x] and brackets. Unary '-' is forbidden
 */
class ExpressionParser
{
    string input;
    int answer;

    /** Checks if the symbol is an operator*/
    bool IsOperator(char c)
    {
        return c == '-' || c == '+' || c == '*' || c == '/';
    }
    /** Sets the priority for the operator*/
    int Priority(char op)
    {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return -1;
    }
    /**Processes operation op for integers*/
    void ProcessOperation(vector< int > &st, char op)
    {
        int b = st.back();
        st.pop_back();
        int a = st.back();
        st.pop_back();

        switch (op)
        {
            case '+':  st.push_back (a + b);  break;
            case '-':  st.push_back (a - b);  break;
            case '*':  st.push_back (a * b);  break;
            case '/':  st.push_back (a / b);  break;
        }
    }
    /**Processes operation op for floats*/
    void ProcessOperation(vector< double > &st, char op)
    {
        double b = st.back();
        st.pop_back();
        double a = st.back();
        st.pop_back();

        switch (op)
        {
            case '+':  st.push_back (a + b);  break;
            case '-':  st.push_back (a - b);  break;
            case '*':  st.push_back (a * b);  break;
            case '/':  st.push_back (a / b);  break;
        }
    }
    /**Caclulates the function, which has the vector of parametres and returns the integer result*/
    int ProcessFunction(string function, vector< int > arguments)
    {
        if (function == "pow")
        {
            int x = arguments[1];
            int ans = 1;
            int power = arguments[0];
            for (int i = 0; i < (int)power; ++i)
                ans *= x;
            return ans;
        }
        if (function == "abs") return mabs(arguments.back());
        if (function == "sin") return sin(arguments.back() * 3.14 / 180.);
        if (function == "cos") return cos(arguments.back() * 3.14 / 180.);
        if (function == "log") return log(arguments.back());
        if (function == "sqrt") return sqrt(arguments.back());
        if (function == "sqr") return arguments.back() * arguments.back();
        if (function == "sinh") return sinh(arguments.back());
        if (function == "cosh") return cosh(arguments.back());
        return 0;
    }
    /**Caclulates the function, which has the vector of parametres and returns the float result*/
    double ProcessFunction(string function, vector< double > arguments)
    {
        if (function == "pow")
        {
            double x = arguments[1];
            double ans = 1;
            double power = arguments[0];
            for (int i = 0; i < (int)power; ++i)
                ans *= x;
            return ans;
        }
        if (function == "abs") return mabs(arguments.back());
        if (function == "sin") return sin(arguments.back() * 3.14 / 180.);
        if (function == "cos") return cos(arguments.back() * 3.14 / 180.);
        if (function == "log") return log(arguments.back());
        if (function == "sqrt") return sqrt(arguments.back());
        if (function == "sqr") return arguments.back() * arguments.back();
        if (function == "sinh") return sinh(arguments.back());
        if (function == "cosh") return cosh(arguments.back());
        return 0;
    }
    /**Calculates the formulae, which is set as a string and returns integer value*/
    int CalculateInt(string s)
    {
        vector< char > op;
        vector< int > st;
        for (int i = 0; i < (int)s.length(); ++i)
        {
            if (s[i] != ' ' && !(s[i] >= '0' && s[i] <= '9'))
            {
                if ((s[i] >= 'a' && s[i] <= 'z') || s[i] == ',')
                    op.push_back(s[i]);
                else if (s[i] == '[')
                    op.push_back(s[i]);
                else if (s[i] == ']')
                {
                    vector< int > arguments;
                    while (op.back() != '[')
                    {
                        if (op.back() == ',')
                        {
                            arguments.push_back(st.back());
                            st.pop_back();
                            op.pop_back();
                        }
                        else
                            ProcessOperation(st, op.back()), op.pop_back();
                    }
                    arguments.push_back(st.back());
                    st.pop_back();
                    op.pop_back();
                    string function;
                    while (op.back() >= 'a' && op.back() <= 'z')
                        function.push_back(op.back()), op.pop_back();
                    reverse(function.begin(), function.end());
                    st.push_back(ProcessFunction(function, arguments));
                }
                else if (s[i] == '(')
                    op.push_back('(');
                else if (s[i] == ')')
                {
                    while (op.back() != '(')
                        ProcessOperation(st, op.back()), op.pop_back();
                    op.pop_back();
                }
                else if (IsOperator(s[i]))
                {
                    char current_operator = s[i];
                    while (!op.empty() && Priority(op.back()) >= Priority(current_operator))
                        ProcessOperation(st, op.back()), op.pop_back();
                    op.push_back(current_operator);
                }
            }
            else
            {
                if (s[i] == ' ') continue;
                string number;
                number.push_back(s[i]);
                while (i + 1 < (int)s.length() && s[i + 1] >= '0' && s[i + 1] <= '9')
                    number.push_back(s[++i]);
                st.push_back(atoi(number.c_str()));
            }
        }
        while (!op.empty())
            ProcessOperation(st, op.back()),  op.pop_back();
        return st.back();
    }
    /**Calculates the formulae, which is set as a string and returns float value*/
    double CalculateDouble(string s)
    {
        vector< char > op;
        vector< double > st;
        for (int i = 0; i < (int)s.length(); ++i)
        {
            if (s[i] != ' ' && !(s[i] >= '0' && s[i] <= '9') && s[i] != '.')
            {
                //------Unary
                if (s[i] == '(' && s[i + 1] == '-')
                {
                    string number;
                    number.push_back(s[++i]);
                    while (i + 1 < (int)s.length() && ((s[i + 1] >= '0' && s[i + 1] <= '9') || s[i + 1] == '.'))
                        number.push_back(s[++i]);
                    st.push_back(atof(number.c_str()));
                    ++i;
                    continue;
                }
                //------End of Unary

                if ((s[i] >= 'a' && s[i] <= 'z') || s[i] == ',')
                    op.push_back(s[i]);
                else if (s[i] == '[')
                    op.push_back(s[i]);
                else if (s[i] == ']')
                {
                    vector< double > arguments;
                    while (op.back() != '[')
                    {
                        if (op.back() == ',')
                        {
                            arguments.push_back(st.back());
                            st.pop_back();
                            op.pop_back();
                        }
                        else
                            ProcessOperation(st, op.back()), op.pop_back();
                    }
                    arguments.push_back(st.back());
                    st.pop_back();
                    op.pop_back();
                    string function;
                    while (op.back() >= 'a' && op.back() <= 'z')
                        function.push_back(op.back()), op.pop_back();
                    reverse(function.begin(), function.end());
                    st.push_back(ProcessFunction(function, arguments));
                }
                else if (s[i] == '(')
                    op.push_back('(');
                else if (s[i] == ')')
                {
                    while (op.back() != '(')
                        ProcessOperation(st, op.back()), op.pop_back();
                    op.pop_back();
                }
                else if (IsOperator(s[i]))
                {
                    char current_operator = s[i];
                    while (!op.empty() && Priority(op.back()) >= Priority(current_operator))
                        ProcessOperation(st, op.back()), op.pop_back();
                    op.push_back(current_operator);
                }
            }
            else
            {
                if (s[i] == ' ') continue;
                string number;
                number.push_back(s[i]);
                while (i + 1 < (int)s.length() && ((s[i + 1] >= '0' && s[i + 1] <= '9') || s[i + 1] == '.'))
                    number.push_back(s[++i]);
                st.push_back(atof(number.c_str()));
            }
        }
        while (!op.empty())
            ProcessOperation(st, op.back()),  op.pop_back();
        return st.back();
    }
public:
    ExpressionParser(){};
    ExpressionParser(string s, int &ans)
    {
        string strans;
        bool flag = false;
        for (size_t i = 0; i < s.length(); ++i)
        {
            if (s[i] == '=')
            {
                flag = true;
                continue;
            }
            if (s[i] != ' ' && !flag)
                input.push_back(s[i]);
            if (s[i] != ' ' && flag)
                strans.push_back(s[i]);
        }
        answer = atoi(strans.c_str());
        ans = answer;
    }
    /** Returns the result of the formula, with the substituted x and y and returns float value*/
    double Result(double x, double y)
    {
        string ready;
        string x_in_string;
        string y_in_string;
        //double -> string for x
        ostringstream forx;
        forx << x;
        // double -> string for y
        ostringstream fory;
        fory << y;

        x_in_string = forx.str();
        y_in_string = fory.str();

        for (size_t i = 0; i < input.length(); ++i)
        {
            if (input[i] == 'x')
                ready += x_in_string;
            else if (input[i] == 'y')
                ready += y_in_string;
            else ready.push_back(input[i]);
        }
        return CalculateDouble(ready);
    }
    /** Returns the result of the formula, with the substituted x and y from the gens alleles and returns int value*/
    int Result(Gen gen)
    {
        string ready;
        for (size_t i = 0; i < input.length(); ++i)
        {
            if (input[i] == 'x')
                ready += gen.x_to_str;
            else if (input[i] == 'y')
                ready += gen.y_to_str;
            else ready.push_back(input[i]);
        }
        return CalculateInt(ready);
    }
};


/**
 * Class Diofantine is used for finding the roots of the set of two equations
 * by using a genetic algorithm.
 * parser1 - the first expression
 * parser2 - the second expression
 * curgeneration - current generation of the individuals
 * sumfitness - sum of all the fitnesses of the individuals in the generation, used for
 * calculating normalized fitnesses for each gen
*/
class Diofantine
{
public:
    ExpressionParser parser1, parser2;
    vector< Gen > curgeneration;
    double sumfitness;
    Diofantine(string s1, string s2): parser1(s1, global_answer1), parser2(s2, global_answer2)
    {
        sumfitness = 0;
        for (int i = 0; i < POPULATION_SIZE; ++i)
            curgeneration.push_back(Gen());
    }
    /** Initializes current generations*/
    void InitCurgeneration()
    {
        sumfitness = 0;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            curgeneration[i].result1 = parser1.Result(curgeneration[i]);
            curgeneration[i].result2 = parser2.Result(curgeneration[i]);
            curgeneration[i].delta1 = mabs(curgeneration[i].result1 - global_answer1);
            curgeneration[i].delta2 = mabs(curgeneration[i].result2 - global_answer2);
            curgeneration[i].fitness = 1. / double(curgeneration[i].delta1 + curgeneration[i].delta2);
            sumfitness += curgeneration[i].fitness;
        }
        for (int i = 0; i < POPULATION_SIZE; ++i)
            curgeneration[i].likelihood += (i - 1 >= 0 ? curgeneration[i].likelihood : 0.) + curgeneration[i].fitness / sumfitness;
    }

    /**
     * Returns random individual from the current generation by the number, where each
     * individual has its own range of the number according to which this individual can be chosen
     */
    Gen GetByNumber(int probobility)
    {
        Gen ans = curgeneration[0];
        for (int i = 0; i < POPULATION_SIZE; ++i)
            if (curgeneration[i].likelihood < double(probobility))
                ans = curgeneration[i];
        return ans;
    }
    /**
     * THE SET OF FUNCTIONS FOR CHOSING PARENTS
     * All of the functions return the vector of pairs of parents
     */

    /**
     * The easiest way of choosing parents, where the random number from 1 to n
     * is set for each individual. The individual with this random number is the
     * second parent
     */
    vector< pair< Gen, Gen> > Panmixia()
    {
        vector< pair< Gen, Gen > > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
            ans.push_back(make_pair(curgeneration[i], curgeneration[rand() % POPULATION_SIZE]));
        return ans;
    }

    /**
     * The first parent is random-chosen and the second parent is chosen as
     * the closest one to the first parent
     */
    vector< pair< Gen, Gen > > Inbreeding()
    {
        vector< pair< Gen, Gen > > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Gen par1 = curgeneration[rand() % POPULATION_SIZE];
            Gen par2 = curgeneration[0];
            for (int iter = 0; iter != MAXITER; ++iter)
                if (par1 == par2)
                    par2 = curgeneration[rand() % POPULATION_SIZE];
                else break;
            for (int j = 0; j < POPULATION_SIZE; ++j)
            {
                if (curgeneration[j] == par1)
                    continue;
                if (mabs(curgeneration[j].delta1 + curgeneration[j].delta2 - par1.delta1 + par1.delta2) < mabs(par2.delta1 + par2.delta2 - par1.delta1 - par1.delta2))
                    par2 = curgeneration[j];
            }
            ans.push_back(make_pair(par1, par2));
        }
        return ans;
    }

    /**
     * The first parent is random-chosen and the second parent is chosen as
     * the less close one to the first parent
     */
    vector< pair< Gen, Gen > > Outbreeding()
    {
        vector< pair< Gen, Gen > > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Gen par1 = curgeneration[rand() % POPULATION_SIZE];
            Gen par2 = curgeneration[0];
            for (int iter = 0; iter != MAXITER; ++iter)
                if (par1 == par2)
                    par2 = curgeneration[rand() % POPULATION_SIZE];
                else break;

            for (int j = 0; j < POPULATION_SIZE; ++j)
            {
                if (curgeneration[j] == par1)
                    continue;
                    if (mabs(curgeneration[j].delta1 + curgeneration[j].delta2 - par1.delta1 + par1.delta2) > mabs(par2.delta1 + par2.delta2 - par1.delta1 - par1.delta2))
                    par2 = curgeneration[j];
            }
            ans.push_back(make_pair(par1, par2));
        }
        return ans;
    }

    /**
     * It is allowed for the individual to be parent only if individual's fitness is
     * greater than a set threshold
     */
    vector< pair< Gen, Gen > > Selection()
    {
        vector< pair< Gen, Gen > > ans;
        double avgfitness = sumfitness / double(POPULATION_SIZE);
        vector< Gen > stillingame;
        for (int i = 0; i < POPULATION_SIZE; ++i)
            if (curgeneration[i].fitness > avgfitness)
                stillingame.push_back(curgeneration[i]);
        for (int i = 0; i < POPULATION_SIZE; ++i)
            ans.push_back(make_pair(stillingame[rand() % stillingame.size()], stillingame[rand() % stillingame.size()]));
        return ans;
    }

    /**
     * T random individuals are chosen N times and the list of the individuals
     * on the current step is sorted by the fitness. We chose the best individual
     * on the each step of algorithm and make the list of N individuals. Then we
     * chose 2*N random individuals from this list, which are the parents
     */
    vector< pair< Gen, Gen > > TournamentSelection()
    {
        vector< pair< Gen, Gen > > ans;
        vector< Gen > best;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            vector< Gen > current_step;
            for (int j = 0; j < TOURNIR_CONST; ++j)
                current_step.push_back(curgeneration[rand() % POPULATION_SIZE]);
            Gen winner = current_step[0];
            for (int j = 0; j < TOURNIR_CONST; ++j)
                if (current_step[j].fitness > winner.fitness)
                    winner = current_step[j];
            best.push_back(winner);
        }
        for (int i = 0; i < POPULATION_SIZE; ++i)
            ans.push_back(make_pair(best[rand() % POPULATION_SIZE], best[rand() % POPULATION_SIZE]));
        return ans;
    }

    /**
     * Individuals are chosen by the fitness
     * The more the fitness is the more the chance of the individual is to be chosen
     */
    vector< pair< Gen, Gen > > RouletteWheelSelection()
    {
        vector< pair< Gen, Gen > > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Gen par1 = GetByNumber(rand() % 101);
            Gen par2 = GetByNumber(rand() % 101);
            for (int iter = 0; iter != MAXITER; ++iter)
                if (par1 == par2)
                    par2 = GetByNumber(rand() % 101);
                else break;
            ans.push_back(make_pair(par1, par2));
        }
        return ans;
    }

    /**
     * Recombinate the alleles of two individuals in a random way by
     * changing first alleles or second alleles with the same probobility
     */
    vector< Gen > Crossover(vector< pair< Gen, Gen > > parents)
    {
        vector< Gen > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Gen son;
            if (rand() % 2)
            {
                son.x = parents[i].first.x;
                son.y = parents[i].second.y;
            }
            else
            {
                son.x = parents[i].second.x;
                son.y = parents[i].first.y;
            }
            ans.push_back(son);
        }
        return ans;
    }

    /**
     * Mutation
     * Choses if the current is mutated with the probobility MUTAUION_PROBOBILITY
     * If it is, then choses for each allele if it is mutated. If it is, then
     * 30% proboblity - current value + random number from 0 to 14
     * 30% probobility - |current value - random number from 0 to 14|
     * 30% probobility - the new value from 0 to max possible
     */
    vector< Gen > Mutation(vector< Gen > children)
    {
        vector< Gen > ans;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            if (!(rand() % (100 / MUTATION_PROBOBILITY)))
            {
                if (!(rand() % (100 / CUR_GEN_MUTATION_PROBOBILITY)))
                {
                    //cout << "MUTATION1\n";
                    int dividing = rand() % 3;
                    if (dividing == 0)
                        children[i].x += rand() % ((MAX_ROOT + 10 )/ 2);
                    if (dividing == 1)
                        children[i].x = mabs(children[i].x - rand() % ((MAX_ROOT + 10 )/ 3));
                    if (dividing == 2)
                        children[i].x = rand() % (MAX_ROOT + 1);
                }
                if (!(rand() % (100 / CUR_GEN_MUTATION_PROBOBILITY)))
                {
                    //cout << "MUTATION2\n";
                    int dividing = rand() % 3;
                    if (dividing == 0)
                        children[i].y += rand() % ((MAX_ROOT + 10 )/ 2);
                    if (dividing == 1)
                        children[i].y = mabs(children[i].x - rand() % ((MAX_ROOT + 10 )/ 3));
                    if (dividing == 2)
                        children[i].y = rand() % (MAX_ROOT + 1);
                }
            }
            children[i].x_to_str = children[i].IntToString(children[i].x);
            children[i].y_to_str = children[i].IntToString(children[i].y);
            ans.push_back(children[i]);
        }
        return ans;
    }

    /**
     * Choses the individuals, which continues theirs existance
     * by the sorting current generation and the generation of the children
     * by the fitness and chooses the best N individuals
     */
    vector< Gen > EliteSelection(vector< Gen> children)
    {
        vector< Gen > ans;
        vector< pair< double, Gen > > sortbyfitness;
        double elitesumfitness = sumfitness;
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            children[i].result1 = parser1.Result(children[i]);
            children[i].result2 = parser2.Result(children[i]);
            children[i].delta1 = mabs(children[i].result1 - global_answer1);
            children[i].delta2 = mabs(children[i].result2 - global_answer2);
            children[i].fitness = 1. / double(children[i].delta1 + children[i].delta2);
            elitesumfitness += children[i].fitness;
        }
        for (int i = 0; i < POPULATION_SIZE; ++i)
            sortbyfitness.push_back(make_pair(curgeneration[i].fitness / elitesumfitness, curgeneration[i]));
        for (int i = 0; i < POPULATION_SIZE; ++i)
            sortbyfitness.push_back(make_pair(children[i].fitness / elitesumfitness, children[i]));
        sort(sortbyfitness.begin(), sortbyfitness.end());
        reverse(sortbyfitness.begin(), sortbyfitness.end());
        for (int i = 0; i < POPULATION_SIZE; ++i)
            ans.push_back(sortbyfitness[i].second);
        return ans;
    }

    /**
     * Solves the set of the equations with the parameters
     * for the selection, choosing parents and mutation
     */
    void Solve()
    {
        for (int generations = 0; generations < MAXGENERATIONS; ++generations)
        {
            /**DELETE THIS COMMENT IF YOU WANT TO SEE THE PROGRESS*///cout << "Generation " << generations << endl;
            InitCurgeneration();
            for (int i = 0; i < POPULATION_SIZE; ++i)
            {
                /**DELETE THIS COMMENT IF YOU WANT TO SEE THE PROGRESS*///curgeneration[i].Print();
                if (curgeneration[i].delta1 == 0 && curgeneration[i].delta2 == 0)
                {
                    cout << "SOLUTION HAS BEEN FOUND IN THE " << generations << " GENERATION: x = " << curgeneration[i].x << ", y = " << curgeneration[i].y << "\n";
                    return;
                }
            }
            /**DELETE THIS COMMENT IF YOU WANT TO SEE THE PROGRESS*///cin.get();

            vector< pair< Gen, Gen > > parents = RouletteWheelSelection();
            vector< Gen > children = Crossover(parents);
            children = Mutation(children);
            curgeneration = EliteSelection(children);
        }
        cout << "SOLUTION HAS NOT BEEN FOUND \n";
    }
};

int main()
{
    //freopen("input.txt", "r", stdin);
    srand(time(NULL));

    //for (int i = 0; i < 10; ++i)
    //{
    //    string in = "input";
    //    in.push_back(i + 48);
    //    in += ".txt";
    //    freopen(in.c_str(), "r", stdin);
        string s1, s2;
        getline(cin, s1);
        getline(cin, s2);
        Diofantine A(s1, s2);
        A.Solve();
    //}
    return 0;
}
