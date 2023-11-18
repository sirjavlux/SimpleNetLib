#include "../SimpleNetAPI/Packet/Packet.h"
#include "../SimpleNetAPI/Packet/PacketComponent.h"
#include "gtest/gtest.h"

class TestComponent final : public PacketComponent
{
public:
    TestComponent(const int16_t InIdentifier, const uint16_t InSize) : PacketComponent(InIdentifier, InSize) { }
    
    int integerValue = 0;
};

TEST(PacketCreation, PacketTests)
{
    Packet packet = Packet(
        EPacketPacketType::ClientToServer, EPacketHandlingType::None);

    for (int i = 0; i < 20; ++i)
    {
        TestComponent testComponent(0, sizeof(TestComponent));
        testComponent.integerValue = i;
        packet.AddComponent(testComponent);
    }

    
}