# Configure

You can define leading symbol at `base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,DEC);`   
DEC:"decimal"  

[List of Leading symbol](https://en.bitcoin.it/wiki/List_of_address_prefixes)  

### File Location

path: `~/firovm/src/chainparams.cpp`

## Example  

```C++
/**
 * Main network
 */
class CMainParams : public CChainParams {
        // ... 
        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,82);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,7);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,210);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};
        
        // ... 
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
        // ...  

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,178);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,185);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // ... 
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
        // ...  
        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,178);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // ... 
};
```

References  
 - https://github.com/bitcoin/bitcoin/blob/0.14/src/chainparams.cpp#L129-L131
 - https://github.com/bitcoin/bitcoin/blob/0.14/src/chainparams.cpp#L229-L231
 - https://github.com/bitcoin/bitcoin/blob/0.14/src/chainparams.cpp#L325-L327
