#include <gtest/gtest.h>

#include <random>
#include <string>
#include <vector>

#include "rt-pg/property_graph.h"
#include "uuid.h"

TEST(TEST_PG, TEST_PG_ADD_NODE_WITHOUT_UUID_OR_PROPERTIES) {
  rtpg::PropertyGraph pg;
  uuids::uuid_name_generator gen(uuids::uuid_namespace_url);
  std::string s_mid1 = "http://example.com/model1";
  std::string s_mid2 = "http://example.com/model2";
  uuids::uuid mid1 = gen(s_mid1);
  uuids::uuid mid2 = gen(s_mid2);
  std::vector<uuids::uuid> mids = {mid1, mid2};
  auto ret = pg.AddNode(mids);
  EXPECT_TRUE(ret.has_value());
  rtpg::Handle h = ret.value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h);
  EXPECT_EQ(ptr->mids().at(0), mid1);
  EXPECT_EQ(ptr->mids().at(1), mid2);
}

TEST(TEST_PG, TEST_PG_ADD_NODE_WITH_UUID_NO_PROPERTIES) {
  rtpg::PropertyGraph pg;
  std::random_device rd;
  std::mt19937 gen(rd());
  uuids::uuid_random_generator uuid_rangen(&gen);
  uuids::uuid_name_generator uuid_name_gen(uuids::uuid_namespace_url);
  std::string s_mid1 = "http://example.com/model1";
  std::string s_mid2 = "http://example.com/model2";
  uuids::uuid mid1 = uuid_name_gen(s_mid1);
  uuids::uuid mid2 = uuid_name_gen(s_mid2);
  std::vector<uuids::uuid> mids = {mid1, mid2};
  uuids::uuid uid = uuid_rangen();
  auto ret = pg.AddNode(uid, mids);
  EXPECT_TRUE(ret.has_value());
  rtpg::Handle h = ret.value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h);
  EXPECT_EQ(ptr->mids().at(0), mid1);
  EXPECT_EQ(ptr->mids().at(1), mid2);
  EXPECT_EQ(ptr->id(), uid);
}

TEST(TEST_PG, TEST_PG_ADD_NODE_WITHOUT_UUID_WITH_PROPERTIES) {
  rtpg::PropertyGraph pg;
  uuids::uuid_name_generator gen(uuids::uuid_namespace_url);
  std::string s_mid1 = "http://example.com/model1";
  std::string s_mid2 = "http://example.com/model2";
  uuids::uuid mid1 = gen(s_mid1);
  uuids::uuid mid2 = gen(s_mid2);
  std::vector<uuids::uuid> mids = {mid1, mid2};
  rtpg::Properties props = {
      {"BoolProp", rtpg::Property{true}},
      {"IntProp", rtpg::Property{42l}},
      {"DoubleProp", rtpg::Property{42.0}},
      {"StringProp", rtpg::Property{std::string("Hello!")}}};

  auto ret = pg.AddNode(mids, props);
  EXPECT_TRUE(ret.has_value());
  rtpg::Handle h = ret.value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h);
  EXPECT_EQ(ptr->mids().at(0), mid1);
  EXPECT_EQ(ptr->mids().at(1), mid2);
  EXPECT_TRUE(std::get<bool>(ptr->properties().at("BoolProp")));
  EXPECT_EQ(std::get<int64_t>(ptr->properties().at("IntProp")), 42l);
  EXPECT_NEAR(std::get<double>(ptr->properties().at("DoubleProp")), 42.0,
              0.001);
  EXPECT_EQ(std::get<std::string>(ptr->properties().at("StringProp")),
            "Hello!");
}

// TODO: Add cases that fail to add a node
TEST(TEST_PG, TEST_PG_ADD_NODE_EXISTING_UUID) {
  rtpg::PropertyGraph pg;
  auto ret1 = pg.AddNode({});
  rtpg::Handle h = ret1.value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h);
  auto ret2 = pg.AddNode(ptr->id(), {});
  EXPECT_FALSE(ret2.has_value());
}

