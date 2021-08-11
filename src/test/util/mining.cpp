// Copyright (c) 2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/util/mining.h>

#include <chainparams.h>
#include <consensus/merkle.h>
#include <key_io.h>
#include <miner.h>
#include <node/context.h>
#include <script/standard.h>
#include <validation.h>

CTxIn generatetoaddress(const NodeContext& node, const std::string& address)
{
    const auto dest = DecodeDestination(address);
    assert(IsValidDestination(dest));
    const auto coinbase_script = GetScriptForDestination(dest);

    return MineBlock(node, coinbase_script);
}

CTxIn MineBlock(const NodeContext& node, const CScript& coinbase_scriptPubKey)
{
    auto block = PrepareBlock(node, coinbase_scriptPubKey);

    if(gArgs.IsArgSet("-signerkey")) {            
            std::string strPrivKey = gArgs.GetArg("-signerkey", "");
            bool invalid;
            std::vector<unsigned char> vchPrivKey = DecodeBase64(strPrivKey.c_str(), &invalid);
            if (invalid) {
                throw ("CreateNewBlock(): signerkey has invalided base64");
            }
            CKey key;
            key.SetPrivKey(CPrivKey(vchPrivKey.begin(), vchPrivKey.end()));
            // sign and appen signature to the block
            std::vector<unsigned char> vchSig;
            bool isSigned = key.Sign(block->GetHashWithoutSign(), vchSig);
            block->vchBlockSig = vchSig;
    }else{
            throw ("Signerkey does not provide");
    }

    bool processed{ProcessNewBlock(Params(), block, true, nullptr)};
    assert(processed);

    return CTxIn{block->vtx[0]->GetHash(), 0};
}

std::shared_ptr<CBlock> PrepareBlock(const NodeContext& node, const CScript& coinbase_scriptPubKey)
{
    assert(node.mempool);
    auto block = std::make_shared<CBlock>(
        BlockAssembler{*node.mempool, Params()}
            .CreateNewBlock(coinbase_scriptPubKey)
            ->block);

    LOCK(cs_main);
    block->nTime = ::ChainActive().Tip()->GetMedianTimePast() + 1;
    block->hashMerkleRoot = BlockMerkleRoot(*block);

    return block;
}
