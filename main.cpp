#include <memory>
#include <iostream>
#include <string>
#include <future>
#include <atomic>

#include <boost/lexical_cast.hpp>

#include "src/wallet/api/wallet2_api.h"
#include "contrib/epee/include/misc_log_ex.h"

namespace {
    struct WalletListener: public Monero::WalletListener
    {
        WalletListener() : active_(false) {}
        WalletListener(const WalletListener& w) = delete;

        virtual ~WalletListener() {}
        virtual void moneySpent(const std::string &txId, uint64_t amount) {}
        virtual void moneyReceived(const std::string &txId, uint64_t amount) {}
        virtual void unconfirmedMoneyReceived(const std::string &txId, uint64_t amount) {}

        virtual void newBlock(uint64_t height) {
            if (active_) {
                std::cout << "updated blockchain height: " << height << std::endl;
            }
        }

        virtual void updated() {}
        virtual void refreshed() {
            if (active_) {
                promise_->set_value();
            }
        }

        void waitRefresh() {
            promise_.reset(new std::promise<void>());
            auto future = promise_->get_future();
            active_ = true;

            future.wait();

            active_ = false;
        }

        std::atomic<bool> active_;
        std::unique_ptr<std::promise<void>> promise_;
    };

    struct Context {
        Context() {
            listener.reset(new WalletListener());
            listener2.reset(new WalletListener());
            listener3.reset(new WalletListener());
        }

        Context(const Context& context) = delete;
        Context(Context&& context) = default;

        std::unique_ptr<Monero::Wallet> wallet;
        std::unique_ptr<Monero::Wallet> wallet2;
        std::unique_ptr<Monero::Wallet> wallet3;

        std::unique_ptr<WalletListener> listener;
        std::unique_ptr<WalletListener> listener2;
        std::unique_ptr<WalletListener> listener3;
    };
}

Context genWallets(std::unique_ptr<Monero::WalletManager>& walletManager) {
    Context context;
    context.wallet = std::unique_ptr<Monero::Wallet>(walletManager->createWallet("test-wallet", "123", "en", Monero::NetworkType::MAINNET));
    context.wallet2 = std::unique_ptr<Monero::Wallet>(walletManager->createWallet("test-wallet2", "123", "en", Monero::NetworkType::MAINNET));
    context.wallet3 = std::unique_ptr<Monero::Wallet>(walletManager->createWallet("test-wallet3", "123", "en", Monero::NetworkType::MAINNET));

    auto msig1 = context.wallet->getMultisigInfo();
    auto msig2 = context.wallet2->getMultisigInfo();
    auto msig3 = context.wallet3->getMultisigInfo();

    std::cout << "~~~~~~ make multisig" << std::endl;
    auto info1 = context.wallet->makeMultisig({msig1, msig2, msig3}, 2);
    auto info2 = context.wallet2->makeMultisig({msig1, msig2, msig3}, 2);
    auto info3 = context.wallet3->makeMultisig({msig1, msig2, msig3}, 2);

    std::cout << "~~~~~~~ w1 status: " << context.wallet->status() << std::endl;
    std::cout << "~~~~~~~ w2 status: " << context.wallet2->status() << std::endl;
    std::cout << "~~~~~~~ w3 status: " << context.wallet3->status() << std::endl;

    std::cout << "~~~~~~~ w1 finalize success: " << std::boolalpha << context.wallet->finalizeMultisig({info1, info2, info3}) << std::endl;
    std::cout << "~~~~~~~ w2 finalize success: " << std::boolalpha << context.wallet2->finalizeMultisig({info1, info2, info3}) << std::endl;
    std::cout << "~~~~~~~ w3 finalize success: " << std::boolalpha << context.wallet3->finalizeMultisig({info1, info2, info3}) << std::endl;

    return context;
}

