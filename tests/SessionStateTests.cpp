#include <doctest.h>
#include <cleanMqtt/Mqtt/State/SessionState/SessionState.h>
#include <cleanMqtt/Mqtt/Enums/ClientErrorCode.h>
#include <cleanMqtt/Mqtt/State/SessionState/ISessionStatePersistantStore.h>
#include <cleanMqtt/Mqtt/State/SessionState/MessageContainer.h>
#include <cleanMqtt/Mqtt/State/SessionState/MessageContainerData.h>
#include <string>
#include <vector>

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

namespace 
{
    struct DummyPersistantStore : ISessionStatePersistantStore 
    {
        std::vector<std::string> log;
        bool write(const char* clientId, std::uint32_t expiry, const SavedData& data) override
        {
            log.push_back("write:" + std::string(clientId));
            return true;
        }

        bool updateMessage(const char* clientId, std::uint16_t packetId, PublishMessageStatus newStatus, bool bringToEnd) override
        {
            log.push_back("update:" + std::string(clientId));
            return true;
        }

        bool removeMessage(const char* clientId, std::uint16_t packetId) override
        {
            log.push_back("remove:" + std::string(clientId));
            return true;
        }

        bool removeFromStore(const char* clientId) override 
        {
            log.push_back("clear:" + std::string(clientId));
            return true;
        }

        bool readAll(const char* clientId, std::vector<const SavedData>& outData) override
        {
            log.push_back("readAll:" + std::string(clientId));
            return true;
        }

        bool removeExpiredFromStore() override
        {
            log.push_back("removeExpired");
            return true;
        }
    };

    PublishMessageData createTestPublishMessageData(const std::string& topic = "test/topic", Qos qos = Qos::QOS_1)
    {
        ByteBuffer payload(100);
        const std::uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
        payload.append(data, 4);
        
        PublishOptions options;
        options.qos = qos;
        
        return PublishMessageData{topic, std::move(payload), std::move(options)};
    }
}

TEST_SUITE("SessionState Tests")
{
    TEST_CASE("Constructor initializes members")
    {
        DummyPersistantStore store;
        SessionState state("client1", 1000, 500, &store);
    }

    TEST_CASE("addMessage No_Error")
    {
        DummyPersistantStore store;
        SessionState state("client2", 1000, 500, &store);

        ByteBuffer bytes{ 0 };
        PublishMessageData msgData{ "", std::move(bytes), {} };
        auto result = state.addMessage(42, std::move(msgData));
        CHECK(result == ClientErrorCode::No_Error);
        CHECK(store.log.size() == 1);
        CHECK(store.log[0].find("write:client2") != std::string::npos);
    }

    TEST_CASE("addMessage error on store write fail")
    {
        struct FailingStore : DummyPersistantStore 
        {
            bool write(const char* clientId, std::uint32_t expiry, const SavedData& data) override
            { 
                return false; 
            }
        } store;

        SessionState state("client3", 1000, 500, &store);

        ByteBuffer bytes{ 0 };
        PublishMessageData msgData{ "", std::move(bytes), {} };
        auto result = state.addMessage(43, std::move(msgData));
        CHECK(result == ClientErrorCode::Failed_Writing_To_Persistent_Storage);
    }

    TEST_CASE("updateMessage updates status and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client4", 1000, 500, &store);

        ByteBuffer bytes{ 0 };
        PublishMessageData msgData{ "", std::move(bytes), {} };
        state.addMessage(44, std::move(msgData));
        state.updateMessage(44, PublishMessageStatus::WaitingForPubComp);
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("update:client4") != std::string::npos);
    }

    TEST_CASE("removeMessage erases and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client5", 1000, 500, &store);

        ByteBuffer bytes{ 0 };
        PublishMessageData msgData{ "", std::move(bytes), {} };
        state.addMessage(45, std::move(msgData));
        state.removeMessage(45);
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("remove:client5") != std::string::npos);
    }

    TEST_CASE("clear removes all and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client6", 1000, 500, &store);

        ByteBuffer bytes{ 0 };
        PublishMessageData msgData{ "", std::move(bytes), {} };
        state.addMessage(46, std::move(msgData));
        state.clear();
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("clear:client6") != std::string::npos);
    }
}

