// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <miner.h>

#include <amount.h>
#include <chain.h>
#include <chainparams.h>
#include <coins.h>
#include <consensus/consensus.h>
#include <consensus/merkle.h>
#include <consensus/tx_verify.h>
#include <consensus/validation.h>
#include <policy/feerate.h>
#include <policy/policy.h>
#include <pow.h>
#include <primitives/transaction.h>
#include <timedata.h>
#include <util/convert.h>
#include <util/moneystr.h>
#include <util/system.h>
#include <net.h>
#include <key_io.h>
#ifdef ENABLE_WALLET
#include <wallet/wallet.h>
#endif

#include <algorithm>
#include <utility>

unsigned int nBytecodeTimeBuffer = BYTECODE_TIME_BUFFER;

int64_t UpdateTime(CBlockHeader* pblock, const Consensus::Params& consensusParams, const CBlockIndex* pindexPrev)
{
    int64_t nOldTime = pblock->nTime;
    int64_t nNewTime = std::max(pindexPrev->GetMedianTimePast()+1, GetAdjustedTime());

    if (nOldTime < nNewTime)
        pblock->nTime = nNewTime;

    // Updating time can change work required on testnet:
    if (consensusParams.fPowAllowMinDifficultyBlocks)
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensusParams);

    return nNewTime - nOldTime;
}

BlockAssembler::Options::Options() {
    blockMinFeeRate = CFeeRate(DEFAULT_BLOCK_MIN_TX_FEE);
    nBlockMaxWeight = DEFAULT_BLOCK_MAX_WEIGHT;
}

BlockAssembler::BlockAssembler(const CTxMemPool& mempool, const CChainParams& params, const Options& options)
    : chainparams(params),
      m_mempool(mempool)
{
    blockMinFeeRate = options.blockMinFeeRate;
    // Limit weight to between 4K and MAX_BLOCK_WEIGHT-4K for sanity:
    nBlockMaxWeight = std::max<size_t>(4000, std::min<size_t>(dgpMaxBlockWeight - 4000, options.nBlockMaxWeight));
}

static BlockAssembler::Options DefaultOptions()
{
    // Block resource limits
    // If -blockmaxweight is not given, limit to DEFAULT_BLOCK_MAX_WEIGHT
    BlockAssembler::Options options;
    options.nBlockMaxWeight = gArgs.GetArg("-blockmaxweight", DEFAULT_BLOCK_MAX_WEIGHT);
    CAmount n = 0;
    if (gArgs.IsArgSet("-blockmintxfee") && ParseMoney(gArgs.GetArg("-blockmintxfee", ""), n)) {
        options.blockMinFeeRate = CFeeRate(n);
    } else {
        options.blockMinFeeRate = CFeeRate(DEFAULT_BLOCK_MIN_TX_FEE);
    }
    return options;
}

BlockAssembler::BlockAssembler(const CTxMemPool& mempool, const CChainParams& params)
    : BlockAssembler(mempool, params, DefaultOptions()) {}

#ifdef ENABLE_WALLET
BlockAssembler::BlockAssembler(const CTxMemPool& mempool, const CChainParams& params, CWallet *_pwallet)
    : BlockAssembler(mempool, params)
{
    pwallet = _pwallet;
}
#endif

void BlockAssembler::resetBlock()
{
    inBlock.clear();

    // Reserve space for coinbase tx
    nBlockWeight = 4000;
    nBlockSigOpsCost = 400;
    fIncludeWitness = false;

    // These counters do not include coinbase tx
    nBlockTx = 0;
    nFees = 0;
}

void BlockAssembler::RebuildRefundTransaction(){
    CMutableTransaction contrTx(originalRewardTx);
    contrTx.vout[0].nValue = nFees + GetBlockSubsidy(nHeight, chainparams.GetConsensus());
    contrTx.vout[0].nValue -= bceResult.refundSender;
    int i=contrTx.vout.size();
    contrTx.vout.resize(contrTx.vout.size()+bceResult.refundOutputs.size());
    for(CTxOut& vout : bceResult.refundOutputs){
        contrTx.vout[i]=vout;
        i++;
    }
    pblock->vtx[0] = MakeTransactionRef(std::move(contrTx));
}


