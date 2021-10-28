FiroVM
=====================================

https://firo.org

What is FiroVM?
----------------

FiroVM is sidechain to Firo for enable smart contract capability with virtual machine. It is support and compatible with Solidity.

License
-------

FiroVM is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `main` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/nopslide/firovm/tags) are created
regularly to indicate new official, stable release versions of FiroVM.

Configurations
-------------------
1. fvm.conf
        
        validatekey=BAb04pScLRHoYaNLmTFgDG1xtqTVlRzEgeXQTzvNIKQYWNhKBbbmWchat1MbA0VOT/KnNmBA01Hbz4zhcf/7KSE=
        signerkey=MIIBEwIBAQQgXMpMLJNgNdDO2zo1ir2O2eCcge+KSPgzgvpLANVtbwKggaUwgaICAQEwLAYHKoZIzj0BAQIhAP////////////////////////////////////7///wvMAYEAQAEAQcEQQR5vmZ++dy7rFWgYpXOhwsHApv82y3OKNlZ8oFbFvgXmEg62ncmo8RlXaT7/A4RCKj9F7RIpoVUGZxH0I/7ENS4AiEA/////////////////////rqu3OavSKA7v9JejNA2QUECAQGhRANCAAQG9OKUnC0R6GGjS5kxYAxtcbak1ZUcxIHl0E87zSCkGFjYSgW25lnIWrdTGwNFTk/ypzZgQNNR28+M4XH/+ykh

Linux Build Instructions and Notes
==================================

Dependencies
----------------------
1.  Update packages

        sudo apt-get update

2.  Install required packages

        sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils libboost-all-dev libgmp-dev cmake

3.  Install Berkeley DB 4.8

        sudo apt-get install software-properties-common
        sudo add-apt-repository ppa:bitcoin/bitcoin
        sudo apt-get update
        sudo apt-get install libdb4.8-dev libdb4.8++-dev

4.  Install QT 5

        sudo apt-get install libminiupnpc-dev libzmq3-dev
        sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler libqrencode-dev

Alternatively, you can use a [depends build](depends/README.md) to handle dependencies.

Build
----------------------
1.  Clone the source:

        git clone https://github.com/

2.  Build Firo-core:

    Configure and build the headless Firo binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.
        
        ./autogen.sh
        ./configure
        make
        make install #optional

    Note that the use of a [depends build](depends/README.md) necessitates passing the `--prefix` option to `./configure`.

3.  It is recommended to build and run the unit tests:

        ./configure --enable-tests
        make check

4.  After building passed output file/binaries will append in src directory

        firovmd firovm-tx firovm-wallet firovm-cli

5.  moving or copy binaries to /usr/local/bin for using in any directory

Docker
----------------------
1. Clone & Build image:

        git clone https://github.com/
        cd firovm
        docker build -t firovm .

2. Start firod

        docker run -it --rm -p 8545:8545 -v /home/user/.fvm:/home/firovm/.fvm --name firovm firovm
        # Logs...

3. Call rpc

        # On another terminal
        docker exec -it firovm firovm-cli getnetworkinfo

        # Or make it callable by just `firovm-cli`
        alias firovm-cli='docker exec -it firovm firovm-cli'
        firovm-cli getnetworkinfo
        
Testing
-------

### Runing regtest for smart contract testing
 for testing smart contract please following instruction [documentation](doc/run-regtest-smartcontract.md)

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and macOS, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

### Notes 

This repo have been tested with third party tool  
lib for rpc call and smartcontract development [qtumjs](https://github.com/qtumproject/qtumjs)  
cli for smartcontract deployment [solar](https://github.com/qtumproject/solar)  
to config genesis on EVM chain [genesis](./src/cpp-ethereum/libethashseal/genesis/fvmMainNetwork.cpp)  
DAO contract DGP Qtum pre-loaded contract [source](https://github.com/qtumproject/qtum-dgp)
