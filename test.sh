#!/bin/sh

msg1=ce0677bb30baa8cf067c88db9811f4333d131bf8bcf12fe7065d211dce971008
sig1=0190f27b8b488db00b00606796d2987f6a5f59ae62ea05effe84fef5b8b0e549984a691139ad57a3f0b906637673aa2f63d1f55cb1a69199d4009eea23ceaddc93

msg2=00c547e4f7b0f325ad1e56f57e26c745b09a3e503d86e00e5255ff7f715d3d1c
sig2=0100b1693892219d736caba55bdb67216e485557ea6b6af75f37096c9aa6a5a75f00b940b1d03b21e36b0e47e79769f095fe2ab855bd91e3a38756b7d75a9c4549


echo == sig 1 libsecp256k1:
./recover-libsecp256k1 $msg1 $sig1

echo == sig 2 libsecp256k1:
./recover-libsecp256k1 $msg2 $sig2


echo == sig 1 openssl:
./recover-openssl $msg1 $sig1

echo == sig 2 openssl:
./recover-openssl $msg2 $sig2


echo == sig 1 btcec:
go run recover-btcec.go $msg1 $sig1

echo == sig 2 btcec:
go run recover-btcec.go $msg2 $sig2

