COMPILER = g++
DIRECTORY_WITH_MAIN_HEADER = ./include
FLAGS = -std=c++14 -I ${DIRECTORY_WITH_MAIN_HEADER} \
		-Wall -Werror -Wvla -Wextra -pedantic -O3 
		

test: VariantTypeImpl VariantType unit_test main 
	ar rcs lib/libvarianttype.a VariantTypeImpl.o VariantType.o 
	@rm -f VariantTypeImpl.o VariantType.o
	$(COMPILER) $(FLAGS) lib/libvarianttype.a *.o -o test_exec
	@rm -f *.o
	@./test_exec

VariantTypeImpl: src/VariantTypeImpl.cpp
	$(COMPILER) $(FLAGS) -c src/VariantTypeImpl.cpp

VariantType: src/VariantType.cpp
	$(COMPILER) $(FLAGS) -c src/VariantType.cpp

unit_test: unit_test.cpp
	$(COMPILER) $(FLAGS) -c unit_test.cpp

main: test_main.cpp
	$(COMPILER) $(FLAGS) -c test_main.cpp

memory_track: memory_overrides.cpp
	${COMPILER} ${FLAGS} -c memory_overrides.cpp

debug: FLAGS = -std=c++14 -g3 -DDEBUG
debug: test

clean: 
	rm -f *.o
	rm -f test_exec
	rm -f lib/libvarianttype.a
