package main

import (
	"encoding/hex"
	"fmt"
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
	msg, err := hex.DecodeString(os.Args[1])
	if err != nil || len(msg) != 32 {
		fatal("invalid message, need 64 hex characters")
	}
	sig, err := hex.DecodeString(os.Args[2])
	if err != nil || len(sig) != 65 {
		fatal("invalid signature, need 130 hex characters")
	}

	sig[0] += 27
	pub, _, err := btcec.RecoverCompact(btcec.S256(), sig, msg)
	if err != nil {
		fatal("recover failed: " + err.Error())
	}
	fmt.Printf("pubkey: %X\n", pub.SerializeUncompressed())
}
