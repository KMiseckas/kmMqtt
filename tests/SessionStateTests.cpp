#include <doctest.h>
#include <cleanMqtt/Mqtt/State/SessionState/SessionState.h>
#include <cleanMqtt/Mqtt/Enums/ClientErrorCode.h>
#include <cleanMqtt/Mqtt/State/SessionState/ISessionStatePersistantStore.h>
#include <string>
#include <vector>

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
}

TEST_SUITE("SessionState Tests")
{
    TEST_CASE("Constructor initializes members")
    {
        DummyPersistantStore store;
        SessionState state("client1", 1000, 500, &store);
        CHECK(state.m_clientId == std::string("client1"));
        CHECK(state.m_retryInterval.count() == 500);
        CHECK(state.m_sessionExpiryInterval.count() == 1000);
        CHECK(state.m_persistantStore == &store);
    }

    TEST_CASE("addMessage No_Error")
    {
        DummyPersistantStore store;
        SessionState state("client2", 1000, 500, &store);

        PublishMessageData msgData;
        auto result = state.addMessage(42, std::move(msgData));
        CHECK(result == ClientErrorCode::No_Error);
        CHECK(store.log.size() == 1);
        CHECK(store.log[0].find("write:client2") != std::string::npos);
    }

    TEST_CASE("addMessage error on store write fail")
    {
        struct FailingStore : DummyPersistantStore 
        {
            bool write(const char*, std::uint32_t, const PublishMessageData&) override 
            { 
                return false; 
            }
        } store;
        SessionState state("client3", 1000, 500, &store);

        PublishMessageData msgData;
        auto result = state.addMessage(43, std::move(msgData));
        CHECK(result == ClientErrorCode::Failed_Writing_To_Persistent_Storage);
    }

    TEST_CASE("updateMessage updates status and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client4", 1000, 500, &store);

        PublishMessageData msgData;
        state.addMessage(44, std::move(msgData));
        state.updateMessage(44, PublishMessageStatus::WaitingForPubComp);
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("update:client4") != std::string::npos);
    }

    TEST_CASE("removeMessage erases and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client5", 1000, 500, &store);

        PublishMessageData msgData;
        state.addMessage(45, std::move(msgData));
        state.removeMessage(45);
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("remove:client5") != std::string::npos);
    }

    TEST_CASE("clear removes all and calls persistantStore")
    {
        DummyPersistantStore store;
        SessionState state("client6", 1000, 500, &store);

        PublishMessageData msgData;
        state.addMessage(46, std::move(msgData));
        state.clear();
        CHECK(store.log.size() == 2);
        CHECK(store.log[1].find("clear:client6") != std::string::npos);
    }

    TEST_CASE("shouldBringToFront logic")
    {
        DummyPersistantStore store;
        SessionState state("client7", 1000, 500, &store);

        CHECK(state.shouldBringToFront(PublishMessageStatus::WaitingForPubAck, PublishMessageStatus::WaitingForPubComp) == true);
        CHECK(state.shouldBringToFront(PublishMessageStatus::WaitingForPubAck, PublishMessageStatus::WaitingForPubRel) == true);
        CHECK(state.shouldBringToFront(PublishMessageStatus::WaitingForPubAck, PublishMessageStatus::WaitingForPubAck) == false);
        CHECK(state.shouldBringToFront(PublishMessageStatus::WaitingForPubAck, PublishMessageStatus::Published) == false);
    }
}
