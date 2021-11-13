/*
		This file is part of cpp-ethereum.

		cpp-ethereum is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		cpp-ethereum is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../GenesisInfo.h"

static std::string const c_genesisInfoAisMainNetwork = std::string() +
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
		"networkID" : "0x51",
		"chainID": "0x51",
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
		"extraData": "",
		"gasLimit": "0x1388"
	}	
}
)E";
