# Configuration

The `chainID` and `networkID` genesis of `aleth` can be changed and configure, Moreover if you want to pre-loaded smart contract you can allocate `address` in the `accounts` section and put 
the compiled byte code of your smart contract to `code` property, like the example below.  

### File Location  
path: `~/firovm/src/cpp-ethereum/libethashseal/genesis/fvmMainNetwork.cpp`  

## Example
```C++
#include "../GenesisInfo.h"

static dev::h256 const c_genesisStateRootFvmMainNetwork("2eb...825");
static std::string const c_genesisInfoFvmMainNetwork = std::string() +
R"E(
{
	"sealEngine": "NoProof",
	"params": {
		"accountStartNonce": "0x00",
		"homesteadForkBlock": "0x00",
		"daoHardforkBlock": "0xfffffffffffffff",
		"EIP150ForkBlock": "0x00",
		"EIP158ForkBlock": "0x00",
		"byzantiumForkBlock": "0x00",
		"constantinopleForkBlock": "0x00",
		"constantinopleFixForkBlock": "0x00",
		"istanbulForkBlock": "0x00",
		"muirGlacierForkBlock": "0x00",
		"qip6ForkBlock": "0x00",
		"networkID" : "0x51",                     // change networkID here `decimal`
		"chainID": "0x51",                        // change chainID here `decimal`
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000"
	},
	"genesis": {
		"nonce": "0x0000000000000042",
		"difficulty": "0x400000000",
		"mixHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"author": "0x0000000000000000000000000000000000000000",
		"timestamp": "0x00",
		"parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"extraData": "0x5174756d4d61696e4e6574c9987fd35877cdbbbb84ffeb5315ab1f86c21398c0",
		"gasLimit": "0x1388"
	},
	"accounts": {
		"0000000000000000000000000000000000000080": { "code": 
		"0x60...29",
		"storage": {"3": "30","5": "21600"} },
		"0000000000000000000000000000000000000081": { "code": 
		"0x60...29",
		"storage": {"3": "30","5": "21600"} },
		"0000000000000000000000000000000000000082": { "code": 
		"0x60...29",
		"storage": {"3": "30","5": "21600"} },
		"0000000000000000000000000000000000000083": { "code": 
		"0x06...29",
		"storage": {"3": "30","5": "21600"} },
		"0000000000000000000000000000000000000084": { "code": 
		"0x06...29",
		"storage": {"3": "30","5": "21600"} }
	}
}
)E";
```
## Notes
- pre-loaded smart contract can be use as Decentralized Governance Protocols (DGPs) contract or Decentralized Autonomous Organization (DAO) contract.   
- pre-loaded smart contract can be use and call on the consensus layer if needed.  
