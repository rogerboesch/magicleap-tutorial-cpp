mabu ctest.package -t device -s $MLCERT
mldb uninstall com.rogerboesch.ctest
mldb install .out/ctest/ctest.mpk
mldb launch com.rogerboesch.ctest