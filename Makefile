COMPILER = g++
FLAGS = -std=c++14 -Wall -Werror -Wvla -Wextra -pedantic -O3 

test: VariantTypeImpl VariantType unit_test
	$(COMPILER) $(FLAGS) VariantTypeImpl.o VariantType.o test.o -o test_exec
	./test_exec

VariantTypeImpl: VariantTypeImpl.cpp
	$(COMPILER) $(FLAGS) -c VariantTypeImpl.cpp

VariantType: VariantType.cpp
	$(COMPILER) $(FLAGS) -c VariantType.cpp

unit_test: test.cpp
	$(COMPILER) $(FLAGS) -c test.cpp

clean: 
	rm -f *.o
	rm -f test_exec
