#include <doctest.h>
#include <kmMqtt/Mqtt/Packets/BasePacket.h>

using namespace kmMqtt::mqtt;

struct MockEncodeHeader : public kmMqtt::IEncodeHeader {
    mutable int encodeCalled = 0;
    mutable int sizeCalled = 0;
    void encode(kmMqtt::ByteBuffer&) const override { ++encodeCalled; }
    std::size_t getEncodedBytesSize() const noexcept override { ++sizeCalled; return 1; }
};

struct MockDecodeHeader : public kmMqtt::IDecodeHeader {
    mutable int decodeCalled = 0;
    kmMqtt::mqtt::DecodeResult decode(const kmMqtt::ByteBuffer&) override { ++decodeCalled; return {}; }
};

class TestPacket : public BasePacket 
{
public:
    TestPacket(const FixedHeaderFlags& flags) : BasePacket(flags) {}
    TestPacket(kmMqtt::ByteBuffer&& buffer) : BasePacket(std::move(buffer)) {}
    PacketType getPacketType() const noexcept override { return PacketType::CONNECT; }

    // Expose protected methods for testing
    using BasePacket::addEncodeHeader;
    using BasePacket::addDecodeHeader;
};

TEST_SUITE("Base Packet Tests")
{
    TEST_CASE("Construct with FixedHeaderFlags")
    {
        FixedHeaderFlags flags(0x0F);
        TestPacket packet(flags);
        CHECK(packet.getFixedHeader().flags.getFlags() == 0x0F);
        CHECK(packet.getPacketType() == PacketType::CONNECT);
    }

    TEST_CASE("Move construction and assignment")
    {
        FixedHeaderFlags flags(0x01);
        TestPacket packet1(flags);
        TestPacket packet2(std::move(packet1));
        CHECK(packet2.getPacketType() == PacketType::CONNECT);

        TestPacket packet3(FixedHeaderFlags(0x02));
        packet3 = std::move(packet2);
        CHECK(packet3.getPacketType() == PacketType::CONNECT);
    }

    TEST_CASE("Construct with ByteBuffer")
    {
        kmMqtt::ByteBuffer buf(10);
        TestPacket packet(std::move(buf));
        CHECK(packet.getDataBuffer().capacity() == 10);
    }

    TEST_CASE("Encode succeeds on minimum buffer size")
    {
        FixedHeaderFlags flags(0x00);
        TestPacket packet(flags);
        auto result = packet.encode();
        CHECK(result.packetType == PacketType::CONNECT);
        // Should succeed: minimum MQTT packet is 2 bytes (header + remaining length)
        CHECK(result.code == EncodeErrorCode::NO_ERROR);
    }

    TEST_CASE("Decode returns error on nullptr or small buffer")
    {
        FixedHeaderFlags flags(0x00);
        TestPacket packet(flags);

        // Set a too-small buffer
        kmMqtt::ByteBuffer buf(1);
        TestPacket packet2(std::move(buf));
        auto result2 = packet2.decode();
        CHECK(result2.code == DecodeErrorCode::MALFORMED_PACKET);
    }

    TEST_CASE("Add encode header and check encode is called")
    {
        FixedHeaderFlags flags(0x01);
        TestPacket packet(flags);
        auto* header = new MockEncodeHeader();
        packet.addEncodeHeader(header);
        packet.encode();
        CHECK(header->encodeCalled == 1);
        CHECK(header->sizeCalled > 0);
        delete header;
    }

    TEST_CASE("Add decode header and check decode is called")
    {
        // Fill buffer with dummy data to avoid MALFORMED_PACKET error
        kmMqtt::ByteBuffer buf(2);
        for (std::size_t i = 0; i < buf.capacity(); ++i) 
        {
            buf += 0x00;
        }
        TestPacket packet(std::move(buf));
        auto* header = new MockDecodeHeader();
        packet.addDecodeHeader(header);
        auto result = packet.decode();
        CHECK(header->decodeCalled >= 1);
        delete header;
    }
}