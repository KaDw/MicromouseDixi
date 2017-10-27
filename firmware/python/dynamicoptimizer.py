import numpy as np
import matplotlib.pyplot as plt

class optimizer:
    L, R, t = [], [], []  # left/right wheel positiom and time

    def fromCSV(self, filename):
        """ from file in format: L R
        values are in int variable"""
        try:
            data = []
            lastT = 0
            with open(filename) as f:
                for l in f.readlines():
                    s = [x.strip() for x in l.split()]
                    data.append((int(s[0]), int(s[1]), lastT))
                    lastT += 1
            #L, R, t = zip(*data)
        except:
            pass


if __name__ == "__main__":
    # Find negative values
    def eval_func(ind):
        score = 0.0
        for x in range(0, len(ind)):
            if ind[x] <= 0.0: score += 0.1
        return score


    # Genome instance
    genome = G1DList.G1DList(20)
    genome.setParams(rangemin=-6.0, rangemax=6.0)

    # Change the initializator to Real values
    genome.initializator.set(Initializators.G1DListInitializatorReal)

    # Change the mutator to Gaussian Mutator
    genome.mutator.set(Mutators.G1DListMutatorRealGaussian)

    # The evaluator function (objective function)
    genome.evaluator.set(eval_func)

    # Genetic Algorithm Instance
    ga = GSimpleGA.GSimpleGA(genome)
    ga.selector.set(Selectors.GRouletteWheel)
    ga.nGenerations = 100

    # Do the evolution
    ga.evolve(10)

    # Best individual
    print(ga.bestIndividual())