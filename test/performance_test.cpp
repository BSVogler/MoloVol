#include <benchmark/benchmark.h>
#include "vector.h"
#include "atom.h"
#include "space.h"
#include "model.h"
#include "importmanager.h"
#include <vector>
#include <string>
#include <cmath>

// Performance test for Vector class operations
static void BM_VectorOperations(benchmark::State& state) {
  Vector vec1(1.2, 2.4, 3.6);
  Vector vec2(6.0, 4.8, 3.0);

  for (auto _ : state) {
    Vector sum = vec1 + vec2;
    Vector diff = vec1 - vec2;
    Vector scaled = vec1 * 2.0;
    double length = vec1.length();
    double dot_product = vec1 * vec2;
    double distance = Vector::distance(vec1, vec2);
  }
}
BENCHMARK(BM_VectorOperations);

// Performance test for Atom class operations
static void BM_AtomOperations(benchmark::State& state) {
  Atom atom1("C", {1.0, 2.0, 3.0}, 1.7, 6);
  Atom atom2("O", {4.0, 5.0, 6.0}, 1.52, 8);

  for (auto _ : state) {
    double distance = Vector::distance(atom1.getPosVec(), atom2.getPosVec());
    double radius_sum = atom1.getRad() + atom2.getRad();
    double atomic_number_sum = atom1.number + atom2.number;
  }
}
BENCHMARK(BM_AtomOperations);

// Performance test for Space class operations
static void BM_SpaceOperations(benchmark::State& state) {
  std::vector<Atom> atoms = {
    Atom("C", {1.0, 2.0, 3.0}, 1.7, 6),
    Atom("O", {4.0, 5.0, 6.0}, 1.52, 8),
    Atom("H", {7.0, 8.0, 9.0}, 1.2, 1)
  };

  double grid_size = 0.2;
  int depth = 4;
  double r_probe = 1.4;
  bool unit_cell_option = false;
  std::array<double, 3> unit_cell_axes = {0, 0, 0};

  Space space(atoms, grid_size, depth, r_probe, unit_cell_option, unit_cell_axes);

  for (auto _ : state) {
    space.assignTypeInGrid(atoms, {}, 1.4, 0, false, false);
  }
}
BENCHMARK(BM_SpaceOperations);

// Performance test for Model class operations
static void BM_ModelOperations(benchmark::State& state) {
  std::string elements_file_path = "inputfile/elements.txt";
  std::string structure_file_path = "inputfile/example_C60.xyz";

  Model model;
  model.importElemFile(elements_file_path);
  model.readAtomsFromFile(structure_file_path);

  for (auto _ : state) {
    model.generateData();
  }
}
BENCHMARK(BM_ModelOperations);

// Performance test for Model::processUnitCell
static void BM_ModelProcessUnitCell(benchmark::State& state) {
  std::string structure_file_path = "inputfile/example_C60.xyz";
  std::string elements_file_path = "inputfile/elements.txt";

  Model model;
  model.importElemFile(elements_file_path);
  model.readAtomsFromFile(structure_file_path);

  for (auto _ : state) {
    model.processUnitCell();
  }
}
BENCHMARK(BM_ModelProcessUnitCell);

// Main function to run the benchmarks
int main(int argc, char** argv) {
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}
