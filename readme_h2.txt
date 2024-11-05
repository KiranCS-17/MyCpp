client compilation:
 g++ -std=c++20 -o client2  h2_client_2.cxx -lnghttp2_asio -lboost_system -lssl -lcrypto -lpthread

Server compilation:
 g++ -std=c++11 -o server h2_server.cpp -lnghttp2_asio -lboost_system -lssl -lcrypto -lpthread
 
 export LD_LIBRARY_PATH=/home/kics/examples/nghttp2-asio/lib/:$LD_LIBRARY_PATH
 
 
 install and make g++ latest default:
 -----------------------------------
 sudo apt install g++-10
 sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 90

https://github.com/nghttp2/nghttp2-asio/
https://github.com/nghttp2/

sudo apt install nlohmann-json3-dev
sudo apt install libnghttp2-dev
sudo yum install nghttp2-devel