// Add edge tests
TEST(TEST_PG, TEST_PG_ADD_EDGE_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  uuids::uuid uid1 = pg.GetConstNodePtr(h1)->id();
  rtpg::Handle h2 = pg.AddNode({}).value();
  uuids::uuid uid2 = pg.GetConstNodePtr(h2)->id();
  auto ret = pg.AddEdge(uid1, uid2, uuids::uuid());
  EXPECT_TRUE(ret.has_value());
  rtpg::Handle edge_handle = ret.value();
  EXPECT_EQ(pg.GetConstEdgePtr(edge_handle)->tail(), h1);
  EXPECT_EQ(pg.GetConstEdgePtr(edge_handle)->head(), h2);
  EXPECT_EQ(pg.GetConstNodePtr(h1)->out_edges()[0], edge_handle);
  EXPECT_EQ(pg.GetConstNodePtr(h2)->in_edges()[0], edge_handle);
}

TEST(TEST_PG, TEST_PG_ADD_EDGE_UUID_MISSING_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  uuids::uuid uid1 = pg.GetConstNodePtr(h1)->id();
  auto ret = pg.AddEdge(uid1, uuids::uuid(), uuids::uuid());
  EXPECT_FALSE(ret.has_value());
}

TEST(TEST_PG, TEST_PG_ADD_EDGE_UUID_SAME_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  uuids::uuid uid1 = pg.GetConstNodePtr(h1)->id();
  auto ret = pg.AddEdge(uid1, uid1, uuids::uuid());
  EXPECT_FALSE(ret.has_value());
}

TEST(TEST_PG, TEST_PG_ADD_EDGE_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  auto ret = pg.AddEdge(h1, h2, uuids::uuid());
  EXPECT_TRUE(ret.has_value());
  rtpg::Handle edge_handle = ret.value();
  EXPECT_EQ(pg.GetConstEdgePtr(edge_handle)->tail(), h1);
  EXPECT_EQ(pg.GetConstEdgePtr(edge_handle)->head(), h2);
  EXPECT_EQ(pg.GetConstNodePtr(h1)->out_edges()[0], edge_handle);
  EXPECT_EQ(pg.GetConstNodePtr(h2)->in_edges()[0], edge_handle);
}

TEST(TEST_PG, TEST_PG_ADD_EDGE_HANDLE_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2;
  h2.index = 42;
  h2.pattern = 42;
  auto ret = pg.AddEdge(h1, h2, uuids::uuid());
  EXPECT_FALSE(ret.has_value());
}

TEST(TEST_PG, TEST_PG_ADD_EDGE_HANDLE_SAME_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  auto ret = pg.AddEdge(h1, h1, uuids::uuid());
  EXPECT_FALSE(ret.has_value());
}

// Remove node tests
TEST(TEST_PG, TEST_PG_REMOVE_NODE_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  uuids::uuid uid = pg.GetConstNodePtr(h1)->id();
  EXPECT_TRUE(pg.RemoveNode(uid));
  EXPECT_EQ(nullptr, pg.GetConstNodePtr(uid));
  EXPECT_EQ(nullptr, pg.GetConstNodePtr(h1));
}

TEST(TEST_PG, TEST_PG_REMOVE_NODE_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  EXPECT_FALSE(pg.RemoveNode(uuids::uuid()));
}

TEST(TEST_PG, TEST_PG_REMOVE_NODE_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  EXPECT_TRUE(pg.RemoveNode(h1));
  EXPECT_EQ(nullptr, pg.GetConstNodePtr(h1));
}

TEST(TEST_PG, TEST_PG_REMOVE_NODE_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1;
  h1.index = 42;
  EXPECT_FALSE(pg.RemoveNode(h1));
}

TEST(TEST_PG, TEST_PG_REMOVE_NODE_REMOVES_EDGE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  auto ret = pg.AddEdge(h1, h2, uuids::uuid());
  EXPECT_TRUE(ret.has_value());
  EXPECT_FALSE(pg.GetConstNodePtr(h2)->in_edges().empty());
  EXPECT_TRUE(pg.RemoveNode(h1));
  EXPECT_TRUE(pg.GetConstNodePtr(h2)->in_edges().empty());
}

// Remove edge tests
TEST(TEST_PG, TEST_PG_REMOVE_EDGE_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  auto ret = pg.AddEdge(h1, h2, uuids::uuid());
  EXPECT_TRUE(ret.has_value());
  EXPECT_TRUE(pg.RemoveEdge(pg.GetConstEdgePtr(ret.value())->id()));
  EXPECT_TRUE(pg.GetConstNodePtr(h1)->out_edges().empty());
  EXPECT_TRUE(pg.GetConstNodePtr(h2)->in_edges().empty());
}

