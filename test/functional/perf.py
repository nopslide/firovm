#!/usr/bin/env python3
# Copyright (c) 2020 The Firo developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import decimal
import re
import time
import sys

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *

class WalletTest (BitcoinTestFramework):

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1

    def run_test(self):   
        print('Mining blocks ...')
        self.nodes[0].generatetoaddress(65200, self.nodes[0].getnewaddress(), 1)

		#addrList = set()
		#for x in range(10000):
		#	changeAddress = self.nodes[0].getrawchangeaddress()
		#	addrList.add(changeAddress )
		#	self.nodes[0].sendtoaddress(changeAddress ,1)
		#	self.nodes[0].generate(1)

		#if len(self.nodes[0].listunspent()) < 10000:
		#	print("UTXO does not enough...")

        for t in range(65000):
            changeAddress = self.nodes[0].getrawchangeaddress()
            self.nodes[0].sendtoaddress(changeAddress ,0.001)

        sys.stdout = open("/tmp/perf.txt", "w")
        print("BEFORE : Mempool is ", self.nodes[0].getmempoolinfo()['size'])
        start_time = time.time()
        self.nodes[0].generatetoaddress(1, self.nodes[0].getnewaddress(), 0)
        print("--- %s seconds ---" % (time.time() - start_time))
        print("AFTER : Mempool is ", self.nodes[0].getmempoolinfo()['size'])
        self.sync_all()
        sys.stdout.close()


if __name__ == '__main__':
    WalletTest().main()