Optional<int64_t> BlockAssembler::m_last_block_num_txs{nullopt};
Optional<int64_t> BlockAssembler::m_last_block_weight{nullopt};

std::unique_ptr<CBlockTemplate> BlockAssembler::CreateNewBlock(const CScript& scriptPubKeyIn, bool fMineWitnessTx, int64_t* pTotalFees, int32_t txProofTime, int32_t nTimeLimit)
{
    int64_t nTimeStart = GetTimeMicros();

    resetBlock();

    pblocktemplate.reset(new CBlockTemplate());

    if(!pblocktemplate.get())
        return nullptr;
    pblock = &pblocktemplate->block; // pointer for convenience

    this->nTimeLimit = nTimeLimit;

    // Add dummy coinbase tx as first transaction
    pblock->vtx.emplace_back();
    pblocktemplate->vTxFees.push_back(-1); // updated at end
    pblocktemplate->vTxSigOpsCost.push_back(-1); // updated at end

    LOCK2(cs_main, m_mempool.cs);
    CBlockIndex* pindexPrev = ::ChainActive().Tip();
    assert(pindexPrev != nullptr);
    nHeight = pindexPrev->nHeight + 1;

    pblock->nVersion = ComputeBlockVersion(pindexPrev, chainparams.GetConsensus());
    // -regtest only: allow overriding block.nVersion with
    // -blockversion=N to test forking scenarios
    if (chainparams.MineBlocksOnDemand())
        pblock->nVersion = gArgs.GetArg("-blockversion", pblock->nVersion);

    
    /* if(txProofTime == 0) {
        txProofTime = GetAdjustedTime();
    }

    pblock->nTime = txProofTime;*/
    pblock->nTime = GetAdjustedTime();

    const int64_t nMedianTimePast = pindexPrev->GetMedianTimePast();

    nLockTimeCutoff = (STANDARD_LOCKTIME_VERIFY_FLAGS & LOCKTIME_MEDIAN_TIME_PAST)
                       ? nMedianTimePast
                       : pblock->GetBlockTime();

    // Decide whether to include witness transactions
    // This is only needed in case the witness softfork activation is reverted
    // (which would require a very deep reorganization).
    // Note that the mempool would accept transactions with witness data before
    // IsWitnessEnabled, but we would only ever mine blocks after IsWitnessEnabled
    // unless there is a massive block reorganization with the witness softfork
    // not activated.
    // TODO: replace this with a call to main to assess validity of a mempool
    // transaction (which in most cases can be a no-op).
    fIncludeWitness = IsWitnessEnabled(pindexPrev, chainparams.GetConsensus());

    int64_t nTime1 = GetTimeMicros();

    m_last_block_num_txs = nBlockTx;
    m_last_block_weight = nBlockWeight;

    // Create coinbase transaction.
    CMutableTransaction coinbaseTx;
    coinbaseTx.vin.resize(1);
    coinbaseTx.vin[0].prevout.SetNull();
    coinbaseTx.vout.resize(1);
    coinbaseTx.vout[0].scriptPubKey = scriptPubKeyIn;
    coinbaseTx.vout[0].nValue = nFees + GetBlockSubsidy(nHeight, chainparams.GetConsensus());
    coinbaseTx.vin[0].scriptSig = CScript() << nHeight << OP_0;
    originalRewardTx = coinbaseTx;
    pblock->vtx[0] = MakeTransactionRef(std::move(coinbaseTx));

    FvmDGP fvmDGP(globalState.get(), fGettingValuesDGP);
    globalSealEngine->setFvmSchedule(fvmDGP.getGasSchedule(nHeight));
    uint32_t blockSizeDGP = fvmDGP.getBlockSize(nHeight);
    minGasPrice = fvmDGP.getMinGasPrice(nHeight);
    if(gArgs.IsArgSet("-min-tx-gas-price")) {
        CAmount stakerMinGasPrice;
        if(ParseMoney(gArgs.GetArg("-min-tx-gas-price", ""), stakerMinGasPrice)) {
            minGasPrice = std::max(minGasPrice, (uint64_t)stakerMinGasPrice);
        }
    }
    hardBlockGasLimit = fvmDGP.getBlockGasLimit(nHeight);
    softBlockGasLimit = gArgs.GetArg("-soft-block-gas-limit", hardBlockGasLimit);
    softBlockGasLimit = std::min(softBlockGasLimit, hardBlockGasLimit);
    txGasLimit = gArgs.GetArg("-max-tx-gas-limit", softBlockGasLimit);

    nBlockMaxWeight = blockSizeDGP ? blockSizeDGP * WITNESS_SCALE_FACTOR : nBlockMaxWeight;
    
    dev::h256 oldHashStateRoot(globalState->rootHash());
    dev::h256 oldHashUTXORoot(globalState->rootHashUTXO());
    int nPackagesSelected = 0;
    int nDescendantsUpdated = 0;
    addPackageTxs(nPackagesSelected, nDescendantsUpdated, minGasPrice);
    pblock->hashStateRoot = uint256(h256Touint(dev::h256(globalState->rootHash())));
    pblock->hashUTXORoot = uint256(h256Touint(dev::h256(globalState->rootHashUTXO())));
    globalState->setRoot(oldHashStateRoot);
    globalState->setRootUTXO(oldHashUTXORoot);

    //this should already be populated by AddBlock in case of contracts, but if no contracts
    //then it won't get populated
    RebuildRefundTransaction();

    pblocktemplate->vchCoinbaseCommitment = GenerateCoinbaseCommitment(*pblock, pindexPrev, chainparams.GetConsensus());
    pblocktemplate->vTxFees[0] = -nFees;

    // The total fee is the Fees minus the Refund
    if (pTotalFees)
        *pTotalFees = nFees - bceResult.refundSender;


    LogPrintf("CreateNewBlock(): block weight: %u txs: %u fees: %ld sigops %d\n", GetBlockWeight(*pblock), nBlockTx, nFees, nBlockSigOpsCost);

    // Fill in header
    pblock->hashPrevBlock  = pindexPrev->GetBlockHash();    
    UpdateTime(pblock, chainparams.GetConsensus(), pindexPrev);
    pblock->nBits          = GetNextWorkRequired(pindexPrev, pblock, chainparams.GetConsensus());
    pblock->nNonce         = 0;
    pblocktemplate->vTxSigOpsCost[0] = WITNESS_SCALE_FACTOR * GetLegacySigOpCount(*pblock->vtx[0]);

    BlockValidationState state;
    if (!TestBlockValidity(state, chainparams, *pblock, pindexPrev, false, false)) {
        throw std::runtime_error(strprintf("%s: TestBlockValidity failed: %s", __func__, state.ToString()));
    }
    int64_t nTime2 = GetTimeMicros();

    LogPrint(BCLog::BENCH, "CreateNewBlock() packages: %.2fms (%d packages, %d updated descendants), validity: %.2fms (total %.2fms)\n", 0.001 * (nTime1 - nTimeStart), nPackagesSelected, nDescendantsUpdated, 0.001 * (nTime2 - nTime1), 0.001 * (nTime2 - nTimeStart));

    return std::move(pblocktemplate);
}

