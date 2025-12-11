#include<sys/random.h>
#include<iostream>
#include<cmath>
#include<cassert>

class pboolean {
	float value;

public:
	template<typename name>
	static pboolean compareRangedValues(name left, name right, name lowest, name highest) {
		double normalLeft = (double)left;
		double normalRight = (double)right;
		double normalLowest = (double)lowest;
		double normalHighest = (double)highest;
		normalLeft -= normalLowest;
		normalRight -= normalLowest;
		normalHighest -= normalLowest;
		return std::fabs((normalLeft / normalHighest) - (normalRight / normalHighest));
	}

	static pboolean compareMemory(const void* left, const void* right, size_t size, bool areStrings = false) {
		const unsigned char* leftAccessible = reinterpret_cast<const unsigned char*>(left);
		const unsigned char* rightAccessible = reinterpret_cast<const unsigned char*>(right);
		size_t index = 0;
		while(index < size && (!areStrings || (leftAccessible[index] != '\0' && rightAccessible[index] != '\0'))) {
			if(leftAccessible[index] != rightAccessible[index]) {
				return 1 - std::fabs((float)leftAccessible[index] - (float)rightAccessible[index]) / 255.0;
			}
			index++;
		}
		return 1;
	}

	pboolean(float value) {
		assert(value >= 0.0);
		assert(value <= 1.0);
		this -> value = value;
	}

	pboolean operator&&(pboolean other) {
		return this -> value * other.value;
	}

	pboolean operator~() {
		return 1 - this -> value;
	}

	pboolean operator||(pboolean other) {
		return ~(~*this && ~other);
	}

	operator bool() {
		uint64_t value;
		getrandom(&value, sizeof(uint64_t), 0);
		float threshold = (long double)value / (long double)(UINT64_MAX - 1);
		return threshold < this -> value;
	}

	pboolean operator==(pboolean other) {
		return std::fabs(this -> value - other.value);
	}

	pboolean operator!=(pboolean other) {
		return ~(*this == other);
	}
};

void testExpression(pboolean value) {
	if(value) {
		std::cout << "true" << std::endl;
	} else {
		std::cout << "false" << std::endl;
	}
}

int main() {
	std::cout << "0.0: ";
	testExpression(0.0);
	std::cout << "0.25: ";
	testExpression(0.25);
	std::cout << "0.5: ";
	testExpression(0.5);
	std::cout << "0.75: ";
	testExpression(0.75);
	std::cout << "1.0: ";
	testExpression(1.0);
	std::cout << "~0.75: ";
	testExpression(~pboolean(0.75));
	std::cout << "0.5 && 0.5: ";
	testExpression(pboolean(0.5) && pboolean(0.5));
	std::cout << "0.75 || 0.75: ";
	testExpression(pboolean(0.75) || pboolean(0.75));
	std::cout << "number comparison: ";
	testExpression(pboolean::compareRangedValues(5, 10, 0, 50));
	std::cout << "memory comparison: ";
	testExpression(pboolean::compareMemory(reinterpret_cast<const void*>("apple"), reinterpret_cast<const void*>("apples and oranges"), 10, true));
}