TEST(TEST_PG, TEST_PG_REMOVE_EDGE_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  EXPECT_FALSE(pg.RemoveEdge(uuids::uuid()));
}

TEST(TEST_PG, TEST_PG_REMOVE_EDGE_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  auto ret = pg.AddEdge(h1, h2, uuids::uuid());
  EXPECT_TRUE(ret.has_value());
  EXPECT_TRUE(pg.RemoveEdge(ret.value()));
  EXPECT_TRUE(pg.GetConstNodePtr(h1)->out_edges().empty());
  EXPECT_TRUE(pg.GetConstNodePtr(h2)->in_edges().empty());
}

TEST(TEST_PG, TEST_PG_REMOVE_EDGE_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h;
  h.index = 42;
  h.pattern = 42;
  EXPECT_FALSE(pg.RemoveEdge(h));
}

TEST(TEST_PG, TEST_PG_GET_NODE_PTR_UUID) {
  rtpg::PropertyGraph pg;
  std::random_device rd;
  std::mt19937 gen(rd());
  uuids::uuid_random_generator uuid_rangen(&gen);
  uuids::uuid uid = uuid_rangen();
  pg.AddNode(uid, {}).value();
  rtpg::NodePtr ptr = pg.GetNodePtr(uid);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_NODE_PTR_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  std::random_device rd;
  std::mt19937 gen(rd());
  uuids::uuid_random_generator uuid_rangen(&gen);
  uuids::uuid uid = uuid_rangen();
  pg.AddNode(uid, {}).value();
  rtpg::NodePtr ptr = pg.GetNodePtr(uuids::uuid());
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_NODE_PTR_UUID) {
  rtpg::PropertyGraph pg;
  std::random_device rd;
  std::mt19937 gen(rd());
  uuids::uuid_random_generator uuid_rangen(&gen);
  uuids::uuid uid = uuid_rangen();
  pg.AddNode(uid, {}).value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(uid);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_NODE_PTR_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  std::random_device rd;
  std::mt19937 gen(rd());
  uuids::uuid_random_generator uuid_rangen(&gen);
  uuids::uuid uid = uuid_rangen();
  pg.AddNode(uid, {}).value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(uuids::uuid());
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_NODE_PTR_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::NodePtr ptr = pg.GetNodePtr(h1);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_NODE_PTR_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  h1.index = 42;
  rtpg::NodePtr ptr = pg.GetNodePtr(h1);
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_NODE_PTR_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h1);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_NODE_PTR_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  h1.index = 42;
  rtpg::ConstNodePtr ptr = pg.GetConstNodePtr(h1);
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_EDGE_PTR_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  rtpg::EdgePtr ptr = pg.GetEdgePtr(pg.GetConstEdgePtr(eh1)->id());
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_EDGE_PTR_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  pg.AddEdge(h1, h2, uuids::uuid());
  rtpg::EdgePtr ptr = pg.GetEdgePtr(uuids::uuid());
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_EDGE_PTR_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  rtpg::ConstEdgePtr ptr = pg.GetConstEdgePtr(pg.GetConstEdgePtr(eh1)->id());
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_EDGE_PTR_WRONG_UUID) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  pg.AddEdge(h1, h2, uuids::uuid());
  rtpg::ConstEdgePtr ptr = pg.GetConstEdgePtr(uuids::uuid());
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_EDGE_PTR_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  rtpg::EdgePtr ptr = pg.GetEdgePtr(eh1);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_EDGE_PTR_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  eh1.index = 42;
  rtpg::EdgePtr ptr = pg.GetEdgePtr(eh1);
  ASSERT_EQ(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_EDGE_PTR_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  rtpg::ConstEdgePtr ptr = pg.GetConstEdgePtr(eh1);
  ASSERT_NE(nullptr, ptr);
}

TEST(TEST_PG, TEST_PG_GET_CONST_EDGE_PTR_WRONG_HANDLE) {
  rtpg::PropertyGraph pg;
  rtpg::Handle h1 = pg.AddNode({}).value();
  rtpg::Handle h2 = pg.AddNode({}).value();
  rtpg::Handle eh1 = pg.AddEdge(h1, h2, uuids::uuid()).value();
  eh1.index = 42;
  rtpg::ConstEdgePtr ptr = pg.GetConstEdgePtr(eh1);
  ASSERT_EQ(nullptr, ptr);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}