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
        pass


if __name__ == '__main__':
    unittest.main()
