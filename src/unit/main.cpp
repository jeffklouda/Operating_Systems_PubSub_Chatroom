// SYSTEM INCLUDES

#include <gtest/gtest.h>

// C++ PROJECT INCLUDES
#include <ps_client/client.h>

TEST(client_tests, subscribe_added_to_queue){
    const char* host = "localhost";
    const char* port = "9500";
    const char* cid  = "mdalonz1";

    Client client(host, port, cid);

    EchoCallback e;
    const char* topic = "echo";

    client.subscribe(topic, &e);
    
    ASSERT_TRUE(client.get_outMessages()->size() > 0); 
}

TEST(client_tests, unsubscribe_added_to_queue){
    const char* host = "localhost";
    const char* port = "9500";
    const char* cid  = "mdalonz1";

    Client client(host, port, cid);

    EchoCallback e;
    const char* topic = "echo";

    client.subscribe(topic, &e);

    client.unsubscribe(topic);

    ASSERT_TRUE(client.get_outMessages()->size() > 0);

}

TEST(message_tests, message_struct_assigned){
    Message message_test = { "MESSAGE", "echo", "mdalonz1", 12345, "Hello World", 11}; 
    ASSERT_TRUE(message_test.type == "MESSAGE");
    ASSERT_TRUE(message_test.topic == "echo");
    ASSERT_TRUE(message_test.sender == "mdalonz1");
    ASSERT_TRUE(message_test.nonce == 12345);
    ASSERT_TRUE(message_test.body == "Hello World");
    ASSERT_TRUE(message_test.length == 11);
}

TEST(thread_tests, thread_test_full){
    Thread thread_test;
    Thread* thread_pointer = NULL;
    thread_pointer = &thread_test;
    ASSERT_TRUE(thread_pointer != NULL);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
