#pragma once
#include "constrainthandler.h"

class MutationManager {
	protected:
		int const D;
		ConstraintHandler* const ch;
		virtual Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const=0;
	public:
		MutationManager(int const D, ConstraintHandler * const ch):D(D), ch(ch){};
		virtual ~MutationManager(){};
		virtual void prepare(std::vector<Solution*>const& genomes){};
		Solution* mutate(std::vector<Solution*>const& genomes, int const i, double const F);
};

extern std::map<std::string, std::function<MutationManager* (int const, ConstraintHandler* const)>> const mutations;

class Rand1MutationManager : public MutationManager {
	public:
		Rand1MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTB1MutationManager : public MutationManager {
	private:
		Solution const* best;
	public:
		TTB1MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTB2MutationManager : public MutationManager {
	private:
		Solution const* best;
	public:
		TTB2MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTPB1MutationManager : public MutationManager {
	public:
		TTPB1MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Best1MutationManager: public MutationManager {
	private:
		Solution const* best;
	public:
		Best1MutationManager(int const D, ConstraintHandler* const ch):MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Best2MutationManager: public MutationManager {
	private:
		Solution const* best;
	public:
		Best2MutationManager(int const D, ConstraintHandler* const ch):MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Rand2MutationManager: public MutationManager {
	public:
		Rand2MutationManager(int const D, ConstraintHandler* const ch):MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Rand2DirMutationManager : public MutationManager {
	public:
		Rand2DirMutationManager(int const D, ConstraintHandler* const ch):MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class NSDEMutationManager : public MutationManager {
	public:
		NSDEMutationManager(int const D, ConstraintHandler* const ch):MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TrigonometricMutationManager : public MutationManager {
	private:
		double const gamma;
		Solution* trigonometricMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
		Solution* rand1Mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
	public:
		TrigonometricMutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch), gamma(0.05){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TwoOpt1MutationManager : public MutationManager {
	public:
		TwoOpt1MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch) {};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TwoOpt2MutationManager : public MutationManager {
	public:
		TwoOpt2MutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class ProximityMutationManager : public MutationManager {
	private:
		std::vector< std::vector<double> > Rp;
		std::vector< std::vector<double> > Rd;
	public:
		ProximityMutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class RankingMutationManager : public MutationManager {
	private:
		std::map<Solution*, double> probability;
		Solution* pickRanked(std::vector<Solution*> & possibilities) const;
	public:
		RankingMutationManager(int const D, ConstraintHandler* const ch): MutationManager(D, ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* do_mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};
