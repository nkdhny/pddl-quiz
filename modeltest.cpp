#include <gtest/gtest.h>
#include <iostream>
#include "model.h"


TEST(pddl, mustEnableOperation) {
    objects o;
    o.push_back(object("a"));
    o.push_back(object("b"));

    relation r("r");

    ground_relation gr(r, o);

    state empty;
    state s = empty.add(gr);


    varaibles v;
    v.push_back(varaible(0));
    v.push_back(varaible(1));

    varaibles inv;
    inv.push_back(varaible(1));
    inv.push_back(varaible(0));

    preconditions pre;
    pre.push_back(std::make_pair(r, v));

    preconditions invp;
    invp.push_back(std::make_pair(r, inv));

    preconditions full;
    full.push_back(std::make_pair(r, v));
    full.push_back(std::make_pair(r, inv));


    effects eff;

    operation op(pre, eff);
    operation opInv(invp, eff);
    operation opFull(full, eff);


    EXPECT_TRUE(op.isDefined(s, o));
    EXPECT_FALSE(opInv.isDefined(s, o));
    EXPECT_FALSE(opFull.isDefined(s, o));
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
