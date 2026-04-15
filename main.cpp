#include <iostream>
#include <cmath>
#include <numeric>
#include <cstdint>
#include <string>
#include <algorithm>
#include <limits>
#include <iomanip> 

enum class MenuOption {
    PowerMod = 1,
    EuclidCheck = 2,
    ModInverse = 3,
    Shamir = 4,
    Diophantine = 5,
    MitmAttack = 6,
    Exit = 7,
    Invalid = 0
};

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool isPrime(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

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

uint64_t extended_gcd_with_table(uint64_t a, uint64_t b, int64_t &u, int64_t &v) {
    int64_t x0 = 1, x1 = 0; 
    int64_t y0 = 0, y1 = 1;
    uint64_t r0 = a, r1 = b;

    std::cout << "\n" << std::setw(45) << std::setfill('-') << "" << std::setfill(' ') << "\n";
    std::cout << std::setw(10) << "Remainder" << std::setw(10) << "U" << std::setw(10) << "V" << std::setw(10) << "Quotient" << "\n";
    std::cout << std::setw(45) << std::setfill('-') << "" << std::setfill(' ') << "\n";
    std::cout << std::setw(10) << r0 << std::setw(10) << x0 << std::setw(10) << y0 << std::setw(10) << "-" << "\n";

    while (r1 != 0) {
        uint64_t q = r0 / r1; 
        
        std::cout << std::setw(10) << r1 << std::setw(10) << x1 << std::setw(10) << y1 << std::setw(10) << q << "\n";

        uint64_t next_r = r0 % r1;
        r0 = r1;
        r1 = next_r;

        // Вычисление новых коэффициентов: x_j = x_{j-2} - q*x_{j-1} 
        int64_t next_x = x0 - static_cast<int64_t>(q) * x1;
        int64_t next_y = y0 - static_cast<int64_t>(q) * y1;

        x0 = x1; x1 = next_x;
        y0 = y1; y1 = next_y;
    }

    std::cout << std::setw(10) << 0 << std::setw(10) << x1 << std::setw(10) << y1 << std::setw(10) << "-" << "\n";
    std::cout << std::setw(45) << std::setfill('-') << "" << std::setfill(' ') << "\n";

    u = x0; 
    v = y0; 
    return r0; 
}

// Поиск обратного элемента (степень -1)
uint64_t modInverse_with_table(uint64_t c, uint64_t m) {
    int64_t u, v;
    uint64_t g = extended_gcd_with_table(c, m, u, v);
    
    if (g != 1) return 0;

    // Если число u отрицательное, прибавляем модуль
    int64_t res = u % static_cast<int64_t>(m);
    if (res < 0) res += m;
    
    return static_cast<uint64_t>(res);
}

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
    bool running = true;
    while (running) {
        std::cout << "\n========== CRYPTO MENU ==========" << std::endl;
        std::cout << "1. a^x mod p (Fermat/Log)" << std::endl;
        std::cout << "2. Euclid Table: a*u + b*v = GCD" << std::endl;
        std::cout << "3. Inverse Table: c^(-1) mod m" << std::endl;
        std::cout << "4. Shamir Three-Pass Protocol" << std::endl;
        std::cout << "5. Diophantine (275a + 145b = 10)" << std::endl;
        std::cout << "6. MITM Attack Emulation" << std::endl;
        std::cout << "7. Exit" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << "Enter choice (1-7): ";

        int rawChoice;
        if (!(std::cin >> rawChoice)) {
            std::cout << "Error: Numbers only." << std::endl;
            clearInput();
            continue;
        }

        MenuOption choice = (rawChoice >= 1 && rawChoice <= 7) ? static_cast<MenuOption>(rawChoice) : MenuOption::Invalid;

        switch (choice) {
            case MenuOption::PowerMod: {
                uint64_t a, x, p;
                std::cout << "Enter a, x, p: ";
                if (!(std::cin >> a >> x >> p)) { clearInput(); break; }
                if (!isPrime(p)) std::cout << "p is not prime!\n";
                else std::cout << "Result: " << power(a, x, p) << "\n";
                break;
            }

            case MenuOption::EuclidCheck: {
                uint64_t a, b;
                int64_t u, v;
                std::cout << "Enter a and b: ";
                if (!(std::cin >> a >> b)) { clearInput(); break; }
                uint64_t g = extended_gcd_with_table(a, b, u, v);
                std::cout << "GCD: " << g << ", u: " << u << ", v: " << v << "\n";
                break;
            }

            case MenuOption::ModInverse: {
                uint64_t c, m;
                std::cout << "Enter c and m (to find c^-1 mod m): ";
                if (!(std::cin >> c >> m)) { clearInput(); break; }
                uint64_t inv = modInverse_with_table(c, m);
                if (inv == 0) std::cout << "Inverse doesn't exist.\n";
                else std::cout << "Inverse element: " << inv << "\n";
                break;
            }

            case MenuOption::Shamir: {
                uint64_t p, msg, ca, cb;
                std::cout << "Enter P, Msg, Ca, Cb: ";
                if (!(std::cin >> p >> msg >> ca >> cb)) { clearInput(); break; }
                uint64_t da = modInverse_with_table(ca, p - 1);
                uint64_t db = modInverse_with_table(cb, p - 1);
                if (da == 0 || db == 0) std::cout << "Check failed (GCD != 1)\n";
                else {
                    uint64_t res = power(power(power(power(msg, ca, p), cb, p), da, p), db, p);
                    std::cout << "Final Decrypted Message: " << res << "\n";
                }
                break;
            }

            case MenuOption::Diophantine: {
                uint64_t A = 275, B = 145, C = 10;
                int64_t u, v;
                uint64_t g = extended_gcd_with_table(A, B, u, v);
                if (C % g != 0) std::cout << "No solutions for 275a + 145b = 10\n";
                else {
                    int64_t factor = static_cast<int64_t>(C / g);
                    std::cout << "Solutions: a = " << u * factor << ", b = " << v * factor << "\n";
                }
                break;
            }

            case MenuOption::MitmAttack: {
                uint64_t n, g, x, y, e1, e2;
                std::cout << "Enter n, g, Alice_x, Bob_y, Eva_e1, Eva_e2: ";
                if (!(std::cin >> n >> g >> x >> y >> e1 >> e2)) { clearInput(); break; }
                
                uint64_t kAE = power(power(g, e1, n), x, n);
                uint64_t kEA = power(power(g, x, n), e1, n);
                
                char sender;
                while (true) {
                    std::cout << "\nWho sends SMS? (A/B/q): ";
                    std::cin >> sender;
                    if (sender == 'q') break;
                    clearInput();
                    std::string msg;
                    std::cout << "Enter message: ";
                    std::getline(std::cin, msg);
                    if (sender == 'A') {
                        std::string intercepted = transform(transform(msg, kAE), 26 - (kEA % 26));
                        std::cout << "[Eva Intercepted]: " << intercepted << "\n";
                    }
                }
                break;
            }

            case MenuOption::Exit:
                running = false;
                break;

            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }
    return 0;
}
