#include "mutationmanager.h"
#include "util.h"

using Eigen::ArrayXXd;

std::function<MutationManager* (ConstraintHandler* const)> MutationManager::create(std::string const id){
#define ALIAS(X, Y) if (id == X) return [](ConstraintHandler* const ch){return new Y(ch);};
	ALIAS("R1", Rand1MutationManager)
	ALIAS("T1", TTB1MutationManager)
	ALIAS("T2", TTB2MutationManager)
	ALIAS("P1", TTPB1MutationManager)
	ALIAS("B1", Best1MutationManager)
	ALIAS("B2", Best2MutationManager)
	ALIAS("R2", Rand2MutationManager)
	ALIAS("RD", Rand2DirMutationManager)
	ALIAS("NS", NSDEMutationManager)
	ALIAS("TR", TrigonometricMutationManager)
	ALIAS("O1", TwoOpt1MutationManager)
	ALIAS("O2", TwoOpt2MutationManager)
	ALIAS("PX", ProximityMutationManager)
	ALIAS("RA", RankingMutationManager)
	throw std::invalid_argument("no such MutationManager: " + id);
}

Solution* MutationManager::mutate(std::vector<Solution*>const& genomes, int const i, double const F){
	int resamples = 0;
	while (true){
		Solution* const m = doMutation(genomes, i, F);
		if (!ch->resample(m, resamples)){
			ch->repair(m); //generic repair
			return m;
		}
		resamples++;
		delete m;
	}
}

// Rand/1
Solution* Rand1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Target-to-best/1
void TTB1MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* TTB1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	Solution* const m = new Solution(
			genomes[i]->X() + F * (best->X() - genomes[i]->X() + xr[0]->X() - xr[1]->X())
		);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Target-to-best/2
void TTB2MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* TTB2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 4);
	Solution* const m = new Solution(
			genomes[i]->X() + F * (best->X() - genomes[i]->X() + xr[0]->X() 
				- xr[1]->X() + xr[2]->X() - xr[3]->X())
		);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Target-to-pbest/1
Solution* TTPB1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	Solution const* const pBest = getPBest(genomes); // pBest is sampled for each mutation
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	Solution* const m = new Solution(
			genomes[i]->X() + F * (pBest->X() - genomes[i]->X() + xr[0]->X() - xr[1]->X())
		);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Best/1
void Best1MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* Best1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	Solution* const m = new Solution(best->X() + F * (xr[0]->X() - xr[1]->X()));
	ch->repair(m, best, genomes[i]);
	return m;
}

// Best/2
void Best2MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* Best2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 4);
	Solution* const m = new Solution(best->X() + F * (xr[0]->X() - xr[1]->X() + xr[2]->X() - xr[3]->X()));
	ch->repair(m, best, genomes[i]);
	return m;
}

// Rand/2
Solution* Rand2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 5);
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X() + xr[3]->X() - xr[4]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Rand/2/dir
Solution* Rand2DirMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> xr = pickRandom(remove(genomes,i), 4);

	if (xr[1]->getFitness() < xr[0]->getFitness())
		std::swap(xr[0], xr[1]);

	if (xr[3]->getFitness() < xr[2]->getFitness())
		std::swap(xr[2], xr[3]);

	Solution* m = new Solution(xr[0]->X() + F/2. * (xr[0]->X() - xr[1]->X() + xr[2]->X() - xr[3]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// NSDE
Solution* NSDEMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const /*F*/) const {
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);
	double const F = (rng.randDouble(0,1) <= .5 ? rng.normalDistribution(.5,.5) : rng.cauchyDistribution(0,1));
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Trigonometric
Solution* TrigonometricMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	return rng.randDouble(0,1) <= gamma ? trigonometricMutation(genomes, i, F) : rand1Mutation(genomes, i, F);
}

Solution* TrigonometricMutationManager::trigonometricMutation(std::vector<Solution*>const& genomes, int const i, 
		double const /*F*/) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);

	double const pPrime = std::abs(xr[0]->getFitness()) + std::abs(xr[1]->getFitness()) + std::abs(xr[2]->getFitness());
	double const p0 = std::abs(xr[0]->getFitness()) / pPrime;
	double const p1 = std::abs(xr[1]->getFitness()) / pPrime;
	double const p2 = std::abs(xr[2]->getFitness()) / pPrime;

	VectorXd mutant = (xr[0]->X() + xr[1]->X() + xr[2]->X())/3.;
	Solution const base = Solution(mutant); // only used for correction strategies

	mutant += (p1-p0) * (xr[0]->X() - xr[1]->X()) + (p2-p1) * (xr[1]->X() - xr[2]->X()) + 
		(p0-p2) * (xr[2]->X() - xr[0]->X());
	
	Solution* m = new Solution(mutant);
	ch->repair(m, &base, genomes[i]);
	return m;
}

Solution* TrigonometricMutationManager::rand1Mutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);
	VectorXd const mutant = xr[0]->X() + F * (xr[1]->X() - xr[2]->X());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Two-opt/1
Solution* TwoOpt1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> xr = pickRandom(remove(genomes,i), 3);
	if (xr[1]->getFitness() < xr[0]->getFitness())
		std::swap(xr[0], xr[1]);
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Two-opt/2
Solution* TwoOpt2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> xr = pickRandom(remove(genomes,i), 5);
	if (xr[1]->getFitness() < xr[0]->getFitness())
		std::swap(xr[0], xr[1]);
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X() + xr[3]->X() - xr[4]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Proximity-based Rand/1
void ProximityMutationManager::prepare(std::vector<Solution*>const& genomes){
	int const size = genomes.size();
	//Initialize the matrices
	ArrayXXd Rd(size, size);
	Rd.matrix().diagonal().fill(0.);

	// Fill distance matrix
	for (int i = 0; i < size-1; i++){ // Only fill upper triangle
		for (int j = i + 1; j < size; j++){
			double const dist = std::max(distance(genomes[i], genomes[j]), 1.0e-10);
			Rd(i,j) = Rd(j,i) = dist;
		}
	}

	Rp = (Rd.colwise() / Rd.rowwise().sum()).inverse();
}

Solution* ProximityMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<double> prob(Rp.row(i).begin(), Rp.row(i).end());
	std::vector<Solution*> const xr = rouletteSelect(remove(genomes, i), remove(prob, i), 3, false);
	Solution* const m = new Solution(xr[0]->X() + F * (xr[1]->X() - xr[2]->X()));
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Ranking based
void RankingMutationManager::prepare(std::vector<Solution*>const& genomes){
	int const size = genomes.size();
	probability.clear();
	std::vector<Solution*> const sorted = sortOnFitness(genomes);
	for (int i = 0; i < size; i++)
		probability[sorted[i]] = double(size - (i+1)) / double(size);
}

Solution* RankingMutationManager::pickRanked(std::vector<Solution*>& possibilities) const{
	int index;
	do {
		index = rng.randInt(0, possibilities.size()-1);
	} while (rng.randDouble(0,1) > probability.at(possibilities[index]));

	Solution* const pick = possibilities[index];
	possibilities.erase(possibilities.begin() + index);
	return pick;
}

Solution* RankingMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	Solution const * const pBest = getPBest(genomes); // pBest is sampled for each mutation
	std::vector<Solution*> possibilities = remove(genomes,i);
	Solution const* const xr0 = pickRanked(possibilities); // N.B. Ranked instead of Random (removes from possibilities)
	Solution const* const xr1 = pickRandom(possibilities, 1)[0];

	Solution* const m = new Solution(
			genomes[i]->X() + F * (pBest->X() - genomes[i]->X() + xr0->X() - xr1->X())
		);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}
