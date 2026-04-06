#include <iostream>
#include <cmath>
#include <numeric>
#include <cstdint>
#include <string>
#include <algorithm>
#include <limits>

// Проверка на простое число
bool isPrime(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Быстрое возведение в степень
uint64_t power(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (__uint128_t(res) * base) % mod;
        base = (__uint128_t(base) * base) % mod;
        exp /= 2;
    }
    return res;
}

// Расширенный алгоритм Евклида
uint64_t extended_gcd(uint64_t a, uint64_t b, int64_t &x, int64_t &y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int64_t x1, y1;
    uint64_t gcd = extended_gcd(b % a, a, x1, y1);
    x = y1 - (static_cast<int64_t>(b / a)) * x1;
    y = x1;
    return gcd;
}

// Нахождение обратного элемента
uint64_t modInverse(uint64_t e, uint64_t phi) {
    int64_t x, y;
    uint64_t g = extended_gcd(e, phi, x, y);
    if (g != 1) return 0;
    return (x % static_cast<int64_t>(phi) + static_cast<int64_t>(phi)) % static_cast<int64_t>(phi);
}

// Функция шифрования текста (Цезарь для наглядности MITM)
std::string transform(std::string text, uint64_t key) {
    int64_t shift = static_cast<int64_t>(key % 26);
    for (char &c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            int64_t res = (static_cast<int64_t>(c - base) + shift) % 26;
            if (res < 0) res += 26;
            c = static_cast<char>(base + res);
        }
    }
    return text;
}