void BlockAssembler::onlyUnconfirmed(CTxMemPool::setEntries& testSet)
{
    for (CTxMemPool::setEntries::iterator iit = testSet.begin(); iit != testSet.end(); ) {
        // Only test txs not already in the block
        if (inBlock.count(*iit)) {
            testSet.erase(iit++);
        }
        else {
            iit++;
        }
    }
}

bool BlockAssembler::TestPackage(uint64_t packageSize, int64_t packageSigOpsCost) const
{
    // TODO: switch to weight-based accounting for packages instead of vsize-based accounting.
    if (nBlockWeight + WITNESS_SCALE_FACTOR * packageSize >= nBlockMaxWeight)
        return false;
    if (nBlockSigOpsCost + packageSigOpsCost >= (uint64_t)dgpMaxBlockSigOps)
        return false;
    return true;
}

// Perform transaction-level checks before adding to block:
// - transaction finality (locktime)
// - premature witness (in case segwit transactions are added to mempool before
//   segwit activation)
bool BlockAssembler::TestPackageTransactions(const CTxMemPool::setEntries& package)
{
    for (CTxMemPool::txiter it : package) {
        if (!IsFinalTx(it->GetTx(), nHeight, nLockTimeCutoff))
            return false;
        if (!fIncludeWitness && it->GetTx().HasWitness())
            return false;
    }
    return true;
}

