#include <functional>

#include <gmock/gmock.h>

#include <common/hash.h>

TEST(Hash, HashesMd5Correctly)
{
    EXPECT_EQ(md5("").hexdigest(), "d41d8cd98f00b204e9800998ecf8427e");
    EXPECT_EQ(md5("test1\ntest2\n\n").hexdigest(), "054ebd383f08970e4400b4cfcd125405");
    // For testing md5file() refer to BMS file format tests, because of CRLF issues with git.
}

TEST(Hash, HashStdHash)
{
    HashMD5 hash{"a93abcc44cd96fa78d11c84c82549081"};
    EXPECT_EQ(hash, hash);
    std::hash<HashMD5> hasher;
    EXPECT_EQ(hasher(hash), hasher(hash));
    EXPECT_NE(hasher(hash), hasher(HashMD5{"f431c993d79c6714a9ba11cc896611df"}));
}

TEST(Hash, HexToBinToHex)
{
    static const std::string hash = "40e94aa51dc5c0ccc5aad4e6aefdde2a";
    const std::string hex = lunaticvibes::hex2bin(hash);
    EXPECT_EQ(lunaticvibes::bin2hex({hex.data(), hex.size()}), hash);
}
