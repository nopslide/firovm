

# Virtual Machine

**Task Name** : Virtual Machine Testing

**Description** : To test that the smart contract can be executed on the blockchain network

## Configuration

In order to run on Regression mode, in Ubuntu, following configuration will be put in ~/.fvm/fvm.conf
    
    rpcuser=x
    rpcpassword=y
    regtest=1
    server=1
    txindex=1
    debug=1
    fallbackfee=1
    daemon=1
    [regtest]
    rpcport=8545

## Test Steps

After running the daemon using command `firovmd`, To demonstrate how to deploy and interact with a simple we will use this contract:
    
    pragma solidity ^0.4.0;

    contract QtumTest {
       uint storedNumber;
       function QtumTest() {
           storedNumber=1;
       }
       function setNumber(uint number) public{
           storedNumber = number;
       }
       function logNumber() constant public{
            log1("storedNumber", uintToBytes(storedNumber));
       }
       function returnNumber() constant public returns (uint){
           return storedNumber;
       }
       function deposit() public payable{
       }
       function withdraw() public{
           if(!msg.sender.send(this.balance)){
               throw;
           }
       }
       //utility function
       function uintToBytes(uint v) constant returns (bytes32 ret) {
           if (v == 0) {
               ret = '0';
           }
           else {
               while (v > 0) {
                   ret = bytes32(uint(ret) / (2 ** 8));
                   ret |= bytes32(((v % 10) + 48) * 2 ** (8 * 31));
                   v /= 10;
               }
           }
           return ret;
       }
    }
	 

It compiles to the following EVM bytecode, To compile solidity source code using `solc` or using solidity compiler that integrating form third part SDKs or tools. e.g. Hardhat, Truffle and Brownie

    6060604052341561000c57fe5b5b60016000819055505b5b6102bd806100266000396000f30060606040523615610076576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680633450bd6a146100785780633ccfd60b1461009e5780633fb5c1cb146100b057806394e8767d146100d05780639f2c436f1461010c578063d0e30db01461011e575bfe5b341561008057fe5b610088610128565b6040518082815260200191505060405180910390f35b34156100a657fe5b6100ae610133565b005b34156100b857fe5b6100ce6004808035906020019091905050610190565b005b34156100d857fe5b6100ee600480803590602001909190505061019b565b60405180826000191660001916815260200191505060405180910390f35b341561011457fe5b61011c610246565b005b61012661028e565b005b600060005490505b90565b3373ffffffffffffffffffffffffffffffffffffffff166108fc3073ffffffffffffffffffffffffffffffffffffffff16319081150290604051809050600060405180830381858888f19350505050151561018d57610000565b5b565b806000819055505b50565b600060008214156101ce577f3000000000000000000000000000000000000000000000000000000000000000905061023d565b5b600082111561023c5761010081600190048115156101e957fe5b0460010290507f01000000000000000000000000000000000000000000000000000000000000006030600a8481151561021e57fe5b06010260010281179050600a8281151561023457fe5b0491506101cf565b5b8090505b919050565b61025160005461019b565b6000191660405180807f73746f7265644e756d6265720000000000000000000000000000000000000000815250600c01905060405180910390a15b565b5b5600a165627a7a72305820326efcd34df5fdba07e7a1afe7ffd4b42873ef749ae9a5915db46fd20b9c251c0029

And finally, has the following JSON interface file:

    [{"constant":true,"inputs":[],"name":"returnNumber","outputs":[{"name":"","type":"uint256"}],"payable":false,"type":"function"},{"constant":false,"inputs":[],"name":"withdraw","outputs":[],"payable":false,"type":"function"},{"constant":false,"inputs":[{"name":"number","type":"uint256"}],"name":"setNumber","outputs":[],"payable":false,"type":"function"},{"constant":true,"inputs":[{"name":"v","type":"uint256"}],"name":"uintToBytes","outputs":[{"name":"ret","type":"bytes32"}],"payable":false,"type":"function"},{"constant":true,"inputs":[],"name":"logNumber","outputs":[],"payable":false,"type":"function"},{"constant":false,"inputs":[],"name":"deposit","outputs":[],"payable":true,"type":"function"},{"inputs":[],"payable":false,"type":"constructor"}]

