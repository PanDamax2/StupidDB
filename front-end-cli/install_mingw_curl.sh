mkdir -p mingw_curl_temp
cd mingw_curl_temp
wget https://curl.se/windows/dl-8.17.0_2/curl-8.17.0_2-win64-mingw.zip
unzip curl-8.17.0_2-win64-mingw.zip
cd curl-8.17.0_2-win64-mingw
sudo cp -r include/curl /usr/x86_64-w64-mingw32/include/
sudo cp lib/libcurl.dll.a /usr/x86_64-w64-mingw32/lib/libcurl.a
sudo cp bin/libcurl-x64.dll /usr/x86_64-w64-mingw32/bin/
cd ..
rm -rf mingw_curl_temp
