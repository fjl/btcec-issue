#!/bin/sh

echo "RECOVER"

checkrecover () {
    echo ================================
    echo msg = $msg
    echo sig = $sig
    echo --- libsecp256k1
    ./recover-libsecp256k1 $msg $sig
    echo --- openssl
    ./recover-openssl $msg $sig
    echo --- btcec
    go run recover-btcec.go $msg $sig
}

# (1) sanity check
msg=ce0677bb30baa8cf067c88db9811f4333d131bf8bcf12fe7065d211dce971008
sig=0190f27b8b488db00b00606796d2987f6a5f59ae62ea05effe84fef5b8b0e549984a691139ad57a3f0b906637673aa2f63d1f55cb1a69199d4009eea23ceaddc93
checkrecover

# (2) recovering sig produces an invalid curve point
msg=00c547e4f7b0f325ad1e56f57e26c745b09a3e503d86e00e5255ff7f715d3d1c
sig=0100b1693892219d736caba55bdb67216e485557ea6b6af75f37096c9aa6a5a75f00b940b1d03b21e36b0e47e79769f095fe2ab855bd91e3a38756b7d75a9c4549
checkrecover

# (3) very low R, S values
msg=ba09edc1275a285fb27bfe82c4eea240a907a0dbaf9e55764b8f318c37d5974f
sig=00000000000000000000000000000000000000000000000000000000000000002c0000000000000000000000000000000000000000000000000000000000000004
checkrecover


echo "\n\nSCALAR MULT"

checkscalarmult () {
    echo ================================
    echo point = $point
    echo scalar = $scalar
    echo --- openssl
    ./scalarmult-openssl $point $scalar
    echo --- btcec
    go run scalarmult-btcec.go $point $scalar
}

# intermediate result from (3) above.
# note point fails IsPointOnCurve
point=04000000000000000000000000000000000000000000000000000000000000002C420E7A99BBA18A9D3952597510FD2B6728CFEAFC21A4E73951091D4D8DDBE94E
scalar=a2e8ba2e8ba2e8ba2e8ba2e8ba2e8ba219b51835b55cc30ebfe2f6599bc56f58
checkscalarmult