bool BlockAssembler::AttemptToAddContractToBlock(CTxMemPool::txiter iter, uint64_t minGasPrice) {
    if (nTimeLimit != 0 && GetAdjustedTime() >= nTimeLimit - nBytecodeTimeBuffer) {
        return false;
    }
    
    dev::h256 oldHashStateRoot(globalState->rootHash());
    dev::h256 oldHashUTXORoot(globalState->rootHashUTXO());
    // operate on local vars first, then later apply to `this`
    uint64_t nBlockWeight = this->nBlockWeight;
    uint64_t nBlockSigOpsCost = this->nBlockSigOpsCost;

    unsigned int contractflags = GetContractScriptFlags(nHeight, chainparams.GetConsensus());
    FvmTxConverter convert(iter->GetTx(), NULL, &pblock->vtx, contractflags);

    ExtractFvmTX resultConverter;
    if(!convert.extractionFvmTransactions(resultConverter)){
        //this check already happens when accepting txs into mempool
        //therefore, this can only be triggered by using raw transactions on the staker itself
        LogPrintf("AttemptToAddContractToBlock(): Fail to extract contacts from tx %s\n", iter->GetTx().GetHash().ToString());
        return false;
    }
    std::vector<FvmTransaction> fvmTransactions = resultConverter.first;
    dev::u256 txGas = 0;
    for(FvmTransaction fvmTransaction : fvmTransactions){
        txGas += fvmTransaction.gas();
        if(txGas > txGasLimit) {
            // Limit the tx gas limit by the soft limit if such a limit has been specified.
            LogPrintf("AttemptToAddContractToBlock(): The gas needed is bigger than -staker-max-tx-gas-limit for the contract tx %s\n", iter->GetTx().GetHash().ToString());
            return false;
        }

        if(bceResult.usedGas + fvmTransaction.gas() > softBlockGasLimit){
            // If this transaction's gasLimit could cause block gas limit to be exceeded, then don't add it
            // Log if the contract is the only contract tx
            if(bceResult.usedGas == 0)
                LogPrintf("AttemptToAddContractToBlock(): The gas needed is bigger than -staker-soft-block-gas-limit for the contract tx %s\n", iter->GetTx().GetHash().ToString());
            return false;
        }
        if(fvmTransaction.gasPrice() < minGasPrice){
            //if this transaction's gasPrice is less than the current DGP minGasPrice don't add it
            LogPrintf("AttemptToAddContractToBlock(): The gas price is less than -staker-min-tx-gas-price for the contract tx %s\n", iter->GetTx().GetHash().ToString());
            return false;
        }
    }
    // We need to pass the DGP's block gas limit (not the soft limit) since it is consensus critical.
    ByteCodeExec exec(*pblock, fvmTransactions, hardBlockGasLimit, ::ChainActive().Tip());
    if(!exec.performByteCode()){
        //error, don't add contract
        globalState->setRoot(oldHashStateRoot);
        globalState->setRootUTXO(oldHashUTXORoot);
        LogPrintf("AttemptToAddContractToBlock(): Perform byte code fails for the contract tx %s\n", iter->GetTx().GetHash().ToString());
        return false;
    }

    ByteCodeExecResult testExecResult;
    if(!exec.processingResults(testExecResult)){
        globalState->setRoot(oldHashStateRoot);
        globalState->setRootUTXO(oldHashUTXORoot);
        LogPrintf("AttemptToAddContractToBlock(): Processing results fails for the contract tx %s\n", iter->GetTx().GetHash().ToString());
        return false;
    }

    if(bceResult.usedGas + testExecResult.usedGas > softBlockGasLimit){
        // If this transaction could cause block gas limit to be exceeded, then don't add it
        globalState->setRoot(oldHashStateRoot);
        globalState->setRootUTXO(oldHashUTXORoot);
        // Log if the contract is the only contract tx
        if(bceResult.usedGas == 0)
            LogPrintf("AttemptToAddContractToBlock(): The gas used is bigger than -staker-soft-block-gas-limit for the contract tx %s\n", iter->GetTx().GetHash().ToString());
        return false;
    }

    //apply contractTx costs to local state
    nBlockWeight += iter->GetTxWeight();
    nBlockSigOpsCost += iter->GetSigOpCost();
    //apply value-transfer txs to local state
    for (CTransaction &t : testExecResult.valueTransfers) {
        nBlockWeight += GetTransactionWeight(t);
        nBlockSigOpsCost += GetLegacySigOpCount(t);
    }

    //calculate sigops from new refund/proof tx

    //first, subtract old proof tx
    nBlockSigOpsCost -= GetLegacySigOpCount(*pblock->vtx[0]);

    // manually rebuild refundtx
    CMutableTransaction contrTx(*pblock->vtx[0]);
    int i=contrTx.vout.size();
    contrTx.vout.resize(contrTx.vout.size()+testExecResult.refundOutputs.size());
    for(CTxOut& vout : testExecResult.refundOutputs){
        contrTx.vout[i]=vout;
        i++;
    }
    nBlockSigOpsCost += GetLegacySigOpCount(contrTx);
    //all contract costs now applied to local state

    //Check if block will be too big or too expensive with this contract execution
    if (nBlockSigOpsCost * WITNESS_SCALE_FACTOR > (uint64_t)dgpMaxBlockSigOps ||
            nBlockWeight > dgpMaxBlockWeight) {
        //contract will not be added to block, so revert state to before we tried
        globalState->setRoot(oldHashStateRoot);
        globalState->setRootUTXO(oldHashUTXORoot);
        return false;
    }

    //block is not too big, so apply the contract execution and it's results to the actual block

    //apply local bytecode to global bytecode state
    bceResult.usedGas += testExecResult.usedGas;
    bceResult.refundSender += testExecResult.refundSender;
    bceResult.refundOutputs.insert(bceResult.refundOutputs.end(), testExecResult.refundOutputs.begin(), testExecResult.refundOutputs.end());
    bceResult.valueTransfers = std::move(testExecResult.valueTransfers);

    pblock->vtx.emplace_back(iter->GetSharedTx());
    pblocktemplate->vTxFees.push_back(iter->GetFee());
    pblocktemplate->vTxSigOpsCost.push_back(iter->GetSigOpCost());
    this->nBlockWeight += iter->GetTxWeight();
    ++nBlockTx;
    this->nBlockSigOpsCost += iter->GetSigOpCost();
    nFees += iter->GetFee();
    inBlock.insert(iter);

    for (CTransaction &t : bceResult.valueTransfers) {
        pblock->vtx.emplace_back(MakeTransactionRef(std::move(t)));
        this->nBlockWeight += GetTransactionWeight(t);
        this->nBlockSigOpsCost += GetLegacySigOpCount(t);
        ++nBlockTx;
    }
    //calculate sigops from new refund/proof tx
    this->nBlockSigOpsCost -= GetLegacySigOpCount(*pblock->vtx[0]);
    RebuildRefundTransaction();
    this->nBlockSigOpsCost += GetLegacySigOpCount(*pblock->vtx[0]);

    bceResult.valueTransfers.clear();

    return true;
}