Context openWallets(std::unique_ptr<Monero::WalletManager>& walletManager) {
    Context context;

    context.wallet = std::unique_ptr<Monero::Wallet>(walletManager->openWallet("test-wallet", "123", Monero::NetworkType::MAINNET));
    context.wallet->setListener(context.listener.get());
    context.wallet->init("monero1.exan.tech:18081");
    context.wallet->setTrustedDaemon(true);

    context.wallet2 = std::unique_ptr<Monero::Wallet>(walletManager->openWallet("test-wallet2", "123", Monero::NetworkType::MAINNET));
    context.wallet2->setListener(context.listener2.get());
    context.wallet2->init("monero1.exan.tech:18081");
    context.wallet2->setTrustedDaemon(true);

    context.wallet3 = std::unique_ptr<Monero::Wallet>(walletManager->openWallet("test-wallet3", "123", Monero::NetworkType::MAINNET));
    context.wallet3->setListener(context.listener3.get());
    context.wallet3->init("monero1.exan.tech:18081");
    context.wallet3->setTrustedDaemon(true);

    return context;
}

void balance(Context& context) {
    std::cout << "w1 balance: " << Monero::Wallet::displayAmount(context.wallet->balance()) << ", unlocked balance: " << Monero::Wallet::displayAmount(context.wallet->unlockedBalance()) << std::endl;
    std::cout << "w2 balance: " << Monero::Wallet::displayAmount(context.wallet2->balance()) << ", unlocked balance: " << Monero::Wallet::displayAmount(context.wallet2->unlockedBalance()) << std::endl;
    std::cout << "w3 balance: " << Monero::Wallet::displayAmount(context.wallet3->balance()) << ", unlocked balance: " << Monero::Wallet::displayAmount(context.wallet3->unlockedBalance()) << std::endl;
}

void sync(Context& context) {
    std::cout << "syncing wallet 1" << std::endl;
    context.wallet->startRefresh();
    context.listener->waitRefresh();

    std::cout << "syncing wallet 2" << std::endl;
    context.wallet2->startRefresh();
    context.listener2->waitRefresh();

    std::cout << "syncing wallet 3" << std::endl;
    context.wallet3->startRefresh();
    context.listener3->waitRefresh();
}

void saveWallets(Context& context) {
    std::cout << "saving wallet 1" << std::endl;
    context.wallet->store("test-wallet");

    std::cout << "saving wallet 2" << std::endl;
    context.wallet2->store("test-wallet2");

    std::cout << "saving wallet 3" << std::endl;
    context.wallet3->store("test-wallet3");
}

void exchangeOutputs(Context& context) {
    std::string outputs, outputs2, outputs3;

    std::cout << "exporting wallet 1 outputs" << std::endl;
    if (!context.wallet->exportMultisigImages(outputs)) {
        throw std::runtime_error("couldn't export multisig images");
    }

    std::cout << "exporting wallet 2 outputs" << std::endl;
    if (!context.wallet2->exportMultisigImages(outputs2)) {
        throw std::runtime_error("couldn't export multisig images");
    }

    std::cout << "exporting wallet 3 outputs" << std::endl;
    if (!context.wallet3->exportMultisigImages(outputs3)) {
        throw std::runtime_error("couldn't export multisig images");
    }

    std::cout << "importing outputs by wallet 1" << std::endl;
    context.wallet->importMultisigImages({outputs2, outputs3});
    if (context.wallet->status() != 0) {
        throw std::runtime_error("couldn't import multisig images");
    }

    std::cout << "importing outputs by wallet 2" << std::endl;
    context.wallet2->importMultisigImages({outputs, outputs3});
    if (context.wallet2->status() != 0) {
        throw std::runtime_error("couldn't import multisig images");
    }

    std::cout << "importing outputs by wallet 3" << std::endl;
    context.wallet3->importMultisigImages({outputs, outputs2});
    if (context.wallet3->status() != 0) {
        throw std::runtime_error("couldn't import multisig images");
    }

    std::cout << "outputs exchanged successfully" << std::endl;
}

