#include <windows.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>

void Solve(long double a, long double b, long double h, long double eps) {
	std::cout << '\n';
	std::cout << "      x          S(x)        Y(x)     abs(S - Y)  n   " << '\n';
	for (long double x = a; x < b + h / 2; x += h) {
		int n = 0;
		long double Y, S, f, p, intp, fracp, trash, fi = M_PI / 4, x_powed = 1;
		// Y = pow(exp, x * cos(fi)) * cos(x * sin(fi));
		__asm
		{
			FLD fi
			FCOS
			FLD x
			FMUL
			FLDL2E
			FMUL
			FSTP p
		}
		__asm {
			FLD p
			FLD1
			FSCALE
			FSTP intp
			FSTP trash
		}
		__asm
		{
			FLD1
			FLD p
			FPREM
			F2XM1
			FADD
			FSTP fracp
		}
		__asm {
			FLD intp
			FLD fracp
			FMUL
			FLD fi
			FSIN
			FLD x
			FMUL
			FCOS
			FMUL
			FSTP Y
		}
		__asm
		{
			FLD1
			FSTP S

			FLD1
			FSTP f
		}
		while (abs(S - Y) > eps) {
			n++;
			__asm
			{
				FLD f
				FILD n
				FMUL
				FSTP f
				FLD x_powed
				FLD x
				FMUL
				FSTP x_powed
			}
			__asm {
				FLD S
				FILD n
				FLD fi
				FMUL
				FCOS
				FLD f
				FDIV
				FLD x_powed
				FMUL
				FADD
				FSTP S
			}
		}
		std::cout << std::setprecision(3) << std::scientific << std::showpos << "  " << x << "  "
			<< S << "  " << Y << "  " << abs(S - Y) << "  " << std::noshowpos
			<< std::right << std::setw(2) << n << "  \n";
	}
}

int main() {
	float a;
	float b;
	float h;
	float eps;
	std::cout << "Enter a: ";
	std::cin >> a;
	std::cout << "Enter b: ";
	std::cin >> b;
	std::cout << "Enter h: ";
	std::cin >> h;
	std::cout << "Enter eps: ";
	std::cin >> eps;
	Solve(a, b, h, eps);
	return 0;
}
