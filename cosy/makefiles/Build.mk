sources := $(wildcard $(SRC)*.cc)
headers := $(wildcard include/cosy/*.h)
headers := $(filter-out %IntegralTypes.h %IntType.h %Macros.h %Bitset.h, \
	$(headers))

objects := $(patsubst %.cc, $(OBJ)%.o, $(sources))

tests := $(wildcard tests/units/*.test.cc)
tests_objects := $(patsubst %.cc, $(OBJ)%.o, $(tests))
tests_objects += $(objects)

lib := libcosy.a

$(call REQUIRE-DIR,  $(LIB)$(lib))
$(call REQUIRE-DIR,  $(BIN)test)
$(call REQUIRE-DIR, $(objects))
$(call REQUIRE-DIR, $(tests_objects))
$(call REQUIRE-DEP, $(sources))
$(call REQUIRE-DEP, $(tests))


$(LIB)$(lib): $(objects)

CFLAGS += -Iinclude/ -fPIC -Wall -Wextra

default: CFLAGS += -O3 -flto -DNDEBUG
default: $(LIB)$(lib)


################################################################################
# TESTS

GTEST_DIR = third_party/gtest/googletest/

test : CFLAGS  +=  -I $(GTEST_DIR)include/ -O0 -D DEBUG -g
test : LDFLAGS +=  -L $(GTEST_DIR) -lgtest -lgtest_main -lpthread -lglog

test: $(BIN)test
run-test: test
	$(call cmd-call, ./$(BIN)test)
run-test-valgrind: test
	$(call cmd-call, valgrind --leak-check=full ./$(BIN)test)
run-test-gdb: test
	$(call  cmd-call, gdb --args ./$(BIN)test)


# Generic rules

$(LIB)%.a:
	$(call cmd-ar, $@, $^)

$(BIN)test: $(tests_objects)
	$(call cmd-ld, $@, $^, $(LDFLAGS))


################################################################################
# STYLE

check-style: $(sources) $(headers)
	$(call cmd-call, ./scripts/cpplint.py --root=$$PWD $^)
