CC = gcc -std=c11
FLAGS = -Wall -Wextra -Werror 
TST_LIBS = -lcheck
ifeq ($(shell uname), Linux)
	TST_LIBS += -lpthread -lrt -lsubunit -lm
endif
SRCS=$(wildcard s21_*.c)
OBJS=$(SRCS:.c=.o)

LIB_NAME = s21_matrix
TEST_FILES = $(wildcard test*.c)

all: clean style test

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

$(LIB_NAME).a: $(OBJS)
	ar rc $(LIB_NAME).a $^
	ranlib $(LIB_NAME).a
	rm -rf *.o

style:
	cp ../materials/linters/.clang-format ./
	clang-format -i *.c *.h
	clang-format -n *.c *.h
	rm .clang-format

test: clean $(LIB_NAME).a
	$(CC) $(FLAGS) $(TEST_FILES) -L. $(LIB_NAME).a $(TST_LIBS) -o test
	./test

add_coverage_flag:
	$(eval FLAGS += --coverage)

gcov_report: add_coverage_flag test
	rm test*.gcda test*.gcno
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory gcov_report
	open ./gcov_report/index.html


clean:
	rm -rf *.o test *.a *.gcno *.gcda *.gcov *.html *.css gcov_report/ coverage.info

rebuild: clean $(LIB_NAME).a