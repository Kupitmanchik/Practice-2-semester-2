package main

import (
	"bufio"
	"fmt"
	"math/big"
	"os"
	"strings"
)

// --- Вспомогательные функции ---

// Проверка на простое число
func isPrime(n uint64) bool {
	if n <= 1 {
		return false
	}
	if n <= 3 {
		return true
	}
	if n%2 == 0 || n%3 == 0 {
		return false
	}
	for i := uint64(5); i*i <= n; i += 6 {
		if n%i == 0 || n%(i+2) == 0 {
			return false
		}
	}
	return true
}

// Модульное возведение в степень (Логарифмический алгоритм)
func power(base, exp, mod uint64) uint64 {
	b, e, m := new(big.Int).SetUint64(base), new(big.Int).SetUint64(exp), new(big.Int).SetUint64(mod)
	res := new(big.Int).Exp(b, e, m)
	return res.Uint64()
}

// Нахождение обратного элемента через расширенный алгоритм Евклида
func modInverse(e, phi uint64) uint64 {
	bigE, bigPhi := new(big.Int).SetUint64(e), new(big.Int).SetUint64(phi)
	res := new(big.Int).ModInverse(bigE, bigPhi)
	if res == nil {
		return 0
	}
	return res.Uint64()
}

// Расширенный алгоритм Евклида для коэффициентов a и b
func extendedGCD(a, b int64) (int64, int64, int64) {
	bigA, bigB := big.NewInt(a), big.NewInt(b)
	x, y, gcd := new(big.Int), new(big.Int), new(big.Int)
	gcd.GCD(x, y, bigA, bigB)
	return x.Int64(), y.Int64(), gcd.Int64()
}

// Шифрование текста (Цезарь для демонстрации MITM)
func transform(text string, key uint64) string {
	shift := int(key % 26)
	var result strings.Builder
	for _, c := range text {
		if (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') {
			base := 'a'
			if c >= 'A' && c <= 'Z' {
				base = 'A'
			}
			res := (int(c-base) + shift) % 26
			if res < 0 {
				res += 26
			}
			result.WriteByte(byte(int(base) + res))
		} else {
			result.WriteRune(c)
		}
	}
	return result.String()
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		fmt.Println("\n--- Menu ---")
		fmt.Println("1. a^x mod p = ? (Fermat & Logarithm)")
		fmt.Println("2. Euclid's algorithm. c*d mod m = 1")
		fmt.Println("3. Euclid's algorithm. c^(-1) mod m = d")
		fmt.Println("4. Shamir's Three-Pass Protocol")
		fmt.Println("5. Linear Diophantine Equation (439a + 118b = 3)")
		fmt.Println("6. Attack emulation (MITM)")
		fmt.Println("7. Exit")
		fmt.Print("Enter number 1-7: ")

		var choice int
		fmt.Scan(&choice)
		if choice == 7 {
			break
		}

		switch choice {
		case 1: //
			var a, x, p uint64
			fmt.Print("Enter a, x, p: ")
			fmt.Scan(&a, &x, &p)
			if !isPrime(p) {
				fmt.Println("Error: p must be prime.")
			} else {
				fmt.Printf("Ferma: a^x mod p = %d\n", power(a, x%(p-1), p))
				fmt.Printf("Logarithm: a^x mod p = %d\n", power(a, x, p))
			}

		case 2, 3: //
			var c, m uint64
			fmt.Print("Enter c, m: ")
			fmt.Scan(&c, &m)
			d := modInverse(c, m)
			if d == 0 {
				fmt.Println("Error: GCD != 1")
			} else {
				fmt.Printf("Result d = %d\n", d)
			}

		case 4: //
			var p, msg, ca, cb uint64
			fmt.Print("Enter prime P, message M, Ca, Cb: ")
			fmt.Scan(&p, &msg, &ca, &cb)
			da, db := modInverse(ca, p-1), modInverse(cb, p-1)
			if da == 0 || db == 0 {
				fmt.Println("Error: GCD(C, P-1) must be 1")
			} else {
				x1 := power(msg, ca, p)
				x2 := power(x1, cb, p)
				x3 := power(x2, da, p)
				x4 := power(x3, db, p)
				fmt.Printf("Decrypted message M = %d\n", x4)
			}

		case 5: //
			var A, B, C int64 = 439, 118, 3
			x, y, g := extendedGCD(A, B)
			if C%g != 0 {
				fmt.Println("No solutions")
			} else {
				factor := C / g
				fmt.Printf("Found solution: a = %d, b = %d\n", x*factor, y*factor)
			}

		case 6: //
			var n, g, x, y, e, eDiff uint64
			fmt.Print("Enter n, g: ")
			fmt.Scan(&n, &g)
			fmt.Print("Alice secret x, Bob secret y: ")
			fmt.Scan(&x, &y)
			fmt.Print("Eva secrets e1, e2: ")
			fmt.Scan(&e, &eDiff)

			A, B, E, EDiff := power(g, x, n), power(g, y, n), power(g, e, n), power(g, eDiff, n)
			kAE, kEA := power(E, x, n), power(A, e, n)
			kBE, kEB := power(EDiff, y, n), power(B, eDiff, n)

			for {
				fmt.Print("\nSender (A/B/q): ")
				var s string
				fmt.Scan(&s)
				if s == "q" {
					break
				}
				scanner.Scan() // Clear buffer

				if s == "A" {
					fmt.Print("Alice text: ")
					scanner.Scan()
					txt := scanner.Text()
					enc := transform(txt, kAE)
					dec := transform(enc, 26-(kEA%26))
					fmt.Printf("Eva intercepted: %s\nE text to Bob: ", dec)
					scanner.Scan()
					fake := scanner.Text()
					fmt.Printf("Bob received: %s\n", fake)
					_ = transform(fake, kBE) // Симуляция отправки
				} else if s == "B" {
					fmt.Print("Bob text: ")
					scanner.Scan()
					txt := scanner.Text()
					enc := transform(txt, kBE)
					dec := transform(enc, 26-(kEB%26))
					fmt.Printf("Eva intercepted: %s\nE text to Alice: ", dec)
					scanner.Scan()
					fake := scanner.Text()
					fmt.Printf("Alice received: %s\n", fake)
					_ = transform(fake, kAE) // Симуляция отправки
				}
			}
		}
	}
}
