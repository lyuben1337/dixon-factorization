#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

// Функція для перевірки, чи є число простим
bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;

    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i <= sqrt(n); i += 6)
        if (n % i == 0 || n % (i+2) == 0)
            return false;

    return true;
}

// Функція для отримання бази факторів (простих чисел менших або рівних межі)
std::vector<int> getFactorBase(int limit) {
    std::vector<int> base;
    for (int i = 2; i <= limit; ++i) {
        if (isPrime(i))
            base.push_back(i);
    }
    return base;
}

// Функція для факторизації числа за базою факторів
bool factorize(int n, const std::vector<int>& base, std::map<int, int>& factors) {
    factors.clear();
    int temp = n;
    for (int p : base) {
        int count = 0;
        while (temp % p == 0) {
            temp /= p;
            ++count;
        }
        if (count > 0)
            factors[p] = count;
    }
    return temp == 1;
}

int main() {
    int N;
    std::cout << "Введіть число для факторизації методом Діксона: ";
    std::cin >> N;

    std::cout << "\nМетод Діксона для факторизації числа " << N << ":\n";

    // Крок 1: Вибір бази факторів
    int factorBaseLimit = 7; // Можна змінити для більш складних чисел
    std::vector<int> factorBase = getFactorBase(factorBaseLimit);

    std::cout << "Крок 1: Вибираємо базу факторів: { ";
    for (int p : factorBase)
        std::cout << p << " ";
    std::cout << "}\n";

    // Крок 2: Знаходимо значення x і відповідні x^2 mod N, що факторизуються за базою факторів
    std::vector<int> xValues;
    std::vector<std::map<int, int>> factorExponents;

    int x = static_cast<int>(std::ceil(std::sqrt(N)));
    int maxIterations = N; // Ліміт ітерацій, щоб уникнути нескінченного циклу

    std::cout << "\nКрок 2: Пошук значень x, для яких x^2 mod N факторизується за базою факторів.\n";

    while (xValues.size() < factorBase.size() + 1 && maxIterations--) {
        int xSquaredModN = (x * x) % N;

        std::map<int, int> factors;
        if (factorize(xSquaredModN, factorBase, factors)) {
            xValues.push_back(x);
            factorExponents.push_back(factors);

            std::cout << "\nЗнайдено x = " << x << ":\n";
            std::cout << x << "^2 mod " << N << " = " << xSquaredModN << " факторизується як: ";

            for (auto it = factors.begin(); it != factors.end(); ++it) {
                if (it != factors.begin())
                    std::cout << " * ";
                std::cout << it->first << "^" << it->second;
            }
            std::cout << "\n";
        }
        x++;
    }

    // Крок 3: Пошук комбінації з парними показниками
    std::cout << "\nКрок 3: Пошук комбінації з парними показниками степенів.\n";

    int size = xValues.size();
    bool found = false;
    std::vector<int> exponents(size);

    // Перебір всіх можливих комбінацій
    for (int i = 1; i < (1 << size); ++i) {
        std::map<int, int> combinedFactors;
        std::vector<int> indices;

        for (int j = 0; j < size; ++j) {
            if (i & (1 << j)) {
                indices.push_back(j);
                for (auto& f : factorExponents[j])
                    combinedFactors[f.first] += f.second;
            }
        }

        // Перевірка на парність показників
        bool allEven = true;
        for (auto& f : combinedFactors) {
            if (f.second % 2 != 0) {
                allEven = false;
                break;
            }
        }

        if (allEven) {
            found = true;
            std::cout << "\nЗнайдена комбінація з парними показниками для індексів: ";
            for (int idx : indices)
                std::cout << idx << " ";
            std::cout << "\n";

            // Крок 4: Обчислення значень x та y
            long long xProd = 1;
            long long yProd = 1;

            for (int idx : indices)
                xProd *= xValues[idx];

            xProd %= N;

            for (auto& f : combinedFactors) {
                int exp = f.second / 2;
                yProd *= static_cast<long long>(std::pow(f.first, exp));
            }
            yProd %= N;

            std::cout << "Обчислюємо x = ";
            for (int idx : indices) {
                if (idx != indices[0])
                    std::cout << " * ";
                std::cout << xValues[idx];
            }
            std::cout << " mod " << N << " = " << xProd << "\n";

            std::cout << "Обчислюємо y = ";
            for (auto& f : combinedFactors) {
                std::cout << f.first << "^(" << f.second << "/2) ";
            }
            std::cout << "mod " << N << " = " << yProd << "\n";

            // Крок 5: Обчислення НСД
            long long gcd = std::gcd(static_cast<long long>(std::abs(xProd - yProd)), static_cast<long long>(N));

            std::cout << "\nКрок 4: Обчислюємо НСД(|x - y|, N) = НСД(" << std::abs(xProd - yProd) << ", " << N << ") = " << gcd << "\n";

            if (gcd != 1 && gcd != N) {
                std::cout << "\nЗнайдений нетривіальний дільник числа " << N << ": " << gcd << "\n";
                std::cout << "Інший дільник: " << N / gcd << "\n";
            } else {
                std::cout << "\nНе вдалося знайти нетривіальний дільник. Спробуйте з більшою базою факторів або іншими параметрами.\n";
            }

            break;
        }
    }

    if (!found) {
        std::cout << "\nНе вдалося знайти комбінацію з парними показниками. Спробуйте збільшити базу факторів або кількість ітерацій.\n";
    }

    return 0;
}
