#include "mutationmanager.h"
#include "util.h"

using Eigen::ArrayXXd;

std::function<MutationManager* (ConstraintHandler* const)> MutationManager::create(std::string const id){
#define LC(X) [](ConstraintHandler* const ch){return new X(ch);}
	if (id == "R1") return LC(Rand1MutationManager);
	if (id == "T1") return LC(TTB1MutationManager);
	if (id == "T2") return LC(TTB2MutationManager);
	if (id == "P1") return LC(TTPB1MutationManager);
	if (id == "B1") return LC(Best1MutationManager);
	if (id == "B2") return LC(Best2MutationManager);
	if (id == "R2") return LC(Rand2MutationManager);
	if (id == "RD") return LC(Rand2DirMutationManager);
	if (id == "NS") return LC(NSDEMutationManager);
	if (id == "TR") return LC(TrigonometricMutationManager);
	if (id == "O1") return LC(TwoOpt1MutationManager);
	if (id == "O2") return LC(TwoOpt2MutationManager);
	if (id == "PX") return LC(ProximityMutationManager);
	if (id == "RA") return LC(RankingMutationManager);
	throw std::invalid_argument("no such MutationManager: " + id);
}

Solution* MutationManager::mutate(std::vector<Solution*>const& genomes, int const i, double const F){
	int resamples = 0;
	while (true){
		Solution* m = doMutation(genomes, i, F);
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
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Target-to-best/1
void TTB1MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* TTB1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	VectorXd const mutant = genomes[i]->getX() + F * (best->getX() - genomes[i]->getX() 
			+ xr[0]->getX() - xr[1]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Target-to-best/2
void TTB2MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* TTB2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 4);
	VectorXd const mutant = genomes[i]->getX() + F * (best->getX() - genomes[i]->getX() 
			+ xr[0]->getX() - xr[1]->getX() + xr[2]->getX() - xr[3]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Target-to-pbest/1
Solution* TTPB1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	Solution const* const pBest = getPBest(genomes); // pBest is sampled for each mutation
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	VectorXd const mutant = genomes[i]->getX() + F * (pBest->getX() - genomes[i]->getX() + 
			xr[0]->getX() - xr[1]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}

// Best/1
void Best1MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* Best1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 2);
	VectorXd const mutant = best->getX() + F * (xr[0]->getX() - xr[1]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, best, genomes[i]);
	return m;
}

// Best/2
void Best2MutationManager::prepare(std::vector<Solution*>const& genomes){
	best = getBest(genomes);
}

Solution* Best2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 4);
	VectorXd const mutant =  best->getX() + F * (xr[0]->getX() - xr[1]->getX() + xr[2]->getX() - xr[3]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, best, genomes[i]);
	return m;
}

// Rand/2
Solution* Rand2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 5);
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX() + xr[3]->getX() - xr[4]->getX());
	Solution* const m = new Solution(mutant);
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

	VectorXd const mutant = xr[0]->getX() + F/2. * (xr[0]->getX() - xr[1]->getX() + xr[2]->getX() - xr[3]->getX());
	Solution* m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// NSDE
Solution* NSDEMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const /*F*/) const {
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);
	double const F = (rng.randDouble(0,1) < 0.5 ? rng.normalDistribution(0.5,0.5) : rng.cauchyDistribution(0,1));
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Trigonometric
Solution* TrigonometricMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	return rng.randDouble(0,1) <= gamma ? trigonometricMutation(genomes, i, F) : rand1Mutation(genomes, i, F);
}

Solution* TrigonometricMutationManager::trigonometricMutation(std::vector<Solution*>const& genomes, int const i, double const /*F*/) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);

	double const pPrime = std::abs(xr[0]->getFitness()) + std::abs(xr[1]->getFitness()) + std::abs(xr[2]->getFitness());
	double const p0 = std::abs(xr[0]->getFitness()) / pPrime;
	double const p1 = std::abs(xr[1]->getFitness()) / pPrime;
	double const p2 = std::abs(xr[2]->getFitness()) / pPrime;

	VectorXd mutant = (xr[0]->getX() + xr[1]->getX() + xr[2]->getX())/3.;
	Solution const base = Solution(mutant); // only used for correction strategies

	mutant += (p1-p0) * (xr[0]->getX() - xr[1]->getX()) + (p2-p1) * (xr[1]->getX() - xr[2]->getX()) + 
		(p0-p2) * (xr[2]->getX() - xr[0]->getX()) ;
	
	Solution* m = new Solution(mutant);
	ch->repair(m, &base, genomes[i]);
	return m;
}

Solution* TrigonometricMutationManager::rand1Mutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> const xr = pickRandom(remove(genomes,i), 3);
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Two-opt/1
Solution* TwoOpt1MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> xr = pickRandom(remove(genomes,i), 3);
	if (xr[1]->getFitness() < xr[0]->getFitness())
		std::swap(xr[0], xr[1]);
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Two-opt/2
Solution* TwoOpt2MutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<Solution*> xr = pickRandom(remove(genomes,i), 5);
	if (xr[1]->getFitness() < xr[0]->getFitness())
		std::swap(xr[0], xr[1]);
	VectorXd const mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX() + xr[3]->getX() - xr[4]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Proximity-based Rand/1
void ProximityMutationManager::prepare(std::vector<Solution*>const& genomes){
	int const size = genomes.size();
	//Initialize the matrices
	ArrayXXd Rd = ArrayXXd::Zero(size, size);
	Rp = ArrayXXd::Zero(size, size);

	// Fill distance matrix
	for (int i = 0; i < size-1; i++){ // Only fill upper triangle
		for (int j = i + 1; j < size; j++){
			double const dist = std::max(distance(genomes[i], genomes[j]), 1.0e-12);
			Rd(i,j) = dist;
			Rd(j,i) = dist;
		}
	}

	Rp = 1. / (Rd.colwise() / Rd.rowwise().sum());
}

Solution* ProximityMutationManager::doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const{
	std::vector<double> prob(Rp.row(i).begin(), Rp.row(i).end());
	std::vector<Solution*> xr = rouletteSelect(remove(genomes, i), remove(prob, i), 3, false);
	VectorXd mutant = xr[0]->getX() + F * (xr[1]->getX() - xr[2]->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, xr[0], genomes[i]);
	return m;
}

// Ranking based
void RankingMutationManager::prepare(std::vector<Solution*>const& genomes){
	int const size = genomes.size();
	probability.clear();

	std::vector<Solution*> sorted = genomes;
	sortOnFitness(sorted);

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
	VectorXd const mutant = genomes[i]->getX() + F * (pBest->getX() - genomes[i]->getX() + xr0->getX() - xr1->getX());
	Solution* const m = new Solution(mutant);
	ch->repair(m, genomes[i], genomes[i]);
	return m;
}
