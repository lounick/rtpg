import unittest
import uuid
import os
import rtpg


class PGTest(unittest.TestCase):
    def testAddNodeWithoutUUIDORProperties(self):
        pg = rtpg.PropertyGraph()
        s_mid1 = "http://example.com/model1"
        s_mid2 = "http://example.com/model2"
        mid1 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid1)
        mid2 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid2)
        uuids = rtpg.VectorUUID()
        uuids.append(rtpg.UUID.from_string(str(mid1)))
        uuids.append(rtpg.UUID.from_string(str(mid2)))
        h1 = pg.AddNode(uuids)
        self.assertIsNotNone(h1)
        node = pg.GetConstNodePtr(h1)
        self.assertEqual(node.mids()[0].to_string(), str(mid1))
        self.assertEqual(node.mids()[1].to_string(), str(mid2))

    def testAddNodeWithUUIDNOProperties(self):
        pg = rtpg.PropertyGraph()
        s_mid1 = "http://example.com/model1"
        s_mid2 = "http://example.com/model2"
        mid1 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid1)
        mid2 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid2)
        uuids = rtpg.VectorUUID()
        uuids.append(rtpg.UUID.from_string(str(mid1)))
        uuids.append(rtpg.UUID.from_string(str(mid2)))
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), uuids)
        self.assertIsNotNone(h1)
        node = pg.GetConstNodePtr(h1)
        self.assertEqual(node.mids()[0].to_string(), str(mid1))
        self.assertEqual(node.mids()[1].to_string(), str(mid2))
        self.assertEqual(node.id().to_string(), str(uid))

    def testAddNodeWithoutUUIDWithProperties(self):
        pg = rtpg.PropertyGraph()
        s_mid1 = "http://example.com/model1"
        s_mid2 = "http://example.com/model2"
        mid1 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid1)
        mid2 = uuid.uuid5(uuid.NAMESPACE_URL, s_mid2)
        uuids = rtpg.VectorUUID()
        uuids.append(rtpg.UUID.from_string(str(mid1)))
        uuids.append(rtpg.UUID.from_string(str(mid2)))
        props = rtpg.MapProperties()
        props["BoolProp"] = True
        props["IntProp"] = 42
        props["DoubleProp"] = 42.0
        props["StringProp"] = "Hello!"
        h1 = pg.AddNode(uuids, props)
        node = pg.GetConstNodePtr(h1)
        self.assertEqual(node.mids()[0].to_string(), str(mid1))
        self.assertEqual(node.mids()[1].to_string(), str(mid2))
        self.assertEqual(props, node.properties())

    def testAddNodeExistingUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        node = pg.GetConstNodePtr(h1)
        h2 = pg.AddNode(node.id(), rtpg.VectorUUID())
        self.assertIsNone(h2)

    def testAddEdgeUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        uid1 = pg.GetConstNodePtr(h1).id()
        h2 = pg.AddNode(rtpg.VectorUUID())
        uid2 = pg.GetConstNodePtr(h2).id()
        he = pg.AddEdge(uid1, uid2, rtpg.UUID())
        self.assertEqual(pg.GetConstEdgePtr(he).tail(), h1)
        self.assertEqual(pg.GetConstEdgePtr(he).head(), h2)
        self.assertEqual(pg.GetConstNodePtr(h1).out_edges()[0], he)
        self.assertEqual(pg.GetConstNodePtr(h2).in_edges()[0], he)

    def testAddEdgeUUIDMissingUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        uid1 = pg.GetConstNodePtr(h1).id()
        he = pg.AddEdge(uid1, rtpg.UUID(), rtpg.UUID())
        self.assertIsNone(he)

    def testAddEdgeUUIDSameUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        uid1 = pg.GetConstNodePtr(h1).id()
        he = pg.AddEdge(uid1, uid1, rtpg.UUID())
        self.assertIsNone(he)

    def testAddEdgeHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        self.assertEqual(pg.GetConstEdgePtr(he).tail(), h1)
        self.assertEqual(pg.GetConstEdgePtr(he).head(), h2)
        self.assertEqual(pg.GetConstNodePtr(h1).out_edges()[0], he)
        self.assertEqual(pg.GetConstNodePtr(h2).in_edges()[0], he)

    def testAddEdgeHandleWrongHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = rtpg.Handle()
        h2.index = 42
        h2.pattern = 42
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        self.assertIsNone(he)

    def testAddEdgeHandleSameHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h1, rtpg.UUID())
        self.assertIsNone(he)

    def testRemoveNodeUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        uid = pg.GetConstNodePtr(h1).id()
        self.assertTrue(pg.RemoveNode(uid))
        self.assertIsNone(pg.GetConstNodePtr(uid))
        self.assertIsNone(pg.GetConstNodePtr(h1))

    def testRemoveNodeWrongUUID(self):
        pg = rtpg.PropertyGraph()
        self.assertFalse(pg.RemoveNode(rtpg.UUID()))

    def testRemoveNodeHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        self.assertTrue(pg.RemoveNode(h1))
        self.assertIsNone(pg.GetConstNodePtr(h1))

    def testRemoveNodeWrongHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = rtpg.Handle()
        h1.index = 42
        self.assertFalse(pg.RemoveNode(h1))

    def testRemoveNodeRemovesEdge(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        self.assertGreater(len(pg.GetConstNodePtr(h2).in_edges()), 0)
        self.assertTrue(pg.RemoveNode(h1))
        self.assertEqual(len(pg.GetConstNodePtr(h2).in_edges()), 0)

    def testRemoveEdgeUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        self.assertTrue(pg.RemoveEdge(pg.GetConstEdgePtr(he).id()))
        self.assertEqual(len(pg.GetConstNodePtr(h1).out_edges()), 0)
        self.assertEqual(len(pg.GetConstNodePtr(h2).in_edges()), 0)

    def testRemoveEdgeWrongUUID(self):
        pg = rtpg.PropertyGraph()
        self.assertFalse(pg.RemoveEdge(rtpg.UUID()))

    def testRemoveEdgeHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        self.assertTrue(pg.RemoveEdge(he))
        self.assertEqual(len(pg.GetConstNodePtr(h1).out_edges()), 0)
        self.assertEqual(len(pg.GetConstNodePtr(h2).in_edges()), 0)

    def testRemoveEdgeWrongHandle(self):
        pg = rtpg.PropertyGraph()
        he = rtpg.Handle()
        he.index = 42
        self.assertFalse(pg.RemoveEdge(he))

    def testGetNodePtrUUID(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetNodePtr(rtpg.UUID.from_string(str(uid)))
        self.assertIsNotNone(node)

    def testGetNodePtrWrongUUID(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetNodePtr(rtpg.UUID())
        self.assertIsNone(node)

    def testGetConstNodePtrUUID(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetConstNodePtr(rtpg.UUID.from_string(str(uid)))
        self.assertIsNotNone(node)

    def testGetConstNodePtrWrongUUID(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetConstNodePtr(rtpg.UUID())
        self.assertIsNone(node)

    def testGetNodePtrHandle(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetNodePtr(h1)
        self.assertIsNotNone(node)

    def testGetNodePtrWrongHandle(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        h2 = rtpg.Handle()
        h2.index = 42
        node = pg.GetNodePtr(h2)
        self.assertIsNone(node)

    def testGetConstNodePtrHandle(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        node = pg.GetConstNodePtr(h1)
        self.assertIsNotNone(node)

    def testGetConstNodePtrWrongHandle(self):
        pg = rtpg.PropertyGraph()
        uid = uuid.uuid4()
        h1 = pg.AddNode(rtpg.UUID.from_string(str(uid)), rtpg.VectorUUID())
        h2 = rtpg.Handle()
        h2.index = 42
        node = pg.GetConstNodePtr(h2)
        self.assertIsNone(node)

    def testGetEdgePtrUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetEdgePtr(pg.GetConstEdgePtr(he).id())
        self.assertIsNotNone(edge)

    def testGetEdgePtrWrongUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetEdgePtr(rtpg.UUID())
        self.assertIsNone(edge)

    def testGetConstEdgePtrUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetConstEdgePtr(pg.GetEdgePtr(he).id())
        self.assertIsNotNone(edge)

    def testGetConstEdgePtrWrongUUID(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetConstEdgePtr(rtpg.UUID())
        self.assertIsNone(edge)

    def testGetEdgePtrHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetEdgePtr(he)
        self.assertIsNotNone(edge)

    def testGetEdgePtrWrongHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        he2 = rtpg.Handle()
        he2.index = 42
        edge = pg.GetEdgePtr(he2)
        self.assertIsNone(edge)

    def testGetConstEdgePtrHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        edge = pg.GetConstEdgePtr(he)
        self.assertIsNotNone(edge)

    def testGetConstEdgePtrWrongHandle(self):
        pg = rtpg.PropertyGraph()
        h1 = pg.AddNode(rtpg.VectorUUID())
        h2 = pg.AddNode(rtpg.VectorUUID())
        he = pg.AddEdge(h1, h2, rtpg.UUID())
        he2 = rtpg.Handle()
        he2.index = 42
        edge = pg.GetConstEdgePtr(he2)
        self.assertIsNone(edge)


if __name__ == '__main__':
    unittest.main()
