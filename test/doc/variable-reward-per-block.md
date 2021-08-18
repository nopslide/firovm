

# Variable Reward Per Block

**Task Name** : Variable Reward Per Block Testing

**Description** : To test that the reward per block is dynamic due to input reward per block from the calculation


## Test Steps

1. generate new address
	> ./firovm-cli getnewaddress
	
2. call `getblockchaininfo` to see an existing supply
	> ./firovm-cli getblockchaininfo
	 
3. generate a block with 10 coins in coinbase
	> ./firovm-cli generatetoaddress 1 TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ **10**

4. call `getblockchaininfo` to see an existing supply
	> ./firovm-cli getblockchaininfo
	
5. generate a block with 15 coins in coinbase
	> ./firovm-cli generatetoaddress 1 TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ **15**
	
6. call `getblockchaininfo` to see balance in moneysupply
	> ./firovm-cli getblockchaininfo
	
## Expected Results

Step 1, Should result in an new address for example, `TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ`
Step 2,  With `getblockchaininfo`, should return following

    {
	  ...
      "moneysupply": 0,
      ...
    }
Step 3, After the steps are executed, command `generatetoaddress` should return `blockhash` like 

    [
      "14416261557a6f4f185f76ef14629b729e532338cf9299fff6be3009af993d92"
    ]
Step 4,  With `getblockchaininfo`, should return following

    {
	  ...
      "moneysupply": 10,
      ...
    }
Step 5, After the steps are executed, command `generatetoaddress` should return `blockhash` like 

    [
      "23316261557a6f4f185f76ef14629b729e532338cf9299fff6be3009af993d88"
    ]
Step 6,  With `getblockchaininfo`, should return following

    {
	  ...
      "moneysupply": 25,
      ...
    }


## Results

    bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getnewaddress
	TXRFnNkHbNuqVt2xDXWfBbHHYyXu9vvt2x
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getblockchaininfo
	{
	  "chain": "regtest",
	  "blocks": 0,
	  "headers": 0,
	  "bestblockhash": "61d1d322da50dc9961e99d57a14189138065b49046167834bc58e0de84671e14",
	  "moneysupply": 0,
	  "maxsupply": 21000000,
	  "mediantime": 1624337565,
	  "verificationprogress": 1,
	  "initialblockdownload": true,
	  "size_on_disk": 392,
	  "pruned": false,
	  "warnings": ""
	}
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 1 TXRFnNkHbNuqVt2xDXWfBbHHYyXu9vvt2x 10
	[
	  "92c1e863766f0142800218a8970e9ab6a65b67de4824e79c4950c24e9973a9e5"
	]
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getblockchaininfo
	{
	  "chain": "regtest",
	  "blocks": 1,
	  "headers": 1,
	  "bestblockhash": "92c1e863766f0142800218a8970e9ab6a65b67de4824e79c4950c24e9973a9e5",
	  "moneysupply": 10,
	  "maxsupply": 21000000,
	  "mediantime": 1629264524,
	  "verificationprogress": 1,
	  "initialblockdownload": false,
	  "size_on_disk": 827,
	  "pruned": false,
	  "warnings": ""
	}
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 1 TXRFnNkHbNuqVt2xDXWfBbHHYyXu9vvt2x 15
	[
	  "81f13cba95188398b732d6e48e8f5467564a97c3d786afdae7ff05c6da0fe9fe"
	]
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getblockchaininfo
	{
	  "chain": "regtest",
	  "blocks": 2,
	  "headers": 2,
	  "bestblockhash": "81f13cba95188398b732d6e48e8f5467564a97c3d786afdae7ff05c6da0fe9fe",
	  "moneysupply": 25,
	  "maxsupply": 21000000,
	  "mediantime": 1629264524,
	  "verificationprogress": 1,
	  "initialblockdownload": false,
	  "size_on_disk": 1262,
	  "pruned": false,
	  "warnings": ""
	}

