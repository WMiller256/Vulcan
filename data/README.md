<h1 align = "center"> Data </h1>
&emsp; &emsp; Sim data files for speed comparisions and {CBody} configurations

<h2 align = "center"> Files </h2>

<i>BS-threadTesting.1.txt</i> and <i>BS-threadTesting.2.txt</i> - speed comparisions for different 
parallelization methods (Open MP, Manual and Single) in performing <i>n</i> iterations of the 
Bulirsch-Stoer method to solve a basic ODE. This is analagous to using the Bulirsch-Stoer method to 
simulate a single body in a Gauge field (i.e. a planet well outside the Hill spheres of any stars but 
still influenced by a galactic-center blackhole) and gives an accurate depiction of the relative speeds 
of the different parallelization methods within a single timestep with a given number of bodies. The 
format is <parallelmethod> <iterations> <time in microseconds> <number of threads>. "omp" corresponds to 
Open MP parallelization using #pragma omp for schedule(dynamic), "explicit" corresponds to using 
std::threads to manually parallelize, and "single" corresponds to single threading. There are figures
summarizing this data in the Figures folder.

<i>solarSystem.txt</i> - solar system configuration file.
