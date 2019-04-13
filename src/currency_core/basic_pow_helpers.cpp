// Copyright (c) 2018-2019 Zano Project
// Copyright (c) 2018-2019 Hyle Team
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "include_base_utils.h"
using namespace epee;

#include "basic_pow_helpers.h"
#include "currency_format_utils.h"
#include "serialization/binary_utils.h"
#include "serialization/stl_containers.h"
#include "currency_core/currency_config.h"
#include "crypto/crypto.h"
#include "crypto/hash.h"
#include "common/int-util.h"
#include "ethereum/libethash/ethash/ethash.hpp"
#include "ethereum/libethash/ethash/progpow.hpp"

namespace currency
{

  //--------------------------------------------------------------
  //global object 
//   crypto::ethash::cache_manager cache;
//   void ethash_set_use_dag(bool use_dag)
//   {
//     cache.set_use_dag(use_dag);
//   }
//   //------------------------------------------------------------------
//   const uint8_t* ethash_get_dag(uint64_t epoch, uint64_t& dag_size)
//   {
//     return cache.get_dag(epoch, dag_size);
//   }
  //------------------------------------------------------------------
  int ethash_height_to_epoch(uint64_t height)
  {
    return height / ETHASH_EPOCH_LENGTH;
  }
  //--------------------------------------------------------------
  crypto::hash ethash_epoch_to_seed(int epoch)
  {
    auto res_eth = ethash_calculate_epoch_seed(epoch);
    crypto::hash result = currency::null_hash;
    memcpy(&result.data, &res_eth, sizeof(res_eth));
    return result;
  }
  //--------------------------------------------------------------
  crypto::hash get_block_longhash(uint64_t height, const crypto::hash& block_long_ash, uint64_t nonce)
  {
    int epoch = ethash_height_to_epoch(height);
    const auto& context = progpow::get_global_epoch_context_full(static_cast<int>(epoch));
    auto res_eth = progpow::hash(context, height, *(ethash::hash256*)&block_long_ash, nonce);
    crypto::hash result = currency::null_hash;
    memcpy(&result.data, &res_eth.final_hash, sizeof(res_eth.final_hash));
    return result;
  }
  //---------------------------------------------------------------
  void get_block_longhash(const block& b, crypto::hash& res)
  {
    /*
    Since etherium hash has a bit different approach in minig, to adopt our code we made little hack:
    etherium hash calculates from block's hash and(!) nonce, both passed into PoW hash function.
    To achieve the same effect we make blob of data from block in normal way, but then set to zerro nonce
    inside serialized buffer, and then pass this nonce to ethash algo as a second argument, as it expected.
    */
    blobdata bd = get_block_hashing_blob(b);

    access_nonce_in_block_blob(bd) = 0;
    crypto::hash bl_hash = crypto::cn_fast_hash(bd.data(), bd.size());

    res = get_block_longhash(get_block_height(b), bl_hash, b.nonce);
  }
  //---------------------------------------------------------------
  crypto::hash get_block_longhash(const block& b)
  {
    crypto::hash p = null_hash;
    get_block_longhash(b, p);
    return p;
  }
}