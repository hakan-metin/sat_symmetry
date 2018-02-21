
EXAMPLES := examples/

examples: $(BIN)CNFBlissSymmetries $(BIN)CNFSaucySymmetries
solvers: $(BIN)minisat $(BIN)minisat_static

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)CNFSaucySymmetries)
$(call REQUIRE-DIR, $(BIN)minisat)

$(BIN)CNFBlissSymmetries: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)CNFBlissSymmetries: $(EXAMPLES)CNFBlissSymmetries.cc | default
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

$(BIN)CNFSaucySymmetries: LDFLAGS += -lcosy -lsaucy  -lz
$(BIN)CNFSaucySymmetries: $(EXAMPLES)CNFSaucySymmetries.cc | default
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))


##### Solvers
# Minisat

FORCE:
$(BIN)minisat: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)
	$(call cmd-make, , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat)

FORCE:
$(BIN)minisat_static: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)
	$(call cmd-make, rs , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat_static)

clean-solvers:
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)


.PHONY: examples FORCE
