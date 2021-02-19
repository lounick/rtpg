#include <gtest/gtest.h>
#include <rt-pg/property_graph.h>

TEST(TEST_PG, TEST_PG_ADD_NODE_WITHOUT_UUID) {
  rtpg::PropertyGraph pg();
  EXPECT_FALSE(true);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}