TEST_SUITE("MessageContainerData Tests")
{
    TEST_CASE("MessageContainerData - Constructor with QOS_1")
    {
        auto msgData = createTestPublishMessageData("test/qos1", Qos::QOS_1);
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData containerData(100, std::move(msgData), retryTime, true);

        CHECK(containerData.data.packetID == 100);
        CHECK(containerData.data.status == PublishMessageStatus::WaitingForAck);
        CHECK(containerData.data.publishMsgData.topic == "test/qos1");
        CHECK(containerData.canRetry == true);
    }

    TEST_CASE("MessageContainerData - Constructor with QOS_2")
    {
        auto msgData = createTestPublishMessageData("test/qos2", Qos::QOS_2);
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData containerData(200, std::move(msgData), retryTime, false);

        CHECK(containerData.data.packetID == 200);
        CHECK(containerData.data.status == PublishMessageStatus::WaitingForPubRec);
        CHECK(containerData.data.publishMsgData.topic == "test/qos2");
        CHECK(containerData.canRetry == false);
    }

    TEST_CASE("MessageContainerData - getRetryPacketType for QOS_1")
    {
        SUBCASE("WaitingForAck returns PUBLISH")
        {
            auto msgData = createTestPublishMessageData("test", Qos::QOS_1);
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData containerData(1, std::move(msgData), retryTime);

            CHECK(containerData.getRetryPacketType() == PacketType::PUBLISH);
        }
    }

    TEST_CASE("MessageContainerData - getRetryPacketType for QOS_2")
    {
        auto msgData = createTestPublishMessageData("test", Qos::QOS_2);
        TimePoint retryTime = std::chrono::steady_clock::now();
        MessageContainerData containerData(2, std::move(msgData), retryTime);

        SUBCASE("WaitingForPubRec returns PUBLISH")
        {
            containerData.data.status = PublishMessageStatus::WaitingForPubRec;
            CHECK(containerData.getRetryPacketType() == PacketType::PUBLISH);
        }

        SUBCASE("WaitingForPubRel returns PUBLISH_RECEIVED")
        {
            containerData.data.status = PublishMessageStatus::WaitingForPubRel;
            CHECK(containerData.getRetryPacketType() == PacketType::PUBLISH_RECEIVED);
        }

        SUBCASE("WaitingForPubComp returns PUBLISH_RELEASED")
        {
            containerData.data.status = PublishMessageStatus::WaitingForPubComp;
            CHECK(containerData.getRetryPacketType() == PacketType::PUBLISH_RELEASED);
        }

        SUBCASE("NeedToSendPubComp returns PUBLISH_COMPLETE")
        {
            containerData.data.status = PublishMessageStatus::NeedToSendPubComp;
            CHECK(containerData.getRetryPacketType() == PacketType::PUBLISH_COMPLETE);
        }
    }

    TEST_CASE("MessageContainerData - Retry time handling")
    {
        auto msgData = createTestPublishMessageData();
        auto now = std::chrono::steady_clock::now();
        auto futureTime = now + std::chrono::seconds(10);
        
        MessageContainerData containerData(10, std::move(msgData), futureTime, true);

        CHECK(containerData.nextRetryTime > now);
        CHECK(containerData.canRetry == true);
    }

    TEST_CASE("MessageContainerData - Payload preservation")
    {
        ByteBuffer payload(100);
        const std::uint8_t testData[] = {0xDE, 0xAD, 0xBE, 0xEF};
        payload.append(testData, 4);
        
        PublishOptions options;
        options.qos = Qos::QOS_1;
        
        PublishMessageData msgData{"test/payload", std::move(payload), std::move(options)};
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData containerData(50, std::move(msgData), retryTime);

        CHECK(containerData.data.publishMsgData.payload.size() == 4);
        CHECK(containerData.data.publishMsgData.payload[0] == 0xDE);
        CHECK(containerData.data.publishMsgData.payload[1] == 0xAD);
        CHECK(containerData.data.publishMsgData.payload[2] == 0xBE);
        CHECK(containerData.data.publishMsgData.payload[3] == 0xEF);
    }
}

