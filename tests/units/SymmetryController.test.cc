#include <gtest/gtest.h>

#include "cosy/SymmetryController.h"

namespace cosy {

TEST(SymmetryController, Constructor)  {
    const std::string cnf_filename("tests/resources/simple.cnf");
    const std::string sym_filename("tests/resources/simple.cnf.sym");

    std::unique_ptr<LiteralAdapter<Literal>> adapter
        (new LiteralAdapter<Literal>());

    SymmetryController<Literal> symmetry(cnf_filename, sym_filename, adapter);
}

}  // namespace cosy