This info can easily be retrieved for any contract by using [Browser Solidity](https://ethereum.github.io/browser-solidity/), inputing your contract's source code, and then on the right hand side clicking "contract details"

1. We need to deploy the contract:
 > ./firovm-cli createcontract 6060604052341561000c57fe5b5b60016000819055505b5b6102bd806100266000396000f30060606040523615610076576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680633450bd6a146100785780633ccfd60b1461009e5780633fb5c1cb146100b057806394e8767d146100d05780639f2c436f1461010c578063d0e30db01461011e575bfe5b341561008057fe5b610088610128565b6040518082815260200191505060405180910390f35b34156100a657fe5b6100ae610133565b005b34156100b857fe5b6100ce6004808035906020019091905050610190565b005b34156100d857fe5b6100ee600480803590602001909190505061019b565b60405180826000191660001916815260200191505060405180910390f35b341561011457fe5b61011c610246565b005b61012661028e565b005b600060005490505b90565b3373ffffffffffffffffffffffffffffffffffffffff166108fc3073ffffffffffffffffffffffffffffffffffffffff16319081150290604051809050600060405180830381858888f19350505050151561018d57610000565b5b565b806000819055505b50565b600060008214156101ce577f3000000000000000000000000000000000000000000000000000000000000000905061023d565b5b600082111561023c5761010081600190048115156101e957fe5b0460010290507f01000000000000000000000000000000000000000000000000000000000000006030600a8481151561021e57fe5b06010260010281179050600a8281151561023457fe5b0491506101cf565b5b8090505b919050565b61025160005461019b565b6000191660405180807f73746f7265644e756d6265720000000000000000000000000000000000000000815250600c01905060405180910390a15b565b5b5600a165627a7a72305820326efcd34df5fdba07e7a1afe7ffd4b42873ef749ae9a5915db46fd20b9c251c0029

If it shows insufficent balance, You have to mine blocks to get coins first.

   >./firovm-cli getnewaddress

For example, we got `TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2` as output. Then we call generate rpc method to give coins to the address
    
   > ./firovm-cli generatetoaddress 200 TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2 20

Then you can call `createcontract` again, then call. Moreover you can using script `auto_block_generate.sh` to auto-mining blocks every 15 seconds, and you can also change the period of block time If needed.

>./firovm-cli generatetoaddress 1 TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2 20

2. To make a transaction into a block, and get confirmation. Now wait for your contract to be included in a block. You should be able to confirm it made it into a block by using:

> ./firovm-cli getaccountinfo 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9
	
In order to interact with the contract, you must create raw ABI data from the interface JSON file. The easiest tool to assist in this is ethabi, https://github.com/paritytech/ethabi

Make sure the JSON file is saved somewhere, we will call it interface.json. 

In order to get the `storedNumber` variable we use the `returnNumber()` function. We can construct the ABI values by using ethabi:

    ethabi encode function ~/interface.json returnNumber

The result of this is:

    3450bd6a

3. Now, because we are not changing state, we use `callcontract`:

> ./firovm-cli callcontract 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9 3450bd6a

To change the storedNumber we can do an on-chain execution using `sendtocontract`. First, we need the ABI data:

    ethabi encode function ~/interface.json setNumber -p 123456 --lenient
    3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240

4. Note we use --lenient so that we do not need to provide a full 256bit value as a parameter. Now, we can execute the contract directly:

> ./firovm-cli sendtocontract 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9 3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240

5. Generate a block again with a following command

> ./firovm-cli generatetoaddress 1 TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2 20

6. Call contract again, to see changing output result
>    ./firovm-cli callcontract 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9 3450bd6a

## Expected Results

Step 1, Should result in something like, 

    {
      "txid": "72b0e0576d289c1e4e6c777431e4845f77d0884d3b3cff0387a5f4a1a3a874ea",
      "sender": "qZbjaE8N18ZU1m7851G7QGhvxKL74SRBTt",
      "hash160": "aff3e34ab836edb8d214a993d9da105915e4a6e9",
      "address": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9"
    }

Step 2,  If the contract was successfully executed and persisted in the blockchain, you should see something like this

    {
      "address": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9",
      "balance": 0,
      "storage": {
        "290decd9548b62a8d60345a988386fc84ba6bc95484008f6362f93160ef3e563": {
          "0000000000000000000000000000000000000000000000000000000000000000": "0000000000000000000000000000000000000000000000000000000000000001"
        }
      },
      "code": "..."
    }

Step 3,  This results in a lot of data that can be useful in different contexts (including gas estimates), but we are only concerned about the `output` field, which is the value of `storedNumber`

    {
      "address": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9",
      "executionResult": {
        "gasUsed": 21664,
        "excepted": "None",
        "newAddress": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9",
        "output": "0000000000000000000000000000000000000000000000000000000000000001",
        "codeDeposit": 0,
        "gasRefunded": 0,
        "depositSize": 0,
        "gasForDeposit": 0
      },
      "transactionReceipt": {
        "stateRoot": "ffbeb0377d43c6ed443a2840259ff5ead5158016ab54d55ef21b7b11aa71947f",
        "gasUsed": 21664,
        "bloom": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "log": [
        ]
      }
    }

Step 4, Should result in something like,


    {
      "txid": "72b0e0576d289c1e4e6c777431e4845f77d0884d3b3cff0387a5f4a1a3a874ea",
      "sender": "qZbjaE8N18ZU1m7851G7QGhvxKL74SRBTt",
      "hash160": "aff3e34ab836edb8d214a993d9da105915e4a6e9"
    }


Step 5, Should result in something like,


    [
      "23316261557a6f4f185f76ef14629b729e532338cf9299fff6be3009af993d88"
    ]
   
   
Step 6, Should result in something like,

    {
      "address": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9",..
        "output": "3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240",
        ...
      }
    }

