
# Variable Supply 

**Task Name** : Variable Supply Testing 
**Description** : To test that the total supply of the native token is dynamic due to input total supply 


## Test Steps

1. generate new address
	> ./firovm-cli getnewaddress
	 
2. set max supply to 11111 with generate a block with 10 coins in coinbase
	> ./firovm-cli generatetoaddress 1 TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ 10 **11111**

3. call `getblockchaininfo` to see new maxsupply
	> ./firovm-cli getblockchaininfo
	
## Expected Results

Step 1, Should result in an new address for example, `TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ`
Step 2, After the steps are executed, command `generatetoaddress` should return `blockhash` like 

    [
      "14416261557a6f4f185f76ef14629b729e532338cf9299fff6be3009af993d92"
    ]

Step 3,  With `getblockchaininfo`, should return following

    {
      "chain": "regtest",
      "blocks": 1,
      "headers": 1,
      "bestblockhash": "007a4f7453942b605429673d9aae083e98e2c95bca283afb2a4fcc8fba861692",
      "moneysupply": 10,
      "maxsupply": 11111,
      "mediantime": 1629104210,
      "verificationprogress": 1,
      "initialblockdownload": false,
      "size_on_disk": 827,
      "pruned": false,
      "warnings": ""
    }

