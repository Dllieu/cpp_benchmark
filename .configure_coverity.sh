ls -l
pwd
cov-configure --comptype gcc --compiler /usr/bin/g++-7 --template
cmake -DCMAKE_CXX_COMPILER=g++-7 -DCMAKE_BUILD_TYPE=Release ..
