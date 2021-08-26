
# Variable Supply 

**Task Name** : Variable Supply Testing 

**Description** : To test that the total supply of the native token is dynamic due to input total supply 


## Test Steps

1. generate new address
	> ./firovm-cli getnewaddress
	 
2. call `getblockchaininfo` to see an existing supply
	> ./firovm-cli getblockchaininfo
	
3. set max supply to 11111 with generate a block with 10 coins in coinbase
	> ./firovm-cli generatetoaddress 1 TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ 10 **11111**

4. call `getblockchaininfo` to see new maxsupply
	> ./firovm-cli getblockchaininfo
	
## Expected Results

Step 1, Should result in an new address for example, `TTm4hmoQzJDfmRvQb4TfBaBw9EFSJXfUdQ`
Step 2,  With `getblockchaininfo`, should return following

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
    
Step 3, After the steps are executed, command `generatetoaddress` should return `blockhash` like 

    [
      "14416261557a6f4f185f76ef14629b729e532338cf9299fff6be3009af993d92"
    ]

Step 4,  With `getblockchaininfo`, should return following

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

## Results

    bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getnewaddress
	TVooVAMSfhGDjgEewBun43mf8BMg8wkzry

	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getblockchaininfo
	{
		...
		"maxsupply": 21000000,
		...
	}
		
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 1 TVooVAMSfhGDjgEewBun43mf8BMg8wkzry 10 11111
	[
		  "6d2de5c236e2d5f7b9f84719409ed754c667b66e56c6cc0dff2f16aad6898b7a"
	]

	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getblockchaininfo
	{
	    ...
	    "maxsupply": 11111,
	    ...
	}



