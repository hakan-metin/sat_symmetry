
EXAMPLES := examples/

examples: default $(BIN)CNFBlissSymmetries $(BIN)CNFSaucySymmetries
solvers: $(BIN)minisat
# $(BIN)minisatsimp

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)CNFSaucySymmetries)
$(call REQUIRE-DIR, $(BIN)minisat)
$(call REQUIRE-DIR, $(BIN)minisatsimp)
$(call REQUIRE-DIR, $(BIN)minisat_release)
$(call REQUIRE-DIR, $(BIN)maple_scavel)

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

$(BIN)minisat_release: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)
	$(call cmd-make, r, $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat_release)

$(BIN)minisatsimp: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/simp)
	$(call cmd-make, , $(EXAMPLES)solvers/minisat/simp)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/simp/minisat)

$(BIN)maple_scavel: default FORCE
	$(call cmd-make, clean, $(EXAMPLES)solvers/maple_scavel/core)
	$(call cmd-make, r, $(EXAMPLES)solvers/maple_scavel/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/maple_scavel/core/Maple_LCM_Scavel_release)



clean-solvers:
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)


.PHONY: examples FORCE
