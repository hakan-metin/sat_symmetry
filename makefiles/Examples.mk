
EXAMPLES := examples/

examples: default $(BIN)CNFBlissSymmetries $(BIN)CNFSaucySymmetries

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)CNFSaucySymmetries)

$(BIN)CNFBlissSymmetries: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)CNFBlissSymmetries: $(EXAMPLES)CNFBlissSymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

$(BIN)CNFSaucySymmetries: LDFLAGS += -lcosy -lsaucy  -lz
$(BIN)CNFSaucySymmetries: $(EXAMPLES)CNFSaucySymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

.PHONY: examples
