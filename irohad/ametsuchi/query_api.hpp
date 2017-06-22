/*
Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef IROHA_QUERY_API_HPP
#define IROHA_QUERY_API_HPP
#include <common.hpp>
#include <dao/dao.hpp>
#include <string>
#include <vector>
#include "rxcpp/rx-observable.hpp"
#include "rxcpp/rx.hpp"

namespace iroha {

  namespace ametsuchi {

    class QueryApi {
      /**
       * Get account by it's first public key.
       * @param pub_key
       * @return DAO Account
       */
      virtual iroha::dao::Account get_account(
          iroha::crypto::ed25519::pubkey_t pub_key) = 0;
      /**
       * Get asset by full name. For example USD#soramitsu.co.jp
       * @param full_name of an asset (name#domain)
       * @return DAO Asset
       */
      virtual iroha::dao::Asset get_asset(std::string asset_full_name) = 0;
      /**
       * Get domain by domain's full name. For example soramitsu.co.jp
       * @param full_name of a domain
       * @return DAO Domain
       */
      virtual iroha::dao::Domain get_domain(std::string domain_full_name) = 0;

      /**
       * Get wallet by wallet_id
       * @param wallet_id
       * @return DAO Wallet
       */
      virtual iroha::dao::Wallet get_wallet(std::string wallet_id) = 0;
      /**
       * Get all wallets of a account.
       * @param pub_key of a account
       * @return vector of DAO Wallet
       */
      virtual std::vector<iroha::dao::Wallet> get_account_wallets(
          iroha::crypto::ed25519::pubkey_t pub_key) = 0;

      /**
       * Get all asset of a domain.
       * @param  full_name of a domain
       * @return vector of DAO Asset
       */
      virtual std::vector<iroha::dao::Asset> get_domain_assets(
          std::string domain_full_name) = 0;
      /**
       * Get all transactions of an account.
       * @param pub_key - account's first public key
       * @return observable of DAO Transaction
       */
      virtual rxcpp::observable<iroha::dao::Transaction>
      get_account_transactions(iroha::crypto::ed25519::pubkey_t pub_key) = 0;

      /**
       * Get all transactions with a certain asset
       * @param asset_full_name - full name of an asset, i.e. name#domain
       * @return observable of DAO Transaction
       */
      virtual rxcpp::observable<iroha::dao::Transaction> get_asset_transactions(
          std::string asset_full_name) = 0;
      /**
       * Get all transactions of a certain wallet
       * @param wallet_id - unique wallet
       * @return observable of DAO Transaction
       */
      virtual rxcpp::observable<iroha::dao::Transaction>
      get_wallet_transactions(std::string wallet_id) = 0;
    };
  }
}

#endif  // IROHA_QUERY_API_HPP
