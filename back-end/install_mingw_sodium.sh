mkdir -p mingw_sodium_temp
cd mingw_sodium_temp
wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.18-mingw.tar.gz
tar -xf libsodium-1.0.18-mingw.tar.gz
sudo cp -r libsodium-win64/include/sodium /usr/x86_64-w64-mingw32/include/
sudo cp -r libsodium-win64/include/sodium.h /usr/x86_64-w64-mingw32/include/
sudo cp libsodium-win64/lib/libsodium.a /usr/x86_64-w64-mingw32/lib/
cd ..
rm -rf mingw_sodium_temp
