#include <windows.h>

#include <iostream>
#include <string>
#include <functional>
#include <regex>

std::string Solve(long double a, long double b, long double c) {
	const long double LD_4 = 4.0L;
	if (a == 0) {
		return "Уравнение не является квадратным";
	}
	long double D;
	__asm
	{
		FLD b
		FLD b
		FMUL
		FLD LD_4
		FLD a
		FLD c
		FMUL
		FMUL
		FSUB
		FSTP D
	}
	std::cout << "\nДискриминант: " << D << '\n';
	if (!isfinite(D)) {
		return "Дискриминант не является конечным числом";
	}
	if (D < 0.0L) {
		return "Нет вещественных корней";
	}
	if (D == 0.0L) {
		long double x;
		__asm
		{
			FLD b
			FCHS
			FLD a
			FLD a
			FADD
			FDIV
			FSTP x
		}
		return "x = " + std::to_string(x);
	}
	if (D > 0.0L) {
		long double x1, x2;
		__asm
		{
			FLD b
			FCHS
			FLD D
			FSQRT
			FSUB
			FLD a
			FLD a
			FADD
			FDIV
			FSTP x1
		}
		__asm
		{
			FLD b
			FCHS
			FLD D
			FSQRT
			FADD
			FLD a
			FLD a
			FADD
			FDIV
			FSTP x2
		}
		return "x1 = " + std::to_string(x1) + ", " +
			"x2 = " + std::to_string(x2);
	}
	return "Что-то пошло не так";
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	long double a, b, c;
	std::cout << "Коэффициент а: ";
	std::cin >> a;
	std::cout << "\nКоэффициент b: ";
	std::cin >> b;
	std::cout << "\nКоэффициент c: ";
	std::cin >> c;
	std::cout << '\n';
	if (a != 1) {
		std::cout << ((a == -1) ? '-' : a);
	}
	std::cout << "x^2 " << ((b < 0) ? '-' : '+') << ' ' << std::abs(b) << "x "
		<< ((c < 0) ? '-' : '+') << ' ' << abs(c) << " = 0\n\n" << Solve(a, b, c) << '\n';
	return 0;
}
