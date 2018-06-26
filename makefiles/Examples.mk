
EXAMPLES := examples/

examples: default $(BIN)CNFBlissSymmetries $(BIN)CNFSaucySymmetries
solvers: $(BIN)minisat_release

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)CNFSaucySymmetries)
$(call REQUIRE-DIR, $(BIN)minisat_release)

$(BIN)CNFBlissSymmetries: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)CNFBlissSymmetries: $(EXAMPLES)CNFBlissSymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

$(BIN)CNFSaucySymmetries: LDFLAGS += -lcosy -lsaucy  -lz
$(BIN)CNFSaucySymmetries: $(EXAMPLES)CNFSaucySymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))


##### Solvers
# Minisat

FORCE:
$(BIN)minisat_release: default FORCE
	$(call cmd-make, clean , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-make, r , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat_release)

clean-solvers:
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)


.PHONY: examples FORCE
