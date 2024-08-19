#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <limits>


// Object to resemble discreet transaction period associated to an investment.
class CashFlow {
public:
    int period;     //t
    double amount;  //  C_t

    CashFlow(int p, double a) :
        period(p), amount(a) {}
};


// Object to resemble and investment, presented as a vector of cashflows.
class Investment {
private:
    std::vector<CashFlow> cashFlows;

public:

    // Allows caller to add individual cashflow events inceementally.
    void addCashFlow(int period, double amount) {
        cashFlows.emplace_back(period, amount);
    }

    // Allows user to view entered cashflows
    void displayCashFlows() const {
        for (const auto& cf : cashFlows) {
            std::cout << "period: " << cf.period << ", amount: " << cf.amount << std::endl;
        }
    }

    double calcNpv(double rate) const {
        double npvValue = 0.0;
        for (const auto& cf : cashFlows) {
            npvValue += cf.amount / std::pow(1 + rate, cf.period);
        }
        return npvValue;
    }

    double calcNpvDerivative(double rate) const {
        double npvDerivativeValue = 0.0;
        for (const auto& cf : cashFlows) {
            npvDerivativeValue -= cf.period * cf.amount / std::pow(1 + rate, cf.period + 1);
        }
        return npvDerivativeValue;
    }

    double irrNewtonRaphson(double guess = 0.1, double tolerance = 1e-6,  int maxIteration = 1000) const {
        double rate = guess;
        for (int i = 0; i < maxIteration; i++) {
            double fValue = calcNpv(rate);
            double fDerivative = calcNpvDerivative(rate);

            if (std::abs(fValue) < std::numeric_limits<double>::epsilon()) {
                std::cerr << "Derivative is zero, can't continue with Newton-Raphson Iteration. " << std::endl;
                return std::numeric_limits<double>::infinity();
            }

            double nextRate = rate - fValue / fDerivative;

            if (std::abs(nextRate - rate) < tolerance) {
                return nextRate;
            }
            rate = nextRate;
        }

        std::cerr << "Max iterations reached, IRR might not have converged. " << std::endl;
        return rate;
    }
};

int main()
{
    Investment investment;   
    double initialInvestment;
    int period = 0;
    double amount;

    std::cout << "Please enter the initial investment, as a negative cashflow: "; 
    std::cin >> initialInvestment;

    investment.addCashFlow(period, initialInvestment);

    while (true) {
        period += 1;
        std::cout << "Please enter a your cashflows following your initial investment, or enter -1 to calculate IRR: ";
        std::cin >> amount;

        if (amount == -1) {
            break;
        }

        investment.addCashFlow(period, amount);
    }

    investment.displayCashFlows();

    double irr = investment.irrNewtonRaphson();

    if (irr == std::numeric_limits<double>::infinity()) {
        std::cout << "IRR calculation failed. " << std::endl;
        return 1;
    }

    std::cout << "Calculate IRR: " << irr * 100 <<  "%" <<std::endl;
       return 0;
}