void send(Context& context, const std::string& dest, uint64_t amount) {
    context.wallet2->segregatePreForkOutputs(false);
    context.wallet2->keyReuseMitigation2(false);

    context.wallet->segregatePreForkOutputs(false);
    context.wallet->keyReuseMitigation2(false);

    auto transaction = context.wallet->createTransaction(dest, {}, amount, 7);
    if (context.wallet->status()) {
        throw std::runtime_error("couldn't create transaction: " + context.wallet->errorString());
    }

    std::cout << "signing transaction with wallet 1" << std::endl;
    auto signData = transaction->multisigSignData();
    if (transaction->status()) {
        throw std::runtime_error("couldn't get sign data: " + transaction->errorString());
    }

    std::cout << "sign data: " << signData << std::endl;

    auto signedTransaction = context.wallet2->restoreMultisigTransaction(signData);
    if (context.wallet2->status()) {
        throw std::runtime_error("couldn't restore transaction: " + context.wallet2->errorString());
    }

    std::cout << "signing transaction with wallet 2" << std::endl;
    signedTransaction->signMultisigTx();
    if (signedTransaction->status()) {
        throw std::runtime_error("couldn't sign transaction: " + signedTransaction->errorString());
    }
    std::cout << "transaction successfully signed";

    auto txs = signedTransaction->txid();

    std::cout << "sending transaction..." << std::endl;
    if (!signedTransaction->commit()) {
        throw std::runtime_error("couldn't commit transaction: " + signedTransaction->errorString());
    }

    std::cout << "Commit successfull. Transactions count: " << txs.size() << std::endl;
    for (const auto& id: txs) {
        std::cout << "transaction id: " << id << std::endl;
    }
}

void closeWallets(std::unique_ptr<Monero::WalletManager>& walletManager, Context& context, bool store) {
    std::cout << "closing wallet 1" << std::endl;
    if (!walletManager->closeWallet(context.wallet.get(), store)) {
        std::cout << "couldn't close wallet 1: " << walletManager->errorString() << std::endl;
    } else {
        std::cout << "wallet 1 closed" << std::endl;
        context.wallet.release();
    }

    std::cout << "closing wallet 2" << std::endl;
    if (!walletManager->closeWallet(context.wallet2.get(), store)) {
        std::cout << "couldn't close wallet 2: " << walletManager->errorString() << std::endl;
    } else {
        std::cout << "wallet 2 closed" << std::endl;
        context.wallet2.release();
    }

    std::cout << "closing wallet 3" << std::endl;
    if (!walletManager->closeWallet(context.wallet3.get(), store)) {
        std::cout << "couldn't close wallet 3: " << walletManager->errorString() << std::endl;
    } else {
        std::cout << "wallet 3 closed" << std::endl;
        context.wallet3.release();
    }
}

int main(int argc, char ** argv) {
    try {
        if (argc < 2) {
            std::cerr << "not enough arguments" << std::endl;
            return 1;
        }

        auto cmd = std::string(argv[1]);

        Monero::WalletManagerFactory::setLogLevel(Monero::WalletManagerFactory::LogLevel_Min);
        auto walletManager = std::unique_ptr<Monero::WalletManager>(Monero::WalletManagerFactory::getWalletManager());
        if (cmd == "generate") {
            genWallets(walletManager);
        } else if (cmd == "balance") {
            std::cout << "opening wallets" << std::endl;
            auto context = openWallets(walletManager);
            balance(context);
        } else if (cmd == "sync") {
            std::cout << "opening wallets" << std::endl;
            auto context = openWallets(walletManager);

            std::cout << "syncing wallets" << std::endl;
            sync(context);
            saveWallets(context);
        } else if (cmd == "outputs") {
            std::cout << "opening wallets" << std::endl;
            auto context = openWallets(walletManager);

            exchangeOutputs(context);
            sync(context);
            saveWallets(context);
        } else if (cmd == "send") {
            if (argc != 4) {
                std::cerr << "more arguments required" << std::endl;
                return 4;
            }

            std::cout << "opening wallets" << std::endl;
            auto context = openWallets(walletManager);
            exchangeOutputs(context);
            sync(context);

            send(context, argv[2], Monero::Wallet::amountFromString(argv[3]));
            saveWallets(context);
            closeWallets(walletManager, context, false);
        } else {
            std::cerr << "unknown command: " << cmd << std::endl;
            return 2;
        }
    } catch (const std::exception& e) {
        std::cerr << "error occured: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}
