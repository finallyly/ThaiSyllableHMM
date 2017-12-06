#########################################################################
#   Copyright (C) 2017 All rights reserved.
#   
#   FileName:build.sh
#   Creator:yuliu1@microsoft.com
#   Time:11/2017/05
#   Description:
#
#   Update:
#
#########################################################################
#!/bin/bash

# please add your code here!
cmake -Dbuild_OffLineUseHMM=TRUE -DCMAKE_BUILD_TYPE=DEBUG .
