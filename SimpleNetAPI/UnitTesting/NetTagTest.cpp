#include "../SimpleNetLib/Utility/NetTag.h"
#include "gtest/gtest.h"

TEST(InitializationAndDeletion, NetTagTest)
{
   const NetTag tag = NetTag("Test123");

   EXPECT_EQ(tag, NetTag("Test123"));
   EXPECT_NE(tag, NetTag("Lol"));

   NetTag oldTag;
   for (int i = 0; i < 100000; ++i)
   {
      const NetTag newTag = NetTag("Test Tag Lol 1337");
      oldTag = newTag;
   }

   for (int i = 0; i < 100000; ++i)
   {
       const NetTag invalidTag;
       EXPECT_TRUE(!invalidTag.IsValid());
   }
   
   std::cout << oldTag.ToCStr() << std::endl;
}