TEST_SUITE("MessageContainer Tests")
{
    TEST_CASE("MessageContainer - Default construction")
    {
        MessageContainer container;

        CHECK(container.size() == 0);
        CHECK(container.begin() == container.end());
    }

    TEST_CASE("MessageContainer - Push single message")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(100, std::move(msgData), retryTime);
        auto iter = container.push(std::move(data));

        CHECK(container.size() == 1);
        CHECK(container.contains(100));
        CHECK(iter->data.packetID == 100);
    }

    TEST_CASE("MessageContainer - Push multiple messages")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 10; ++i)
        {
            auto msgData = createTestPublishMessageData("test/topic/" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        CHECK(container.size() == 10);
        
        for (std::uint16_t i = 1; i <= 10; ++i)
        {
            CHECK(container.contains(i));
        }
    }

    TEST_CASE("MessageContainer - Contains returns false for non-existent packet")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(100, std::move(msgData), retryTime);
        container.push(std::move(data));

        CHECK(container.contains(100) == true);
        CHECK(container.contains(101) == false);
        CHECK(container.contains(99) == false);
    }

    TEST_CASE("MessageContainer - Get existing message")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData("test/get");
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(200, std::move(msgData), retryTime);
        container.push(std::move(data));

        auto* iter = container.get(200);
        
        CHECK(iter != nullptr);
        CHECK((*iter)->data.packetID == 200);
        CHECK((*iter)->data.publishMsgData.topic == "test/get");
    }

    TEST_CASE("MessageContainer - Get non-existent message returns nullptr")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(300, std::move(msgData), retryTime);
        container.push(std::move(data));

        auto* iter = container.get(999);
        
        CHECK(iter == nullptr);
    }

    TEST_CASE("MessageContainer - Erase existing message")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(400, std::move(msgData), retryTime);
        container.push(std::move(data));

        CHECK(container.size() == 1);
        CHECK(container.contains(400));

        container.erase(400);

        CHECK(container.size() == 0);
        CHECK(container.contains(400) == false);
    }

    TEST_CASE("MessageContainer - Erase non-existent message is safe")
    {
        MessageContainer container;
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        
        MessageContainerData data(500, std::move(msgData), retryTime);
        container.push(std::move(data));

        CHECK(container.size() == 1);
        
        container.erase(999);
        
        CHECK(container.size() == 1);
        CHECK(container.contains(500));
    }

    TEST_CASE("MessageContainer - Erase from middle of list")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 5; ++i)
        {
            auto msgData = createTestPublishMessageData("test/" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        CHECK(container.size() == 5);

        container.erase(3);

        CHECK(container.size() == 4);
        CHECK(container.contains(1));
        CHECK(container.contains(2));
        CHECK(container.contains(3) == false);
        CHECK(container.contains(4));
        CHECK(container.contains(5));
    }

    TEST_CASE("MessageContainer - MoveToEnd moves message to end of list")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 5; ++i)
        {
            auto msgData = createTestPublishMessageData("test/" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i * 10, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        container.moveToEnd(10);

        std::vector<std::uint16_t> order;
        for (const auto& msg : container)
        {
            order.push_back(msg.data.packetID);
        }

        CHECK(order.size() == 5);
        CHECK(order[0] == 20);
        CHECK(order[1] == 30);
        CHECK(order[2] == 40);
        CHECK(order[3] == 50);
        CHECK(order[4] == 10);
    }

    TEST_CASE("MessageContainer - MoveToEnd with non-existent packet is safe")
    {
        MessageContainer container;
        
        auto msgData = createTestPublishMessageData();
        TimePoint retryTime = std::chrono::steady_clock::now();
        MessageContainerData data(100, std::move(msgData), retryTime);
        container.push(std::move(data));

        CHECK(container.size() == 1);
        
        container.moveToEnd(999);
        
        CHECK(container.size() == 1);
    }

    TEST_CASE("MessageContainer - MoveToEnd already at end")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 3; ++i)
        {
            auto msgData = createTestPublishMessageData();
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        container.moveToEnd(3);

        std::vector<std::uint16_t> order;
        for (const auto& msg : container)
        {
            order.push_back(msg.data.packetID);
        }

        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);
    }

    TEST_CASE("MessageContainer - Clear removes all messages")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 10; ++i)
        {
            auto msgData = createTestPublishMessageData();
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        CHECK(container.size() == 10);

        container.clear();

        CHECK(container.size() == 0);
        CHECK(container.begin() == container.end());
        
        for (std::uint16_t i = 1; i <= 10; ++i)
        {
            CHECK(container.contains(i) == false);
        }
    }

    TEST_CASE("MessageContainer - Clear on empty container is safe")
    {
        MessageContainer container;

        CHECK(container.size() == 0);
        
        container.clear();
        
        CHECK(container.size() == 0);
    }

    TEST_CASE("MessageContainer - Iterator traversal")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 5; ++i)
        {
            auto msgData = createTestPublishMessageData("test/" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i * 100, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        std::vector<std::uint16_t> packetIds;
        for (auto it = container.begin(); it != container.end(); ++it)
        {
            packetIds.push_back(it->data.packetID);
        }

        CHECK(packetIds.size() == 5);
        CHECK(packetIds[0] == 100);
        CHECK(packetIds[1] == 200);
        CHECK(packetIds[2] == 300);
        CHECK(packetIds[3] == 400);
        CHECK(packetIds[4] == 500);
    }

    TEST_CASE("MessageContainer - Const iterator traversal")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 3; ++i)
        {
            auto msgData = createTestPublishMessageData();
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        const MessageContainer& constContainer = container;

        int count = 0;
        for (auto it = constContainer.cbegin(); it != constContainer.cend(); ++it)
        {
            count++;
        }

        CHECK(count == 3);
    }

    TEST_CASE("MessageContainer - Range-based for loop")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 4; ++i)
        {
            auto msgData = createTestPublishMessageData("topic" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i + 1000, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        std::vector<std::string> topics;
        for (const auto& msg : container)
        {
            topics.push_back(msg.data.publishMsgData.topic);
        }

        CHECK(topics.size() == 4);
        CHECK(topics[0] == "topic1");
        CHECK(topics[1] == "topic2");
        CHECK(topics[2] == "topic3");
        CHECK(topics[3] == "topic4");
    }

    TEST_CASE("MessageContainer - Complex operations sequence")
    {
        MessageContainer container;

        for (std::uint16_t i = 1; i <= 5; ++i)
        {
            auto msgData = createTestPublishMessageData("test/" + std::to_string(i));
            TimePoint retryTime = std::chrono::steady_clock::now();
            MessageContainerData data(i, std::move(msgData), retryTime);
            container.push(std::move(data));
        }

        CHECK(container.size() == 5);

        container.erase(2);
        container.erase(4);

        CHECK(container.size() == 3);
        CHECK(container.contains(1));
        CHECK(container.contains(3));
        CHECK(container.contains(5));

        container.moveToEnd(1);

        std::vector<std::uint16_t> order;
        for (const auto& msg : container)
        {
            order.push_back(msg.data.packetID);
        }

        CHECK(order[0] == 3);
        CHECK(order[1] == 5);
        CHECK(order[2] == 1);

        auto msgData = createTestPublishMessageData("new");
        TimePoint retryTime = std::chrono::steady_clock::now();
        MessageContainerData data(10, std::move(msgData), retryTime);
        container.push(std::move(data));

        CHECK(container.size() == 4);

        container.clear();

        CHECK(container.size() == 0);
    }

    TEST_CASE("MessageContainer - Message data integrity after operations")
    {
        MessageContainer container;

        //Add message with specific data
        ByteBuffer payload(100);
        const std::uint8_t testData[] = {0x11, 0x22, 0x33, 0x44, 0x55};
        payload.append(testData, 5);
        
        PublishOptions options;
        options.qos = Qos::QOS_2;
        options.retain = true;
        options.topicAlias = 42;
        
        PublishMessageData msgData{"important/topic", std::move(payload), std::move(options)};
        TimePoint retryTime = std::chrono::steady_clock::now();
        MessageContainerData data(777, std::move(msgData), retryTime, true);
        
        container.push(std::move(data));

        //Retrieve and verify
        auto* iter = container.get(777);
        REQUIRE(iter != nullptr);

        const auto& msg = (*iter)->data.publishMsgData;
        CHECK(msg.topic == "important/topic");
        CHECK(msg.payload.size() == 5);
        CHECK(msg.payload[0] == 0x11);
        CHECK(msg.payload[4] == 0x55);
        CHECK(msg.options.qos == Qos::QOS_2);
        CHECK(msg.options.retain == true);
        CHECK(msg.options.topicAlias == 42);
        CHECK((*iter)->canRetry == true);
    }

    TEST_CASE("MessageContainer - Packet ID uniqueness")
    {
        MessageContainer container;

        //Add initial message
        auto msgData1 = createTestPublishMessageData("first");
        TimePoint retryTime = std::chrono::steady_clock::now();
        MessageContainerData data1(100, std::move(msgData1), retryTime);
        container.push(std::move(data1));

        CHECK(container.size() == 1);

        //Erase and re-add with same packet ID
        container.erase(100);
        CHECK(container.size() == 0);

        auto msgData2 = createTestPublishMessageData("second");
        MessageContainerData data2(100, std::move(msgData2), retryTime);
        container.push(std::move(data2));

        CHECK(container.size() == 1);
        
        auto* iter = container.get(100);
        REQUIRE(iter != nullptr);
        CHECK((*iter)->data.publishMsgData.topic == "second");
    }
}
