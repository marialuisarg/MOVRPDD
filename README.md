<a name="readme-top"></a>


<!-- PROJECT LOGO -->
<br />
<div align="center">
  <!-- <a href="https://github.com/github_username/repo_name">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

<h3 align="center">Multi-objective Optimization Model for the Vehicle Routing Problem with Drone Delivery and Dynamic Flight Endurance</h3>

  <p align="center">

    Maria Luísa Riolino Guimarães
     
    Stênio Sã Rosário Furtado Soares  
    Luciana Brugiolo Gonçalves  
    Lorenza Leão Oliveira Moreno

    DCC/UFJF
  </p>
</div>

This project has been developed as part of my undergraduate research project at the Computer Science Department of the Federal University of Juiz de Fora (UFJF). The aim of this project is to address a routing problem within the context of green logistics, focusing on optimizing transportation routes to minimize environmental impact while ensuring efficient delivery using an hybrid approach with trucks and drones.

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Compilation</a></li>
      </ul>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#references">References</a></li>
  </ol>
</details>


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* C++ Compiler
* Python 3.10.8 or latest versions

### Compilation

1. Clone the repo
   ```sh
   git clone https://github.com/Routing-Problems-in-Green-Logistic/MOVRPDD-MariaLuisa.git
   ```
2. Compile files
   ```sh
   make
   ```
3. For random constructor, run 
   ```sh
   ./MOVRPDD <instance_file> <alpha> <population_size> <number_of_iterations> <tournament_size>
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap

- [x] Greedy Constructor for truck routes
- [x] Random Greedy Constructor for truck routes
- [x] Drone route construction algorithm
- [x] Routes plotting in Python
- [x] Solution Encoder and Decoder
- [x] Fast Non-Dominated Sorting Algorithm
- [x] Crowding Distance Calculation
- [x] PMX Crossover
- [x] OX Crossover
- [x] Mutation Operators (swap, insert and reverse)
- [x] Multi-dimension Local Search
- [x] NSGA-II Procedure

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## References

* [ZHANG, Shuai et al, A novel multi-objective optimization model for the vehicle routing problem with drone delivery and dynamic flight endurance, Computers & Industrial Engineering, v. 173, p. 108679, 2022.](https://www.sciencedirect.com/science/article/abs/pii/S0360835222006672)
* [K. Deb, A. Pratap, S. Agarwal and T. Meyarivan, "A fast and elitist multiobjective genetic algorithm: NSGA-II," in IEEE Transactions on Evolutionary Computation, vol. 6, no. 2, pp. 182-197, April 2002](https://ieeexplore.ieee.org/abstract/document/996017?casa_token=m4C9zZko_2sAAAAA:1CBCeNPnHPgAwoMH2yAYoZCl6SOnZYsWeNPvafHQFzt5to_Iu8b8sXH9QjI9fvbE9YvfikXsplo)
* [PRINS, Christian, A simple and effective evolutionary algorithm for the vehicle routing problem, Computers & Operations Research, v. 31, n. 12, p. 1985–2002, 2004.](https://www.sciencedirect.com/science/article/abs/pii/S0305054803001588)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
