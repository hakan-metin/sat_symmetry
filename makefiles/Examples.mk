
EXAMPLES := examples/

examples: default $(BIN)CNFBlissSymmetries $(BIN)FullStatsCNF
solvers: default $(BIN)minisat

$(call REQUIRE-DIR, $(BIN)CNFBlissSymmetries)
$(call REQUIRE-DIR, $(BIN)FullStatsCNF)
$(call REQUIRE-DIR, $(BIN)minisat)

$(BIN)CNFBlissSymmetries: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)CNFBlissSymmetries: $(EXAMPLES)CNFBlissSymmetries.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))

$(BIN)FullStatsCNF: LDFLAGS += -lcosy -lbliss  -lz
$(BIN)FullStatsCNF: $(EXAMPLES)FullStatsCNF.cc
	$(call cmd-cxx-bin, $@, $<, $(LDFLAGS))


##### Solvers
# Minisat

FORCE:
$(BIN)minisat: FORCE
	$(call cmd-make, , $(EXAMPLES)solvers/minisat/core)
	$(call cmd-cp, $@, $(EXAMPLES)solvers/minisat/core/minisat)

clean-solvers:
	$(call cmd-make, clean, $(EXAMPLES)solvers/minisat/core)


.PHONY: examples FORCE
