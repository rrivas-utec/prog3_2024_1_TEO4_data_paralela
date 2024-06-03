#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <algorithm>
#include <numeric>
#include <ranges>

void summarize(std::vector<int>& source, int& subtotal, int start, int stop) {
    subtotal = std::accumulate(
        std::next(source.begin(), start),
        std::next(source.begin(), stop), 0);
}

template <typename Iterator, typename T = typename Iterator::value_type>
void summarize_2(Iterator initial, int& subtotal, int start, int stop) {
    subtotal = std::accumulate(
        std::next(initial, start),
        std::next(initial, stop), 0);
}

template <typename Iterator, typename T = typename Iterator::value_type>
int suma_paralela(Iterator start, Iterator stop, T initial_value) {

    // Calculando variables
    auto nt = std::thread::hardware_concurrency() - 1;
    auto n = std::distance(start, stop);
    auto range = std::distance(start, stop) / nt;
    auto residuo = n % nt;
    std::cout << "rango: " << range << " residuo: " << residuo << std::endl;

    // Crear un vector de subtotales
    std::vector<int> subtotales(nt);

    // Crear la lista de hilos
    std::vector<std::jthread> vt;
    vt.reserve(nt);

    // Asignar a los hilos su tarea
    for (int i = 0; i < nt; ++i) {
        vt.emplace_back(summarize_2<Iterator, T>, start, std::ref(subtotales[i]), i*range, (i+1)*range);
    }
    // juntar los hilos
    for (int i = 0; i < nt; ++i) {
        vt[i].join();
    }

    // Suma de subtotales
    int result = std::accumulate(subtotales.begin(), subtotales.end(), 0);
    result += std::accumulate(next(start, nt * range), stop, 0);
    return result;
}

void ejemplo_1() {
    int n = 10030;
    std::vector<int> v1(n);
    std::iota(v1.begin(), v1.end(), 1);

    // Calculo esperado
    auto total_esperado = std::accumulate(v1.begin(), v1.end(), 0);
    std::cout << "valor esperado: " << total_esperado << std::endl;

    // Calculando variables
    auto nt = std::thread::hardware_concurrency() - 1;
    auto range = n / nt;
    auto residuo = n % nt;
    std::cout << "rango: " << range << " residuo: " << residuo << std::endl;

    // Crear un vector de subtotales
    std::vector<int> subtotales(nt);

    // Crear la lista de hilos
    std::vector<std::jthread> vt;
    vt.reserve(nt);

    // Asignar a los hilos su tarea
    for (int i = 0; i < nt; ++i) {
        vt.emplace_back(summarize, std::ref(v1), std::ref(subtotales[i]), i*range, (i+1)*range);
    }
    // juntar los hilos
    for (int i = 0; i < nt; ++i) {
        vt[i].join();
    }

    // Suma de subtotales
    int total_obtenido = std::accumulate(subtotales.begin(), subtotales.end(), 0);
    // Suma de residuo
    total_obtenido += std::accumulate(next(v1.begin(), nt * range), v1.end(), 0);
    std::cout << "valor obtenido: " << total_obtenido << std::endl;
}

void ejemplo_2() {
    int n = 10030;
    std::vector<int> v1(n);
    std::iota(v1.begin(), v1.end(), 1);

    // Calculo esperado
    auto total_esperado = std::accumulate(v1.begin(), v1.end(), 0);
    std::cout << "valor esperado: " << total_esperado << std::endl;
    int total_obtenido = suma_paralela(v1.begin(), v1.end(), 0);
    std::cout << "valor obtenido: " << total_obtenido << std::endl;
}

int main() {
    ejemplo_2();
    return 0;
}
