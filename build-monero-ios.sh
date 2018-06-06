# cd build/ios && ../../build-monero-ios.sh
BOOST="$PWD/../../../Apple-Boost-BuildScript/build/boost/1.66.0/ios/prefix/include"
BOOST_LIB="$PWD/../../../Apple-Boost-BuildScript/build/boost/1.66.0/ios/build/arm64"
OPENSSL="$PWD/../../../OpenSSL-for-iPhone"
#sudo ln -s /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libkern/OSTypes.h /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS11.4.sdk/usr/include/libkern/OSTypes.h
#sudo ln -s /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/IOKit.framework/Versions/A/Headers/ /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/IOKit.framework/

cmake DBOOST_IGNORE_SYSTEM_PATHS=TRUE -DEMBEDDED_WALLET=TRUE -DUSE_DEVICE_LEDGER=0 -D BUILD_TESTS=OFF -D ARCH="arm64" -D STATIC=ON -D BUILD_64=ON -D CMAKE_BUILD_TYPE=release -D IOS=true -D BUILD_TAG="ios" -D BOOST_ROOT=$BOOST  -D BOOST_LIBRARYDIR=$BOOST_LIB -D OPENSSL_ROOT_DIR=$OPENSSL -D OPENSSL_CRYPTO_LIBRARY=$OPENSSL/bin/iPhoneOS11.4-arm64.sdk/lib/libcrypto.a -D OPENSSL_SSL_LIBRARY=$OPENSSL/bin/iPhoneOS11.4-arm64.sdk -D CMAKE_POSITION_INDEPENDENT_CODE:BOOL=true -D EMBEDDED_WALLET=ON ../..
