ls -l
pwd
cov-configure --comptype gcc --compiler /usr/bin/g++-5 --template
cmake -DCMAKE_CXX_COMPILER=g++-5 -DCMAKE_BUILD_TYPE=Release ..
