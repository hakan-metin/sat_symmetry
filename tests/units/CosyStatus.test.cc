// // Copyright 2017 Hakan Metin - LIP6

// #include <gtest/gtest.h>

// #include "cosy/CosyStatus.h"

// namespace cosy {


// class CosyStatusTest : public testing::Test {
//  protected:

//     virtual void SetUp() {
//         const int num_vars = 6;

//         // (1 2) (3 -4 5) (-1 -2) (-3 4 -5)
//         permutation = std::unique_ptr<Permutation>(new Permutation(num_vars));

//         // Use implicit conversion of int to Literal
//         permutation->addToCurrentCycle(1);
//         permutation->addToCurrentCycle(2);
//         permutation->closeCurrentCycle();

//         permutation->addToCurrentCycle(3);
//         permutation->addToCurrentCycle(-4);
//         permutation->addToCurrentCycle(5);
//         permutation->closeCurrentCycle();

//         permutation->addToCurrentCycle(-1);
//         permutation->addToCurrentCycle(-2);
//         permutation->closeCurrentCycle();

//         permutation->addToCurrentCycle(-3);
//         permutation->addToCurrentCycle(4);
//         permutation->addToCurrentCycle(-5);
//         permutation->closeCurrentCycle();

//         order = std::unique_ptr<Order>
//             (new IncreaseOrder(num_vars, TRUE_LESS_FALSE));

//         assignment.resize(num_vars);

//         status = std::unique_ptr<CosyStatus>
//             (new CosyStatus(*permutation, *order, assignment));
//     }

//     std::unique_ptr<Permutation> permutation;
//     std::unique_ptr<Order> order;
//     Trail trail;
//     Assignment assignment;

//     std::unique_ptr<CosyStatus> status;
// };

// TEST_F(CosyStatusTest, InitialState) {
//     ASSERT_EQ(status->state(), ACTIVE);
// }

// TEST_F(CosyStatusTest, EmptyOrderNotify) {
//     status->updateNotify(3);
//     ASSERT_EQ(status->state(), ACTIVE);
// }


// }  // namespace cosy
