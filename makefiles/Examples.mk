
EXAMPLES := examples/

examples: default $(BIN)CNFBlissSymmetries $(BIN)CNFSaucySymmetries
solvers: $(BIN)glucose_release

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)CNFSaucySymmetries)
$(call REQUIRE-DIR, $(BIN)minisat)
$(call REQUIRE-DIR, $(BIN)glucose_release)


$(BIN)CNFBlissSymmetries: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)CNFBlissSymmetries: $(EXAMPLES)CNFBlissSymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

$(BIN)CNFSaucySymmetries: LDFLAGS += -lcosy -lsaucy  -lz
$(BIN)CNFSaucySymmetries: $(EXAMPLES)CNFSaucySymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))


##### Solvers
# Minisat

FORCE:
$(BIN)minisat: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)
	$(call cmd-make, , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat)

$(BIN)glucose_release: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/glucose-3.0/core)
	$(call cmd-make, r, $(EXAMPLES)solvers/glucose-3.0/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/glucose-3.0/core/glucose_release)

clean-solvers:
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)


.PHONY: examples FORCE