void BlockAssembler::AddToBlock(CTxMemPool::txiter iter)
{
    pblock->vtx.emplace_back(iter->GetSharedTx());
    pblocktemplate->vTxFees.push_back(iter->GetFee());
    pblocktemplate->vTxSigOpsCost.push_back(iter->GetSigOpCost());
    nBlockWeight += iter->GetTxWeight();
    ++nBlockTx;
    nBlockSigOpsCost += iter->GetSigOpCost();
    nFees += iter->GetFee();
    inBlock.insert(iter);

    bool fPrintPriority = gArgs.GetBoolArg("-printpriority", DEFAULT_PRINTPRIORITY);
    if (fPrintPriority) {
        LogPrintf("fee %s txid %s\n",
                  CFeeRate(iter->GetModifiedFee(), iter->GetTxSize()).ToString(),
                  iter->GetTx().GetHash().ToString());
    }
}

int BlockAssembler::UpdatePackagesForAdded(const CTxMemPool::setEntries& alreadyAdded,
        indexed_modified_transaction_set &mapModifiedTx)
{
    int nDescendantsUpdated = 0;
    for (CTxMemPool::txiter it : alreadyAdded) {
        CTxMemPool::setEntries descendants;
        m_mempool.CalculateDescendants(it, descendants);
        // Insert all descendants (not yet in block) into the modified set
        for (CTxMemPool::txiter desc : descendants) {
            if (alreadyAdded.count(desc))
                continue;
            ++nDescendantsUpdated;
            modtxiter mit = mapModifiedTx.find(desc);
            if (mit == mapModifiedTx.end()) {
                CTxMemPoolModifiedEntry modEntry(desc);
                modEntry.nSizeWithAncestors -= it->GetTxSize();
                modEntry.nModFeesWithAncestors -= it->GetModifiedFee();
                modEntry.nSigOpCostWithAncestors -= it->GetSigOpCost();
                mapModifiedTx.insert(modEntry);
            } else {
                mapModifiedTx.modify(mit, update_for_parent_inclusion(it));
            }
        }
    }
    return nDescendantsUpdated;
}

