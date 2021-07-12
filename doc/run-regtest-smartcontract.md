# Configuration

In order to run on Regression mode, in Ubuntu, following configuration will be put in ~/.fvm/fvm.conf
    
    rpcuser=x
    rpcpassword=y
    regtest=1
    server=1
    txindex=1
    debug=1
    fallbackfee=1
    daemon=1

After running the daemon, To demonstrate how to deploy and interact with a simple we will use this contract:
    
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

It compiles to the following EVM bytecode 

    6060604052341561000c57fe5b5b60016000819055505b5b6102bd806100266000396000f30060606040523615610076576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680633450bd6a146100785780633ccfd60b1461009e5780633fb5c1cb146100b057806394e8767d146100d05780639f2c436f1461010c578063d0e30db01461011e575bfe5b341561008057fe5b610088610128565b6040518082815260200191505060405180910390f35b34156100a657fe5b6100ae610133565b005b34156100b857fe5b6100ce6004808035906020019091905050610190565b005b34156100d857fe5b6100ee600480803590602001909190505061019b565b60405180826000191660001916815260200191505060405180910390f35b341561011457fe5b61011c610246565b005b61012661028e565b005b600060005490505b90565b3373ffffffffffffffffffffffffffffffffffffffff166108fc3073ffffffffffffffffffffffffffffffffffffffff16319081150290604051809050600060405180830381858888f19350505050151561018d57610000565b5b565b806000819055505b50565b600060008214156101ce577f3000000000000000000000000000000000000000000000000000000000000000905061023d565b5b600082111561023c5761010081600190048115156101e957fe5b0460010290507f01000000000000000000000000000000000000000000000000000000000000006030600a8481151561021e57fe5b06010260010281179050600a8281151561023457fe5b0491506101cf565b5b8090505b919050565b61025160005461019b565b6000191660405180807f73746f7265644e756d6265720000000000000000000000000000000000000000815250600c01905060405180910390a15b565b5b5600a165627a7a72305820326efcd34df5fdba07e7a1afe7ffd4b42873ef749ae9a5915db46fd20b9c251c0029

And finally, has the following JSON interface file:

    [{"constant":true,"inputs":[],"name":"returnNumber","outputs":[{"name":"","type":"uint256"}],"payable":false,"type":"function"},{"constant":false,"inputs":[],"name":"withdraw","outputs":[],"payable":false,"type":"function"},{"constant":false,"inputs":[{"name":"number","type":"uint256"}],"name":"setNumber","outputs":[],"payable":false,"type":"function"},{"constant":true,"inputs":[{"name":"v","type":"uint256"}],"name":"uintToBytes","outputs":[{"name":"ret","type":"bytes32"}],"payable":false,"type":"function"},{"constant":true,"inputs":[],"name":"logNumber","outputs":[],"payable":false,"type":"function"},{"constant":false,"inputs":[],"name":"deposit","outputs":[],"payable":true,"type":"function"},{"inputs":[],"payable":false,"type":"constructor"}]

This info can easily be retrieved for any contract by using [Browser Solidity](https://ethereum.github.io/browser-solidity/), inputing your contract's source code, and then on the right hand side clicking "contract details" 

First, we need to deploy the contract:

    ./firovm-cli createcontract 6060604052341561000c57fe5b5b60016000819055505b5b6102bd806100266000396000f30060606040523615610076576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680633450bd6a146100785780633ccfd60b1461009e5780633fb5c1cb146100b057806394e8767d146100d05780639f2c436f1461010c578063d0e30db01461011e575bfe5b341561008057fe5b610088610128565b6040518082815260200191505060405180910390f35b34156100a657fe5b6100ae610133565b005b34156100b857fe5b6100ce6004808035906020019091905050610190565b005b34156100d857fe5b6100ee600480803590602001909190505061019b565b60405180826000191660001916815260200191505060405180910390f35b341561011457fe5b61011c610246565b005b61012661028e565b005b600060005490505b90565b3373ffffffffffffffffffffffffffffffffffffffff166108fc3073ffffffffffffffffffffffffffffffffffffffff16319081150290604051809050600060405180830381858888f19350505050151561018d57610000565b5b565b806000819055505b50565b600060008214156101ce577f3000000000000000000000000000000000000000000000000000000000000000905061023d565b5b600082111561023c5761010081600190048115156101e957fe5b0460010290507f01000000000000000000000000000000000000000000000000000000000000006030600a8481151561021e57fe5b06010260010281179050600a8281151561023457fe5b0491506101cf565b5b8090505b919050565b61025160005461019b565b6000191660405180807f73746f7265644e756d6265720000000000000000000000000000000000000000815250600c01905060405180910390a15b565b5b5600a165627a7a72305820326efcd34df5fdba07e7a1afe7ffd4b42873ef749ae9a5915db46fd20b9c251c0029

This should result in something like so:

    {
      "txid": "72b0e0576d289c1e4e6c777431e4845f77d0884d3b3cff0387a5f4a1a3a874ea",
      "sender": "qZbjaE8N18ZU1m7851G7QGhvxKL74SRBTt",
      "hash160": "aff3e34ab836edb8d214a993d9da105915e4a6e9",
      "address": "5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9"
    }

If it shows insufficent balance, You have to mine blocks to get coins first.

    ./firovm-cli getnewaddress 

For example, we got TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2 as output. Then we call generate rpc method to give coins to the address

    ./firovm-cli generatetoaddress 200 TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2

Then you can call `createcontract` again, then call

    ./firovm-cli generatetoaddress 1 TMkeCahCkZqAf6nwZ3XYt18RiYSJmjeit2

To make a transaction into a block, and get confirmation. Now wait for your contract to be included in a block. You should be able to confirm it made it into a block by using:

    ./firovm-cli getaccountinfo 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9

If the contract was successfully executed and persisted in the blockchain, you should see something like this


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

In order to interact with the contract, you must create raw ABI data from the interface JSON file. The easiest tool to assist in this is ethabi, https://github.com/paritytech/ethabi

Make sure the JSON file is saved somewhere, we will call it interface.json. 

In order to get the `storedNumber` variable we use the `returnNumber()` function. We can construct the ABI values by using ethabi:

    ethabi encode function ~/interface.json returnNumber

The result of this is:

    3450bd6a

Now, because we are not changing state, we use `callcontract`:

    ./firovm-cli callcontract 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9 3450bd6a

This results in a lot of data that can be useful in different contexts (including gas estimates), but we are only concerned about the `output` field, which is the value of `storedNumber`

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

To change the storedNumber we can do an on-chain execution using `sendtocontract`. First, we need the ABI data:

    ethabi encode function ~/interface.json setNumber -p 123456 --lenient
    3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240

Note we use --lenient so that we do not need to provide a full 256bit value as a parameter. Now, we can execute the contract directly:

    ./firovm-cli sendtocontract 5bde092dbecb84ea1a229b4c5b25dfc9cdc674d9 3fb5c1cb000000000000000000000000000000000000000000000000000000000001e240

Afterwards, we can call `returnNumber()` again and check the `output` field:

    "output": "000000000000000000000000000000000000000000000000000000000001e240",

This is 123456 encoded as hex. 