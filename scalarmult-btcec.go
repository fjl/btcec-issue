package main

import (
	"encoding/hex"
	"fmt"
	"math/big"
	"os"

	"github.com/btcsuite/btcd/btcec"
)

func fatal(msg string) {
	fmt.Fprintln(os.Stderr, msg)
	os.Exit(1)
}

func main() {
	if len(os.Args) != 3 {
		fatal("need two arguments, message hash and signature")
	}
	pointbytes, err := hex.DecodeString(os.Args[1])
	if err != nil || len(pointbytes) != 65 {
		fatal("invalid ec point, need 130 hex characters")
	}
	scalar, err := hex.DecodeString(os.Args[2])
	if err != nil {
		fatal("second argument is invalid hex")
	}

	x := new(big.Int).SetBytes(pointbytes[1:33])
	y := new(big.Int).SetBytes(pointbytes[33:])
	result := &btcec.PublicKey{Curve: btcec.S256()}
	result.X, result.Y = btcec.S256().ScalarMult(x, y, scalar)
	fmt.Printf("result: %X\n", result.SerializeUncompressed())
}