// Skip entries in mapTx that are already in a block or are present
// in mapModifiedTx (which implies that the mapTx ancestor state is
// stale due to ancestor inclusion in the block)
// Also skip transactions that we've already failed to add. This can happen if
// we consider a transaction in mapModifiedTx and it fails: we can then
// potentially consider it again while walking mapTx.  It's currently
// guaranteed to fail again, but as a belt-and-suspenders check we put it in
// failedTx and avoid re-evaluation, since the re-evaluation would be using
// cached size/sigops/fee values that are not actually correct.
bool BlockAssembler::SkipMapTxEntry(CTxMemPool::txiter it, indexed_modified_transaction_set &mapModifiedTx, CTxMemPool::setEntries &failedTx)
{
    assert(it != m_mempool.mapTx.end());
    return mapModifiedTx.count(it) || inBlock.count(it) || failedTx.count(it);
}

void BlockAssembler::SortForBlock(const CTxMemPool::setEntries& package, std::vector<CTxMemPool::txiter>& sortedEntries)
{
    // Sort package by ancestor count
    // If a transaction A depends on transaction B, then A's ancestor count
    // must be greater than B's.  So this is sufficient to validly order the
    // transactions for block inclusion.
    sortedEntries.clear();
    sortedEntries.insert(sortedEntries.begin(), package.begin(), package.end());
    std::sort(sortedEntries.begin(), sortedEntries.end(), CompareTxIterByAncestorCount());
}

