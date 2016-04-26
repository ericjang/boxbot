#!/bin/bash

rm -rf /ltmp/e2c
scp -r brahe:/ltmp/e2c /ltmp/.
tensorboard --logdir=/ltmp/e2c