int main() {
    int choice;
    do {
        std::cout << "\n1. a^x mod p = ?." << std::endl;
        std::cout << "2. Euclid's algorithm. c*d mod m = 1." << std::endl;
        std::cout << "3. Euclid's algorithm. c^(-1) mod m = d." << std::endl;
        std::cout << "4. Shamir's Three-Pass Protocol." << std::endl;
        std::cout << "5. Linear Diophantine Equation (275a + 145b = 10)." << std::endl;
        std::cout << "6. Attack emulation (MITM)." << std::endl;
        std::cout << "7. Exit" << std::endl;
        std::cout << "Enter number 1-7: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                uint64_t a, x, p;
                std::cout << "Enter a, x, p: ";
                std::cin >> a >> x >> p;
                if (!isPrime(p) || std::gcd(a, p) != 1) {
                    std::cout << "Error: p must be prime and GCD(a, p) must be 1." << std::endl;
                } else {
                    std::cout << "Ferma: a^x mod p = " << power(a, x % (p - 1), p) << std::endl;
                    std::cout << "Logarithm: a^x mod p = " << power(a, x, p) << std::endl;
                }
                break;
            }
            case 2: {
                uint64_t c, m;
                int64_t x, y;
                std::cout << "Enter c, m: ";
                std::cin >> c >> m;
                uint64_t g = extended_gcd(c, m, x, y);
                if (g != 1) std::cout << "Error: GCD != 1" << std::endl;
                else {
                    uint64_t d = (x % static_cast<int64_t>(m) + static_cast<int64_t>(m)) % static_cast<int64_t>(m);
                    std::cout << "d = " << d << std::endl;
                }
                break;
            }
            case 3: {
                uint64_t c, m;
                std::cout << "Enter c, m: ";
                std::cin >> c >> m;
                uint64_t d = modInverse(c, m);
                if (d == 0) std::cout << "No inverse" << std::endl;
                else std::cout << "c^(-1) mod m = " << d << std::endl;
                break;
            }
            case 4: {
                uint64_t p, msg, ca, cb;
                std::cout << "Enter prime P, message M, Ca, Cb: ";
                std::cin >> p >> msg >> ca >> cb;
                uint64_t da = modInverse(ca, p - 1);
                uint64_t db = modInverse(cb, p - 1);
                if (da == 0 || db == 0) {
                    std::cout << "Error: GCD(C, P-1) must be 1" << std::endl;
                } else {
                    uint64_t x1 = power(msg, ca, p);
                    uint64_t x2 = power(x1, cb, p);
                    uint64_t x3 = power(x2, da, p);
                    uint64_t x4 = power(x3, db, p);
                    std::cout << "Decrypted message M = " << x4 << std::endl;
                }
                break;
            }
            case 5: {
                uint64_t A = 275, B = 145, C = 10;
                int64_t x, y;
                uint64_t g = extended_gcd(A, B, x, y);
                if (C % g != 0) std::cout << "No solutions" << std::endl;
                else {
                    int64_t factor = static_cast<int64_t>(C / g);
                    std::cout << "Found solution: a = " << x * factor << ", b = " << y * factor << std::endl;
                }
                break;
            }
            case 6: {
                uint64_t n, g, x, y, e, e_diff;
                std::cout << "\n--- MITM Attack Emulation ---" << std::endl;
                std::cout << "Enter n: "; std::cin >> n;
                std::cout << "Enter g, 1 < g < n: "; std::cin >> g;
                
                std::cout << "A. Enter x: "; std::cin >> x;
                uint64_t A = power(g, x, n);
                
                std::cout << "B. Enter y: "; std::cin >> y;
                uint64_t B = power(g, y, n);
                
                std::cout << "E. Enter e: "; std::cin >> e;
                uint64_t E = power(g, e, n);

                std::cout << "Enter a different 'e' value: "; std::cin >> e_diff;
                uint64_t E_diff = power(g, e_diff, n);

                // Ключи для перехвата сообщений от Алисы
                uint64_t key_alice_eva = power(E, x, n); 
                uint64_t key_eva_alice = power(A, e, n); 

                // Ключи для перехвата сообщений от Боба
                uint64_t key_bob_eva = power(E_diff, y, n);
                uint64_t key_eva_bob = power(B, e_diff, n);

                char sender;
                while (true) {
                    std::cout << "\nEnter who is sending the SMS or exit. A if Alice, B if Bob, q if exit: ";
                    std::cin >> sender;
                    if (sender == 'q') break;

                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string text, fake_text;

                    if (sender == 'A') {
                        std::cout << "A: Enter the text: ";
                        std::getline(std::cin, text);

                        // Шифрование Алисой (думает, что шлет Бобу)
                        std::string encrypted = transform(text, key_alice_eva);
                        std::cout << "Encrypted text: " << encrypted << std::endl;

                        // Расшифровка Евой
                        std::string intercepted = transform(encrypted, 26 - (key_eva_alice % 26));
                        std::cout << "Eva deciphered the text: " << intercepted << std::endl;

                        // ПОДМЕНА СООБЩЕНИЯ
                        std::cout << "E: Enter the text: ";
                        std::getline(std::cin, fake_text);

                        // Ева шифрует поддельное сообщение для Боба
                        std::string for_bob = transform(fake_text, key_bob_eva);
                        std::cout << "Encrypted text: " << for_bob << std::endl;
                        std::cout << "Bob deciphered the text: " << fake_text << std::endl;
                    } 
                    else if (sender == 'B') {
                        std::cout << "B: Enter the text: ";
                        std::getline(std::cin, text);

                        std::string encrypted = transform(text, key_bob_eva);
                        std::cout << "Encrypted text: " << encrypted << std::endl;

                        std::string intercepted = transform(encrypted, 26 - (key_eva_bob % 26));
                        std::cout << "Eva deciphered the text: " << intercepted << std::endl;

                        std::cout << "E: Enter the text: ";
                        std::getline(std::cin, fake_text);

                        std::string for_alice = transform(fake_text, key_alice_eva);
                        std::cout << "Encrypted text: " << for_alice << std::endl;
                        std::cout << "Alice deciphered the text: " << fake_text << std::endl;
                    }
                }
                break;
            }
            case 7:
                std::cout << "Exiting..." << std::endl;
                break;
        }
    } while (choice != 7);

    return 0;
}