// This transaction selection algorithm orders the mempool based
// on feerate of a transaction including all unconfirmed ancestors.
// Since we don't remove transactions from the mempool as we select them
// for block inclusion, we need an alternate method of updating the feerate
// of a transaction with its not-yet-selected ancestors as we go.
// This is accomplished by walking the in-mempool descendants of selected
// transactions and storing a temporary modified state in mapModifiedTxs.
// Each time through the loop, we compare the best transaction in
// mapModifiedTxs with the next transaction in the mempool to decide what
// transaction package to work on next.
void BlockAssembler::addPackageTxs(int &nPackagesSelected, int &nDescendantsUpdated, uint64_t minGasPrice)
{
    // mapModifiedTx will store sorted packages after they are modified
    // because some of their txs are already in the block
    indexed_modified_transaction_set mapModifiedTx;
    // Keep track of entries that failed inclusion, to avoid duplicate work
    CTxMemPool::setEntries failedTx;

    // Start by adding all descendants of previously added txs to mapModifiedTx
    // and modifying them for their already included ancestors
    UpdatePackagesForAdded(inBlock, mapModifiedTx);

    CTxMemPool::indexed_transaction_set::index<ancestor_score_or_gas_price>::type::iterator mi = m_mempool.mapTx.get<ancestor_score_or_gas_price>().begin();
    CTxMemPool::txiter iter;

    // Limit the number of attempts to add transactions to the block when it is
    // close to full; this is just a simple heuristic to finish quickly if the
    // mempool has a lot of entries.
    const int64_t MAX_CONSECUTIVE_FAILURES = 1000;
    int64_t nConsecutiveFailed = 0;

    while (mi != m_mempool.mapTx.get<ancestor_score_or_gas_price>().end() || !mapModifiedTx.empty()) {
        if(nTimeLimit != 0 && GetAdjustedTime() >= nTimeLimit){
            //no more time to add transactions, just exit
            return;
        }
        // First try to find a new transaction in mapTx to evaluate.
        if (mi != m_mempool.mapTx.get<ancestor_score_or_gas_price>().end() &&
            SkipMapTxEntry(m_mempool.mapTx.project<0>(mi), mapModifiedTx, failedTx)) {
            ++mi;
            continue;
        }

        // Now that mi is not stale, determine which transaction to evaluate:
        // the next entry from mapTx, or the best from mapModifiedTx?
        bool fUsingModified = false;

        modtxscoreiter modit = mapModifiedTx.get<ancestor_score_or_gas_price>().begin();
        if (mi == m_mempool.mapTx.get<ancestor_score_or_gas_price>().end()) {
            // We're out of entries in mapTx; use the entry from mapModifiedTx
            iter = modit->iter;
            fUsingModified = true;
        } else {
            // Try to compare the mapTx entry to the mapModifiedTx entry
            iter = m_mempool.mapTx.project<0>(mi);
            if (modit != mapModifiedTx.get<ancestor_score_or_gas_price>().end() &&
                    CompareModifiedEntry()(*modit, CTxMemPoolModifiedEntry(iter))) {
                // The best entry in mapModifiedTx has higher score
                // than the one from mapTx.
                // Switch which transaction (package) to consider
                iter = modit->iter;
                fUsingModified = true;
            } else {
                // Either no entry in mapModifiedTx, or it's worse than mapTx.
                // Increment mi for the next loop iteration.
                ++mi;
            }
        }

        // We skip mapTx entries that are inBlock, and mapModifiedTx shouldn't
        // contain anything that is inBlock.
        assert(!inBlock.count(iter));

        uint64_t packageSize = iter->GetSizeWithAncestors();
        CAmount packageFees = iter->GetModFeesWithAncestors();
        int64_t packageSigOpsCost = iter->GetSigOpCostWithAncestors();
        if (fUsingModified) {
            packageSize = modit->nSizeWithAncestors;
            packageFees = modit->nModFeesWithAncestors;
            packageSigOpsCost = modit->nSigOpCostWithAncestors;
        }

        if (packageFees < blockMinFeeRate.GetFee(packageSize)) {
            // Everything else we might consider has a lower fee rate
            return;
        }

        if (!TestPackage(packageSize, packageSigOpsCost)) {
            if (fUsingModified) {
                // Since we always look at the best entry in mapModifiedTx,
                // we must erase failed entries so that we can consider the
                // next best entry on the next loop iteration
                mapModifiedTx.get<ancestor_score_or_gas_price>().erase(modit);
                failedTx.insert(iter);
            }

            ++nConsecutiveFailed;

            if (nConsecutiveFailed > MAX_CONSECUTIVE_FAILURES && nBlockWeight >
                    nBlockMaxWeight - 4000) {
                // Give up if we're close to full and haven't succeeded in a while
                break;
            }
            continue;
        }

        CTxMemPool::setEntries ancestors;
        uint64_t nNoLimit = std::numeric_limits<uint64_t>::max();
        std::string dummy;
        m_mempool.CalculateMemPoolAncestors(*iter, ancestors, nNoLimit, nNoLimit, nNoLimit, nNoLimit, dummy, false);

        onlyUnconfirmed(ancestors);
        ancestors.insert(iter);

        // Test if all tx's are Final
        if (!TestPackageTransactions(ancestors)) {
            if (fUsingModified) {
                mapModifiedTx.get<ancestor_score_or_gas_price>().erase(modit);
                failedTx.insert(iter);
            }
            continue;
        }

        // This transaction will make it in; reset the failed counter.
        nConsecutiveFailed = 0;

        // Package can be added. Sort the entries in a valid order.
        std::vector<CTxMemPool::txiter> sortedEntries;
        SortForBlock(ancestors, sortedEntries);

        bool wasAdded=true;
        for (size_t i=0; i<sortedEntries.size(); ++i) {
            if(!wasAdded || (nTimeLimit != 0 && GetAdjustedTime() >= nTimeLimit))
            {
                //if out of time, or earlier ancestor failed, then skip the rest of the transactions
                mapModifiedTx.erase(sortedEntries[i]);
                wasAdded=false;
                continue;
            }
            const CTransaction& tx = sortedEntries[i]->GetTx();
            if(wasAdded) {
                if (tx.HasCreateOrCall()) {
                    wasAdded = AttemptToAddContractToBlock(sortedEntries[i], minGasPrice);
                    if(!wasAdded){
                        if(fUsingModified) {
                            //this only needs to be done once to mark the whole package (everything in sortedEntries) as failed
                            mapModifiedTx.get<ancestor_score_or_gas_price>().erase(modit);
                            failedTx.insert(iter);
                        }
                    }
                } else {
                    AddToBlock(sortedEntries[i]);
                }
            }
            mapModifiedTx.erase(sortedEntries[i]);
        }

        if(!wasAdded){
            //skip UpdatePackages if a transaction failed to be added (match TestPackage logic)
            continue;
        }

        ++nPackagesSelected;

        // Update transactions that depend on each of these
        nDescendantsUpdated += UpdatePackagesForAdded(ancestors, mapModifiedTx);
    }
}

void IncrementExtraNonce(CBlock* pblock, const CBlockIndex* pindexPrev, unsigned int& nExtraNonce)
{
    // Update nExtraNonce
    static uint256 hashPrevBlock;
    if (hashPrevBlock != pblock->hashPrevBlock)
    {
        nExtraNonce = 0;
        hashPrevBlock = pblock->hashPrevBlock;
    }
    ++nExtraNonce;
    unsigned int nHeight = pindexPrev->nHeight+1; // Height first in coinbase required for block.version=2
    CMutableTransaction txCoinbase(*pblock->vtx[0]);
    txCoinbase.vin[0].scriptSig = (CScript() << nHeight << CScriptNum(nExtraNonce));
    assert(txCoinbase.vin[0].scriptSig.size() <= 120);

    pblock->vtx[0] = MakeTransactionRef(std::move(txCoinbase));
    pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);
}