## Results

    bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli getnewaddress
	TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 200 TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32 20
	[
	  "4082bc25e79124c3ea59056dec49ab513ef56b87c0a71b4e5ff57f7a8f9ca8a5",
	  "30701a71992a995e96d0e49edfc9ca50751cae9fa3b6510ec70028c14b44889d",
	  ...
	  "8e9f8958cf66f7bce03851eac7982d29625f37c3ebb0887fad08d3585b51a751",
	  "93350209a50597ab9a80a972a0646b976bd77c290015240ae257f4bc7c8bd57f"
	]
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$./firovm-cli createcontract 6060604052341561000c57fe5b5b60016000819055505b5b6102bd806100266000396000f30060606040523615610076576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680633450bd6a146100785780633ccfd60b1461009e5780633fb5c1cb146100b057806394e8767d146100d05780639f2c436f1461010c578063d0e30db01461011e575bfe5b341561008057fe5b610088610128565b6040518082815260200191505060405180910390f35b34156100a657fe5b6100ae610133565b005b34156100b857fe5b6100ce6004808035906020019091905050610190565b005b34156100d857fe5b6100ee600480803590602001909190505061019b565b60405180826000191660001916815260200191505060405180910390f35b341561011457fe5b61011c610246565b005b61012661028e565b005b600060005490505b90565b3373ffffffffffffffffffffffffffffffffffffffff166108fc3073ffffffffffffffffffffffffffffffffffffffff16319081150290604051809050600060405180830381858888f19350505050151561018d57610000565b5b565b806000819055505b50565b600060008214156101ce577f3000000000000000000000000000000000000000000000000000000000000000905061023d565b5b600082111561023c5761010081600190048115156101e957fe5b0460010290507f01000000000000000000000000000000000000000000000000000000000000006030600a8481151561021e57fe5b06010260010281179050600a8281151561023457fe5b0491506101cf565b5b8090505b919050565b61025160005461019b565b6000191660405180807f73746f7265644e756d6265720000000000000000000000000000000000000000815250600c01905060405180910390a15b565b5b5600a165627a7a72305820326efcd34df5fdba07e7a1afe7ffd4b42873ef749ae9a5915db46fd20b9c251c0029
	{
	  "txid": "098973e9ec4b43ac9f0cac9b5b8105b99183f6472cdba9c1b0e1ff22124df105",
	  "sender": "TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32",
	  "hash160": "ab16fd925b2ee58c69dfd8762fe9c4db2e63069d",
	  "address": "c9219691ac57731b802e2d65e6e59b95bc52fcc4"
	}
	
	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 1 TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32 20
	[
	  "69ba4913022fe9d68b9f62cbf5f714993b5a103411b37c17574933205d49d0da"
	]

	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli callcontract c9219691ac57731b802e2d65e6e59b95bc52fcc4 3450bd6a
	{
	  "address": "c9219691ac57731b802e2d65e6e59b95bc52fcc4",
	  "executionResult": {
	    "gasUsed": 22056,
	    "excepted": "None",
	    "newAddress": "c9219691ac57731b802e2d65e6e59b95bc52fcc4",
	    "output": "0000000000000000000000000000000000000000000000000000000000000001",
	    "codeDeposit": 0,
	    "gasRefunded": 0,
	    "depositSize": 0,
	    "gasForDeposit": 0,
	    "exceptedMessage": ""
	  },
	  "transactionReceipt": {
	    "stateRoot": "45fe14c144c9d7d734ae402498a44f79b637965406b092db5045ce7edd6493f9",
	    "utxoRoot": "56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421",
	    "gasUsed": 22056,
	    "bloom": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
	    "log": [
	    ]
	  }
	}

	bushido@DESKTOP-S1I71OG:~/fvm/src$./firovm-cli sendtocontract c9219691ac57731b802e2d65e6e59b95bc52fcc4 3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240  
	{
	  "txid": "a346d2a948f4c68b1c8fbfb235fbdc1ccdcdc927d8c88eb66c5eb37e50c088c7",
	  "sender": "TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32",
	  "hash160": "ab16fd925b2ee58c69dfd8762fe9c4db2e63069d"
	}

	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli generatetoaddress 1 TRZr6GfjRdJrXeLPgMDUtuHswNuAZNkU32 20
	[
	  "588564de63153279cf3ff7a4bb22dbefc5e27d233181767324e17c61432949ec"
	]

	bushido@DESKTOP-S1I71OG:~/fvm/src$ ./firovm-cli callcontract c9219691ac57731b802e2d65e6e59b95bc52fcc4 3450bd6a
	{
	  "address": "c9219691ac57731b802e2d65e6e59b95bc52fcc4",
	  "executionResult": {
	    "gasUsed": 22056,
	    "excepted": "None",
	    "newAddress": "c9219691ac57731b802e2d65e6e59b95bc52fcc4",
	    "output": "000000000000000000000000000000000000000000000000000000000001e240",
	    "codeDeposit": 0,
	    "gasRefunded": 0,
	    "depositSize": 0,
	    "gasForDeposit": 0,
	    "exceptedMessage": ""
	  },
	  "transactionReceipt": {
	    "stateRoot": "c59558f44017016be95566f9f74e0ba05f2dd724e506664e9391e44785b7f4cf",
	    "utxoRoot": "56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421",
	    "gasUsed": 22056,
	    "bloom": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
	    "log": [
	    ]
	  }
	}
