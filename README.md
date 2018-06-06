# monero-multisig-api-example
Show case of using monero multisig wallet api

# How To Build
* build monero dependencies
* cmake . 
* make

# In case of IOS
- first, build dependencies and place into ../monero-libs/ios-arm64. This should contain something like
```
ls ../monero-libs/ios-arm64/
app-monero			libboost_locale.a		libboost_unit_test_framework.a	libeasylogging.a		libssl.a
libblocks.a			libboost_program_options.a	libcheckpoints.a		libepee.a			libunbound.a
libboost.a			libboost_random.a		libcncrypto.a			liblmdb.a			libversion.a
libboost_atomic.a		libboost_regex.a		libcommon.a			libminiupnpc.a			libwallet.a
libboost_chrono.a		libboost_serialization.a	libcrypto.a			libmnemonics.a			libwallet_merged.a
libboost_date_time.a		libboost_signals.a		libcryptonote_basic.a		libmultisig.a
libboost_exception.a		libboost_system.a		libcryptonote_core.a		libringct.a
libboost_filesystem.a		libboost_thread.a		libdevice.a			libringct_basic.a
```
- second, build example
```
mkdir -p build/ios
cd build/ios
../../build-monero-ios.sh
```

# How To Make Sure It Works
* ./app-monero generate
* ./app-monero sync
* ./app-monero send <address> <amount>
