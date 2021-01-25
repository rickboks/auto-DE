#pragma once
#include "constrainthandler.h"
#include "Eigen/Dense"

class MutationManager {
	protected:
		ConstraintHandler* const ch;
		virtual Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const=0;
	public:
		static std::function<MutationManager* (ConstraintHandler* const)> create(std::string const id);
		MutationManager(ConstraintHandler * const ch):ch(ch){};
		virtual ~MutationManager(){};
		virtual void prepare(std::vector<Solution*>const& /*genomes*/){};
		Solution* mutate(std::vector<Solution*>const& genomes, int const i, double const F);
};

class Rand1MutationManager : public MutationManager {
	public:
		Rand1MutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTB1MutationManager : public MutationManager {
	private:
		Solution const* best;
	public:
		TTB1MutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTB2MutationManager : public MutationManager {
	private:
		Solution const* best;
	public:
		TTB2MutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TTPB1MutationManager : public MutationManager {
	public:
		TTPB1MutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Best1MutationManager: public MutationManager {
	private:
		Solution const* best;
	public:
		Best1MutationManager(ConstraintHandler* const ch):MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Best2MutationManager: public MutationManager {
	private:
		Solution const* best;
	public:
		Best2MutationManager(ConstraintHandler* const ch):MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Rand2MutationManager: public MutationManager {
	public:
		Rand2MutationManager(ConstraintHandler* const ch):MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class Rand2DirMutationManager : public MutationManager {
	public:
		Rand2DirMutationManager(ConstraintHandler* const ch):MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class NSDEMutationManager : public MutationManager {
	public:
		NSDEMutationManager(ConstraintHandler* const ch):MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TrigonometricMutationManager : public MutationManager {
	private:
		double const gamma;
		Solution* trigonometricMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
		Solution* rand1Mutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
	public:
		TrigonometricMutationManager(ConstraintHandler* const ch): MutationManager(ch), gamma(0.05){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TwoOpt1MutationManager : public MutationManager {
	public:
		TwoOpt1MutationManager(ConstraintHandler* const ch): MutationManager(ch) {};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class TwoOpt2MutationManager : public MutationManager {
	public:
		TwoOpt2MutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class ProximityMutationManager : public MutationManager {
	private:
		Eigen::ArrayXXd Rp;
	public:
		ProximityMutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};

class RankingMutationManager : public MutationManager {
	private:
		std::map<Solution*, double> probability;
		Solution* pickRanked(std::vector<Solution*> & possibilities) const;
	public:
		RankingMutationManager(ConstraintHandler* const ch): MutationManager(ch){};
		void prepare(std::vector<Solution*>const& genomes);
		Solution* doMutation(std::vector<Solution*>const& genomes, int const i, double const F) const;
};
